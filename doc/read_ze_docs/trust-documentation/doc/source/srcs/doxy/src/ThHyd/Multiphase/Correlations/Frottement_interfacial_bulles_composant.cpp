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

#include <Frottement_interfacial_bulles_composant.h>
#include <Pb_Multiphase.h>
#include <TRUSTTab.h>

Implemente_instanciable(Frottement_interfacial_bulles_composant, "Frottement_interfacial_bulles_composant|Frottement_interfacial_bulles", Frottement_interfacial_base);

Sortie& Frottement_interfacial_bulles_composant::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_bulles_composant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("coeff_derive", &C_d_, Param::REQUIRED);
  param.ajouter("rayon_bulle", &r_bulle_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");

  return is;
}

void Frottement_interfacial_bulles_composant::coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                                                          const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                                                          const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const
{
  int k,l,N = ndv.dimension(0);

  coeff = 0.;

// case r_bulle < 0 <=> sedimentation test case, robust
  double rho_m = 0;
  for (k = 0; k < N; k++) rho_m += alpha(k) * rho(k);
  for (k = 0; k < N; k++)
    for (l = 0; l < N; l++)
      if (l != k)
        if (r_bulle_>0) coeff(k, l, 0) = (coeff(k, l, 1) = 1. / 8 * C_d_ * 3 * alpha(k) * alpha(l) / r_bulle_ * rho_m) * ndv(k, l);
}
