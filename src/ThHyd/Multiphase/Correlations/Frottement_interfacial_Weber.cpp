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
// File:        Frottement_interfacial_Weber.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Frottement_interfacial_Weber.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Frottement_interfacial_Weber, "Frottement_interfacial_Weber", Frottement_interfacial_base);

Sortie& Frottement_interfacial_Weber::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_Weber::readOn(Entree& is)
{
  /* lecture du nombre de Weber critique */
  Param param(que_suis_je());
  param.ajouter("Weber_critique", &We_c);
  param.lire_avec_accolades_depuis(is);

  //identification des phases
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;
  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
    else if (pbm->nom_phase(n).debute_par("gaz") && (n_g < 0 || pbm->nom_phase(n).finit_par("continu"))) n_g = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");
  if (n_g < 0) Process::exit(que_suis_je() + " : gas phase not found!");

  return is;
}

void Frottement_interfacial_Weber::coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                                               const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                                               const DoubleTab& ndv, int e, DoubleTab& coeff) const
{
  double Db = We_c * sigma(n_l, n_g) / (rho(n_l) * ndv(n_l, n_g) * ndv(n_l, n_g)),					//diametre des bulles
         Reb = rho(n_l) * ndv(n_l, n_g) * Db / mu(n_l), 											//Reynolds associe a une bulle
         Cx = 24. / Reb * (1. + 0.1 * pow(Reb, 0.75)); 												//coefficient de trainee

  const double pi=2.*acos(0.);
  coeff = 0;
  coeff(n_l, n_g, 1) = coeff(n_g, n_l, 1) = 6. * alpha(n_g) / (pi * pow(Db, 3.)) * Cx ;
  coeff(n_l, n_g, 0) = coeff(n_g, n_l, 0) = coeff(n_l, n_g, 1) * ndv(n_l, n_g);
}
