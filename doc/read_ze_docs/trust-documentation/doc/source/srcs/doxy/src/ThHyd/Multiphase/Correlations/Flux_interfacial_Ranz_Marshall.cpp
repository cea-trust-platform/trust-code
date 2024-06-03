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

#include <Flux_interfacial_Ranz_Marshall.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Flux_interfacial_Ranz_Marshall, "Flux_interfacial_Ranz_Marshall", Flux_interfacial_base);

Sortie& Flux_interfacial_Ranz_Marshall::printOn(Sortie& os) const { return Flux_interfacial_base::printOn(os); }
Entree& Flux_interfacial_Ranz_Marshall::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("dv_min", &dv_min_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Flux_interfacial_Ranz_Marshall::completer()
{
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (!pbm->has_champ("diametre_bulles")) Process::exit("Flux_interfacial_Ranz_Mashall : bubble diameter needed !");
}

void Flux_interfacial_Ranz_Marshall::coeffs(const input_t& in, output_t& out) const
{
  out.hi = 0.0, out.da_hi = 0.0;
  int k, N = out.hi.dimension(0);
  for (k = 0; k < N; k++)
    if (k != n_l)
      {
        double Re_b = in.rho[n_l] * in.nv[N * n_l + k] * in.d_bulles[k] / in.mu[n_l];
        double Pr = in.mu[n_l] * in.Cp[n_l] / in.lambda[n_l];
        double Nu = 2.0 + 0.6* std::pow(Re_b, 0.5)*std::pow(Pr, 0.3);
        out.hi(n_l, k) = Nu * in.lambda[n_l] / in.d_bulles[k] * 6.0 * std::max(in.alpha[k], a_min) / in.d_bulles[k]; // std::max() pour que le flux interfacial sont non nul
        out.da_hi(n_l, k, k) = in.alpha[k] > a_min ? Nu * in.lambda[n_l] * 6.0 / (in.d_bulles[k] * in.d_bulles[k]) : 0;
        out.hi(k, n_l) = 1e8;
      }
}
