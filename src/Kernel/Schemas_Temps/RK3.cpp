/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        RK3.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <RK3.h>
#include <Equation.h>

Implemente_instanciable(RK3,"Runge_Kutta_ordre_3",TRUSTSchema_RK<Ordre_RK::TROIS>);

Sortie& RK3::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::TROIS>::printOn(s); }

Entree& RK3::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::TROIS>::readOn(s) ; }

// Description:
//    Time step with Runge Kutta order 3,
//    on the equation eqn.
//    Runge Kutta order 3
//     (Williamson version, case 7) is written:
//     q1 = h f(x0)
//     x1 = x0 + b1 q1
//     q2 = h f(x1) + a2 q1
//     x2 = x1 + b2 q2
//     q3 = h f(x2) + a3 q2
//     x3 = x2 + b3 q3
//     with a1=0, a2=-5/9, a3=-153/128
//          b1=1/3, b2=15/16, b3=8/15
int RK3::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{

  // Warning sur les 100 premiers pas de temps si facsec est egal a 1
  // pour faire reflechir l'utilisateur
  int nw = 100;
  if ( nb_pas_dt()>=0 && nb_pas_dt()<=nw && facsec_ == 1 )
    {
      Cerr << finl << "**** Advice (printed only on the first " << nw << " time steps)****" << finl;
      Cerr << "You are using Runge Kutta schema order 3 so if you wish to increase the time step, try facsec between 1 and 3." << finl;
    }

  double a2=-5./9.;
  double a3=-153./128.;
  double b1=1./3.;
  double b2=15./16.;
  double b3=8./15.;

  DoubleTab& xi=eqn.inconnue().valeurs();
  DoubleTab& xipls1=eqn.inconnue().futur();
  DoubleTab qi(xi) ;
  DoubleTab present(xi);

  // q1 = f(x0)
  eqn.derivee_en_temps_inco(qi);

  // q1 = h f(x0)
  qi *= dt_;

  // x1 = x0 + b1 q1
  xi.ajoute(b1, qi);

  // xipls1 = f(x1)

  eqn.derivee_en_temps_inco(xipls1);

  // q2 = h f(x1) + a2 q1
  qi *= a2;
  qi.ajoute(dt_,xipls1);

  // x2 = x1 + b2 q2
  xi.ajoute(b2, qi);

  // xipls1 = f(x2)

  eqn.derivee_en_temps_inco(xipls1);

  // q3 = h f(x2) + a3 q2
  qi *= a3;
  qi.ajoute(dt_,xipls1);

  // x3 = x2 + b3 q3
  xi.ajoute(b3, qi);

  // futur = x3
  xipls1 = xi;

  // xi = (x3-x0)/dt
  xi -= present;
  xi /= dt_;
  update_critere_statio(xi, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());
  xipls1.echange_espace_virtuel();

  // xi = x0;
  xi = present;

  return 1;
}

