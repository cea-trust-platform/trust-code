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
// File:        Fluide_sodium_gaz.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_sodium_gaz.h>
#include <Lois_sodium.h>

Implemente_instanciable(Fluide_sodium_gaz, "Fluide_sodium_gaz", Fluide_reel_base);

Sortie& Fluide_sodium_gaz::printOn(Sortie& os) const
{
  return os;
}

Entree& Fluide_sodium_gaz::readOn(Entree& is)
{
  Fluide_reel_base::readOn(is);
  return is;
}

double Fluide_sodium_gaz::rho_(const double T, const double P) const
{
  return RhoV(T, P);
}

double Fluide_sodium_gaz::dT_rho_(const double T, const double P) const
{
  return DTRhoV(T, P);
}

double Fluide_sodium_gaz::dP_rho_(const double T, const double P) const
{
  return DPRhoV(T, P);
}

double Fluide_sodium_gaz::cp_(const double T, const double P) const
{
  const double eint = HV(T, P) - P / rho_(T, P);
  return eint / T;
}

double Fluide_sodium_gaz::dT_cp_(const double T, const double P) const
{
  Process::exit("pas code");
  return 0;
}

double Fluide_sodium_gaz::dP_cp_(const double T, const double P) const
{
  Process::exit("pas code");
  return 0;
}

double Fluide_sodium_gaz::mu_(const double T) const
{
  return MuV(T);
}

double Fluide_sodium_gaz::lambda_(const double T) const
{
  return LambdaV(T);
}

double Fluide_sodium_gaz::beta_(const double T, const double P) const
{
  return DTIRhoV(T, P) / IRhoV(T, P);
}
