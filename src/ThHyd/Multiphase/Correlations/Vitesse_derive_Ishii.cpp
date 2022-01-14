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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Vitesse_derive_Ishii.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Vitesse_derive_Ishii.h>
#include <Pb_Multiphase.h>
#include <cmath>

Implemente_instanciable(Vitesse_derive_Ishii, "Vitesse_derive_Ishii", Vitesse_derive_base);

Sortie& Vitesse_derive_Ishii::printOn(Sortie& os) const
{
  return os;
}

Entree& Vitesse_derive_Ishii::readOn(Entree& is)
{
  //identification des phases
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;
  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche des id_composite des phases liquide et gaz
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
    else if (pbm->nom_phase(n).debute_par("gaz") && (n_g < 0 || pbm->nom_phase(n).finit_par("continu"))) n_g = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");
  if (n_g < 0) Process::exit(que_suis_je() + " : gas phase not found!");

  Param param(que_suis_je());
  param.ajouter("diametre_hydraulique", &Dh_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Vitesse_derive_Ishii::vitesse_derive(const double& Dh, const DoubleTab& alpha, const DoubleTab& rho, const DoubleTab& g, DoubleTab& C0, DoubleTab& vg0) const
{
  double gz = 10.;
  double D = dimension;
  double C0_ = 1.2 - 0.2 * std::sqrt(rho(n_g) / rho(n_l));

  // A FAIRE : calculs a modifier selon la valeur de alpha
  // Note : la derniere dimension donne le sens de l'apesanteur

  C0(n_g, n_l) = 1 - alpha(n_g) * C0_;
  C0(n_l, n_g) = -alpha(n_l) * C0_;

  /* vitesse de separation du gaz */
  vg0(n_g, n_l, D - 1) = 0.35 * std::sqrt(gz * (rho(n_l) - rho(n_g)) * Dh_ / rho(n_l));	// competition entre flottabilite et acceleration
  vg0(n_l, n_g, D - 1) = vg0(n_g, n_l, D - 1); // la derniere dimension donne le sens de l'apesanteur
  for (int d = 0; d < D - 1; d++)
    {
      vg0(n_g, n_l, d) = 0;
      vg0(n_l, n_g, d) = 0;
    }
}
