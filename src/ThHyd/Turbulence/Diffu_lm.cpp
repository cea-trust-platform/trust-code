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
// File:        Diffu_lm.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Diffu_lm.h>
#include <Fluide_Incompressible.h>
#include <MuLambda_TBLE_base.h>

Implemente_instanciable(Diffu_lm,"Diffu_lm",Diffu_totale_hyd_base);


const double Diffu_lm::A_plus=25.;

Sortie& Diffu_lm::printOn(Sortie& os) const
{
  return os ;
}

Entree& Diffu_lm::readOn( Entree& is)
{
  return is ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Cette methode determine la diffusivite totale a en un point donne du maillage fin de Eq_couch_lim ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Si la composante du cisaillement a la paroi est dans le sens de l'ecoulement

double Diffu_lm::calculer_a_local(int ind)
{
  // OC : 02/2006 ; on pourrait peut etre remplacer ce "calculer_a_local" par un "calculer_a_global", ce qui eviterait de multiples appels, surement couteux, a cette methode.

  double yw, y_plus, a, Damp, S_norm ;
  double yw_N, y_plus_N, S_norm_N, Damp_N ;

  //S_norm est la norme de Sij sur le maillage fin
  //Damp est une fonction de damping
  //u_tau : vitesse de frottement

  int N;
  double kappa = -123;
  Eq_couch_lim& leq = eq_couch_lim.valeur();
  double visco_cin = mu_lambda->getNu(eq_couch_lim_T, ind);
  double visco_cin2 = mu_lambda->getNu(eq_couch_lim_T, ind+1);
  double visco_turb = 0.;
  int nb_comp = leq.get_N_comp();


  N = leq.get_N();

  yw = 0.5*(leq.get_y(ind+1)+leq.get_y(ind));
  y_plus = yw*leq.get_utau_old()/(0.5*(visco_cin+visco_cin2));
  Damp = 1-exp(pow(-y_plus/A_plus,3)); // Damp1
  //Damp = pow(1-exp(-y_plus/A_plus),2); // Damp2

  if(leq.get_nu_t_dyn() == 0)
    {
      kappa=getKappa();
    }
  else
    {
      double visco_cin_N = 0.5*(mu_lambda->getNu(eq_couch_lim_T, N-1)+mu_lambda->getNu(eq_couch_lim_T, N));
      yw_N = 0.5*(leq.get_y(N)+leq.get_y(N-1));
      y_plus_N = yw_N*leq.get_utau_old()/visco_cin_N;
      //Damp_N = 1-exp(pow(-y_plus_N/A_plus,3)); // Damp1
      Damp_N = pow(1-exp(-y_plus_N/A_plus),2); // Damp2

      S_norm_N = 0.;
      double tmp=0.;
      for (int icomp=0; icomp<nb_comp; icomp++)
        {
          tmp = (leq.get_Unp1(icomp,N) - leq.get_Unp1(icomp,N-1))/(leq.get_y(N)-leq.get_y(N-1));
          S_norm_N += tmp*tmp;
        }
      S_norm_N=sqrt(S_norm_N);
      //      kappa = (1./yw_N)*sqrt(leq.get_nu_t_yn()/(Damp_N*S_norm_N));
      kappa = leq.get_nu_t_yn()/(y_plus_N*visco_cin_N*Damp_N);

      if(kappa > getKappa())
        kappa = getKappa();
      else if(kappa < 0.)
        kappa = 0;
    }

  if(ind==0)
    {
      a = visco_cin;
    }
  else
    {
      //Norme de Sij
      S_norm = 0.;
      double tmp=0.;
      for (int icomp=0; icomp<nb_comp; icomp++)
        {
          tmp = (leq.get_Unp1(icomp,ind+1) - leq.get_Unp1(icomp,ind))/(leq.get_y(ind+1)-leq.get_y(ind));
          S_norm += tmp*tmp;
        }
      S_norm=sqrt(S_norm);

      visco_turb = kappa*kappa*yw*yw*S_norm*Damp;
      //visco_turb = kappa*y_plus*visco_cin*Damp;
      a = visco_cin + visco_turb;
    }
  return a;
}

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//  La viscosite turbulente est calculee grace a un modele de longueur de melange ://
//  nu_t = (kappa*y)^2*Damp*|S|                                                     //
//  Cette formulation est proposee dans l'article de Balaras et al.                //
//  AIAA journal vol. 34, no 6, 1996                                               //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//Calcul de la fonction de damping uniquement
//Attention : Ne pas integrer !!!
//////////////////////////////////////////////
double Diffu_lm::calculer_D_local(int ind)
{

  double yw, y_plus, Damp ;

  //Damp est une fonction de damping
  //u_tau : vitesse de frottement


  Eq_couch_lim& leq = eq_couch_lim.valeur();
  const Milieu_base& le_milieu = leq.get_milieu();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, le_milieu);
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();

  double visco_cin = max(tab_visco(0,0),DMINFLOAT);//visco cinematique supposee cste

  yw = 0.5*(leq.get_y(ind+1)+leq.get_y(ind));
  y_plus = yw*leq.get_utau_old()/visco_cin;
  Damp = 1-exp(pow(-y_plus/A_plus,3)); // Damp1

  return Damp;
}
