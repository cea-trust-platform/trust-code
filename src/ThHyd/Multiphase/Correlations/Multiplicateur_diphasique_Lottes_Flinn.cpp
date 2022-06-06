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

#include <Multiplicateur_diphasique_Lottes_Flinn.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Multiplicateur_diphasique_Lottes_Flinn, "Multiplicateur_diphasique_Lottes_Flinn", Multiplicateur_diphasique_base);

Sortie& Multiplicateur_diphasique_Lottes_Flinn::printOn(Sortie& os) const
{
  return os;
}

Entree& Multiplicateur_diphasique_Lottes_Flinn::readOn(Entree& is)
{
  alpha_min_ = 0.9, alpha_max_ = 0.95, n_l = -1, n_g = -1;
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

void Multiplicateur_diphasique_Lottes_Flinn::coefficient(const double *alpha, const double *rho, const double *v, const double *f,
                                                         const double *mu, const double Dh, const double gamma, const double *Fk,
                                                         const double Fm, DoubleTab& coeff) const
{
  double a_tot = alpha[n_l] + (n_g >= 0 ? alpha[n_g] : 0), a_l = alpha[n_l] / a_tot, a_g = n_g >= 0 ? alpha[n_g] / a_tot : 0,
         a_m = alpha_min_, b_m = 1 - a_m, a_M = alpha_max_, b_M = 1 - a_M;

  coeff = 0;
  coeff(n_l, 0) = a_l < b_m ? std::max(a_l - b_M, 0.) / (b_m * b_m) : 1. / (a_l * a_l);
  if (n_g >= 0) coeff(n_g, 0) = std::min(std::max((a_g - a_m) / (a_M - a_m), 0.), 1.);
}
