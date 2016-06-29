/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Loi_Etat_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_base.h>
#include <Champ_Uniforme.h>
#include <Fluide_Quasi_Compressible.h>
#include <Champ_Fonc_Tabule.h>
#include <Zone_VF.h>
#include <Debog.h>

Implemente_base_sans_constructeur(Loi_Etat_base,"Loi_Etat_base",Objet_U);

Loi_Etat_base::Loi_Etat_base()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="temperature";
  */
  Pr_=-1.;
  debug=-1;
}

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
Sortie& Loi_Etat_base::printOn(Sortie& os) const
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
Entree& Loi_Etat_base::readOn(Entree& is)
{
  return is;
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
void Loi_Etat_base::associer_fluide(const Fluide_Quasi_Compressible& fl)
{
  le_fluide = fl;
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
const Champ_Don& Loi_Etat_base::ch_temperature() const
{
  return temperature_;
}
Champ_Don& Loi_Etat_base::ch_temperature()
{
  return temperature_;
}

// Description:
//    Initialise l'inconnue de l'equation de chaleur : ne fai rien
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
void Loi_Etat_base::initialiser_inco_ch()
{
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  tab_rho_n=tab_rho;
  tab_rho_np1=tab_rho;
  calculer_masse_volumique();
  mettre_a_jour(0.);
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();

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
void Loi_Etat_base::preparer_calcul()
{
  remplir_T();

  calculer_masse_volumique();
  mettre_a_jour(0.);
}

// Description:
//    Met a jour la loi d'etat
// Precondition:
// Parametre: double temps
//    Signification: le temps de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_base::mettre_a_jour(double temps)
{
  //remplissage de rho avec rho(n+1)
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int i, n=tab_rho.size_totale();
  for (i=0 ; i<n ; i++)
    {
      // tab_rho_nm1[i]=tab_rho_n[i];
      tab_rho_n[i] = tab_rho_np1[i];
      tab_rho[i] = tab_rho_np1[i];
    }
}

// Description:
//    Calcule la viscosite
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
void Loi_Etat_base::calculer_mu()
{
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu.valeur()))
    {
      if (sub_type(Champ_Fonc_Tabule,mu.valeur()))
        mu.mettre_a_jour(temperature_.valeur().temps());
      else
        {
          Cerr<<"The viscosity field mu of type "<<mu.valeur().que_suis_je()<<" is not recognized.";
          exit();
        }
    }
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
void Loi_Etat_base::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();

  int i, n=tab_lambda.size();
  //La conductivite est soit un champ uniforme soit calculee a partir de la vis  //cosite dynamique et du Pr

  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {

      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)
            {
              tab_lambda[i] = mu0 * tab_Cp[i] / Pr_;
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              tab_lambda[i] = tab_mu[i] * tab_Cp[i] / Pr_;
            }
        }

    }
  tab_lambda.echange_espace_virtuel();
}

// Description:
//    Calcule la viscosite cinematique
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
void Loi_Etat_base::calculer_nu()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& nu= le_fluide->viscosite_cinematique ();
  DoubleTab& tab_nu = nu.valeurs();
  //Cerr<<le_fluide->viscosite_cinematique().valeur().que_suis_je()<<finl;
  int isVDF=0;
  if (nu.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF=1;
  int i, n=tab_nu.size();
  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)
            {
              tab_nu[i] = mu0 / tab_rho[i];
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              tab_nu[i] = tab_mu[i] / tab_rho[i];
            }
        }
    }
  else
    {
      //const IntTab& elem_faces=ref_cast(Zone_VF,ref_cast(Champ_Fonc_P0_VEF,nu.valeur()).zone_dis_base()).elem_faces();
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      double rhoelem;
      int nfe=elem_faces.dimension(1),face;
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem+=tab_rho(elem_faces(i,face));
              rhoelem/=nfe;
              tab_nu[i] = mu0 /rhoelem;
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem+=tab_rho(elem_faces(i,face));
              rhoelem/=nfe;
              tab_nu[i] = tab_mu[i] /rhoelem;
            }
        }

    }
  tab_nu.echange_espace_virtuel();
  Debog::verifier("Loi_Etat_base::calculer_nu tab_nu",tab_nu);
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
void Loi_Etat_base::calculer_alpha()
{
  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();

  int i, n=tab_alpha.size();
  if (sub_type(Champ_Uniforme,lambda.valeur()))
    {
      double lambda0 = tab_lambda(0,0);
      for (i=0 ; i<n ; i++)
        {
          tab_alpha[i] = lambda0 / (tab_rho[i]*tab_Cp[i]);
        }
    }
  else
    {
      for (i=0 ; i<n ; i++)
        {
          tab_alpha[i] = tab_lambda(i) / (tab_rho[i]*tab_Cp[i]);
        }
    }
  tab_alpha.echange_espace_virtuel();
  Debog::verifier("Loi_Etat_base::calculer_alpha alpha",tab_alpha);
}


// Description:
// Ne fait rien
// Surcharge dans Loi_Etat_Melange_GP
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
void Loi_Etat_base::calculer_mu_sur_Sc()
{

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
void Loi_Etat_base::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  double Pth = le_fluide->pression_th();
  int n=tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      tab_rho_np1[som] = calculer_masse_volumique(Pth,tab_ICh[som]);
      //Correction pour calculer la masse volumique a partir de la pression
      //qui permet de conserver la masse
      //tab_rho_np1[som] = calculer_masse_volumique(Pth1,tab_ICh[som]);
      //rho(n+1/2)
      tab_rho[som] = (tab_rho_n[som] + tab_rho_np1[som])/2.;
    }
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
}

// Description:
//    Cas gaz parfait : ne fait rien
//    Cas gaz Reel : doit recalculer l'enthalpie a partir de la pression et la temperature
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
double Loi_Etat_base::calculer_H(double Pth_, double T_) const
{
  return T_;
}

double Loi_Etat_base::Drho_DP(double,double) const
{
  Cerr<<"Drho_DP doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::Drho_DT(double,double) const
{
  Cerr<<"Drho_DT doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::De_DP(double,double) const
{
  Cerr<<"De_DP doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::De_DT(double,double) const
{
  Cerr<<"De_DT doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}


void Loi_Etat_base::creer_champ(const Motcle& motlu)
{
}

const Champ_base& Loi_Etat_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}
void Loi_Etat_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<"Loi_Etat_base : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

}

void Loi_Etat_base::abortTimeStep()
{
  int i, n=tab_rho_n.size_totale();
  for (i=0 ; i<n ; i++)
    {
      tab_rho_np1[i] = tab_rho_n[i];
    }
}
