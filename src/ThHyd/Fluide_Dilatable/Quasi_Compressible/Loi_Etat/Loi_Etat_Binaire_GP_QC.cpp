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
// File:        Loi_Etat_Binaire_GP_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Loi_Etat
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Binaire_GP_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Debog.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Loi_Etat_Binaire_GP_QC,"Loi_Etat_Binaire_Gaz_Parfait_QC",Loi_Etat_Binaire_GP_base);
// XD melange_binaire loi_etat_base melange_binaire -1 Mixing of a binary mixture uder the iso-thermal and iso-bar assumptions.

Loi_Etat_Binaire_GP_QC::Loi_Etat_Binaire_GP_QC() :
  massmol1_(-1),massmol2_(-1),mu1_(-1),mu2_(-1),tempr_(-1),diff_coeff_(-1) { }

// Description:
//    Imprime la loi sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Loi_Etat_Binaire_GP_QC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture d'une loi sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition: l'objet est construit avec les parametres lus
Entree& Loi_Etat_Binaire_GP_QC::readOn(Entree& is)
{
  Cerr<<"Lecture de la loi d'etat Melange Binaire"<<finl;

  Param param(que_suis_je());
  param.ajouter("molar_mass1",&massmol1_,Param::REQUIRED); // XD_ADD_P double Molar mass of species 1 (in kg/mol).
  param.ajouter("molar_mass2",&massmol2_,Param::REQUIRED); // XD_ADD_P double Molar mass of species 2 (in kg/mol).
  param.ajouter("mu1",&mu1_,Param::REQUIRED); // XD_ADD_P double Dynamic viscosity of species 1 (in kg/m.s).
  param.ajouter("mu2",&mu2_,Param::REQUIRED); // XD_ADD_P double Dynamic viscosity of species 2 (in kg/m.s).
  param.ajouter("temperature",&tempr_,Param::REQUIRED); // XD_ADD_P double Temperature (in Kelvin) which will be constant during the simulation since this state law only works for iso-thermal conditions.
  param.ajouter("diffusion_coeff",&diff_coeff_,Param::REQUIRED); // XD_ADD_P double Diffusion coefficient assumed the same for both species (in m2/s).
  param.lire_avec_accolades_depuis(is);

  return is;
}

// Description:
//    Renvoie le type de fluide associe.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
const Nom Loi_Etat_Binaire_GP_QC::type_fluide() const
{
  return "Melange_Binaire";
}

// Description:
//    Associe le fluide a la loi d'etat
// Precondition:
// Parametre: Fluide_Quasi_Compressible& fl
//    Signification: le fluide associe
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Binaire_GP_QC::associer_fluide(const Fluide_Dilatable_base& fl)
{
  Loi_Etat_base::associer_fluide(fl);
}

// Description:
//    Calcule le Cp
//    NE FAIT RIEN : le Cp est constant
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
void Loi_Etat_Binaire_GP_QC::calculer_Cp()
{
  /* Do nothing */
}

// Description:
//    Calcule la conductivite
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
void Loi_Etat_Binaire_GP_QC::calculer_lambda()
{
  /* Do nothing */
}

// Description:
//    Calcule la viscosite dynamique de mélange (depend des fraction massique)
// Voir Wilke  https://aip.scitation.org/doi/pdf/10.1063/1.1747673
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
void Loi_Etat_Binaire_GP_QC::calculer_mu_wilke()
{
  const DoubleTab& tab_Y1 = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_mu = le_fluide->viscosite_dynamique().valeurs();

  int i, n=tab_mu.size();
  for (i=0 ; i<n ; i++)
    {
      double m1om2 = massmol1_/massmol2_;
      double m2om1 = 1./m1om2;
      double mu1omu2 = mu1_/mu2_;
      double mu2omu1 = 1./mu1omu2;

      // XXX : Which is better ?
      double a1 = 1. + sqrt(mu1omu2) * pow(mu2omu1,0.25);
      double a2 = 1. + sqrt(mu2omu1) * pow(mu1omu2,0.25);
      //double a1 = 1. + sqrt(mu1omu2) * exp(0.25*log(mu2omu1));
      //double a2 = 1. + sqrt(mu2omu1) * exp(0.25*log(mu1omu2));
      double b1 = sqrt(8.*(1.+m1om2));
      double b2 = sqrt(8.*(1.+m2om1));
      double phi_12 = m1om2*a1*a1/b1;
      double phi_21 = m2om1*a2*a2/b2;

      double y1 = tab_Y1(i,0);
      double y2 = 1. - y1; // All about binary mixture !

      tab_mu(i,0) = (mu1_*y1)/(y1+y2*phi_12) + (mu2_*y2)/(y2+y1*phi_21);
    }
  tab_mu.echange_espace_virtuel();
  Debog::verifier("calculer_mu_wilke",tab_mu);
}

// Description:
//    Calcule la viscosite dynamique
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
void Loi_Etat_Binaire_GP_QC::calculer_mu()
{
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu.valeur()))
    {
      if (sub_type(Champ_Fonc_Tabule,mu.valeur()))
        {
          Cerr << "We should not have a dynamic viscosity of type Champ_Fonc_Tabule !" << finl;
          Process::exit();
        }
      else
        calculer_mu_wilke();
    }
  else
    {
      Cerr << "We should not have a dynamic viscosity of type Champ_Uniforme !" << finl;
      Process::exit();
    }
}

// Description:
//    Calcule la diffusivite
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
void Loi_Etat_Binaire_GP_QC::calculer_alpha()
{
  /* Do nothing */
}

// Description:
//    Calcule la viscosite dynamique sur Schmidt
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
void Loi_Etat_Binaire_GP_QC::calculer_mu_sur_Sc()
{
  /*
   * ====================================================================
   * Schmidt number : Sc = nu / D = mu / ( rho * D )
   * => rho * D = mu/Sc
   * species equation : div ( rho * D * grad Y1) = div ( mu/Sc * grad Y1)
   * ====================================================================
   */

  Champ_Don& mu_sur_Sc = le_fluide->mu_sur_Schmidt();
  const Champ_base& rho = le_fluide->masse_volumique();
  DoubleTab& tab_mu_sur_Sc = mu_sur_Sc.valeurs();
  const DoubleTab& tab_rho = rho.valeurs();
  const int n=tab_mu_sur_Sc.size();

  if (!sub_type(Champ_Uniforme,mu_sur_Sc.valeur()))
    {
      if (sub_type(Champ_Uniforme,rho))
        {
          Cerr << "We should not have a density field of type Champ_Uniforme !" << finl;
          Process::exit();
        }
      else
        for (int i=0 ; i<n ; i++) tab_mu_sur_Sc(i,0) = tab_rho(i,0)*diff_coeff_;
    }
  else
    {
      Cerr << "We should not have a mu_sur_Sc of type Champ_Uniforme !" << finl;
      Process::exit();
    }

  double temps_champ = rho.temps();
  mu_sur_Sc.valeur().changer_temps(temps_champ);
  tab_mu_sur_Sc.echange_espace_virtuel();
}

// Description:
//    Calcule la viscosite dynamique sur Schmidt
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
void Loi_Etat_Binaire_GP_QC::calculer_nu_sur_Sc()
{
  /*
   * ====================================================================
   * Schmidt number : Sc = nu / D
   * => D = nu/Sc
   * ====================================================================
   */

  Champ_Don& nu_sur_Sc = le_fluide->nu_sur_Schmidt();
  DoubleTab& tab_nu_sur_Sc = nu_sur_Sc.valeurs();
  const int n=tab_nu_sur_Sc.size();

  for (int i=0 ; i<n ; i++) tab_nu_sur_Sc(i,0) = diff_coeff_;

  double temps_champ = le_fluide->masse_volumique().temps();
  nu_sur_Sc.valeur().changer_temps(temps_champ);
  tab_nu_sur_Sc.echange_espace_virtuel();
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
double Loi_Etat_Binaire_GP_QC::calculer_masse_volumique(double P, double Y1) const
{
  /*
   * Mixing species state equation
   * ===========================================================================
   * rho = ( P * M_mix ) / ( R * T)
   * with M_mix = 1 / (Y1/M1 + Y2/M2) = = M2 / (M2Y1/M1 + Y2)
   * But Y2 = 1 - Y1 => M_mix = M2 / (M2Y1/M1 + 1 -Y1) = M2 / (1 + Y1(M2/M1 -1))
   * ===========================================================================
   * See https://doi.org/10.1016/j.ijheatmasstransfer.2020.120470
  */
  const double PM2 = P*massmol2_;
  const double RT = R_GAS*tempr_;
  const double mix = 1.0+Y1*(massmol2_/massmol1_-1.0);
  const double rh = PM2/(RT*mix);

  return rh;
}

// Description:
//    Calcule la pression avec la temperature et la masse volumique
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
double Loi_Etat_Binaire_GP_QC::inverser_Pth(double Y1, double rho)
{
  /*
  // THIS IS OK, but we dont want to enter here !
  double RToM2 = R_GAS*tempr_/massmol2_;
  double mix = 1.0+Y1*(massmol2_/massmol1_-1.0);
  double p_t = rho * RToM2 * mix ;

  return p_t;
  */
  Cerr << "We should not enter in the method Loi_Etat_Binaire_GP_QC::inverser_Pth !" << finl;
  Cerr << "This means that you are trying to solve an ODE for pth which is forbidden for this EOS!" << finl;
  abort();
  throw;
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
void Loi_Etat_Binaire_GP_QC::calculer_masse_volumique()
{
  const DoubleTab& tab_Y1 = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  double Pth = le_fluide->pression_th();

  const int n = tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      tab_rho_np1(som) = calculer_masse_volumique(Pth,tab_Y1(som,0));
      tab_rho(som,0) = 0.5 * ( tab_rho_n(som) + tab_rho_np1(som) );
    }

  const Champ_base& rho_m=le_fluide->get_champ("rho_gaz");
  ref_cast_non_const(DoubleTab,rho_m.valeurs()) = tab_rho_np1;
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
  Debog::verifier("Loi_Etat_Melange_GP::calculer_masse_volumique, tab_rho_np1",tab_rho_np1);
  Debog::verifier("Loi_Etat_Melange_GP::calculer_masse_volumique, tab_rho",tab_rho);
}
