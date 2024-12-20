/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Flux_parietal_Nusselt.h>
#include <Milieu_composite.h>
#include <Pb_Multiphase.h>
#include <cmath>

Implemente_instanciable(Flux_parietal_Nusselt, "Flux_parietal_Nusselt", Flux_parietal_base);

Sortie& Flux_parietal_Nusselt::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_parietal_Nusselt::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("formula", this, Param::REQUIRED);
  param.ajouter("Hnb", &Hnb_);
  param.ajouter("Tsat", &Tsat_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Flux_parietal_Nusselt::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "formula")
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Reading and interpreting the function " << tmp << " ... ";
      nusselt_.setNbVar(2);
      nusselt_.setString(tmp);
      nusselt_.addVar("Re");
      nusselt_.addVar("Pr");
      nusselt_.parseString();
      return 1;
    }
  else
    return Flux_parietal_base::lire_motcle_non_standard(mot, is);
}

void Flux_parietal_Nusselt::qp(const input_t& in, output_t& out) const
{
  const double Re = in.rho[0] * in.v[0] * in.D_h / in.mu[0], Pr = in.mu[0] * in.Cp[0] / in.lambda[0];
  nusselt_.setVar("Re", Re);
  nusselt_.setVar("Pr", Pr);
  const double Nu = nusselt_.eval();

  const double DT = in.Tp - std::min(Tsat_, in.T[0]), dTf_DT = -1.0, dTp_DT = 1.0, dp_DT = 0.0, h_mono = in.lambda[0] / in.D_ch * Nu;
  const double qnb = in.Tp >= Tsat_ ? Hnb_ * pow(in.Tp - Tsat_, a_) : 0.0, dTp_DTnb = in.Tp >= Tsat_ ? a_ * pow(in.Tp - Tsat_, a_ - 1) : 0.0;

  if (out.qpk)            (*out.qpk)(0) = h_mono * DT + qnb;
  if (out.dTf_qpk) (*out.dTf_qpk)(0, 0) = h_mono * dTf_DT;
  if (out.dTp_qpk)    (*out.dTp_qpk)(0) = h_mono * dTp_DT + Hnb_ * dTp_DTnb;
  if (out.dp_qpk)      (*out.dp_qpk)(0) = h_mono * dp_DT;
}
