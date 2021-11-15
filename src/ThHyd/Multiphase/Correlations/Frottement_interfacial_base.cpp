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
// File:        Frottement_interfacial_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Frottement_interfacial_base.h>
Implemente_base(Frottement_interfacial_base, "Frottement_interfacial_base", Correlation_base);
// XD frottement_interfacial source_base frottement_interfacial 0 Source term which corresponds to the phases friction at the interface
// XD attr model chaine(into=["bulles","wallis","sonnenburg"]) model 1 Correlation for friction in bubbly flows if bulles, Correlation for drift flux of Sonnenburg if sonnenburg or Correlation for friction in annular flows if wallis
// XD attr bloc_bulles bloc_b bloc_bulles 1 not set
// XD bloc_b interprete nul 1 not set
// XD attr rayon_bulle floattant rayon_bulle 1 Radius of the bubbles (useful for the correlation and it is required)
// XD attr coeff_derive floattant coeff_derive 1 Drift coefficient (useful for the correlation and it is required)

Sortie& Frottement_interfacial_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_base::readOn(Entree& is)
{
  return is;
}
