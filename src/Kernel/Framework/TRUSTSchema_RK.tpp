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
      qi *= get_a()[i];
      qi.ajoute(dt_, eqn.derivee_en_temps_inco(xip1));

      // on fait ca : x_i = x_{i-1} + b_i * q_i
      xi.ajoute(get_b()[i], qi);
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


#endif /* TRUSTSchema_RK_TPP_included */
