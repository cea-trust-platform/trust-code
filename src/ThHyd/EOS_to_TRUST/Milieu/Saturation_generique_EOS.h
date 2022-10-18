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

#ifndef Saturation_generique_EOS_included
#define Saturation_generique_EOS_included

#include <EOS_to_TRUST_Sat_generique.h>
#include <Saturation_base.h>

class Saturation_generique_EOS : public Saturation_base
{
  Declare_instanciable( Saturation_generique_EOS ) ;
private:
  void Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override
  {
    EOStT.eos_get_T_sat_p(P, res, ncomp, ind);
    Tc_(res); // return res en kelvin => on corrige
  }
  void dP_Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_d_T_sat_d_p_p(P, res, ncomp, ind); }
  void Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override
  {
    EOStT.eos_get_p_sat_T(Tk_(T), res, ncomp, ind);
    Tc_(T); /* put back T in C */
  }
  void dT_Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override
  {
    EOStT.eos_get_d_p_sat_d_T_T(Tk_(T), res, ncomp, ind);
    Tc_(T); /* put back T in C */
  }
  void Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_lvap_p(P, res, ncomp, ind); }
  void dP_Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_d_lvap_d_p_p(P, res, ncomp, ind); }
  void Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_h_l_sat_p(P, res, ncomp, ind); }
  void dP_Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_d_h_l_sat_d_p_p(P, res, ncomp, ind); }
  void Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_h_v_sat_p(P, res, ncomp, ind); }
  void dP_Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override { EOStT.eos_get_d_h_v_sat_d_p_p(P, res, ncomp, ind); }
  void sigma_(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override
  {
    EOStT.eos_get_sigma_pT(P, Tk_(T), res, ncomp, ind);
    Tc_(T); /* put back T in C */
  }

  void compute_all_flux_interfacial(MSpanD sats, int ncomp = 1, int ind = 0)  const override { EOStT.eos_get_all_flux_interfacial(sats,ncomp,ind); }

  EOS_to_TRUST_Sat_generique EOStT;
  Motcle model_name_, fluid_name_;
};

#endif /* Saturation_generique_EOS_included */
