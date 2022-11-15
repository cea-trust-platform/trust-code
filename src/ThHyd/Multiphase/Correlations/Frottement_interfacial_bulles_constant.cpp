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

#include <Frottement_interfacial_bulles_constant.h>
#include <Pb_Multiphase.h>
#include <TRUSTTab.h>

Implemente_instanciable(Frottement_interfacial_bulles_constant, "Frottement_interfacial_bulles_constant", Frottement_interfacial_base);

Sortie& Frottement_interfacial_bulles_constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_bulles_constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("coeff_derive", &C_d_, Param::REQUIRED);
  param.ajouter("rayon_bulle", &r_bulle_);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");

  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if ((r_bulle_<0.) && (!pbm->has_champ("diametre_bulles"))) Process::exit(que_suis_je() + " : there must be a bubble diameter defined in the problem or a bubble radius defined in the force !");

  return is;
}

void Frottement_interfacial_bulles_constant::coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                                                         const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                                                         const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const
{
  int k,N = ndv.dimension(0);

  coeff = 0.;

  for (k = 0; k < N; k++)
    if (k!=n_l)
      {
        if (r_bulle_>0) coeff(k, n_l, 1) = 3. / 8. * C_d_ * alpha(k) / r_bulle_    * rho(n_l) ;
        else            coeff(k, n_l, 1) = 3. / 4. * C_d_ * alpha(k) / d_bulles(k) * rho(n_l) ;
        coeff(n_l, k, 1) = coeff(k, n_l, 1);
        coeff(k, n_l, 0) = coeff(k, n_l, 1) * ndv(n_l,k);
        coeff(n_l, k, 0) = coeff(n_l, k, 1) * ndv(n_l,k);
      }
}
