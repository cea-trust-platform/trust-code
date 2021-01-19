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
// File:        Saturation_constant.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Saturation_constant.h>

Implemente_instanciable(Saturation_constant, "Saturation_constant", Saturation_base);

Sortie& Saturation_constant::printOn(Sortie& os) const
{
  return os;
}

Entree& Saturation_constant::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Tsat", &tsat_);
  param.ajouter("Psat", &psat_);
  param.ajouter("Lvap", &lvap_);
  param.ajouter("Hlsat", &hls_);
  param.ajouter("Hvsat", &hvs_);
  param.lire_avec_accolades_depuis(is);
  // verifications hlsat/hvsat/lvap
  const int i = (lvap_ > 0) + (hls_ > 0) + (hvs_ > 0);
  if (i != 2) Process::exit(que_suis_je() + " Please give 2 properties among {Lvap, Hlsat, Hvsat}");
  if (lvap_ > 0 && hls_ > 0) hvs_ = hls_ + lvap_;
  else if (lvap_ > 0 && hvs_ > 0) hls_ = hvs_ - lvap_;
  else if (hls_ > 0 && hvs_ > 0) lvap_ = hvs_ - hls_;
  else Process::exit(que_suis_je() + "bad parameters");
  return is;
}

double Saturation_constant::Tsat_(const double P) const
{
  return tsat_;
}
double Saturation_constant::dP_Tsat_(const double P) const
{
  return 0;
}
double Saturation_constant::Psat_(const double T) const
{
  return psat_;
}
double Saturation_constant::dT_Psat_(const double T) const
{
  return 0;
}
double Saturation_constant::Lvap_(const double P) const
{
  return lvap_;
}
double Saturation_constant::dP_Lvap_(const double P) const
{
  return 0;
}

double Saturation_constant::Hls_(const double P) const
{
  return hls_;
}

double Saturation_constant::dP_Hls_(const double P) const
{
  return 0;
}

double Saturation_constant::Hvs_(const double P) const
{
  return hvs_;
}

double Saturation_constant::dP_Hvs_(const double P) const
{
  return 0;
}
