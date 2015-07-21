/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Loi_Etat_GR_rhoT.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_GR_rhoT.h>
#include <Motcle.h>
#include <Fluide_Quasi_Compressible.h>
#include <Champ_Uniforme.h>

Implemente_instanciable_sans_constructeur(Loi_Etat_GR_rhoT,"Loi_Etat_Gaz_Reel_rhoT",Loi_Etat_base);

Loi_Etat_GR_rhoT::Loi_Etat_GR_rhoT()
{
  MMole_=-1;
  Cp_=-1;
  R = -1;
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
Sortie& Loi_Etat_GR_rhoT::printOn(Sortie& os) const
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
Entree& Loi_Etat_GR_rhoT::readOn(Entree& is)
{
  double PolyRho_lu=-1;
  double PolyT_lu=-1;
  double MMole_lu=-1;
  double Pr_lu=-1;

  Motcle accferme="}";
  Motcle accouverte="{";

  Motcle motlu;
  is >> motlu;
  Cerr<<"Lecture de la loi d'etat Gaz Reel rhoT"<<finl;
  if (motlu != accouverte)
    {
      Cerr<<" On attendait "<<accouverte<<" au lieu de "<<motlu<<finl;
      abort();
    }
  Motcles les_mots(4);
  {
    les_mots[0] = "Prandtl";
    les_mots[1] = "masse_molaire";
    les_mots[2] = "Poly_rho";
    les_mots[3] = "Poly_T";
    //     les_mots[4] = "Cp";   //pour debuggage
  }
  is >> motlu;
  while(motlu != accferme )
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is>>Pr_;
            Pr_lu=1;
            break;
          }
        case 1 :
          {
            is>>MMole_;
            MMole_lu=1;
            break;
          }
        case 2 :
          {
            int i,j,im,jm;
            is>>im;
            is>>jm;
            PolyRho_.resize(im,jm);
            for (i=0 ; i<im ; i++)
              for (j=0 ; j<jm ; j++)
                is>>PolyRho_(i,j);
            PolyRho_lu=1;
            break;
          }
        case 3 :
          {
            int i,j,im,jm;
            is>>im;
            is>>jm;
            PolyT_.resize(im,jm);
            for (i=0 ; i<im ; i++)
              for (j=0 ; j<jm ; j++)
                is>>PolyT_(i,j);
            PolyT_lu=1;
            break;
          }
        case 4 :
          {
            is>>Cp_;
            PolyRho_lu=1;
            PolyT_lu=1;
            MMole_lu=1;
            if (Cp_<0)
              {
                Cp_ = -Cp_;
                debug=1;
              }
            else
              debug=0;
            R = Cp_ *(1.-1./1.4);
            Cerr<<"Debogage Gaz Reel : Cp="<<Cp_<<" R="<<R<<finl;
            break;
          }
        default :
          {
            Cerr<<"Une loi d'etat "<<que_suis_je()<<" n'a pas la propriete "<<motlu<<finl;
            Cerr<<"On attendait un mot dans :"<<finl<<les_mots<<finl;
            abort();
          }
        }
      is >> motlu;
    }

  if (Pr_lu==-1)
    {
      Cerr<<"ERREUR : on attendait la definition du nombre de Prandtl (Prandtl pr)"<<finl;
      abort();
    }
  if (PolyRho_lu==-1)
    {
      Cerr<<"ERREUR : on attendait la definition du polynome de la masse volumique (Poly_Rho nb_coeff1 nb_coeff2 a00 a01 a02 ...)"<<finl;
      abort();
    }
  else
    {
      int i,j;
      Cerr<<"Polynome de rho :"<<finl;
      for (i=0 ; i<PolyRho_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyRho_.dimension(1) ; j++)
            {
              Cerr<<"   f("<<i<<","<<j<<")= "<<PolyRho_(i,j);
            }
          Cerr<<finl;
        }
    }
  if (PolyT_lu==-1)
    {
      Cerr<<"ERREUR : on attendait la definition du polynome de la temperature (Poly_T nb_coeff1 nb_coeff2 a00 a01 a02 ...)"<<finl;
      abort();
    }
  else
    {
      int i,j;
      Cerr<<"Polynome de T :"<<finl;
      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyT_.dimension(1) ; j++)
            {
              Cerr<<"   f("<<i<<","<<j<<")= "<<PolyT_(i,j);
            }
          Cerr<<finl;
        }
    }
  if (MMole_lu==-1)
    {
      Cerr<<"ERREUR : on attendait la definition de la masse molaire (masse_molaire m)"<<finl;
      abort();
    }
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
const Nom Loi_Etat_GR_rhoT::type_fluide() const
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
void Loi_Etat_GR_rhoT::initialiser()
{
  le_fluide->inco_chaleur()->nommer("enthalpie");

  const DoubleTab& tab_H = le_fluide->inco_chaleur().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int i, n = tab_H.dimension(0);
  DoubleTab& tab_T = temperature_.valeurs();
  //tab_T.resize(n);
  tab_TempC.resize(n);
  double Pth = le_fluide->pression_th();
  for (i=0 ; i<n ; i++)
    {
      tab_rho_n[i] = tab_rho[i];
      tab_TempC[i] = calculer_temperature(Pth,tab_H[i]);
      tab_T[i] = tab_TempC[i];
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
void Loi_Etat_GR_rhoT::initialiser_inco_ch()
{
  //l'inconnue inco_chaleur est d'abord remplie avec la temperature initiale.
  // il faut la transfomer en enthalpie ;
  //les donnes sont la masse volumique et la temperature
  // on doit donc calculer l'enthalpie et la pression
  DoubleTab& tab_TH = le_fluide->inco_chaleur().valeurs();
  double Pth = le_fluide->pression_th();
  int som,n = tab_TH.dimension(0);
  tab_rho_n.resize(n);
  tab_rho_np1.resize(n);
  //  tab_rho_nm1.resize(n);

  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  if (le_fluide->inco_chaleur()->le_nom() == "enthalpie")
    {
      for (som=0 ; som<n ; som++)
        {
          tab_rho_np1[som] = tab_rho[som] = tab_rho_n[som] = calculer_masse_volumique(Pth,tab_TH[som]);
        }
    }
  else
    {
      for (som=0 ; som<n ; som++)
        {
          tab_TH[som] = calculer_H(Pth,tab_TH[som]);
          tab_rho_np1[som] = tab_rho[som] = tab_rho_n[som] = calculer_masse_volumique(Pth,tab_TH[som]);
        }
    }

  Cerr<<"FINLoi_Etat_GR_rhoT::initialiser_H Pth="<<Pth<<"  H="<<tab_TH[0]<<finl;
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
void Loi_Etat_GR_rhoT::remplir_T()
{
  const DoubleTab& tab_H = le_fluide->inco_chaleur().valeurs();
  int i, n = tab_TempC.dimension(0);
  DoubleTab& tab_T = temperature_.valeurs();
  double Pth = le_fluide->pression_th();
  for (i=0 ; i<n ; i++)
    {
      tab_TempC[i] = calculer_temperature(Pth,tab_H[i]);
      tab_T[i] = tab_TempC[i];
    }
  //   Cerr<<"---Loi_Etat_GR::remplir_T h present="<<le_fluide->inco_chaleur().valeurs()(0)<<" passe="<<le_fluide->inco_chaleur().passe()(0)<<" futur="<<le_fluide->inco_chaleur().futur()(0)<<"  local T="<<tab_T(0)<<finl;
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
double Loi_Etat_GR_rhoT::Cp_calc(double P, double h) const
{
  double res;
  if (R==-1)
    res = 1./(DT_DH(P,h));
  else
    res = Cp_;
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
void Loi_Etat_GR_rhoT::calculer_Cp()
{
  double Pth = le_fluide->pression_th();
  const DoubleTab& tab_h = le_fluide->inco_chaleur().valeurs();

  for (int i=0; i<tab_Cp.size(); i++)
    tab_Cp(i) = Cp_calc(Pth,tab_h[i]);
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
void Loi_Etat_GR_rhoT::calculer_lambda()
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
}

// Description:
//    Calcule la masse volumique ponctuelle
// Precondition:
// Parametre: double P
//    Signification: pression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Parametre: double h
//    Signification: enthalpie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour: double
//    Signification: masse volumique correspondante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Loi_Etat_GR_rhoT::calculer_masse_volumique(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      double H = h;
      int i,j;
      for (i=0 ; i<PolyRho_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyRho_.dimension(1) ; j++)
            {
              res += PolyRho_(i,j) *pow(P,j) *pow(H,i);
            }
        }
    }
  else
    {
      //debuggage
      res = P*Cp_/(R*h);
    }
  return res;
}

// Description:
//    Calcule la temperature ponctuelle
// Precondition:
// Parametre: double P
//    Signification: pression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Parametre: double h
//    Signification: enthalpie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour: double
//    Signification: masse volumique correspondante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Loi_Etat_GR_rhoT::calculer_temperature(double P, double h)
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyT_.dimension(1) ; j++)
            {
              res += PolyT_(i,j) *pow(P,i) *pow(H,j);
            }
        }
    }
  else
    {
      //debuggage
      res = h/Cp_;
    }
  return res;
}

// Description:
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
double Loi_Etat_GR_rhoT::calculer_H(double Pth_, double T_) const
{
  double res=0;
  if (R==-1)
    {
      //il faut resoudre c0-T_ + c1.H + c2.H^2 +...=0
      int i,j;
      int it,max_iter = 1000;
      double eps = 1.e-6;
      DoubleVect coef(PolyT_.dimension(1));
      coef = 0;

      for (j=0 ; j<PolyT_.dimension(1) ; j++)
        {
          for (i=0 ; i<PolyT_.dimension(0) ; i++)
            {
              coef(j) += PolyT_(i,j) * pow(Pth_,i);
            }
        }
      coef(0) -= T_;

      double H = -coef(0)/coef(1);
      double dH;
      double num=coef(0), den=0;
      for (j=1 ; j<PolyT_.dimension(1) ; j++)
        {
          num += coef(j) * pow(H,j);
          den += j*coef(j) * pow(H,j-1);
        }
      dH = num/den;
      H -= dH;

      it=0;
      while (dH/H>eps && it<max_iter)
        {
          num=coef(0);
          den=0;
          for (j=1 ; j<PolyT_.dimension(1) ; j++)
            {
              num += coef(j) * pow(H,j);
              den += j*coef(j) * pow(H,j-1);
            }
          dH = num/den;
          H -= dH;
          it++;
        }

      if (dH/H>eps)
        {
          Cerr<<"PB Loi_Etat_GR_rhoT::calculer_H: resolution Newton impossible "<<finl;
          Cerr<<"Pth= "<<Pth_<<" t="<<T_<<" H="<<H<<finl;
          abort();
        }

      res = H;

    }
  else
    {
      //debuggage
      res = Cp_*T_;
    }
  return res;
}

double Loi_Etat_GR_rhoT::Drho_DP(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H =h;
      for (i=1 ; i<PolyRho_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyRho_.dimension(1) ; j++)
            {
              res += i* PolyRho_(i,j) *pow(P,i-1) *pow(H,j);
            }
        }
    }
  else
    {
      //debuggage
      res = 1/(R*h/Cp_);
    }
  return res;
}


//correspond a Drho_DH en fait
double Loi_Etat_GR_rhoT::Drho_DT(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=1 ; i<PolyRho_.dimension(0) ; i++)
        {
          for (j=0 ; j<PolyRho_.dimension(1) ; j++)
            {
              res += i* PolyRho_(i,j) *pow(P,j) *pow(H,i-1);
            }
        }
    }
  else
    {
      //debuggage
      res = -P*Cp_/(R*h*h);
    }
  return res;
}
double Loi_Etat_GR_rhoT::De_DP(double P, double T) const
{
  double res = 0;
  Cerr<<"Loi_Etat_GR_rhoT::De_DP non accede normalement"<<finl;
  abort();
  return res;
}
double Loi_Etat_GR_rhoT::De_DT(double P, double T) const
{
  double res = 0;
  Cerr<<"Loi_Etat_GR_rhoT::De_DP non accede normalement"<<finl;
  abort();
  return res;
}
double Loi_Etat_GR_rhoT::DT_DH(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        {
          for (j=1 ; j<PolyT_.dimension(1) ; j++)
            {
              res += j* PolyT_(i,j) *pow(P,i) *pow(H,j-1);
            }
        }
    }
  else
    {
      //debuggage
      res = Cp_-R;
    }
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
double Loi_Etat_GR_rhoT::inverser_Pth(double H, double rho)
{
  double P = le_fluide->pression_th();

  double acc = (calculer_masse_volumique(P,H) - rho) / Drho_DP(P,H);
  int i=0;
  while (fabs(acc)>1e-8 && i<1000)
    {
      P = P-acc;
      acc = (calculer_masse_volumique(P,H) - rho) / Drho_DP(P,H);
      i++;
    }
  if (fabs(acc)>1e-8)
    {
      Cerr<<"Probleme dans l'inversion de la pression : nb_iter="<<i<<finl;
      Cerr<<" Pth="<<P<<" H="<<H<<" rho="<<rho<<finl;
      abort();
    }
  return P;
}
void Loi_Etat_GR_rhoT::calculer_masse_volumique()
{
  Loi_Etat_base::calculer_masse_volumique();
}
