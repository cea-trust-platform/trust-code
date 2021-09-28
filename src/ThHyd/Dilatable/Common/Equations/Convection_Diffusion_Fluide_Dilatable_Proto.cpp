/****************************************************************************
 * Copyright (c) 2021, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Convection_Diffusion_Fluide_Dilatable_Proto.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Fluide_Dilatable_Proto.h>
#include <Fluide_Weakly_Compressible.h>
#include <Convection_Diffusion_std.h>
#include <EcritureLectureSpecial.h>
#include <Op_Conv_negligeable.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Solveur_Masse.h>
#include <Matrice_Morse.h>
#include <Zone_Cl_dis.h>
#include <DoubleTrav.h>
#include <Operateur.h>
#include <Domaine.h>
#include <Avanc.h>

void Convection_Diffusion_Fluide_Dilatable_Proto::completer_common_impl
(const Champ_Inc& inco, const Zone_Cl_dis& zcl_dis_modif,
 const Zone_Cl_dis& zcl_dis, const Schema_Temps_base& sch,
 const Fluide_Dilatable_base& fld, Equation_base& eq)
{
  zcl_dis_modif_ = zcl_dis_modif;
  zcl_dis_ = zcl_dis;
  inco_ = inco;
  sch_ = sch;
  fld_ = fld;
  eq_ = eq;
}

void Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl
(DoubleTab& Div, const Operateur& op_conv) const
{
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,op_conv.l_op_base()))
    {
      Div=0;
      return;
    }

  DoubleTrav unite(inco_->valeurs());
  unite=1;
  ref_cast_non_const(Operateur_base,op_conv.l_op_base()).associer_zone_cl_dis(zcl_dis_modif_->valeur());

  op_conv.ajouter(unite,Div);
  ref_cast_non_const(Operateur_base,op_conv.l_op_base()).associer_zone_cl_dis(zcl_dis_->valeur());

}

// Description:
//     Renvoie la derivee en temps de l'inconnue de l'equation.
//     Le calcul est le suivant:
//         d(inconnue)/dt = M^{-1} * (sources - somme(Op_{i}(inconnue))) / rho
// Precondition:
// Parametre: DoubleTab& derivee
//    Signification: le tableau des valeurs de la derivee en temps du champ inconnu
//    Valeurs par defaut:
//    Contraintes: ce parametre est remis a zero des l'entree de la methode
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs de la derivee en temps du champ inconnu
//    Contraintes:
// Exception:
// Effets de bord: des communications (si version parallele) sont generees pas cet appel
// Postcondition:
DoubleTab& Convection_Diffusion_Fluide_Dilatable_Proto::derivee_en_temps_inco_sans_solveur_masse_imp
(DoubleTab& derivee,const Operateur& op_diff,const Operateur& op_conv,  const Sources& src,
 Solveur_Masse& solv, const bool is_expl)
{
  /*
   * The equation is like that :
   *
   * d(rho Y)/dt + div( rho*u*Y ) = div( rho*D*grad(Y) )
   *
   * With a non conservative formulation we write:
   * rho d(Y)/dt + Y d(rho)/dt + Y div (rho*u) + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * Using the mass equation, we get :
   * rho d(Y)/dt + rho*u grad(Y) = div( rho*D*grad(Y) )
   *
   * divide by rho =>
   * d(Y)/dt = div( rho*D*grad(Y) ) / rho - u grad(Y)
   *
   * This is what we code here with derivee = d(Y)/dt
   */

  const Schema_Temps_base& sch = sch_.valeur();
  int diffusion_implicite=sch.diffusion_implicite();
  zcl_dis_->les_conditions_limites().set_modifier_val_imp(0);

  /*
   * FIRST TERM : diffusive
   * derivee = div( rho*D*grad(Y) )
   */
  if (!diffusion_implicite) op_diff.ajouter(derivee);

  zcl_dis_->les_conditions_limites().set_modifier_val_imp(1);
  derivee.echange_espace_virtuel();

  // Add source term (if any)
  src.ajouter(derivee);

  // On divise derivee  par rho
  const DoubleTab& tab_rho = fld_->masse_volumique().valeurs();
  int n = tab_rho.dimension(0);
  for (int som=0 ; som<n ; som++)
    derivee(som) /= tab_rho(som);

  derivee.echange_espace_virtuel();

  /*
   * SECOND TERM : convective
   * = - u grad(Y) = [ Y div (rho*u) - div( rho*u*Y ) ] / rho
   */
  DoubleTrav convection(derivee);

  // Add Y div (rho*u)
  const DoubleTab& Y=inco_->valeurs();
  calculer_div_rho_u_impl(convection, op_conv);

  for (int som=0 ; som<n ; som++)
    convection(som) *= (-Y(som));

  // Add convection operator: - div( rho*u*Y )
  op_conv.ajouter(convection);

  // Divide by rho
  for (int som=0 ; som<n ; som++)
    convection(som) /= tab_rho(som);

  /*
   * TOTAL TERM : diffusive + convective + sources
   */
  derivee+=convection;

  if (diffusion_implicite)
    {
      const DoubleTab& Tfutur=inco_->futur();
      DoubleTrav secmem(derivee);
      secmem=derivee;
      solv.appliquer(secmem);
      derivee=(Tfutur);
      solv->set_name_of_coefficient_temporel("masse_volumique");
      eq_->Gradient_conjugue_diff_impl(secmem,derivee);
      solv->set_name_of_coefficient_temporel("no_coeff");
    }

  // 100% explicite
  if (!sch_->diffusion_implicite() && is_expl)
    {
      solv.appliquer(derivee);
      derivee.echange_espace_virtuel();
    }

  return derivee;
}

void Convection_Diffusion_Fluide_Dilatable_Proto::assembler_impl
( Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu,
  const Operateur& op_diff, const Operateur& op_conv,
  const Sources& src, Solveur_Masse& solv)
{
  const DoubleTab& tab_rho = fld_->masse_volumique().valeurs();
  const int n = tab_rho.dimension(0);

  // ajout diffusion (avec rho, D et Y)
  op_diff.l_op_base().contribuer_a_avec(inco, matrice );

  // Add source term (if any)
  src.contribuer_a_avec(inco,matrice);

  const IntVect& tab1= matrice.get_tab1();
  DoubleVect& coeff=matrice.get_set_coeff();
  DoubleVect coeff_diffusif(coeff);

  // on calcule les coefficients de l'op de convection on obtient les coeff de div (rho*u*Y)
  coeff=0;

  op_conv.l_op_base().contribuer_a_avec(inco, matrice );

  // on calcule div(rho * u)
  DoubleTrav derivee2(resu);
  calculer_div_rho_u_impl(derivee2,op_conv);

  for (int som=0 ; som<n ; som++)
    {
      double inv_rho = 1. / tab_rho(som);
      for (int k=tab1(som)-1; k<tab1(som+1)-1; k++)
        coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*inv_rho);

      matrice(som,som)+=derivee2(som)*inv_rho;
    }

  // on a la matrice approchee on recalcule le residu;
  resu=0;
  derivee_en_temps_inco_sans_solveur_masse_imp(resu,op_diff,op_conv,src,solv,false /* implicit */);
  matrice.ajouter_multvect(inco,resu);
}

int Convection_Diffusion_Fluide_Dilatable_Proto::sauvegarder_WC(Sortie& os,
                                                                const Convection_Diffusion_std& eq,
                                                                const Fluide_Dilatable_base& fld)
{
  int bytes=0,a_faire,special;
  bytes += eq.sauvegarder_base(os); // XXX : voir Convection_Diffusion_std
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Fluide_Weakly_Compressible& FWC = ref_cast_non_const(Fluide_Weakly_Compressible,fld);
      Champ_Inc p_tab = FWC.inco_chaleur(); // Initialize with same discretization
      p_tab->nommer("Pression_EOS");
      p_tab->valeurs() = FWC.pression_th_tab(); // Use good values
      if (special && Process::nproc() > 1)
        Cerr << "ATTENTION : For a parallel calculation, the field Pression_EOS is not saved in xyz format ... " << finl;
      else
        bytes += p_tab->sauvegarder(os);
    }

  return bytes;
}

int Convection_Diffusion_Fluide_Dilatable_Proto::reprendre_WC(Entree& is,
                                                              double temps,
                                                              Convection_Diffusion_std& eq,
                                                              Fluide_Dilatable_base& fld,
                                                              Champ_Inc& inco,
                                                              Probleme_base& pb)
{
  // start resuming
  eq.reprendre_base(is); // XXX : voir Convection_Diffusion_std

  // XXX : should be set so that Pression_EOS is read and not initialized from data file
  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,fld);
  FWC.set_resume_flag();
  // resume EOS pressure field
  Champ_Inc p_tab = inco; // Same discretization normally
  p_tab->nommer("Pression_EOS");
  Nom field_tag(p_tab->le_nom());
  field_tag += p_tab.valeur().que_suis_je();
  field_tag += pb.domaine().le_nom();
  field_tag += Nom(temps,pb.reprise_format_temps());

  if (EcritureLectureSpecial::is_lecture_special() && Process::nproc() > 1)
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
