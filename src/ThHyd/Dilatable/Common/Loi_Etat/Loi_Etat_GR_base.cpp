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

#include <Fluide_Quasi_Compressible.h>
#include <Loi_Etat_GR_base.h>
#include <Champ_Uniforme.h>
#include <Motcle.h>

Implemente_base_sans_constructeur(Loi_Etat_GR_base,"Loi_Etat_Gaz_Reel_base",Loi_Etat_base);
// XD loi_etat_gaz_reel_base loi_etat_base loi_etat_gaz_reel_base -1 Basic class for real gases state laws used with a dilatable fluid.

Loi_Etat_GR_base::Loi_Etat_GR_base() : MMole_(-1),Cp_(-1),R(-1) { }

Sortie& Loi_Etat_GR_base::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_GR_base::readOn(Entree& is)
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
const Nom Loi_Etat_GR_base::type_fluide() const
{
  return "Gaz_Reel";
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
void Loi_Etat_GR_base::initialiser()
{
  le_fluide->inco_chaleur()->nommer("enthalpie");

  const DoubleTab& tab_H = le_fluide->inco_chaleur().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int i, n = tab_H.dimension(0);
  DoubleTab& tab_T = temperature_.valeurs();
  tab_TempC.resize(n);
  double Pth = le_fluide->pression_th();
  for (i=0 ; i<n ; i++)
    {
      tab_rho_n(i) = tab_rho(i,0);
      tab_TempC(i) = calculer_temperature(Pth,tab_H(i,0));
      tab_T(i) = tab_TempC(i);
    }
  tab_Cp.ref(le_fluide->capacite_calorifique().valeurs());
  calculer_Cp();
}

// Description:
//    Initialise l'enthalpie
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
void Loi_Etat_GR_base::initialiser_inco_ch()
{
  /*
   * XXX XXX XXX
   * l'inconnue inco_chaleur est d'abord remplie avec la temperature initiale.
   * il faut la transfomer en enthalpie ;
   * les donnes sont la masse volumique et la temperature
   * on doit donc calculer l'enthalpie et la pression
   */

  DoubleTab& tab_TH = le_fluide->inco_chaleur().valeurs();
  double Pth = le_fluide->pression_th();
  int som,n = tab_TH.dimension(0);
  tab_rho_n.resize(n);
  tab_rho_np1.resize(n);

  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  if (le_fluide->inco_chaleur()->le_nom() == "enthalpie")
    {
      for (som=0 ; som<n ; som++)
        tab_rho_np1(som) = tab_rho(som,0) = tab_rho_n(som) = calculer_masse_volumique(Pth,tab_TH(som,0));
    }
  else
    {
      for (som=0 ; som<n ; som++)
        {
          tab_TH(som,0) = calculer_H(Pth,tab_TH(som,0));
          tab_rho_np1(som) = tab_rho(som,0) = tab_rho_n(som) = calculer_masse_volumique(Pth,tab_TH(som,0));
        }
    }
  Cerr<<"FIN Loi_Etat_GR_base::initialiser_H Pth = "<<Pth<<"  H = "<<tab_TH(0,0)<<finl;
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
void Loi_Etat_GR_base::remplir_T()
{
  const DoubleTab& tab_H = le_fluide->inco_chaleur().valeurs();
  int i, n = tab_TempC.dimension(0);
  DoubleTab& tab_T = temperature_.valeurs();
  double Pth = le_fluide->pression_th();
  for (i=0 ; i<n ; i++)
    {
      tab_TempC(i) = calculer_temperature(Pth,tab_H(i,0));
      tab_T(i,0) = tab_TempC(i);
    }
}

// Description:
//    Calcule le Cp en fonction des grandeurs physiques P, T, rho
//    Cp = dh/dT = de/dT - 1/rho^2*drho/dT
// Precondition:
// Parametre: double Tmp
//    Signification: temperature
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Loi_Etat_GR_base::Cp_calc(double P, double h) const
{
  double res;
  if (R==-1) res = 1./(DT_DH(P,h));
  else res = Cp_;

  return res;
}

// Description:
//    Calcule le Cp avec le polynome PolyCp_
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
void Loi_Etat_GR_base::calculer_Cp()
{
  double Pth = le_fluide->pression_th();
  const DoubleTab& tab_h = le_fluide->inco_chaleur().valeurs();
  for (int i=0; i<tab_Cp.size(); i++) tab_Cp(i) = Cp_calc(Pth,tab_h(i,0));
}

// Description:
//    Calcule la conductivite /Cp : equivlent a k*dT/dh
//    pour utiliser l'enthalpie dans l'operatur de diffusion
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
void Loi_Etat_GR_base::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();

  int i, n=tab_lambda.size();
  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(0,0) * tab_Cp(i) / Pr_;
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(i,0) * tab_Cp(i) / Pr_;
        }
    }
}

double Loi_Etat_GR_base::De_DP(double P, double T) const
{
  double res = 0;
  Cerr<<"Loi_Etat_GR_base::De_DP non accede normalement"<<finl;
  abort();
  return res;
}
double Loi_Etat_GR_base::De_DT(double P, double T) const
{
  double res = 0;
  Cerr<<"Loi_Etat_GR_base::De_DP non accede normalement"<<finl;
  abort();
  return res;
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
double Loi_Etat_GR_base::inverser_Pth(double H, double rho)
{
  double P = le_fluide->pression_th();
  double acc = (calculer_masse_volumique(P,H) - rho) / Drho_DP(P,H);
  int i=0;
  while (std::fabs(acc)>1e-8 && i<1000)
    {
      P = P-acc;
      acc = (calculer_masse_volumique(P,H) - rho) / Drho_DP(P,H);
      i++;
    }
  if (std::fabs(acc)>1e-8)
    {
      Cerr<<"Probleme dans l'inversion de la pression : nb_iter="<<i<<finl;
      Cerr<<" Pth="<<P<<" H="<<H<<" rho="<<rho<<finl;
      abort();
    }
  return P;
}

void Loi_Etat_GR_base::calculer_masse_volumique()
{
  Loi_Etat_base::calculer_masse_volumique();
}
