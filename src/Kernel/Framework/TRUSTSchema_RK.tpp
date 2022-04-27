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
// File:        TRUSTSchema_RK.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/63
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTSchema_RK_TPP_included
#define TRUSTSchema_RK_TPP_included

template <Ordre_RK _ORDRE_ > template<Ordre_RK _O_>
enable_if_t<_O_ == Ordre_RK::DEUX_WILLIAMSON || _O_ == Ordre_RK::TROIS_WILLIAMSON || _O_ == Ordre_RK::QUATRE_WILLIAMSON, int>
TRUSTSchema_RK<_ORDRE_>::faire_un_pas_de_temps_eqn_base_generique(Equation_base& eqn)
{
  static constexpr int NB_PTS = (_ORDRE_ == Ordre_RK::DEUX_WILLIAMSON) ? 2 : 3;

  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  DoubleTab& xi = eqn.inconnue().valeurs(), &xip1 = eqn.inconnue().futur();
  DoubleTab present(xi), qi(xi);

  for (int i = 0; i < NB_PTS; i++)
    {
      // on fait ca : q_i = a_{i-1} * q_{i-1} + dt * f(x_{i-1})
      qi *= get_a<_ORDRE_,NB_PTS>()[i];
      qi.ajoute(dt_, eqn.derivee_en_temps_inco(xip1));

      // on fait ca : x_i = x_{i-1} + b_i * q_i
      xi.ajoute(get_b<_ORDRE_,NB_PTS>()[i], qi);
    }

  xip1 = xi;
  xi -= present;
  xi /= dt_;
  update_critere_statio(xi, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  xip1.echange_espace_virtuel();

  // xi = x0;
  xi = present;

  return 1;
}

template <Ordre_RK _ORDRE_ > template<Ordre_RK _O_>
enable_if_t<_O_ == Ordre_RK::DEUX_CLASSIQUE, int>
TRUSTSchema_RK<_ORDRE_>::faire_un_pas_de_temps_eqn_base_generique(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  const double c2 = 0.5;
  DoubleTab& present = eqn.inconnue().valeurs(), &futur = eqn.inconnue().futur();
  DoubleTab k1(present), k2(present); // just for initializing the array structure ...

  DoubleTrav sauv(present);
  sauv = present; // sauv = y0

  eqn.derivee_en_temps_inco(k1); // k1 = f(y0)
  k1 *= dt_; // k1 = h * f(y0)

  present.ajoute(c2, k1); // present = y0 + c2k1

  eqn.derivee_en_temps_inco(k2); // k2 = f(y0 + c2k1)
  k2 *= dt_; // k2 = h * f(y0 + c2k1)

  futur = sauv; // futur = y1 = y0
  futur.ajoute(1., k2); // y1 = y0 + k2

  update_critere_statio(futur, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  futur.echange_espace_virtuel();

  present = sauv; // back to y0

  return 1;
}

template <Ordre_RK _ORDRE_ > template<Ordre_RK _O_>
enable_if_t<_O_ == Ordre_RK::TROIS_CLASSIQUE, int>
TRUSTSchema_RK<_ORDRE_>::faire_un_pas_de_temps_eqn_base_generique(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  const double c2 = 0.5, b2 = 2., b3 = -1.;
  DoubleTab& present = eqn.inconnue().valeurs(), &futur = eqn.inconnue().futur();
  DoubleTab k1(present), k2(present), k3(present); // just for initializing the array structure ...

  DoubleTrav sauv(present);
  sauv = present; // sauv = y0

  eqn.derivee_en_temps_inco(k1); // k1 = f(y0)
  k1 *= dt_; // k1 = h * f(y0)

  present.ajoute(c2, k1); // present = y0 + c2k1

  eqn.derivee_en_temps_inco(k2); // k2 = f(y0 + c2k1)
  k2 *= dt_; // k2 = h * f(y0 + c2k1)

  present = sauv; // back to y0
  present.ajoute(b2, k2); // present = y0 + b2k2
  present.ajoute(b3, k1); // present = y0 + b2k2 + b3k1

  eqn.derivee_en_temps_inco(k3); // k3 = f(y0 + b2k2 + b3k1)
  k3 *= dt_; // k3 = h * f(y0 + b2k2 + b3k1)

  futur = sauv; // futur = y1 = y0
  futur.ajoute(1./6., k1); // y1 = y0 + 1/6 k1
  futur.ajoute(4./6., k2); // y1 = y0 + 1/6 k1 + 4/6 k2
  futur.ajoute(1./6., k3); // y1 = y0 + 1/6 k1 + 4/6 k2 + 1/6 k3

  update_critere_statio(futur, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  futur.echange_espace_virtuel();

  present = sauv; // back to y0

  return 1;
}

template <Ordre_RK _ORDRE_ > template<Ordre_RK _O_>
enable_if_t<_O_ == Ordre_RK::QUATRE_CLASSIQUE, int>
TRUSTSchema_RK<_ORDRE_>::faire_un_pas_de_temps_eqn_base_generique(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  const double c2 = 0.5;
  DoubleTab& present = eqn.inconnue().valeurs(), &futur = eqn.inconnue().futur();
  DoubleTab k1(present), k2(present), k3(present), k4(present); // just for initializing the array structure ...

  DoubleTrav sauv(present);
  sauv = present; // sauv = y0

  eqn.derivee_en_temps_inco(k1); // k1 = f(y0)
  k1 *= dt_; // k1 = h * f(y0)

  present.ajoute(c2, k1); // present = y0 + c2k1

  eqn.derivee_en_temps_inco(k2); // k2 = f(y0 + c2k1)
  k2 *= dt_; // k2 = h * f(y0 + c2k1)

  present = sauv; // back to y0
  present.ajoute(c2, k2); // present = y0 + c2k2

  eqn.derivee_en_temps_inco(k3); // k3 = f(y0 + c2k2)
  k3 *= dt_; // k3 = h * f(y0 + c2k2)

  present = sauv; // back to y0
  present.ajoute(1.0, k3); // present = y0 + 1.0k3

  eqn.derivee_en_temps_inco(k4); // k4 = f(y0 + 1.0k3)
  k4 *= dt_; // k4 = h * f(y0 + 1.0k3)

  futur = sauv; // futur = y1 = y0
  futur.ajoute(1./6., k1); // y1 = y0 + 1/6 k1
  futur.ajoute(2./6., k2); // y1 = y0 + 1/6 k1 + 2/6 k2
  futur.ajoute(2./6., k3); // y1 = y0 + 1/6 k1 + 2/6 k2 + 2/6 k3
  futur.ajoute(1./6., k4); // y1 = y0 + 1/6 k1 + 2/6 k2 + 2/6 k3 + 1/6 k4

  update_critere_statio(futur, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  futur.echange_espace_virtuel();

  present = sauv; // back to y0

  return 1;
}

template <Ordre_RK _ORDRE_ > template<Ordre_RK _O_>
enable_if_t<_O_ == Ordre_RK::QUATRE_CLASSIQUE_3_8, int>
TRUSTSchema_RK<_ORDRE_>::faire_un_pas_de_temps_eqn_base_generique(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1 pour faire reflechir l'utilisateur
  if (nb_pas_dt() >= 0 && nb_pas_dt() <= NW && facsec_ == 1) print_warning(NW);

  DoubleTab& present = eqn.inconnue().valeurs(), &futur = eqn.inconnue().futur();
  DoubleTab k1(present), k2(present), k3(present), k4(present); // just for initializing the array structure ...

  DoubleTrav sauv(present);
  sauv = present; // sauv = y0

  eqn.derivee_en_temps_inco(k1); // k1 = f(y0)
  k1 *= dt_; // k1 = h * f(y0)

  present.ajoute(1. / 3. , k1); // present = y0 + c2k1
  eqn.derivee_en_temps_inco(k2); // k2 = f(y0 + c2k1)
  k2 *= dt_; // k2 = h * f(y0 + c2k1)

  present = sauv; // back to y0
  present.ajoute(-1./3., k1); // present = y0 - 1/3 k1
  present.ajoute(1.0, k2); // present = y0 - 1/3 k1 + k2
  eqn.derivee_en_temps_inco(k3); // k3 = f(y0 - 1/3 k1 + k2)
  k3 *= dt_; // k3 = h * f(y0 - 1/3 k1 + k2)

  present = sauv; // back to y0
  present.ajoute(1.0, k1); // present = y0 + 1.0k1
  present.ajoute(-1.0, k2); // present = y0 + 1.0k1 - 1.0k2
  present.ajoute(1.0, k3); // present = y0 + 1.0k1 - 1.0k2 + 1.0k3
  eqn.derivee_en_temps_inco(k4); // k4 = f(y0 + 1.0k1 - 1.0k2 + 1.0k3)
  k4 *= dt_; // k4 = h * f(y0 + 1.0k1 - 1.0k2 + 1.0k3)

  futur = sauv; // futur = y1 = y0
  futur.ajoute(1./8., k1); // y1 = y0 + 1/8 k1
  futur.ajoute(3./8., k2); // y1 = y0 + 1/8 k1 + 3/8 k2
  futur.ajoute(3./8., k3); // y1 = y0 + 1/8 k1 + 3/8 k2 + 3/8 k3
  futur.ajoute(1./8., k4); // y1 = y0 + 1/8 k1 + 3/8 k2 + 3/8 k3 + 1/8 k4

  update_critere_statio(futur, eqn);

  // Update boundary condition on futur:
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(), temps_courant() + pas_de_temps());
  futur.echange_espace_virtuel();

  present = sauv; // back to y0

  return 1;
}

#endif /* TRUSTSchema_RK_TPP_included */
