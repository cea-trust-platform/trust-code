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

#include <Multiplicateur_diphasique_homogene.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Multiplicateur_diphasique_homogene, "Multiplicateur_diphasique_homogene", Multiplicateur_diphasique_base);

Sortie& Multiplicateur_diphasique_homogene::printOn(Sortie& os) const
{
  return os;
}

Entree& Multiplicateur_diphasique_homogene::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("alpha_min", &alpha_min_);
  param.ajouter("alpha_max", &alpha_max_);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) n_l = 0, n_g = -1; //pas un Pb_Multiphase -> monophasique liquide
  else for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
      if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
      else if (pbm->nom_phase(n).debute_par("gaz") && (n_g < 0 || pbm->nom_phase(n).finit_par("continu"))) n_g = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : main phase not found!");

  return is;
}

void Multiplicateur_diphasique_homogene::coefficient(const double *alpha, const double *rho, const double *v, const double *f,
                                                     const double *mu, const double Dh, const double gamma, const double *Fk,
                                                     const double Fm, DoubleTab& coeff) const
{
  double G = alpha[n_l] * rho[n_l] * std::fabs(v[n_l]) + alpha[n_g] * rho[n_g] * std::fabs(v[n_g]), //debit total
         x = G ? alpha[n_g] * rho[n_g] * v[n_g] / G : 0, //titre
         Phi2 = 1 + x * (rho[n_l] / rho[n_g] - 1), //le multiplicateur!
         frac_g = std::min(std::max((alpha[n_g] - alpha_min_) / (alpha_max_ - alpha_min_), 0.), 1.), frac_l = 1 - frac_g; //fraction appliquee a la vapeur
  coeff = 0;
  coeff(n_l, 0) = frac_l * Phi2;
  coeff(n_g, 0) = frac_g ? frac_g / (alpha[n_g] * alpha[n_g]) : 0; //basculement vers le frottement "vapeur seule"
}
