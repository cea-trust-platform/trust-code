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
// File:        Flux_interfacial_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/CoviMAC
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Flux_interfacial_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Champ_P0_CoviMAC.h>
#include <Matrix_tools.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Flux_interfacial_base.h>
#include <Milieu_composite.h>

Implemente_instanciable(Flux_interfacial_CoviMAC,"Flux_interfacial_P0_CoviMAC", Source_base);

Sortie& Flux_interfacial_CoviMAC::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_CoviMAC::readOn(Entree& is)
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  if (!pbm.has_correlation("flux_interfacial")) Process::exit(que_suis_je() + " : a flux_interfacial correlation must be defined in the global correlations { } block!");
  correlation_ = pbm.get_correlation("flux_interfacial");
  return is;
}

void Flux_interfacial_CoviMAC::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  if (!matrices.count(ch.le_nom().getString())) return; //rien a faire
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleTab& inco = ch.valeurs();

  /* stencil : diagonal par bloc pour les vitesses aux faces, puis chaque composante des vitesses aux elems */
  int ne_tot = zone.nb_elem_tot(), N = inco.line_size(), Np = ref_cast(Navier_Stokes_std, equation().probleme().equation(0)).pression().valeurs().line_size();;

  /* elements */
  for (std::string var :
       { "temperature", "alpha", "pression"
       }) if (matrices.count(var))
      {
        Matrice_Morse& mat = *matrices[var], mat2;
        IntTrav stencil(0, 2);
        stencil.set_smart_resize(1);
        int M = var == "pression" ? Np : N;

        for (int e = 0; e < zone.nb_elem(); e++) for (int k = 0; k < N; k++) for (int l = 0; l < M; l++)
              stencil.append_line(N * e + k, M * e + l);

        Matrix_tools::allocate_morse_matrix(N * ne_tot, M * ne_tot, stencil, mat2);
        mat.nb_colonnes() ? mat += mat2 : mat = mat2;
      }
}

void Flux_interfacial_CoviMAC::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, equation().probleme());
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  // Matrice_Morse *mat = matrices.count(ch.le_nom().getString()) ? matrices.at(ch.le_nom().getString()) : NULL;
  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, equation().zone_dis().valeur());
  const DoubleVect& pe = zone.porosite_elem(), &ve = zone.volumes();
  const tabs_t& der_h = ref_cast(Champ_Inc_base, milc.enthalpie()).derivees();
  const DoubleTab& inco = ch.valeurs(),
                   &alpha = pbm.eq_masse.inconnue().valeurs(),
                    &press = pbm.eq_qdm.pression().valeurs(),
                     &temp  = pbm.eq_energie.inconnue().valeurs(),
                      &h = milc.enthalpie().valeurs(),
                       *dP_h = der_h.count("pression") ? &der_h.at("pression") : NULL,
                        *dT_h = der_h.count("temperature") ? &der_h.at("temperature") : NULL;
  Matrice_Morse *Ma = matrices.count("alpha")       ? matrices.at("alpha")       : NULL,
                 *Mp = matrices.count("pression")    ? matrices.at("pression")    : NULL,
                  *Mt = matrices.count("temperature") ? matrices.at("temperature") : NULL;
  int N = inco.line_size();
  const Flux_interfacial_base& correlation_fi = ref_cast(Flux_interfacial_base, correlation_.valeur().valeur());

  /* elements */
  for (int e = 0; e < zone.nb_elem(); e++)
    for (int n = 0; n < N; n++) for (int k = n + 1; k < N; k++)
        if (milc.has_saturation(n, k))
          {
            int l, g; // indices liquide et gaz
            if (pbm.nom_phase(n).debute_par("liquide")) l = n, g = k;
            else                                        l = k, g = n;

            const double p = press[e], Tl = temp(e, l), Tg = temp(e, g),
                         Ts = milc.get_saturation(l, g).Tsat(p), dP_Ts = milc.get_saturation(l, g).dP_Tsat(p),
                         hls = milc.get_saturation(l, g).Hls(p), hgs = milc.get_saturation(l, g).Hvs(p),
                         dP_hls = milc.get_saturation(l, g).dP_Hls(p), dP_hgs = milc.get_saturation(l, g).dP_Hvs(p),
                         al = alpha(e, l), ag = alpha(e, g), hl = h(e, l),
                         hg = h(e, g), dP_hl = dP_h ? (*dP_h)(e, l) : 0, dP_hg = dP_h ? (*dP_h)(e, g) : 0,
                         dT_hl = dT_h ? (*dT_h)(e, l) : 0, dT_hg = dT_h ? (*dT_h)(e, g) : 0;

            const bool vap = (Tl > Ts) && (al > 0), cond = (Tg < Ts) && ag > 0;

            /* calcul du flux */
            if (vap || cond)
              {
                double fl = 0, fg = 0, dal_fl = 0, dag_fl = 0, dal_fg = 0, dag_fg = 0, dTl_fl = 0, dTg_fg = 0, dP_fl = 0, dP_fg = 0, dTl_fg = 0, dTg_fl = 0;
                correlation_fi.flux(al, ag, Tl, Tg, Ts, dP_Ts, fl, fg, dal_fl, dag_fl, dal_fg, dag_fg, dTl_fl, dTg_fg, dP_fl, dP_fg);

                /* secmem */
                const bool gpos = (fl + fg) < 0;
                // gamma > 0 => du   liquide a hl se vaporise en vapeur  a saturation (hl --> hvsat)
                // gamma < 0 => de la vapeur a hv se condense en liquide a saturation (hv --> hlsat)
                const double hl_t = gpos ? hl : hls, hg_t = gpos ? hgs : hg,
                             dP_hl_t  = gpos ? dP_hl : dP_hls,  dP_hg_t = gpos ? dP_hgs :  dP_hg,
                             dTl_hl_t = gpos ? dT_hl : 0, dTl_hg_t = 0,
                             dTg_hl_t = 0, dTg_hg_t = gpos ? 0 : dT_hg,
                             L = hg_t - hl_t, dP_L = dP_hg_t - dP_hl_t,
                             dTl_L = dTl_hg_t - dTl_hl_t, dTg_L = dTg_hg_t - dTg_hl_t,
                             gamma = -(fl + fg) / L, fac = pe(e) * ve(e),
                             dP_gamma = -(L * (dP_fl + dP_fg) - (fl + fg) * dP_L) / L / L,
                             dal_gamma = - (dal_fl + dag_fl) / L, dag_gamma = -(dag_fl + dag_fg) / L,
                             dTl_gamma = -(L * (dTl_fl + dTl_fg) - (fl + fg) * dTl_L) / L / L,
                             dTg_gamma = -(L * (dTg_fl + dTg_fg) - (fl + fg) * dTg_L) / L / L;

                if (equation().que_suis_je().debute_par("Masse"))
                  {
                    secmem(e, l) -= fac * gamma;
                    secmem(e, g) += fac * gamma;
                    if (Ma)
                      {
                        (*Ma)(N * e + l, N * e + l) += fac * dal_gamma;
                        (*Ma)(N * e + l, N * e + g) += fac * dag_gamma;
                        (*Ma)(N * e + g, N * e + l) -= fac * dal_gamma;
                        (*Ma)(N * e + g, N * e + g) -= fac * dag_gamma;
                      }
                    if (Mt)
                      {
                        (*Mt)(N * e + l, N * e + l) += fac * dTl_gamma;
                        (*Mt)(N * e + l, N * e + g) += fac * dTg_gamma;
                        (*Mt)(N * e + g, N * e + g) -= fac * dTg_gamma;
                        (*Mt)(N * e + g, N * e + l) -= fac * dTl_gamma;
                      }
                    if (Mp)
                      {
                        (*Mp)(N * e + l, e) += fac * dP_gamma;
                        (*Mp)(N * e + g, e) -= fac * dP_gamma;
                      }
                  }
                else if (equation().que_suis_je().debute_par("Energie"))
                  {
                    secmem(e, l) += fac * (fl - (gamma * hl_t));
                    secmem(e, g) += fac * (fg + (gamma * hg_t));
                    if (Ma)
                      {
                        (*Ma)(N * e + l, N * e + l) -= fac * (dal_fl - dal_gamma * hl_t);
                        (*Ma)(N * e + l, N * e + g) -= fac * (dag_fl - dag_gamma * hl_t);
                        (*Ma)(N * e + g, N * e + l) -= fac * (dal_fg + dal_gamma * hg_t);
                        (*Ma)(N * e + g, N * e + g) -= fac * (dag_fg + dag_gamma * hg_t);
                      }
                    if (Mt)
                      {
                        (*Mt)(N * e + l, N * e + l) -= fac * (dTl_fl - (dTl_gamma * hl_t + gamma * dTl_hl_t));
                        (*Mt)(N * e + l, N * e + g) -= fac * (       - (dTg_gamma * hl_t + gamma * dTg_hl_t));
                        (*Mt)(N * e + g, N * e + g) -= fac * (dTg_fg + (dTg_gamma * hg_t + gamma * dTg_hg_t));
                        (*Mt)(N * e + g, N * e + l) -= fac * (         (dTl_gamma * hg_t + gamma * dTl_hg_t));
                      }
                    if (Mp)
                      {
                        (*Mp)(N * e + l, e) -= fac * (-dP_gamma * hl_t - dP_hl_t * gamma);
                        (*Mp)(N * e + g, e) -= fac * ( dP_gamma * hg_t + dP_hg_t * gamma);
                      }
                  }
                else Process::exit("Flux_interfacial_CoviMAC : wrong equation!");
              }
          }
        else Process::exit("flux sans saturation non code");
}
