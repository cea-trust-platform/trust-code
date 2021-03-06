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

#include <Saturation_sodium.h>
#include <Lois_sodium.h>

Implemente_instanciable(Saturation_sodium, "Saturation_sodium", Saturation_base);
// XD saturation_sodium saturation_base saturation_sodium -1 Class for saturation sodium
// XD attr P_ref floattant P_ref 1 Use to fix the pressure value in the closure law. If not specified, the value of the pressure unknown will be used
// XD attr T_ref floattant T_ref 1 Use to fix the temperature value in the closure law. If not specified, the value of the temperature unknown will be used

Sortie& Saturation_sodium::printOn(Sortie& os) const
{
  return os;
}

Entree& Saturation_sodium::readOn(Entree& is)
{
  return Saturation_base::readOn(is);
}

double Saturation_sodium::Tsat_(const double P) const
{
  return Tsat_Na(P);
}
double Saturation_sodium::dP_Tsat_(const double P) const
{
  return DTsat_Na(P);
}
double Saturation_sodium::Psat_(const double T) const
{
  return Psat_Na(T);
}
double Saturation_sodium::dT_Psat_(const double T) const
{
  return DPsat_Na(T);
}
double Saturation_sodium::Lvap_(const double P) const
{
  return Lvap_Na(P);
}
double Saturation_sodium::dP_Lvap_(const double P) const
{
  return DLvap_Na(P);
}
double Saturation_sodium::Hls_(const double P) const
{
  return Hsat(P);
}
double Saturation_sodium::dP_Hls_(const double P) const
{
  return DHsat(P);
}
double Saturation_sodium::Hvs_(const double P) const
{
  return Hsat(P) + Lvap_Na(P);
}
double Saturation_sodium::dP_Hvs_(const double P) const
{
  return DHsat(P) + DLvap_Na(P);
}
double Saturation_sodium::sigma_(const double T, const double P) const
{
  return SigmaL(T);
}
