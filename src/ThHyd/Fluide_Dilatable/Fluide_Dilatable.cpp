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
// File:        Fluide_Dilatable.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Dilatable.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_dis.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc_Tabule.h>
#include <EChaine.h>
#include <Discretisation_base.h>
#include <Loi_Etat_Melange_GP.h>
#include <SFichier.h>
#include <Param.h>
#include <Champ_Fonc_Fonction.h>
#include <Neumann_sortie_libre.h>
#include <Zone_VF.h>

Implemente_base_sans_constructeur(Fluide_Dilatable,"Fluide_Dilatable",Fluide_base);

Fluide_Dilatable::Fluide_Dilatable():traitement_PTh(0),Pth_(-1.),Pth_n(-1.),Pth1(-1.) {}

// Description:
//    Ecrit les proprietes du fluide sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Fluide_Dilatable::printOn(Sortie& os) const
{
  Fluide_base::ecrire(os);
  return os;
}

// Description:
//   Lit les caracteristiques du fluide a partir d'un flot
//   d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Fluide_Dilatable::readOn(Entree& is)
{
  Fluide_base::readOn(is);
  return is;
}

void Fluide_Dilatable::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Fluide_base::discretiser(pb,dis);
}

void Fluide_Dilatable::set_param(Param& param)
{
  Fluide_base::set_param(param);
}

int Fluide_Dilatable::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  return Fluide_base::lire_motcle_non_standard(mot,is);
}

// Description:
//    Verifie que les champs lus l'ont ete correctement.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: l'une des proprietes (rho mu Cp ou lambda) du fluide n'a pas ete definie
// Effets de bord:
// Postcondition:
void Fluide_Dilatable::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (rho.non_nul()) { }
  else
    {
      msg += "The density rho has not been specified. \n";
      err = 1;
    }
  if (mu.non_nul())
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          if (mu(0,0) <= 0)
            {
              msg += "The dynamical viscosity mu is not striclty positive. \n";
              err = 1;
            }
        }
    }
  else
    {
      msg += "The dynamical viscosity mu has not been specified. \n";
      err = 1;
    }
  if (lambda.non_nul()) { }
  else
    {
      msg += "The conductivity lambda has not been specified. \n";
      err = 1;
    }
  if (Cp.non_nul()) { }
  else
    {
      msg += "The heat capacity Cp has not been specified. \n";
      err = 1;
    }

  Milieu_base::verifier_coherence_champs(err,msg);
}

// Description:
//    Complete le fluide avec un Cp constant
// Precondition:
// Parametre: double Cp
//    Signification: le cp du fluide
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Dilatable::set_Cp(double Cp_)
{
  Cp.typer("Champ_Uniforme");
  Champ_Uniforme& ch_Cp = ref_cast(Champ_Uniforme,Cp.valeur());
  ch_Cp.dimensionner(1,1);
  DoubleTab& tab_Cp = Cp.valeurs();
  tab_Cp(0,0) = Cp_;
}

void Fluide_Dilatable::update_rho_cp(double temps)
{
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  DoubleTab& rho_cp = rho_cp_comme_T_.valeurs();
  if (sub_type(Champ_Uniforme,rho))
    rho_cp = rho.valeurs()(0, 0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp = 1.;
      tab_multiply_any_shape(rho_cp, rho.valeurs());
    }
  if (sub_type(Champ_Uniforme, Cp.valeur()))
    rho_cp *= Cp.valeurs()(0, 0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
}

// Description:
//    Renvoie le tableau des valeurs de le temperature
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
const DoubleTab& Fluide_Dilatable::temperature() const
{
  return ch_temperature().valeurs();
}

// Description:
//    Renvoie le champ de le temperature
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
const Champ_Don& Fluide_Dilatable::ch_temperature() const
{
  return loi_etat_->ch_temperature();
}

Champ_Don& Fluide_Dilatable::ch_temperature()
{
  return loi_etat_->ch_temperature();
}

// Description:
//    Prepare le pas de temps
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
void Fluide_Dilatable::preparer_pas_temps()
{
  loi_etat_->mettre_a_jour(0);
}

void Fluide_Dilatable::abortTimeStep()
{
  loi_etat()->abortTimeStep();
  Pth_=Pth_n;
}

void Fluide_Dilatable::creer_champs_non_lus()
{
  // on s'occupe de lamda si mu uniforme et CP uniforme
  // on type lambda en champ uniforme et on met lambda=mu*Cp/Pr
  //
  if (mu.non_nul())
    {
      if (!(lambda.non_nul())||(!sub_type(Champ_Fonc_Tabule,lambda.valeur())))
        if ((sub_type(Champ_Uniforme,mu.valeur()))&&(sub_type(Loi_Etat_GP,loi_etat_.valeur())))
          {
            if (!sub_type(Loi_Etat_Melange_GP,loi_etat_.valeur()))
              {
                // Si mu uniforme et si la loi d'etat est celle d'un gaz parfait
                double lold=-1;
                if (lambda.non_nul())
                  lold=lambda.valeurs()(0,0);
                lambda.typer(mu.valeur().le_type());
                lambda=mu;

                loi_etat_->calculer_lambda();
                double lo=lambda.valeurs()(0,0);
                if (lold!=-1)
                  {
                    if (!est_egal(lold,lo))
                      {
                        Cerr << "Error : mu, lambda, Cp and Prandtl are all specified in your data set." << finl;
                        Cerr << "It is observed that your imput values leads to :" << finl;
                        Cerr.precision(20);
                        Cerr << "mu/(lambda*Cp)=" << loi_etat_->Prandt()*(lo/lold) << " and Prandtl=" << loi_etat_->Prandt() << finl;
                        Cerr<<"Please modify your data set by specifying for instance Prandtl = "<<loi_etat_->Prandt()*(lo/lold)<<finl;
                        Process::exit();
                      }
                  }
              }
          }

      if (lambda.non_nul())
        {
          if (sub_type(Sutherland,lambda.valeur()))
            {
              if (!sub_type(Champ_Uniforme,Cp.valeur()))
                {
                  Cerr << "A sutherland law cannot be requested for the conductivity "<<finl;
                  Cerr << "by indicating Slambda if the heat capacity (Cp) is not uniform."<<finl;
                  Process::exit();
                }
              Sutherland& mu_suth = ref_cast(Sutherland,mu.valeur());
              const double& mu0 = mu_suth.get_A();
              Sutherland& lambda_suth = ref_cast(Sutherland,lambda.valeur());
              double lambda0 = mu0/loi_etat_->Prandt()*Cp(0,0);
              lambda_suth.set_A(lambda0);
              lambda_suth.lire_expression();
            }
        }
    }
}

// Description:
//    Initialise les parametres du fluide.
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
// Postcondition: les parametres du fluide sont initialises
int Fluide_Dilatable::initialiser(const double& temps)
{
  Cerr << "Fluide_Dilatable::initialiser()" << finl;
  if (sub_type(Champ_Don_base, rho)) ref_cast(Champ_Don_base, rho).initialiser(temps);
  mu.initialiser(temps);
  lambda.initialiser(temps);
  Cp.initialiser(temps);
  update_rho_cp(temps);

  // Initialisation des proprietes radiatives du fluide incompressible
  // (Pour un fluide incompressible semi transparent).

  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    {
      coeff_absorption_.initialiser(temps);
      indice_refraction_.initialiser(temps);

      // Initialisation de longueur_rayo
      longueur_rayo_.initialiser(temps);
      if (sub_type(Champ_Uniforme,kappa().valeur()))
        {
          longueur_rayo()->valeurs()(0,0)=1/(3*kappa()(0,0));
        }
      else
        {
          DoubleTab& l_rayo = longueur_rayo_.valeurs();
          const DoubleTab& K = kappa().valeurs();
          for (int i=0; i<kappa().nb_valeurs_nodales(); i++)
            l_rayo[i] = 1/(3*K[i]);
        }
    }
  return 1;
}

// Description:
//    Calcule la pression totale : pression thermodynamique + pression hydrodynamique
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
void Fluide_Dilatable::calculer_pression_tot()
{
  DoubleTab& tab_Ptot = pression_tot_.valeurs();
  int n = tab_Ptot.dimension_tot(0);
  DoubleTab tab_PHyd(n, 1);
  if( n != pression_->valeurs().dimension_tot(0) )
    {
      // Interpolation de pression_ aux elements (ex: P1P0)
      const Zone_dis_base& zone_dis= pression_-> zone_dis_base();
      const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
      const DoubleTab& centres_de_gravites=zone.xp();
      pression_->valeur().valeur_aux(centres_de_gravites,tab_PHyd);
    }
  else
    tab_PHyd = pression_->valeurs();

  for (int i=0 ; i<n ; i++)
    {
      tab_Ptot(i) = tab_PHyd(i) + Pth_;
    }
}

const Champ_base& Fluide_Dilatable::get_champ(const Motcle& nom) const
{
  REF(Champ_base) ref_champ;
  try
    {
      return Fluide_base::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  try
    {
      return loi_etat_->get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  throw Champs_compris_erreur();
}

void Fluide_Dilatable::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Fluide_base::get_noms_champs_postraitables(nom,opt);
  loi_etat_->get_noms_champs_postraitables(nom,opt);
}

// Description:
//    A surcharger dans les classes filles
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Dilatable::mettre_a_jour(double temps)
{
  rho.mettre_a_jour(temps);
  ch_temperature().mettre_a_jour(temps); // Note : it denotes the species Y1 for Pb_Hydraulique_Melange_Binaire_QC
  rho->changer_temps(temps);
  ch_temperature()->changer_temps(temps);
  mu->changer_temps(temps);
  lambda->changer_temps(temps);
  Cp.mettre_a_jour(temps);
  update_rho_cp(temps);
  calculer_pression_tot();
  pression_tot_.mettre_a_jour(temps);

  write_mean_edo(temps); // si besoin (i.e. QC)
}

// Description:
//    Prepare le fluide au calcul.
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
void Fluide_Dilatable::preparer_calcul()
{
  Cerr << "Fluide_Dilatable::preparer_calcul()" << finl;
  Milieu_base::preparer_calcul();
  loi_etat_->preparer_calcul();
  prepare_pressure_edo(); // si besoin (i.e. QC)
  calculer_coeff_T();
  calculer_pression_tot();
  pression_tot_.mettre_a_jour(0);
}
