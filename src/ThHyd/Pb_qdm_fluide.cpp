/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Pb_qdm_fluide.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_qdm_fluide.h>

Implemente_base(Pb_qdm_fluide,"Pb_qdm_fluide",Probleme_base);

Sortie& Pb_qdm_fluide::printOn(Sortie& os) const
{
  Probleme_base::printOn(os);
  return os;
}

Entree& Pb_qdm_fluide::readOn(Entree& is)
{
  Probleme_base::readOn(is);
  return is ;
}

int Pb_qdm_fluide::expression_predefini(const Motcle& motlu, Nom& expression)
{
  if (motlu=="ENERGIE_CINETIQUE_TOTALE")
    {
      expression = "predefini { pb_champ ";
      expression += le_nom();
      expression += " energie_cinetique_totale } ";
      return 1;
    }
  else if (motlu=="ENERGIE_CINETIQUE_ELEM")
    {
      expression = "predefini { pb_champ ";
      expression += le_nom();
      expression += " energie_cinetique_elem } ";
      return 1;
    }
  else
    return Probleme_base::expression_predefini(motlu,expression);
  return 0;
}
