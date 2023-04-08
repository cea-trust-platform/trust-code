/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Fluide_generique_EOS_included
#define Fluide_generique_EOS_included

#include <EOS_to_TRUST_generique.h>
#include <Fluide_reel_base.h>

class Fluide_generique_EOS : public Fluide_reel_base
{
  Declare_instanciable( Fluide_generique_EOS ) ;
public :
  void set_param(Param& param) override;
  MRange unknown_range() const override
  {
    if (tmax_ < -100. )
      return Fluide_reel_base::unknown_range();

    return { { "temperature", { tmin_ - 273.15, tmax_ - 273.15 } }, { "pression", { pmin_, pmax_ } } };
  }

private:
  void rho_(const SpanD T, const SpanD P, SpanD R, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_rho_pT(P, Tk_(T), R, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void dP_rho_(const SpanD T, const SpanD P, SpanD dP_R, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_rho_dp_pT(P, Tk_(T), dP_R, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void dT_rho_(const SpanD T, const SpanD P, SpanD dT_R, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_rho_dT_pT(P, Tk_(T), dT_R, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void h_(const SpanD T, const SpanD P, SpanD H, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_h_pT(P, Tk_(T), H, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void dP_h_(const SpanD T, const SpanD P, SpanD dP_H, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_h_dp_pT(P, Tk_(T), dP_H, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void dT_h_(const SpanD T, const SpanD P, SpanD dT_H, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_h_dT_pT(P, Tk_(T), dT_H, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void cp_(const SpanD T, const SpanD P, SpanD CP, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_cp_pT(P, Tk_(T), CP, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void beta_(const SpanD T, const SpanD P, SpanD B, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_beta_pT(P, Tk_(T), B, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void mu_(const SpanD T, const SpanD P, SpanD M, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_mu_pT(P, Tk_(T), M, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void lambda_(const SpanD T, const SpanD P, SpanD L, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_lambda_pT(P, Tk_(T), L, ncomp, id);
    Tc_(T); /* put back T in C */
  }
  void compute_CPMLB_pb_multiphase_(const MSpanD input, MLoiSpanD prop, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_CPMLB_pb_multiphase_pT(input, prop, ncomp, id);
  }
  void compute_all_pb_multiphase_(const MSpanD input, MLoiSpanD inter, MLoiSpanD bord, int ncomp = 1, int id = 0) const override
  {
    EOStT.tppi_get_all_pb_multiphase_pT(input,inter, bord, ncomp, id);
  }

  EOS_to_TRUST_generique EOStT;
  Motcle model_name_, fluid_name_;
  double tmin_ = -123., tmax_ = -123., pmin_ = -123., pmax_ = -123.;
};

#endif /* Fluide_generique_EOS_included */
