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

#include <Saturation_base.h>
#include <QDM_Multiphase.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <TPPI_tools.h>

Implemente_base(Saturation_base, "Saturation_base", Interface_base);
// XD saturation_base objet_u saturation_base -1 fluide-gas interface with phase change (used in pb_multiphase)

void Saturation_base::set_param(Param& param)
{
  Interface_base::set_param(param);
  param.ajouter("P_ref", &P_ref_);
  param.ajouter("T_ref", &T_ref_);
}

Sortie& Saturation_base::printOn(Sortie& os) const { return os; }

Entree& Saturation_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Saturation_base::mettre_a_jour(double temps)
{
  DoubleTab& sigma_tab = ch_sigma_->valeurs(), &Tsat_tab = ch_Tsat_->valeurs();
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, pb_.valeur());
  const DoubleTab& press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression()->valeurs();

  // on suppose pour le moment que le champ de pression a 1 comp.
  assert(press.line_size() == 1);
  Tsat(press.get_span_tot(), Tsat_tab.get_span_tot(), 1, 0);

  // call sigma
  sigma(Tsat_tab.get_span(), press.get_span(), sigma_tab.get_span(), 1, 0);
  sigma_tab.echange_espace_virtuel();
}

void Saturation_base::Tsat(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  (P_ref_ > 0) ? _Tsat_(P_ref_,res,ncomp,ind) : Tsat_(P,res,ncomp,ind);
}

void Saturation_base::dP_Tsat(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  if (P_ref_ > 0)
    for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = 0.;
  else dP_Tsat_(P,res,ncomp,ind);
}

void Saturation_base::Psat(const SpanD T, SpanD res, int ncomp, int ind) const
{
  assert((int )T.size() == (int )res.size());
  (T_ref_ > 0) ? _Psat_(T_ref_,res,ncomp,ind) : Psat_(T,res,ncomp,ind);
}

void Saturation_base::dT_Psat(const SpanD T, SpanD res, int ncomp, int ind) const
{
  assert((int )T.size() == (int )res.size());
  if (T_ref_ > 0)
    for (int i =0; i < (int)T.size() / ncomp; i++) res[i * ncomp + ind] = 0.;
  else dT_Psat_(T,res,ncomp,ind);
}

void Saturation_base::Lvap(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  (P_ref_ > 0) ? _Lvap_(P_ref_,res,ncomp,ind) : Lvap_(P,res,ncomp,ind);
}

void Saturation_base::dP_Lvap(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  if (P_ref_ > 0)
    for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = 0.;
  else dP_Lvap_(P,res,ncomp,ind);
}

void Saturation_base::Hls(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  (P_ref_ > 0) ? _Hls_(P_ref_,res,ncomp,ind) : Hls_(P,res,ncomp,ind);
}

void Saturation_base::dP_Hls(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  if (P_ref_ > 0)
    for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = 0.;
  else dP_Hls_(P,res,ncomp,ind);
}

void Saturation_base::Hvs(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  (P_ref_ > 0) ? _Hvs_(P_ref_,res,ncomp,ind) : Hvs_(P,res,ncomp,ind);
}

void Saturation_base::dP_Hvs(const SpanD P, SpanD res, int ncomp, int ind) const
{
  assert(ncomp * (int )P.size() == (int )res.size());
  if (P_ref_ > 0)
    for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = 0.;
  else dP_Hvs_(P,res,ncomp,ind);
}

void Saturation_base::get_sigma(const SpanD T, const SpanD P, SpanD sig, int ncomp, int ind) const
{
  assert((int )P.size() == (int )sig.size());
  sigma_(T, P, sig, ncomp, ind);
}

void Saturation_base::compute_all_flux_interfacial_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp, int ind) const
{
  assert((int )sats.size() == 8);

  SpanD Ts__ = sats.at(SAT::T_SAT), dPTs__ = sats.at(SAT::T_SAT_DP),
        Hvs__ = sats.at(SAT::HV_SAT), Hls__ = sats.at(SAT::HL_SAT),
        dPHvs__ = sats.at(SAT::HV_SAT_DP), dPHls__ = sats.at(SAT::HL_SAT_DP),
        Lvap__ = sats.at(SAT::LV_SAT), dPLvap__ = sats.at(SAT::LV_SAT_DP);

  assert(ncomp * (int )P.size() == (int )Ts__.size() && ncomp * (int )P.size() == (int )dPTs__.size());
  assert(ncomp * (int )P.size() == (int )Hvs__.size() && ncomp * (int )P.size() == (int )Hls__.size());
  assert(ncomp * (int )P.size() == (int )dPHvs__.size() && ncomp * (int )P.size() == (int )dPHls__.size());
  assert(ncomp * (int )P.size() == (int )Lvap__.size() && ncomp * (int )P.size() == (int )dPLvap__.size());

  Tsat(P, Ts__, ncomp, ind);
  dP_Tsat(P, dPTs__, ncomp, ind);
  Hvs(P, Hvs__, ncomp, ind);
  Hls(P, Hls__, ncomp, ind);
  dP_Hvs(P, dPHvs__, ncomp, ind);
  dP_Hls(P, dPHls__, ncomp, ind);
  Lvap(P, Lvap__, ncomp, ind);
  dP_Lvap(P, dPLvap__, ncomp, ind);
}

void Saturation_base::compute_all_flux_parietal_pb_multiphase(const SpanD P, MSatSpanD sats, int ncomp, int ind) const
{
  assert((int )sats.size() == 3);

  SpanD Ts__ = sats.at(SAT::T_SAT), Lvap__ = sats.at(SAT::LV_SAT), Sigma__ = sats.at(SAT::SIGMA);

  assert(ncomp * (int )P.size() == (int )Ts__.size() && ncomp * (int )P.size() == (int )Lvap__.size());
  assert(ncomp * (int )P.size() == (int )Sigma__.size());

  Tsat(P, Ts__, ncomp, ind);
  Lvap(P, Lvap__, ncomp, ind);
  sigma_(Ts__ ,P, Sigma__, ncomp, ind);
}
