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
// File:        Loi_Etat_Multi_GP_WC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Loi_Etat
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Multi_GP_WC.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc_base.h>
#include <Probleme_base.h>
#include <Zone_VF.h>
#include <Param.h>
#include <Debog.h>

Implemente_instanciable_sans_constructeur(Loi_Etat_Multi_GP_WC,"Loi_Etat_Multi_Gaz_Parfait_WC",Loi_Etat_Multi_GP_base);
// XD multi_gaz_parfait_WC loi_etat_gaz_parfait_base multi_gaz_parfait_WC -1 Class for perfect gas multi-species mixtures state law used with a weakly-compressible fluid.

Loi_Etat_Multi_GP_WC::Loi_Etat_Multi_GP_WC() : num_espece_(-1) { }

Sortie& Loi_Etat_Multi_GP_WC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_Multi_GP_WC::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Species_number",&num_espece_,Param::REQUIRED); // XD_ADD_P int Number of species you are considering in your problem.
  param.ajouter("Diffusion_coeff",&diffusion_coeff_,Param::REQUIRED); // XD_ADD_P field_base Diffusion coefficient of each species, defined with a Champ_uniforme of dimension equals to the species_number.
  param.ajouter("Molar_mass",&molar_mass_,Param::REQUIRED); // XD_ADD_P field_base Molar mass of each species, defined with a Champ_uniforme of dimension equals to the species_number.
  param.ajouter("Mu",&mu_,Param::REQUIRED); // XD_ADD_P field_base Dynamic viscosity of each species, defined with a Champ_uniforme of dimension equals to the species_number.
  param.ajouter("Cp",&cp_,Param::REQUIRED);// XD_ADD_P double Specific heat at constant pressure of the gas Cp, defined with a Champ_uniforme of dimension equals to the species_number..
  param.ajouter("Prandtl",&Pr_,Param::REQUIRED); // XD_ADD_P double Prandtl number of the gas Pr=mu*Cp/lambda.
  param.lire_avec_accolades_depuis(is);

  // XXX
  if (diffusion_coeff_->valeurs().line_size() != num_espece_ ||
      molar_mass_->valeurs().line_size() != num_espece_ ||
      mu_->valeurs().line_size() != num_espece_  ||
      cp_->valeurs().line_size() != num_espece_ )
    {
      Cerr << "Error while reading the EOS Multi_Gaz_Parfait_WC : This is not allowed !" << finl;
      Cerr << "Verify that the dimension of the fields Diffusion_coeff, Molar_mass, Mu and Cp is = " << num_espece_ << finl;
      Process::exit();
    }

  return is;
}

void Loi_Etat_Multi_GP_WC::initialiser_inco_ch()
{
  /*
   * XXX : In this problem we solve n-1 equations for n species
   * The mass fraction of species n is [ 1 - (y1+y2+ ... +yn-1) ]
   */

  if (liste_Y.size() + 1  != num_espece_ )
    {
      Cerr << "Error in your data file : This is not allowed !" << finl;
      Cerr << "You should define " << num_espece_-1 << " Species equations and not " << liste_Y.size() << finl;
      Process::exit();
    }

  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  Champ_Don& Yn = FWC.fraction_massique_nonresolue();
  Yn.nommer("fraction_massique_nonresolue");
  double t = le_fluide->masse_volumique().temps(); // pas 0 car reprise pt etre
  update_Yn_values(Yn,t);
  Loi_Etat_Multi_GP_base::initialiser_inco_ch();
}

void Loi_Etat_Multi_GP_WC::update_Yn_values(Champ_Don& Yn, double temps)
{
  DoubleTab& tab_Yn = Yn.valeurs();
  Yn.mettre_a_jour(temps);
  tab_Yn = 1.0;// re-initialize as 1
  const int size = liste_Y(0)->valeurs().size();
  assert(tab_Yn.size() == size);
  assert (liste_Y.size() + 1  == num_espece_);

  for (int i=0; i<liste_Y.size(); i++)
    for (int n=0; n<size; n++) tab_Yn(n,0) -= liste_Y(i)->valeurs()(n,0);

  double min_Yn = local_min_vect(tab_Yn);
  if (min_Yn < 0)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "BIG PROBLEM : check your calculation because we are detecting negative mass fraction !" << finl;
      Cerr << "The results are saved to help you !" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
      Probleme_base& pb = FWC.get_problem();
      pb.postraiter(1);
      pb.sauver();
      Process::exit();
    }
}

// Description:
//    Calcule la masse molaire du melange (M)
//    M depend de la mase molaire de chaque espece et de la composition du melange (Yi)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_masse_molaire(DoubleTab& tab_masse_mol_mel) const
{
  assert (sub_type(Champ_Uniforme,masse_molaire_especes().valeur()));

  const int size = tab_masse_mol_mel.size();
  assert (liste_Y(0)->valeurs().size()==size);
  ArrOfDouble inv_M(size);

  const Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  const DoubleTab& Yn = FWC.fraction_massique_nonresolue().valeurs();

  for (int i=0; i < num_espece_; i++)
    {
      const DoubleTab& Y_i = (i == num_espece_ -1) ? Yn : liste_Y(i)->valeurs();
      const double M_i = masse_molaire_especes()->valeurs()(0,i);
      for (int elem=0; elem<size; elem++) inv_M(elem) += Y_i(elem,0)/M_i;
    }

  for (int elem=0; elem<size; elem++) tab_masse_mol_mel(elem,0) = 1.0 / inv_M(elem);
}

// Description:
//    Calcule le Cp du melange
//    Le Cp depend du Cp de chaque espece et de la composition du melange (Yi)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_tab_Cp(DoubleTab& tab_Cp) const
{
  // FIXME : Actuellement on suppose que Cp est pris constant pour chacune des especes
  assert (sub_type(Champ_Uniforme,cp_especes().valeur()));

  tab_Cp = 0;
  const int size =liste_Y(0)->valeurs().size();

  const Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  const DoubleTab& Yn = FWC.fraction_massique_nonresolue().valeurs();

  for (int i=0; i<num_espece_; i++)
    {
      // TODO : FIXME : a voir si Yn->valeurs() est a jour
      const DoubleTab& Y_i = (i == num_espece_ -1) ? Yn : liste_Y(i)->valeurs();
      const double cp_i = cp_especes()->valeurs()(0,i);
      for (int elem=0; elem<size; elem++) tab_Cp(elem,0) += Y_i(elem,0)*cp_i;
    }
}

// Description:
//    Recalcule la masse volumique
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  Champ_Don& Yn = FWC.fraction_massique_nonresolue();
  double temps = le_fluide->masse_volumique().temps();
  update_Yn_values(Yn,temps); // XXX : a voir si l'appel est dans le bon endroit ...
  Loi_Etat_Multi_GP_base::calculer_masse_molaire();

  assert (tab_rho.line_size() == 1);
  const int n = tab_rho.size();
  const DoubleTab& pres = FWC.pression_th_tab();

  for (int som=0 ; som<n ; som++)
    {
      double r = R_GAS / masse_mol_mel(som);
      tab_rho_np1(som) = calculer_masse_volumique(pres(som,0),tab_ICh(som,0),r);
      tab_rho(som,0) = 0.5 * ( tab_rho_n(som) + tab_rho_np1(som) );
    }

  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
  Debog::verifier("Loi_Etat_Multi_GP_WC::calculer_masse_volumique, tab_rho_np1",tab_rho_np1);
  Debog::verifier("Loi_Etat_Multi_GP_WC::calculer_masse_volumique, tab_rho",tab_rho);
}

double Loi_Etat_Multi_GP_WC::calculer_masse_volumique(double P, double T, double r) const
{
  return Loi_Etat_Multi_GP_base::calculer_masse_volumique(P,T,r);
}

double Loi_Etat_Multi_GP_WC::calculer_masse_volumique(double P, double T) const
{
  return Loi_Etat_Multi_GP_base::calculer_masse_volumique(P,T);
}

// Description:
//    Calcule la viscosite dynamique de reference (depend des Yi)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_mu_wilke()
{
  // With Wilke formulation : https://aip.scitation.org/doi/pdf/10.1063/1.1747673
  // See also for mass fractions : https://doi.org/10.1016/j.ijheatmasstransfer.2020.120470

  assert (liste_Y.size() + 1 == num_espece_);
  const int size = liste_Y(0)->valeurs().size();
  DoubleTab phi(size), mu(size);
  mu = 0.;
  phi = 0.;

  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  DoubleTab& Yn = FWC.fraction_massique_nonresolue().valeurs();

  for (int i=0; i < num_espece_ ; i++)
    {
      phi = 0.;
      const double M_i = masse_molaire_especes()->valeurs()(0,i);
      const double mu_i = visc_dynamique_especes()->valeurs()(0,i);

      for (int j=0; j < num_espece_ ; j++)
        if (j != i) // sinon phi_ii = 1
          {
            const double M_j = masse_molaire_especes()->valeurs()(0,j);
            const double mu_j = visc_dynamique_especes()->valeurs()(0,j);

            double a = 1. + sqrt( mu_i / mu_j ) * pow( M_j / M_i , 0.25);
            double b = sqrt( 8. * ( 1. + ( M_i / M_j )));
            double phi_ij = ( M_i / M_j ) * a * a / b;
            // TODO : FIXME : a voir si Yn->valeurs() est a jour
            const DoubleTab& y_j = (j == num_espece_ -1) ? Yn : liste_Y(j)->valeurs();
            // node is elem (VDF) or face (VEF)
            for (int node=0; node<y_j.size(); node++) phi(node) += y_j(node,0) * phi_ij;
          }
      // We add the mass fraction when i = j
      const DoubleTab& y_i = (i == num_espece_ -1) ? Yn : liste_Y(i)->valeurs();
      for (int node=0; node<y_i.size(); node++) mu(node) += mu_i * y_i(node,0) / ( y_i(node,0) + phi(node) );
    }

  calculer_tab_mu(mu, size);
}

// Description:
//    Calcule la viscosite dynamique sur Schmidt = rho * D
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_mu_sur_Sc()
{
  /*
   * ====================================================================
   * Schmidt number : Sc = nu / D = mu / ( rho * D )
   * => rho * D = mu/Sc
   * species equation : div ( rho * D * grad Y1) = div ( mu/Sc * grad Y1)
   * ====================================================================
   */

  Champ_Don& mu_sur_Sc = le_fluide->mu_sur_Schmidt();
  DoubleTab& tab_mu_sur_Sc = mu_sur_Sc.valeurs();
  const Champ_base& rho = le_fluide->masse_volumique();
  const DoubleTab& tab_rho = rho.valeurs();
  const int n = tab_mu_sur_Sc.size();

  // TODO : FIXME : On a tab_mu_sur_Sc.line_size() = 1 :( :/ :(
  // BUG : il faut avoir line_size = num_espece_ car on peut avoir D qui varie entre espèce

  if (!sub_type(Champ_Uniforme,mu_sur_Sc.valeur()))
    {
      if (sub_type(Champ_Uniforme,rho))
        {
          Cerr << "We should not have a density field of type Champ_Uniforme !" << finl;
          Process::exit();
        }
      else
        for (int i=0 ; i<n ; i++)
          {
            // TODO : FIXME : j'ai pris D de l'espèce 1 ...
            tab_mu_sur_Sc(i,0) = tab_rho(i,0) * coeff_diffusion_especes()->valeurs()(0,0);
          }
    }
  else
    {
      Cerr << "We should not have a mu_sur_Sc of type Champ_Uniforme !" << finl;
      Process::exit();
    }

  mu_sur_Sc.valeur().changer_temps(rho.temps());
  tab_mu_sur_Sc.echange_espace_virtuel();
}

// Description:
//    Calcule la viscosite dynamique sur Schmidt = D
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_WC::calculer_nu_sur_Sc()
{
  /*
   * ====================================================================
   * Schmidt number : Sc = nu / D
   * => D = nu/Sc
   * ====================================================================
   */

  Champ_Don& nu_sur_Sc = le_fluide->nu_sur_Schmidt();
  DoubleTab& tab_nu_sur_Sc = nu_sur_Sc.valeurs();
  const int n = tab_nu_sur_Sc.size();

  // TODO : FIXME : On a tab_nu_sur_Sc.line_size() = 1 :( :/ :(
  // BUG : il faut avoir line_size = num_espece_ car on peut avoir D qui varie entre espèce

  // TODO : FIXME : j'ai pris D de l'espèce 1 ...
  for (int i=0 ; i<n ; i++) tab_nu_sur_Sc(i,0) = coeff_diffusion_especes()->valeurs()(0,0);

  double temps_champ = le_fluide->masse_volumique().temps();
  nu_sur_Sc.valeur().changer_temps(temps_champ);
  tab_nu_sur_Sc.echange_espace_virtuel();
}
