/****************************************************************************
* Copyright (c) 2021, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Multiplicateur_diphasique_Muhler_Steinhagen.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Multiplicateur_diphasique_Muhler_Steinhagen.h>
#include <Pb_Multiphase.h>
#include <cmath>

Implemente_instanciable(Multiplicateur_diphasique_Muhler_Steinhagen, "Multiplicateur_diphasique_Muhler_Steinhagen", Multiplicateur_diphasique_base);

Sortie& Multiplicateur_diphasique_Muhler_Steinhagen::printOn(Sortie& os) const
{
  return os;
}

Entree& Multiplicateur_diphasique_Muhler_Steinhagen::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("alpha_min", &alpha_min_);
  param.ajouter("alpha_max", &alpha_max_);
  param.ajouter("min_lottes_flinn", &min_lottes_flinn_);
  param.ajouter("min_sensas", &min_sensas_);
  param.ajouter("a", &a_);
  param.ajouter("b", &b_);
  param.ajouter("c", &c_);
  param.lire_avec_accolades_depuis(is);

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) n_l = 0, n_g = -1; //pas un Pb_Multiphase -> monophasique liquide
  else for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
      if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
      else if (pbm->nom_phase(n).debute_par("gaz") && (n_g < 0 || pbm->nom_phase(n).finit_par("continu"))) n_g = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : main phase not found!");

  return is;
}

void Multiplicateur_diphasique_Muhler_Steinhagen::coefficient(const double *alpha, const double *rho, const double *v, const double *f,
                                                              const double *mu, const double Dh, const double gamma, const double *Fk,
                                                              const double Fm, DoubleTab& coeff) const
{
  int min_ = min_sensas_ || min_lottes_flinn_;
  double G = alpha[n_l] * rho[n_l] * std::fabs(v[n_l]) + alpha[n_g] * rho[n_g] * std::fabs(v[n_g]), //debit total
         x = G ? alpha[n_g] * rho[n_g] * v[n_g] / G : 0, //titre
         fm_sur_rhom = (f[n_l] / rho[n_l] + a_ * (f[n_g] / rho[n_g] - f[n_l] / rho[n_l]) * std::pow(x, b_)) * std::pow(1 - x, 1. / c_) + f[n_g] / rho[n_g] * std::pow(x, c_),
         frac_g = std::min(std::max((alpha[n_g] - alpha_min_) / (alpha_max_ - alpha_min_), 0.), 1.), frac_l = 1 - frac_g, //fraction appliquee au liquide
         mul = min_sensas_ ? std::min(1., 1.4429 * std::pow(alpha[n_l], 0.6492)) : 1;
  coeff = 0;
  /* si min_ == 1 et si Lottes-Flinn/SENSAS donne un frottement plus bas -> on le prend */
  if (min_ && Fk[n_l] * mul < rho[n_l] / f[n_l] * fm_sur_rhom * Fm * (alpha[n_l] * alpha[n_l]))
    coeff(n_l, 0) = mul / (alpha[n_l] * alpha[n_l]);
  else coeff(n_l, 1) = frac_l * rho[n_l] / f[n_l] * fm_sur_rhom, coeff(n_g, 1) = frac_g * rho[n_g] / f[n_g] * fm_sur_rhom;
}
