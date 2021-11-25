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
// File:        Saturation_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Saturation_base.h>
Implemente_base(Saturation_base, "Saturation_base", Objet_U);
// XD saturation_base objet_u saturation_base -1 Basic class for phase change management (used in pb_multiphase)
Sortie& Saturation_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Saturation_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("P_ref", &P_ref_);
  param.ajouter("T_ref", &T_ref_);
  param.ajouter("tension_superficielle", &sigma__);
  param.lire_avec_accolades_depuis(is);
  return is;
}

double Saturation_base::get_Pref() const
{
  return P_ref_;
}

double Saturation_base::Tsat(const double Pi) const
{
  const double P = P_ref_ > 0 ? P_ref_ : Pi;
  return Tsat_(P);
}
double Saturation_base::dP_Tsat(const double P) const
{
  return P_ref_ > 0 ? 0 : dP_Tsat_(P);
}
double Saturation_base::Psat(const double Ti) const
{
  const double T = T_ref_ > 0 ? T_ref_ : Ti;
  return Psat_(T);
}
double Saturation_base::dT_Psat(const double T) const
{
  return T_ref_ > 0 ? 0 : dT_Psat_(T);
}
double Saturation_base::Lvap(const double Pi) const
{
  const double P = P_ref_ > 0 ? P_ref_ : Pi;
  return Lvap_(P);
}
double Saturation_base::dP_Lvap(const double P) const
{
  return P_ref_ > 0 ? 0 : dP_Lvap_(P);
}

double Saturation_base::Hls(const double Pi) const
{
  const double P = P_ref_ > 0 ? P_ref_ : Pi;
  return Hls_(P);
}
double Saturation_base::dP_Hls(const double P) const
{
  return P_ref_ > 0 ? 0 : dP_Hls_(P);
}
double Saturation_base::Hvs(const double Pi) const
{
  const double P = P_ref_ > 0 ? P_ref_ : Pi;
  return Hvs_(P);
}
double Saturation_base::dP_Hvs(const double P) const
{
  return P_ref_ > 0 ? 0 : dP_Hvs_(P);
}
double  Saturation_base::sigma(const double T, const double P) const
{
  return sigma__ > 0 ? sigma__ : sigma_(T,P);
}
