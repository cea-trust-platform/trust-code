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

#include <Portance_interfaciale_Tomiyama.h>
#include <Pb_Multiphase.h>
#include <math.h>

Implemente_instanciable(Portance_interfaciale_Tomiyama, "Portance_interfaciale_Tomiyama", Portance_interfaciale_base);

Sortie& Portance_interfaciale_Tomiyama::printOn(Sortie& os) const
{
  return os;
}

Entree& Portance_interfaciale_Tomiyama::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("constante_gravitation", &g_);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  return is;
}

void Portance_interfaciale_Tomiyama::coefficient(const input_t& in, output_t& out) const
{
  int k, N = out.Cl.dimension(0);

  for (k = 0; k < N; k++)
    if (k!=n_l) // k gas phase
      {
        int ind_trav = (k>n_l) ? (n_l*(N-1)-(n_l-1)*(n_l)/2) + (k-n_l-1) : (k*(N-1)-(k-1)*(k)/2) + (n_l-k-1);

        double Re = in.rho[n_l] * in.nv(n_l, k) * in.d_bulles[k]/in.mu[n_l];
        double Eo = g_ * std::abs(in.rho[n_l]-in.rho[k]) * in.d_bulles[k]*in.d_bulles[k]/in.sigma[ind_trav];
        double f_Eo = .00105*Eo*Eo*Eo - .0159*Eo*Eo - .0204*Eo + .474;
        double Cl;
        if (Eo<4) Cl = std::min( .288*std::tanh( .121*Re ), f_Eo) ;
        else      Cl = f_Eo ;

        out.Cl(k, n_l) = Cl * in.rho[n_l] * in.alpha[k] ;
        out.Cl(n_l, k) =  out.Cl(k, n_l);
      }
}
