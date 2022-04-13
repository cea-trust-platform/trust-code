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
// File:        Champ_Uniforme.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Uniforme.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Uniforme,"Champ_Uniforme",TRUSTChamp_Divers_generique<Champ_Divers_Type::UNIFORME>);
Add_synonym(Champ_Uniforme,"Uniform_field");

Sortie& Champ_Uniforme::printOn(Sortie& os) const
{
  const DoubleTab& tab = valeurs();
  os << tab.size() << " ";
  for (int i = 0; i < tab.size(); i++) os << tab(0, i);
  return os;
}

Entree& Champ_Uniforme::readOn(Entree& is)
{
  DoubleTab& tab = valeurs();
  int dim = lire_dimension(is, que_suis_je());
  dimensionner(1, dim);
  for (int i = 0; i < dim; i++) is >> tab(0, i);
  return is;
}

Champ_base& Champ_Uniforme::affecter_(const Champ_base& ch)
{
  if (sub_type(Champ_Uniforme, ch)) valeurs() = ch.valeurs();
  else
    {
      Cerr << "We do not know how to make Champ_Uniforme::affecter_(" << ch.que_suis_je() << finl;
      Process::exit();
    }
  return *this;
}
