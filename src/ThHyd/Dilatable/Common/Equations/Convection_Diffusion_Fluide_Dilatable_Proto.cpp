/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Convection_Diffusion_Fluide_Dilatable_base.h>
#include <Convection_Diffusion_Fluide_Dilatable_Proto.h>
#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Weakly_Compressible.h>
#include <Convection_Diffusion_std.h>
#include <EcritureLectureSpecial.h>
#include <Op_Conv_negligeable.h>
#include <Schema_Temps_base.h>
#include <Discretisation_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Statistiques.h>
#include <TRUSTTrav.h>
#include <Operateur.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Device.h>

extern Stat_Counter_Id assemblage_sys_counter_;
extern Stat_Counter_Id source_counter_;

void Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl
(DoubleTab& Div, const Convection_Diffusion_Fluide_Dilatable_base& eqn) const
{
  const Operateur& op_conv = eqn.operateur(1);
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,op_conv.l_op_base()))
    {
      Div=0;
      return;
    }

  // on cherche a changer temporairement le domaine_cl
  if (ch_unite_.est_nul())
    {
      ch_unite_ = eqn.inconnue();
      ch_unite_->valeurs() = 1.0;
    }
  ref_cast_non_const(Operateur_Conv_base,op_conv.l_op_base()).associer_champ_temp(ch_unite_, true);

  if (eqn.discretisation().que_suis_je() != "VDF")
    ref_cast_non_const(Operateur_base,op_conv.l_op_base()).associer_domaine_cl_dis(eqn.domaine_cl_modif());

  op_conv.ajouter(ch_unite_->valeurs(), Div);
  ref_cast_non_const(Operateur_Conv_base,op_conv.l_op_base()).associer_champ_temp(eqn.inconnue(), false);

  if (eqn.discretisation().que_suis_je() != "VDF")
    ref_cast_non_const(Operateur_base,op_conv.l_op_base()).associer_domaine_cl_dis(eqn.domaine_Cl_dis());
}

/*! @brief Renvoie la derivee en temps de l'inconnue de l'equation.
 *
 * Le calcul est le suivant:
 *          d(inconnue)/dt = M^{-1} * (sources - somme(Op_{i}(inconnue))) / rho
 *
 * @param (DoubleTab& derivee) le tableau des valeurs de la derivee en temps du champ inconnu
 * @return (DoubleTab&) le tableau des valeurs de la derivee en temps du champ inconnu
 */
DoubleTab& Convection_Diffusion_Fluide_Dilatable_Proto::derivee_en_temps_inco_sans_solveur_masse_impl
(Convection_Diffusion_Fluide_Dilatable_base& eqn, DoubleTab& derivee, const bool is_expl)
{
  /*
   * ATTENTION : THIS IS A GENERIC METHOD THAT IS USED TO SOLVE EXPLICITLY (OR DIFFUSION IMPLICIT)
   * A THERMAL OR SPECIES CONV/DIFF EQUATION. SO THE VARIABLE CAN EITHER BE THE TEMPERATURE T OR
   * THE MASS FRACTION Y.
   *
   * For the species equation for example, we have :
   * d(rho Y)/dt + div( rho*u*Y ) = div( rho*D*grad(Y) )
   *
   * With a non conservative formulation we write:
   * rho d(Y)/dt + Y d(rho)/dt + Y div (rho*u) + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * Using the mass equation, we get :
   * rho d(Y)/dt + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * divide by rho => d(Y)/dt = ( div( rho*D*grad(Y) ) + S ) / rho - u grad(Y)
   *
   * This is what we code here with derivee = d(Y)/dt
   *
   *
   * Similarly for the temperature equation, we code
   *
   * derivee = d(T)/dt = ( div( lambda*grad(T) ) + S ) / (rho*Cp) - u grad(T)
   */

  const Schema_Temps_base& sch = eqn.schema_temps();
  int diffusion_implicite=sch.diffusion_implicite();
  eqn.domaine_Cl_dis().les_conditions_limites().set_modifier_val_imp(0);

  /*
   * FIRST TERM : diffusive
   * derivee = div( rho*D*grad(Y) ) or div( lambda*grad(T) )
   */
  Fluide_Dilatable_base& fluide_dil = eqn.fluide();
  if ( !is_thermal() /* species equation */ || (is_thermal() && fluide_dil.type_fluide()=="Gaz_Parfait" ))
    {
      if (!diffusion_implicite) eqn.operateur(0).ajouter(derivee);
    }
  else // Thermal equation + Gaz reel => No diffusion implicit ...
    {
      eqn.operateur(0).ajouter(fluide_dil.temperature(),derivee);
      if (diffusion_implicite)
        {
          Cerr << "Error: diffusion implicit not implemented in Convection_Diffusion_Chaleur_Fluide_Dilatable_base" << finl;
          Process::exit();
        }
    }

  eqn.domaine_Cl_dis().les_conditions_limites().set_modifier_val_imp(1);
  derivee.echange_espace_virtuel();

  // Add source term (if any, but for temperatur eit is sure !!! )
  eqn.sources().ajouter(derivee);

  // On divise derivee par rho si espece ou gaz reel... sinon par rho*Cp !
  bool flag = is_thermal() && fluide_dil.type_fluide()=="Gaz_Parfait";
  if (flag) fluide_dil.update_rho_cp(sch.temps_courant());
  const DoubleTab& array = flag ? eqn.get_champ("rho_cp_comme_T").valeurs() : fluide_dil.masse_volumique().valeurs();
  tab_divide_any_shape(derivee, array);
  derivee.echange_espace_virtuel();

  /*
   * SECOND TERM : convective
   * = - u grad(Y) = [ Y div (rho*u) - div( rho*u*Y ) ] / rho
   * or
   * = - u grad(T) = [ T div (rho*u) - div( rho*u*T ) ] / rho*Cp
   */
  DoubleTrav convection(derivee);

  // Add Y div (rho*u) or T div (rho*u)
  const DoubleTab& inco = eqn.inconnue().valeurs();
  calculer_div_u_ou_div_rhou(convection);

  tab_multiply_any_shape(convection, inco);
  convection*=-1;

  // Add convection operator: - div( rho*u*Y ) or -div ( rho*u*T )
  eqn.operateur(1).ajouter(convection);

  // Divide by rho if necessary
  if (is_generic())
    {
      const DoubleTab& tab_rho = fluide_dil.masse_volumique().valeurs();
      tab_divide_any_shape(convection, tab_rho);
    }

  // Complete with special source terms from mass equation (if any)
  DoubleTrav mass_source_term(derivee);
  mass_source_term = 0.0;

  const bool has_mass_flux = (sub_type(Navier_Stokes_Fluide_Dilatable_base, fluide_dil.vitesse().equation())) ?
                             ref_cast(Navier_Stokes_Fluide_Dilatable_base, fluide_dil.vitesse().equation()).has_source_masse() : false;

  if (!is_thermal() && has_mass_flux) /* species equation */
    {
      const Source_Masse_Fluide_Dilatable_base& src_masse = ref_cast(Navier_Stokes_Fluide_Dilatable_base, fluide_dil.vitesse().equation()).source_masse();
      src_masse.ajouter_eq_espece(eqn, fluide_dil, is_expl, mass_source_term);
    }

  /*
   * TOTAL TERM : diffusive + convective + sources
   */
  derivee+=convection;

  // si schema implicite
  if (!is_expl && has_mass_flux)
    derivee += mass_source_term; // pour ça on traite le volume par le solveur de masse plus tard ...

  if (diffusion_implicite)
    {
      const DoubleTab& Tfutur=eqn.inconnue().futur();
      DoubleTrav secmem(derivee);
      secmem=derivee; // sans contribution terme source
      eqn.solv_masse().appliquer(secmem);

      if (has_mass_flux)
        secmem += mass_source_term ; // ajoute contribution terme source (deja divise par V)

      derivee = Tfutur;

      is_thermal() ? eqn.solv_masse().set_name_of_coefficient_temporel("rho_cp_comme_T") :
      eqn.solv_masse().set_name_of_coefficient_temporel("masse_volumique");

      eqn.Gradient_conjugue_diff_impl(secmem,derivee);
      eqn.solv_masse().set_name_of_coefficient_temporel("no_coeff");
    }

  // 100% explicite
  if (!sch.diffusion_implicite() && is_expl)
    {
      eqn.solv_masse().appliquer(derivee);

      if (has_mass_flux)
        derivee += mass_source_term; // ajoute contribution terme source (deja divise par V)

      derivee.echange_espace_virtuel();
    }

  return derivee;
} /* END derivee_en_temps_inco_sans_solveur_masse_impl */

void Convection_Diffusion_Fluide_Dilatable_Proto::assembler_impl
( Convection_Diffusion_Fluide_Dilatable_base& eqn,
  Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  /*
   * ATEENTION : THIS IS A GENERIC METHOD THAT IS USED TO SOLVE IMPLICITLY A THERMAL OR SPECIES
   *  CONV/DIFF EQUATION. SO THE VARIABLE CAN EITHER BE THE TEMPERATURE T OR THE MASS FRACTION Y.
   *
   * See the previous method derivee_en_temps_inco_sans_solveur_masse_impl for more description
   * of the treated equations.
   */

  // Elie Saikali
  // Je garde ce test pour debug ... TODO : a voir si on peut virer tout ca...
  int test_op=0;
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE");
    if (theValue != nullptr) test_op=2;
  }
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE_BLOQUANT");
    if (theValue != nullptr) test_op=1;
  }

  Fluide_Dilatable_base& fluide_dil = eqn.fluide();
  const DoubleTab& tab_rho = fluide_dil.masse_volumique().valeurs();
  const int n = tab_rho.dimension(0);

  // ajout diffusion (avec rho, D et Y / ou lambda et T)
  eqn.operateur(0).l_op_base().contribuer_a_avec(inco, matrice );

  // Add source term (if any)
  eqn.sources().contribuer_a_avec(inco,matrice);

  const IntVect& tab1= matrice.get_tab1();
  DoubleVect& coeff=matrice.get_set_coeff();
  DoubleVect coeff_diffusif(coeff);

  // on calcule les coefficients de l'op de convection on obtient les coeff de div (rho*u*Y)
  // ou div(rho*u*T). dans le cas thermique, il faudrait multiplier par cp puis divisier par rho cp
  // on le fera d'un coup...
  coeff=0.;
  eqn.operateur(1).l_op_base().contribuer_a_avec(inco, matrice );

  // on calcule div(rho * u)
  DoubleTrav derivee2(resu);
  calculer_div_u_ou_div_rhou(derivee2);

  if (!is_thermal()) //espece
    {
      for (int som=0 ; som<n ; som++)
        {
          double inv_rho = 1. / tab_rho(som);
          for (int k=tab1(som)-1; k<tab1(som+1)-1; k++) coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*inv_rho);

          matrice(som,som)+=derivee2(som)*inv_rho;
        }
    }
  else if (is_thermal() && fluide_dil.type_fluide()=="Gaz_Parfait")
    {
      fluide_dil.update_rho_cp(eqn.schema_temps().temps_courant());
      const DoubleTab& rhoCp = eqn.get_champ("rho_cp_comme_T").valeurs();

      for (int som=0 ; som<n ; som++)
        {
          double inv_rho = 1. / tab_rho(som);
          if (!is_generic()) inv_rho = 1.;
          double rapport = 1. / rhoCp(som);

          // il faut multiplier toute la ligne de la matrice par rapport
          for (int k=tab1(som)-1; k<tab1(som+1)-1; k++) coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*rapport);

          // ajout de Tdiv(rhou )/rho
          matrice(som,som) += derivee2(som)*inv_rho;
        }
    }
  else
    {
      Cerr<<"The implicit algorithm is available only for perfect gas."<<finl;
      Process::exit();
    }

  // on a la matrice approchee on recalcule le residu;
  resu=0;
  derivee_en_temps_inco_sans_solveur_masse_impl(eqn,resu,false /* implicit */);
  matrice.ajouter_multvect(inco,resu);

  if (test_op)
    {
      DoubleTrav diff(resu), conv(resu);
      eqn.operateur(0).l_op_base().contribuer_au_second_membre(diff);
      eqn.operateur(1).l_op_base().contribuer_au_second_membre(conv);
      eqn.sources().ajouter(diff);
      double Cp = -5.;
      int is_cp_unif= sub_type(Champ_Uniforme,fluide_dil.capacite_calorifique());
      const DoubleTab& tab_cp = fluide_dil.capacite_calorifique().valeurs();
      if (is_cp_unif) Cp=tab_cp(0,0);

      for (int som=0 ; som<n ; som++)
        {
          if (!is_cp_unif) Cp=tab_cp(som);
          double inv_rho=1./tab_rho(som);
          if (!is_generic()) inv_rho=1;
          double rapport=1./(tab_rho(som)*Cp);
          diff(som)=resu(som)-conv(som)*inv_rho-diff(som)*rapport;
        }
      eqn.solv_masse().appliquer(diff);
      double err=mp_max_abs_vect(diff);
      Cerr << eqn.que_suis_je() <<" : Erreur assemblage = " << err << finl;;

      if (err > 1.e-5)
        {
          DoubleVect& diff_=diff;
          Cerr<<" size "<< diff_.size()<<finl;
          for (int i=0; i<diff_.size(); i++)
            if (std::fabs(diff_(i))>1e-5) Cerr<<i << " "<< diff_(i)<< " "<<finl;

          if (test_op==1)
            {
              Cerr<<" pb max case "<<imin_array(diff)<<" ou " <<imax_array(diff)<<finl;
              Process::exit();
            }
        }
    }
} /* END assembler_impl */

void Convection_Diffusion_Fluide_Dilatable_Proto::assembler_blocs(Convection_Diffusion_Fluide_Dilatable_base& eqn,matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  const std::string& nom_inco = eqn.inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco)?matrices.at(nom_inco):nullptr;

  Fluide_Dilatable_base& fluide_dil = eqn.fluide();
  const DoubleTab& tab_rho = fluide_dil.masse_volumique().valeurs();
  const int n = tab_rho.dimension(0);

  Matrice_Morse mat_diff(*mat);
  DoubleTab secmem_tmp(secmem);
  eqn.operateur(0).l_op_base().ajouter_blocs({{nom_inco, &mat_diff}}, secmem_tmp, semi_impl);
  statistiques().end_count(assemblage_sys_counter_);

  statistiques().begin_count(source_counter_);
  for (int i = 0; i < eqn.sources().size(); i++)
    eqn.sources()(i)->ajouter_blocs({{nom_inco, &mat_diff}}, secmem_tmp, semi_impl);
  statistiques().end_count(source_counter_);

  statistiques().begin_count(assemblage_sys_counter_);
  DoubleVect& coeff_diffusif=mat_diff.get_set_coeff();

  const IntVect& tab1= mat->get_tab1();
  DoubleVect& coeff=mat->get_set_coeff();
  coeff = 0;
  eqn.operateur(1).l_op_base().ajouter_blocs(matrices, secmem_tmp, semi_impl);

  // on calcule div(rho * u)
  calculer_div_u_ou_div_rhou(secmem);

  if (!is_thermal()) //espece
    {
      for (int som=0 ; som<n ; som++)
        {
          double inv_rho = 1. / tab_rho(som);
          for (int k=tab1(som)-1; k<tab1(som+1)-1; k++) coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*inv_rho);

          if(mat) (*mat)(som,som)+=secmem(som)*inv_rho;
        }
    }
  else if (is_thermal() && fluide_dil.type_fluide()=="Gaz_Parfait")
    {
      fluide_dil.update_rho_cp(eqn.schema_temps().temps_courant());
      const DoubleTab& rhoCp = eqn.get_champ("rho_cp_comme_T").valeurs();

      for (int som=0 ; som<n ; som++)
        {
          double inv_rho = 1. / tab_rho(som);
          if (!is_generic()) inv_rho = 1.;
          double rapport = 1. / rhoCp(som);

          // il faut multiplier toute la ligne de la matrice par rapport
          for (int k=tab1(som)-1; k<tab1(som+1)-1; k++) coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*rapport);

          // ajout de Tdiv(rhou )/rho
          if(mat) (*mat)(som,som) += secmem(som)*inv_rho;
        }
    }
  else
    {
      Cerr<<"The implicit algorithm is available only for perfect gas."<<finl;
      Process::exit();
    }

  // on a la matrice approchee on recalcule le residu;
  secmem=0;
  derivee_en_temps_inco_sans_solveur_masse_impl(eqn,secmem,false /* implicit */);
  if(mat) mat->ajouter_multvect(eqn.inconnue().valeurs(),secmem);
}


/*
 * Methodes statiques
 */
int Convection_Diffusion_Fluide_Dilatable_Proto::Sauvegarder_WC(Sortie& os,
                                                                const Convection_Diffusion_std& eq,
                                                                const Fluide_Dilatable_base& fld)
{
  int bytes=0,a_faire,special;
  bytes += eq.sauvegarder_base(os); // XXX : voir Convection_Diffusion_std
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Fluide_Weakly_Compressible& FWC = ref_cast_non_const(Fluide_Weakly_Compressible,fld);
      OWN_PTR(Champ_Inc_base) p_tab = FWC.inco_chaleur(); // Initialize with same discretization

      p_tab->nommer("Pression_EOS");
      p_tab->valeurs() = FWC.pression_th_tab(); // Use good values
      if (special && Process::is_parallel())
        Cerr << "ATTENTION : For a parallel calculation, the field Pression_EOS is not saved in xyz format ... " << finl;
      else
        bytes += p_tab->sauvegarder(os);
    }

  return bytes;
}

int Convection_Diffusion_Fluide_Dilatable_Proto::Reprendre_WC(Entree& is,
                                                              double temps,
                                                              Convection_Diffusion_std& eq,
                                                              Fluide_Dilatable_base& fld,
                                                              Champ_Inc_base& inco,
                                                              Probleme_base& pb)
{
  // start resuming
  eq.reprendre_base(is); // XXX : voir Convection_Diffusion_std

  // XXX : should be set so that Pression_EOS is read and not initialized from data file
  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,fld);
  FWC.set_resume_flag();
  // resume EOS pressure field
  OWN_PTR(Champ_Inc_base) p_tab = FWC.inco_chaleur(); // Initialize with same discretization

  p_tab->nommer("Pression_EOS");
  Nom field_tag(p_tab->le_nom());
  field_tag += p_tab->que_suis_je();
  field_tag += pb.domaine().le_nom();
  field_tag += Nom(temps,pb.reprise_format_temps());

  if (EcritureLectureSpecial::is_lecture_special() && Process::is_parallel())
    {
      Cerr << "Error in Convection_Diffusion_Espece_Binaire_WC::reprendre !" << finl;
      Cerr << "Use the sauv file to resume a parallel WC calculation (Pression_EOS is required) ... " << finl;
      Process::exit();
    }
  else
    {
      avancer_fichier(is, field_tag);
      p_tab->reprendre(is);
    }

  // set good field
  FWC.set_pression_th_tab(p_tab->valeurs());

  return 1;
}
