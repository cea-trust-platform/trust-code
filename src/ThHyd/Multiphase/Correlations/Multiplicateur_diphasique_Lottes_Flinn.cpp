/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Multiplicateur_diphasique_Lottes_Flinn.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Multiplicateur_diphasique_Lottes_Flinn.h>

Implemente_instanciable(Multiplicateur_diphasique_Lottes_Flinn, "Multiplicateur_diphasique_Lottes_Flinn", Multiplicateur_diphasique_base);

Sortie& Multiplicateur_diphasique_Lottes_Flinn::printOn(Sortie& os) const
{
  return os;
}

Entree& Multiplicateur_diphasique_Lottes_Flinn::readOn(Entree& is)
{
  alpha_min_ = 0.9, alpha_max_ = 0.95;
  Param param(que_suis_je());
  param.ajouter("alpha_min", &alpha_min_);
  param.ajouter("alpha_max", &alpha_max_);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Multiplicateur_diphasique_Lottes_Flinn::coefficient(const double *alpha, const DoubleTab& Fk, double Fm, DoubleTab& coeff) const
{
  if (Fk.size() != 2) Cerr << que_suis_je() << " : nombre de phases " << Fk.size() << " != 2 non supporte!" << finl, Process::exit();
  double a_l = alpha[0], a_g = alpha[1], a_m = alpha_min_, b_m = 1 - a_m, a_M = alpha_max_, b_M = 1 - a_M;

  coeff(0, 0) = a_l < b_m ? max(a_l - b_M, 0.) / (b_m * b_m) : 1. / (a_l * a_l);
  coeff(1, 0) = min(max((a_g - a_m) / (a_M - a_m), 0.), 1.);
  coeff(0, 1) = coeff(1, 1) = 0; //rien depuis le melange
}
