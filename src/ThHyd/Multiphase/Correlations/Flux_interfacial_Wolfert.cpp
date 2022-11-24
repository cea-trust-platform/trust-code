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

#include <Flux_interfacial_Wolfert.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Flux_interfacial_Wolfert, "Flux_interfacial_Wolfert", Flux_interfacial_base);

Sortie& Flux_interfacial_Wolfert::printOn(Sortie& os) const
{
  return os;
}

Entree& Flux_interfacial_Wolfert::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Pr_t", &Pr_t_);
  param.lire_avec_accolades_depuis(is);

  return is;
}

void Flux_interfacial_Wolfert::completer()
{
  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (!pbm->has_champ("diametre_bulles")) Process::exit("Flux_interfacial_Wolfert : bubble diameter needed !");
}

void Flux_interfacial_Wolfert::coeffs(const input_t& in, output_t& out) const
{
  int k, N = out.hi.dimension(0);
  for (k = 0; k < N; k++)
    if (k != n_l)
      {
        int ind_trav = (k>n_l) ? (n_l*(N-1)-(n_l-1)*(n_l)/2) + (k-n_l-1) : (k*(N-1)-(k-1)*(k)/2) + (n_l-k-1);
        double Ja = in.rho[n_l] * in.Cp[n_l] * (in.T[k] -  in.T[n_l]) / (in.rho[k] * in.Lvap[ind_trav] );
        double Pe = in.rho[n_l] * in.Cp[n_l] * in.nv[N * n_l + k] * in.d_bulles[k] / in.lambda[n_l];

        double Nu = 12./M_PI*Ja  + 2/std::sqrt(M_PI)*(1+in.nut[n_l]*in.rho[n_l]*in.Cp[n_l]/in.lambda[n_l])*std::sqrt(Pe);

        out.hi(n_l, k) = Nu * in.lambda[n_l] / in.d_bulles[k] * 6 * std::max(in.alpha[k], 1e-4) / in.d_bulles[k] ; // std::max() pour que le flux interfacial sont non nul
        out.da_hi(n_l, k, k) = in.alpha[k] > 1e-4 ?
                               Nu * in.lambda[n_l] * 6. / (in.d_bulles[k]*in.d_bulles[k]) :
                               0. ;
        out.hi(k, n_l) = 1e8;
      }
}
