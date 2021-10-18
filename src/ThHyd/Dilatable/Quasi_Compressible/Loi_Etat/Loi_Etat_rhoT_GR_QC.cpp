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
// File:        Loi_Etat_rhoT_GR_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Loi_Etat
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_rhoT_GR_QC.h>
#include <Motcle.h>
#include <Fluide_Quasi_Compressible.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Loi_Etat_rhoT_GR_QC,"Loi_Etat_rhoT_Gaz_Reel_QC",Loi_Etat_GR_base);
// XD rhoT_gaz_reel_QC loi_etat_gaz_reel_base rhoT_gaz_reel_QC 0 Class for real gas state law used with a quasi-compressible fluid.
// XD attr bloc bloc_lecture bloc 0 Description.


Sortie& Loi_Etat_rhoT_GR_QC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_rhoT_GR_QC::readOn(Entree& is)
{
  double PolyRho_lu=-1, PolyT_lu=-1, MMole_lu=-1, Pr_lu=-1;

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
double Loi_Etat_rhoT_GR_QC::calculer_masse_volumique(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      double H = h;
      int i,j;
      for (i=0 ; i<PolyRho_.dimension(0) ; i++)
        for (j=0 ; j<PolyRho_.line_size() ; j++)
          res += PolyRho_(i,j) *pow(P,j) *pow(H,i);
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
double Loi_Etat_rhoT_GR_QC::calculer_temperature(double P, double h)
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        for (j=0 ; j<PolyT_.line_size() ; j++)
          res += PolyT_(i,j) *pow(P,i) *pow(H,j);
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
double Loi_Etat_rhoT_GR_QC::calculer_H(double Pth_, double T_) const
{
  double res=0;
  if (R==-1)
    {
      //il faut resoudre c0-T_ + c1.H + c2.H^2 +...=0
      int i,j, it,max_iter = 1000;
      double eps = 1.e-6;
      DoubleVect coef(PolyT_.line_size());
      coef = 0;

      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        for (j=0 ; j<PolyT_.line_size() ; j++)
          coef(j) += PolyT_(i,j) * pow(Pth_,i);

      coef(0) -= T_;
      double dH, H = -coef(0)/coef(1), num=coef(0), den=0;
      for (j=1 ; j<PolyT_.line_size() ; j++)
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
          for (j=1 ; j<PolyT_.line_size() ; j++)
            {
              num += coef(j) * pow(H,j);
              den += j*coef(j) * pow(H,j-1);
            }
          dH = num / den;
          H -= dH;
          it++;
        }

      if (dH/H>eps)
        {
          Cerr<<"PB Loi_Etat_rhoT_GR_QC::calculer_H: resolution Newton impossible "<<finl;
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

double Loi_Etat_rhoT_GR_QC::Drho_DP(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H =h;
      for (i=1 ; i<PolyRho_.dimension(0) ; i++)
        for (j=0 ; j<PolyRho_.line_size() ; j++)
          res += i* PolyRho_(i,j) *pow(P,i-1) *pow(H,j);
    }
  else
    {
      //debuggage
      res = 1/(R*h/Cp_);
    }
  return res;
}

//correspond a Drho_DH en fait
double Loi_Etat_rhoT_GR_QC::Drho_DT(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=1 ; i<PolyRho_.dimension(0) ; i++)
        for (j=0 ; j<PolyRho_.line_size() ; j++)
          res += i* PolyRho_(i,j) *pow(P,j) *pow(H,i-1);
    }
  else
    {
      //debuggage
      res = -P*Cp_/(R*h*h);
    }
  return res;
}

double Loi_Etat_rhoT_GR_QC::DT_DH(double P, double h) const
{
  double res = 0;
  if (R==-1)
    {
      int i,j;
      double H = h;
      for (i=0 ; i<PolyT_.dimension(0) ; i++)
        for (j=1 ; j<PolyT_.line_size() ; j++)
          res += j* PolyT_(i,j) *pow(P,i) *pow(H,j-1);
    }
  else
    {
      //debuggage
      res = Cp_-R;
    }
  return res;
}

void Loi_Etat_rhoT_GR_QC::calculer_masse_volumique()
{
  Loi_Etat_GR_base::calculer_masse_volumique();
}
