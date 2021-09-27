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
// File:        Loi_Etat_GP_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Loi_Etat
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_GP_base.h>
#include <Motcle.h>
#include <Fluide_Dilatable_base.h>
#include <Champ_Uniforme.h>
#include <Zone_VF.h>
#include <Champ_Fonc_Tabule.h>

Implemente_base_sans_constructeur(Loi_Etat_GP_base,"Loi_Etat_Gaz_Parfait_base",Loi_Etat_base);

Loi_Etat_GP_base::Loi_Etat_GP_base() : Cp_(-1), R_(-1) { }

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
Sortie& Loi_Etat_GP_base::printOn(Sortie& os) const
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
Entree& Loi_Etat_GP_base::readOn(Entree& is)
{
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
const Nom Loi_Etat_GP_base::type_fluide() const
{
  return "Gaz_Parfait";
}

// Description:
//    Associe le fluide a la loi d'etat
// Precondition:
// Parametre: Fluide_Dilatable_base& fl
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
void Loi_Etat_GP_base::associer_fluide(const Fluide_Dilatable_base& fl)
{
  Loi_Etat_base::associer_fluide(fl);
  le_fluide->set_Cp(Cp_);
}

// Description:
//    Initialise la loi d'etat : calcul Pth
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
void Loi_Etat_GP_base::initialiser()
{
  const DoubleTab& tab_Temp = le_fluide->inco_chaleur().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int i, ntot=tab_Temp.size_totale();
  DoubleTab& tab_T = temperature_.valeurs();
  assert(tab_T.size()==tab_Temp.dimension(0));

  tab_rho_n=tab_Temp;
  for (i=0 ; i<ntot ; i++)
    {
      tab_rho_n(i) = tab_rho(i,0);
      tab_T(i,0) = tab_Temp(i,0);
    }
}

// Description:
//    Remplit le tableau de la temperature : T=temp+273.15
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
void Loi_Etat_GP_base::remplir_T()
{
  const DoubleTab& tab_Temp = le_fluide->inco_chaleur().valeurs();
  int i, ntot=tab_Temp.size_totale();
  DoubleTab& tab_T = temperature_.valeurs();
  for (i=0 ; i<ntot ; i++) tab_T(i,0) = tab_Temp(i,0);
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
void Loi_Etat_GP_base::calculer_Cp()
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
void Loi_Etat_GP_base::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda =lambda.valeurs();

  int i, n=tab_lambda.size();

  //La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (sub_type(Champ_Fonc_Tabule,lambda.valeur()))
    {
      lambda.valeur().mettre_a_jour(temperature_.valeur().temps());
      return;
    }
  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = mu0 * Cp_ / Pr_;
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(i,0) * Cp_ / Pr_;
        }
    }
  else
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          tab_lambda(0,0) = mu(0,0) * Cp_ / Pr_;
        }
      else
        {
          Cerr << finl ;
          Cerr << "If lambda is of type Champ_Uniform, mu must also be of type Champ_Uniforme !"<< finl;
          Cerr << "If needed, you can use a Champ_Fonc_Fonction or a Champ_Fonc_Tabule with a constant for mu."<< finl ;
          Cerr << finl ;
          Process::exit();
        }
    }
  tab_lambda.echange_espace_virtuel();
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
void Loi_Etat_GP_base::calculer_alpha()
{
  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  Champ_Don& alpha=le_fluide->diffusivite();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  int isVDF=0;
  if (alpha.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;
  int i, n=tab_alpha.size();

  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++) tab_alpha(i,0) = lambda0 / (tab_rho(i,0) * Cp_);
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_alpha(i,0) = tab_lambda(i,0) / (tab_rho(i,0) * Cp_);
        }
    }
  else
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      double rhoelem;
      int face, nfe = elem_faces.line_size();
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem += tab_rho(elem_faces(i,face),0);
              rhoelem /= nfe;
              tab_alpha(i,0) = lambda0 / ( rhoelem * Cp_ );
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem += tab_rho(elem_faces(i,face),0);
              rhoelem /= nfe;
              tab_alpha(i,0) = tab_lambda(i,0) / ( rhoelem * Cp_ );
            }
        }
    }
  tab_alpha.echange_espace_virtuel();
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
double Loi_Etat_GP_base::calculer_masse_volumique(double P, double T) const
{
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Error, we find a temperature of " << T << " !" << finl;
      Cerr << "Either your calculation has diverged or you don't define" << finl;
      Cerr << "temperature in Kelvin somewhere in your data file." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }
  return P / ( R_ * T );
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
double Loi_Etat_GP_base::inverser_Pth(double T, double rho)
{
  return rho * R_ * T;
}

// Description:
//    Calcule la masse volumique
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
void Loi_Etat_GP_base::calculer_masse_volumique()
{
  Loi_Etat_base::calculer_masse_volumique();
}
