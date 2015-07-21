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
// File:        Eq_ODVM.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Eq_ODVM.h>
#include <Mod_turb_hyd_base.h>
#include <EcrFicCollecte.h>

#define KAPPA 0.415
#define PR_T 0.9
#define R 0.5

Implemente_instanciable(Eq_ODVM,"Eq_ODVM",Objet_U);


Sortie& Eq_ODVM::printOn(Sortie& os) const
{
  return os;
}


Entree& Eq_ODVM::readOn( Entree& is)
{
  return is;
}

/*void Eq_ODVM::init_profile_CHT(double T0, double Flux, double alpha, double nu)
  {
  Flux = Flux/u_tau;
  double Pr = nu/alpha;
  double Beta = pow(3.85*pow(Pr,1./3.)-1.3,2.)+2.12*log(Pr);
  Tm(0) = T0;
  for(int i=1;i<N;i++)
  {
  double y_plus = Y(i)*u_tau/nu;
  double Gamma = (0.01*pow(Pr*y_plus,4.))/(1.+5.*pow(Pr,3.)*y_plus);
  double T_plus = Pr*y_plus*exp(-Gamma);
  T_plus += (2.12*log(1.+y_plus) + Beta)*exp(-1./(Gamma+1e-20));
  Tm(i) = T0 + T_plus*Flux;
  }
  }*/


void Eq_ODVM::init_profile_CHT(double Ts, double Tf)
{
  for(int i=0; i<N; i++) Tm(i) = Ts + Y(i)*(Tf-Ts)/Y(N-1);
}


void Eq_ODVM::initialiser(int n,double G, double dist, double T0, double Tn, double t0, double K,
                          double f)
{
  N = n;
  gamma = G;
  //dy = dist/(N-1);
  y0 = 0.;
  //yn = dist;
  facteur = f;
  tn = t0;
  Tmles_n   = Tn;
  Tples_n   = 0.;
  Tples2m_n = 0.;
  Tw_m_n    = T0;
  Theta = K/(K+1.);
  compt = 0;
  u_tau = 0.;

  Y.resize(N);
  F.resize(N);
  Tm.resize(N);
  Tp.resize(N);
  qb.resize(N);
  qf.resize(N);
  Q.resize(N);
  alpha_tot.resize(N);
  SEC_MEM.resize(N);

  eps.resize(N);
  prod.resize(N);
  mol_diff.resize(N);
  tur_diff.resize(N);

  mailler_fin(dist, facteur);

  for(int i=0; i<N; i++)
    {
      //Y(i)        = i*dy;
      Tm(i)        = (Tn - T0)*Y(i)/dist + T0;
      Tp(i)        = 0.;
      qb(i)        = 0.;
      qf(i)        = 0.;
      eps(i)        = 0.;
      prod(i)        = 0.;
      mol_diff(i)        = 0.;
      tur_diff(i)        = 0.;
      SEC_MEM(i)  = 0.;
      Q(i)         = 1.;
    }
  dy_w = Y(1) - Y(0);

}

void Eq_ODVM::mailler_fin(double dist, double un_facteur)
{

  //********************************
  // Mise en place du maillage fin *
  //********************************

  EcrFicCollecte fic_mesh("tble_mesh.dat",ios::app); // ouverture du fichier conv.dat
  Y(0)=0.;

  if(un_facteur==1.)
    {
      dy=dist/(N-1);
      for(int i=1 ; i<N ; i++)
        {
          Y(i)=i*dy;
        }
    }
  else
    {
      dy = dist/(1.-pow(un_facteur,N-1));
      for (int i=1 ; i<N ; i++)
        {
          Y(i)=dy*(1-pow(un_facteur,i));
        }
    }

  for(int i=0 ; i<N ; i++)
    {
      fic_mesh << i << " " << Y(i) << finl;
    }
  fic_mesh << finl;

  //************************************
  // Fin mise en place du maillage fin *
  //************************************
}

void Eq_ODVM::calculer_dt_stab()
{
  dt=1000.;
  for(int i=1; i<N-1; i++)
    {
      dy=0.5*(Y(i+1)-Y(i-1));
      if((0.5*dy*dy/alpha_tot[i])<dt) dt = 0.5*dy*dy/alpha_tot[i];
    }
}


void Eq_ODVM::calculer_secmem_qb( )
{
  double Ap, Am;

  SEC_MEM[0] = 0;

  for(int i=1; i<N-1; i++)
    {
      dy=0.5*(Y(i+1)-Y(i-1));
      Ap = alpha*(qb[i+1]-qb[i])/(Y(i+1)-Y(i));
      Am = alpha*(qb[i]-qb[i-1])/(Y(i)-Y(i-1));
      mol_diff[i] = (Ap - Am)/(dy);

      Ap = (qb[i+1]-qb[i])/(Y(i+1)-Y(i))*(alpha_tot[i+1]+alpha_tot[i]-2.*alpha)/2.;
      Am = (qb[i]-qb[i-1])/(Y(i)-Y(i-1))*(alpha_tot[i]+alpha_tot[i-1]-2.*alpha)/2.;
      tur_diff[i] = (Ap - Am)/(dy);

      if(i==1)
        {
          qb[1]=qb[0];
          mol_diff[1] = 0;
          tur_diff[1] = 0;
        }

      prod[i] = (alpha_tot[i]-alpha)*(Tm[i+1]-Tm[i-1])*(Tm[i+1]-Tm[i-1])/4./dy/dy;

      double f = (1+tanh(0.08*(30-Y[i]*u_tau/nu)))/2.;
      eps[i] = (1-f)*sqrt(CMU) * qb[i]/R * u_tau*u_tau/(nu+(alpha_tot[i]-alpha)*PR_T);
      eps[i] += f*2.*(qb[1]-qb[0])*alpha/(Y(1)-Y(0))/(Y(1)-Y(0));

      SEC_MEM[i] = mol_diff[i]+tur_diff[i] + prod[i] - eps[i];
    }

}


void Eq_ODVM::calculer_secmem_qf( )
{
  double Ap, Am;

  SEC_MEM[0] = 0;

  for(int i=1; i<N-1; i++)
    {
      dy=0.5*(Y(i+1)-Y(i-1));
      // Pour Flux impose :
      if(i==1) qf[0]=qf[1];

      Ap = alpha*(qf[i+1]-qf[i])/(Y(i+1)-Y(i));
      Am = alpha*(qf[i]-qf[i-1])/(Y(i)-Y(i-1));
      mol_diff[i] = (Ap - Am)/(dy);

      Ap = (qf[i+1]-qf[i])/(Y(i+1)-Y(i))*(alpha_tot[i+1]+alpha_tot[i]-2.*alpha)/2.;
      Am = (qf[i]-qf[i-1])/(Y(i)-Y(i-1))*(alpha_tot[i]+alpha_tot[i-1]-2.*alpha)/2.;
      tur_diff[i] = (Ap - Am)/(dy);

      prod[i] = (alpha_tot[i]-alpha)*(Tm[i+1]-Tm[i-1])*(Tm[i+1]-Tm[i-1])/dy/dy;

      double f = (1+tanh(0.08*(30-Y[i]*u_tau/nu)))/2.;
      eps[i] = (1-f)*sqrt(CMU) * qf[i]/R * u_tau*u_tau/(nu+(alpha_tot[i]-alpha)*PR_T);
      eps[i] += f*2.*(qf[1]-qf[0])*alpha/(Y(1)-Y(0))/(Y(1)-Y(0));

      SEC_MEM[i] = mol_diff[i]+tur_diff[i] + prod[i] - eps[i];
    }

}


void Eq_ODVM::aller_au_temps(double dtn1, double Tw_np1, double Tnp1, double diff, double visco_cin, int type_pb)
{

  compt++;

  double Ap, Am;
  alpha = diff;
  nu = visco_cin;

  // Calculating the filtered values of the LES at time (n+1)
  Tmles_np1        = gamma*Tnp1 + (1.-gamma)*Tmles_n;
  Tples_np1         = Tnp1 - Tmles_np1;
  Tples2m_np1        = gamma*(Tnp1-Tmles_np1)*(Tnp1-Tmles_np1) + (1.-gamma)*Tples2m_n;
  Tw_m_np1        = gamma*Tw_np1 + (1.-gamma)*Tw_m_n;

  if(compt==1)
    {
      Tw_m_np1 = Tw_np1;
    }



  // Calculating the temperature variance ratio for forcing.
  if(compt>=6) for(int i=0; i<N; i++) Q(i) = sqrt(dabs(qb(i))/dabs(qb(N-1)+1.e-20));

  // Calculating the turbulent diffusivity.
  double D;
  double A=19.;
  for(int i=0; i<N; i++)
    {
      D = pow(1. - exp(-Y[i]*u_tau/nu/A),2.);
      alpha_tot[i] = alpha + KAPPA*D*Y[i]*u_tau/PR_T;
    }

  //Cerr << "u_tau = " << u_tau << finl;

  while(tn<dtn1) // Going from LES value at (n) to (n+1).
    {
      if(type_pb==1) Tm(0)   = Tw_m_np1;        // Setting lower BC for mean temp.
      Tm(N-1) = Tmles_np1;                // Setting upper BC for mean temp.
      Tp(N-1) = Tples_np1;                // Setting BC for temp. fluct.
      qb(N-1) = Tples2m_np1;                // Setting BC for variance eqn.
      qf(N-1) = Tples2m_np1;                // Setting BC for variance eqn.

      calculer_dt_stab();
      if((dtn1-tn)<dt) dt = dtn1-tn;


      // Time advancement for MEAN TEMPERATURE AND TEMPERATURE FLUCTUATIONS
      // Isothermal case
      int i;
      if(type_pb==1)
        {
          for(i=0; i<N-1; i++)
            {
              if(i==0) SEC_MEM(i)=0;
              else
                {
                  dy=0.5*(Y(i+1)-Y(i-1));
                  Ap = ((Tm[i+1]-Tm[i])/(Y(i+1)-Y(i)))*(alpha_tot[i+1]+alpha_tot[i])/2.;
                  Am = ((Tm[i]-Tm[i-1])/(Y(i)-Y(i-1)))*(alpha_tot[i]+alpha_tot[i-1])/2.;
                  SEC_MEM[i] = (Ap-Am)/dy;
                }
            }
          for(i=0; i<N-1; i++)
            {
              Tm[i] = Tm[i] + SEC_MEM[i]*dt + F[i]*dt;
              Tp[i] = Q[i]*Tp[N-1];
            }
        }


      //CHT case
      else if(type_pb==2)
        {
          Ap = ((Tm[2]-Tm[1])/(Y(2)-Y(1)))*(alpha_tot[2]+alpha_tot[1])/2.;
          Am = Tw_m_np1;                        // En fait ici Tw_m_np1 est egal au flux filtre que l'on passe par Paroi_ODVM.

          SEC_MEM[1] = (Ap-Am)/(0.5*(Y(2)-Y(0)));
          for(i=2; i<N-1; i++)
            {
              dy=0.5*(Y(i+1)-Y(i-1));
              Ap = ((Tm[i+1]-Tm[i])/(Y(i+1)-Y(i)))*(alpha_tot[i+1]+alpha_tot[i])/2.;
              Am = ((Tm[i]-Tm[i-1])/(Y(i)-Y(i-1)))*(alpha_tot[i]+alpha_tot[i-1])/2.;
              SEC_MEM[i] = (Ap-Am)/dy;
            }
          for(i=1; i<N-1; i++)
            {
              Tm[i] = Tm[i] + SEC_MEM[i]*dt;
              Tp[i] = Q[i]*Tp[N-1];
            }
          dy=Y(1)-Y(0);
          Tm[0] = Tm[1] - Tw_m_np1*dy/alpha;        // En fait ici Tw_m_np1 est egal au flux filtre que l'on passe par Paroi_ODVM.
          Tp[0] = Q[0]*Tp[N-1];
        }



      // Time advancement of TRANSPORT EQUATION FOR TEMPERATURE VARIANCE FOR ISOFLUX CONDITION
      if(type_pb==2)
        {
          calculer_secmem_qf();
          for(i=1; i<N-1; i++)
            {
              qf(i) = qf(i) + SEC_MEM(i)*dt;
            }
          qf(0) = qf(1);
          qb(0) = Theta*Theta*qf(0);
        }


      // Time advancement of TRANSPORT EQUATION FOR TEMPERATURE VARIANCE FOR the CHT case
      calculer_secmem_qb();
      for(i=1; i<N-1; i++)
        {
          qb(i) = qb(i) + SEC_MEM(i)*dt;
        }

      tn+=dt;
    }
  // End of time advancement from (n) to (n+1).


  // Putting the values of (n+1) at (n) for next time iterations.
  tn        = dtn1;
  Tmles_n   = Tmles_np1;
  Tples_n   = Tples_np1;
  Tples2m_n = Tples2m_np1;
  Tw_m_n    = Tw_m_np1;

}

