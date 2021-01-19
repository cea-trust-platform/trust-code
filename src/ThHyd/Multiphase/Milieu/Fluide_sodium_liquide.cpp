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
// File:        Fluide_sodium_liquide.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_sodium_liquide.h>
#include <Lois_sodium.h>

Implemente_instanciable(Fluide_sodium_liquide, "Fluide_sodium_liquide", Fluide_reel_base);

Sortie& Fluide_sodium_liquide::printOn(Sortie& os) const
{
  return os;
}

Entree& Fluide_sodium_liquide::readOn(Entree& is)
{
  Fluide_reel_base::readOn(is);
  return is;
}

double Fluide_sodium_liquide::rho_(const double T, const double P) const
{
  return RhoL(T, P);
}

double Fluide_sodium_liquide::dT_rho_(const double T, const double P) const
{
  return DTRhoL(T, P);
}

double Fluide_sodium_liquide::dP_rho_(const double T, const double P) const
{
  return DPRhoL(T, P);
}

double Fluide_sodium_liquide::h_(const double T, const double P) const
{
  return HL(T, P);
}

double Fluide_sodium_liquide::dT_h_(const double T, const double P) const
{
  return DTHL(T, P);
}

double Fluide_sodium_liquide::dP_h_(const double T, const double P) const
{
  return DPHL(T, P);
}

double Fluide_sodium_liquide::cp_(const double T, const double P) const
{
  return DTHL(T, P);
}

double Fluide_sodium_liquide::mu_(const double T) const
{
  return MuL(T);
}

double Fluide_sodium_liquide::lambda_(const double T) const
{
  return LambdaL(T);
}

double Fluide_sodium_liquide::beta_(const double T, const double P) const
{
  return DTIRhoL(T) / IRhoL(T);
}
