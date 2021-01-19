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
// File:        Correlation.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Correlation.h>

Implemente_deriv(Correlation_base);
Implemente_instanciable(Correlation,"Correlation0",DERIV(Correlation_base));

Sortie& Correlation::printOn(Sortie& os) const
{
  return DERIV(Correlation_base)::printOn(os);
}

void Correlation::set_type_prefix(const Nom& nom)
{
  type_prefix_ = nom;
}

Entree& Correlation::readOn(Entree& is)
{
  /* le premier mot sert a typer la correlation*/
  Nom nom;
  is >> nom;
  DERIV(Correlation_base)::typer(type_prefix_ != "??" ? type_prefix_ + "_" + nom : nom);
  /* le reste lui est passe en parametre */
  return valeur().lire(is);
}

void Correlation::associer_pb_multiphase(const Pb_Multiphase& pb)
{
  valeur().associer_pb_multiphase(pb);
}

Declare_ref(Correlation_base);
