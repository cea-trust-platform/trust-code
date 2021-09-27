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
// File:        Convection_Diffusion_Espece_Binaire_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Espece_Binaire_base.h>
#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Op_Conv_negligeable.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>

Implemente_base(Convection_Diffusion_Espece_Binaire_base,"Convection_Diffusion_Espece_Binaire_base",Convection_Diffusion_Espece_Fluide_Dilatable_base);

Sortie& Convection_Diffusion_Espece_Binaire_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Binaire_base::readOn(Entree& is)
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::readOn(is);
  terme_convectif.set_fichier("Convection_Espece_Binaire");
  terme_convectif.set_description((Nom)"Convective flux =Integral(-rho*Y*u*ndS) [kg/s] if SI units used");
  terme_diffusif.set_fichier("Diffusion_Espece_Binaire");
  terme_diffusif.set_description((Nom)"Diffusive flux=Integral(rho*D*grad(Y)*ndS) [kg/s] if SI units used");

  //On modifie le nom ici pour que le champ puisse etre reconnu si une sonde de fraction_massique est demandee
  if (le_fluide->type_fluide()=="Melange_Binaire") l_inco_ch->nommer("fraction_massique");
  else
    {
      Cerr << "Error in your data file !" << finl;
      Cerr << "The equation " << que_suis_je() << " should only be used with the EOS Melange_Binaire";
      Process::exit();
    }
  champs_compris_.ajoute_champ(l_inco_ch);
  return is;
}

void Convection_Diffusion_Espece_Binaire_base::set_param(Param& param)
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::set_param(param);
}

int Convection_Diffusion_Espece_Binaire_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      /*
       * The diffusive term is div(rho*D*grad(Y))
       * recall that mu_sur_Sc = rho*D and nu_sur_Sc = D
       */

      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else
    return Convection_Diffusion_Espece_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);

}

const Champ_base& Convection_Diffusion_Espece_Binaire_base::diffusivite_pour_pas_de_temps()
{
  return le_fluide->nu_sur_Schmidt(); // D (diffusion coefficient)
}

const Champ_base& Convection_Diffusion_Espece_Binaire_base::vitesse_pour_transport()
{
  // we need rho * u and not u
  const Probleme_base& pb = probleme();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  const Champ_Inc& vitessetransportante = eqn_hydr.rho_la_vitesse() ;
  return vitessetransportante;
}

int Convection_Diffusion_Espece_Binaire_base::preparer_calcul()
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::preparer_calcul();
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);
  return 1;
}

void Convection_Diffusion_Espece_Binaire_base::completer()
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::completer();
}

void Convection_Diffusion_Espece_Binaire_base::calculer_div_rho_u(DoubleTab& Div) const
{
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,operateur(1).l_op_base()))
    {
      Div=0;
      return;
    }

  DoubleTrav unite(inconnue().valeurs());
  unite=1;
  ref_cast_non_const(Operateur_base,operateur(1).l_op_base()).associer_zone_cl_dis(zcl_modif_.valeur());

  operateur(1).ajouter(unite,Div);
  ref_cast_non_const(Operateur_base,operateur(1).l_op_base()).associer_zone_cl_dis(zone_Cl_dis().valeur());

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
DoubleTab& Convection_Diffusion_Espece_Binaire_base::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee=0;
  derivee_en_temps_inco_sans_solveur_masse(derivee);
  if (!schema_temps().diffusion_implicite())
    {
      solveur_masse.appliquer(derivee);
      derivee.echange_espace_virtuel();
    }
  return derivee;
}

DoubleTab& Convection_Diffusion_Espece_Binaire_base::derivee_en_temps_inco_sans_solveur_masse(DoubleTab& derivee)
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

  const Schema_Temps_base& sch=schema_temps();
  int diffusion_implicite=sch.diffusion_implicite();
  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(0);

  /*
   * FIRST TERM : diffusive
   * derivee = div( rho*D*grad(Y) )
   */
  if (!diffusion_implicite)
    operateur(0).ajouter(derivee);

  la_zone_Cl_dis.les_conditions_limites().set_modifier_val_imp(1);
  derivee.echange_espace_virtuel(); // XXX : here or later ?

  // Add source term (if any)
  les_sources.ajouter(derivee);

  // On divise derivee  par rho
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
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
  const DoubleTab& Y=inconnue().valeurs();
  calculer_div_rho_u(convection);

  for (int som=0 ; som<n ; som++)
    convection(som) *= (-Y(som));

  // Add convection operator: - div( rho*u*Y )
  operateur(1).ajouter(convection);

  // Divide by rho
  for (int som=0 ; som<n ; som++)
    convection(som) /= tab_rho(som);

  /*
   * TOTAL TERM : diffusive + convective + sources
   */
  derivee+=convection;

  if (diffusion_implicite)
    {
      const DoubleTab& Tfutur=inconnue().futur();
      DoubleTrav secmem(derivee);
      secmem=derivee;
      solv_masse().appliquer(secmem);
      derivee=(Tfutur);
      solveur_masse->set_name_of_coefficient_temporel("masse_volumique");
      Equation_base::Gradient_conjugue_diff_impl( secmem, derivee ) ;
      solveur_masse->set_name_of_coefficient_temporel("no_coeff");
    }
  return derivee;
}

void Convection_Diffusion_Espece_Binaire_base::assembler( Matrice_Morse& matrice,const DoubleTab& inco, DoubleTab& resu)
{
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int n = tab_rho.dimension(0);

  // ajout diffusion (avec rho, D et Y)
  operateur(0).l_op_base().contribuer_a_avec(inco, matrice );

  // Add source term (if any)
  les_sources.contribuer_a_avec(inco,matrice);

  const IntVect& tab1= matrice.get_tab1();
  DoubleVect& coeff=matrice.get_set_coeff();
  DoubleVect coeff_diffusif(coeff);

  // on calcule les coefficients de l'op de convection on obtient les coeff de div (rho*u*Y)
  coeff=0;

  operateur(1).l_op_base().contribuer_a_avec(inco, matrice );

  // on calcule div(rho * u)
  DoubleTrav derivee2(resu);
  calculer_div_rho_u(derivee2);

  for (int som=0 ; som<n ; som++)
    {
      double inv_rho = 1. / tab_rho(som);
      for (int k=tab1(som)-1; k<tab1(som+1)-1; k++)
        coeff(k)= (coeff(k)*inv_rho+coeff_diffusif(k)*inv_rho);

      matrice(som,som)+=derivee2(som)*inv_rho;
    }

  // on a la matrice approchee on recalcule le residu;
  resu=0;
  derivee_en_temps_inco_sans_solveur_masse(resu);
  matrice.ajouter_multvect(inco,resu);
}
