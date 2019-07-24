/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Eq_ODVM.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eq_ODVM_included
#define Eq_ODVM_included
#include <DoubleTab.h>
#include <Motcle.h>
#include <Ref_Milieu_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Eq_ODVM
//    Classe resolvant l'equation de temperature moyenne, l'equation de
//    fluctuation de temperature et l'equation de variance de temperature
//    en 1-D
//
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Eq_ODVM : public Objet_U
{
  Declare_instanciable(Eq_ODVM);

public :

  void initialiser(int N, double gamma, double dist, double T0,
                   double Tn, double t0, double rat, double f);

  void mailler_fin(double, double) ;

  void calculer_secmem_qb( );
  void calculer_secmem_qf( );
  void calculer_dt_stab();
  void set_U_tau(double u, int j)
  {
    if(j) u_tau=u;
    else u_tau=gamma*u+(1.-gamma)*u_tau;
  };
  /*  void set_U_tau(double u, int j) { if(j) u_tau=u; else u_tau=u; }; */
  /*  void init_profile_CHT(double T0, double Flux, double alpha, double nu);*/
  void init_profile_CHT(double Ts, double Tf);

  void aller_au_temps(double tnp1, double Tw_np1, double Tnp1, double diff, double visco_cin, int type_pb);


  double get_Tm(int i)
  {
    return Tm(i);
  };
  double get_Tp(int i)
  {
    return Tp(i);
  };
  double get_qb(int i)
  {
    return qb(i);
  };
  double get_qf(int i)
  {
    return qf(i);
  };
  double get_Y(int i)
  {
    return Y(i);
  };
  double get_dy_w()
  {
    return dy_w;
  };
  double get_Q(int i)
  {
    return Q(i);
  };

  void set_F(double f)
  {
    for(int i=0 ; i<N ; i++)
      {
        F(i) = f;
      }
  };

private :

  int N;                                // Number of grid-points in the 1-D mesh.
  int compt;
  double dy, y0, dt, u_tau, nu, alpha, Theta, dy_w;
  double tn;
  double gamma;                                // Temporal filter parameter.
  double facteur; //stretching factor

  double Tw_m_n, Tw_m_np1;                // Values of the filtered wall temperature at times n and (n+1).
  double Tmles_n, Tmles_np1;                // Values of the filtered upper- LES field at times n and (n+1).
  double Tples_n, Tples_np1;                // Values of the upper- LES fluctuation field at times n and (n+1).
  double Tples2m_n, Tples2m_np1;        // Values of the filtered upper- LES variance field at times n and (n+1).

  DoubleVect Y;                        // Y positions.
  DoubleVect Tm;                      // Values of the mean temperature.
  DoubleVect Tp;                // Values of the temperature fluctuations.
  DoubleVect qb;                   // Values of the fluctuations given by the variance eqn.
  // at a given thermal activity ratio K.
  DoubleVect qf;                // Values of the fluctuations given by the variance eqn. for iso-flux BC.

  DoubleVect alpha_tot;                // alpha_total = molecular + eddy diffusivity.
  DoubleVect SEC_MEM;                // Right hand-side for time advancement.
  DoubleVect Q;                        // Forcing factor for the temperature fluctuations equation.

  DoubleVect eps;                // Dissipation = filtered gradient of temperature fluctuations squared.
  DoubleVect prod;                // Production term in variance equation.
  DoubleVect mol_diff;                // Molecular diffusion term in variance equation.
  DoubleVect tur_diff;                // Turbulent diffusion term in variance equation.
  DoubleVect F;                        // Terme source production de chaleur


};


#endif



