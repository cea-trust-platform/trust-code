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
// File:        Schema_RK_Rationnel.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_RK_Rationnel.h>
#include <Equation.h>

// XD runge_kutta_rationnel_ordre_2 schema_temps_base runge_kutta_rationnel_ordre_2 -1 This is the Runge-Kutta rational scheme of second order. The method is described in the note: Wambeck - Rational Runge-Kutta methods for solving systems of ordinary differential equations, at the link: https://link.springer.com/article/10.1007/BF02252381. Although rational methods require more computational work than linear ones, they can have some other properties, such as a stable behaviour with explicitness, which make them preferable. The CFD application of this RRK2 scheme is described in the note: https://link.springer.com/content/pdf/10.1007\%2F3-540-13917-6_112.pdf.
Implemente_instanciable(RRK2,"Runge_Kutta_Rationnel_ordre_2",TRUSTSchema_RK<Ordre_RK::RATIO_DEUX>);

Sortie& RRK2::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::RATIO_DEUX>::printOn(s); }

Entree& RRK2::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::RATIO_DEUX>::readOn(s) ; }

// Description:
//    Effectue un pas de temps de Runge Kutta rationnel d'ordre 2, sur l'equation passee en parametre.
//    Le schema de Runge Kutta rationel d'ordre 2:
//     g1=hf(y0)
//     g2=hf(y0+c2g1)
//     y1=y0+(g1g1)/(b1g1+b2g2)
//     ou ab/d = (a(b,d)+b(d,a)-d(a,b)/(d,d)
//     y1=y0+(2g1(g1,b1g1+b2g2)-(b1g1+b2g2)(g1,g1)/(b1g1+b2g2,b1g1+b2g2)
//     y1=y0+(2g1(g1,"g2")-("g2")(g1,g1)/("g2","g2")
//      ordre2 si b2c2=-1/2
//     b2c2<=-1/2 A0 stabilite et I stabilite
//     b2c2<= 1/(2cos(alpha)(2-cos(alpha))) O<=alpha<pi/2 Aalpha stabilite
int RRK2::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  const double b1 = 2.0, b2 = -1, c2 = 0.5;

  DoubleTab& present = eqn.inconnue().valeurs(), &futur = eqn.inconnue().futur();

  // g1=futur=f(y0)
  DoubleTab g1(present), g2(present); // just for initializing the array structure ...

  // sauv=y0
  DoubleTrav sauv(present);
  sauv = present;

  eqn.derivee_en_temps_inco(g1);

  // g1=hf(y0)
  g1 *= dt_;

  // present=y0+c2g1
  present.ajoute(c2, g1);

  // g2=futur=f(y0+c2g1)
  eqn.derivee_en_temps_inco(g2);

  // g2=b2"g2"
  g2 *= (b2 * dt_);

  // g2=b2"g2" + b1g1
  g2.ajoute(b1, g1);

  // normeg2=("g2","g2")
  double normeg2 = mp_carre_norme_vect(g2) + DMINFLOAT;
  // normeg1=-(g1,g1)
  double normeg1 = -mp_carre_norme_vect(g1);
  // psc1=2(g1,"g2")
  double psc1 = 2.0 * mp_prodscal(g1, g2);

  // y1=y0+(2g1(g1,"g2")-("g2")(g1,g1)/("g2","g2")
  futur = g1;
  futur *= psc1;
  futur.ajoute(normeg1, g2);
  futur /= normeg2;
  present = sauv;
  futur /= dt_;
  update_critere_statio(futur, eqn);
  futur *= dt_;
  futur += sauv;
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  futur.echange_espace_virtuel();

  return 1;
}
