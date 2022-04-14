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
// File:        Ch_input_uniforme.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs/Champs_Don
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_input_uniforme.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Interprete.h>

Implemente_instanciable(Ch_input_uniforme,"Champ_input_uniforme",Champ_Uniforme);


Entree& Ch_input_uniforme::readOn(Entree& is)
{
  sous_zone_ok=false;
  read(is);
  valeurs().resize(1, nb_compo_);
  if (initial_value_.size_array()) for (int i = 0; i < nb_compo_; i++) valeurs()(0, i) = initial_value_[i];
  mon_pb->addInputField(*this);
  return is;
}

Sortie& Ch_input_uniforme::printOn(Sortie& os) const
{
  return os;
}

void Ch_input_uniforme::set_nb_comp(int i)
{
  fixer_nb_comp(i);
}

void Ch_input_uniforme::set_name(const Nom& name)
{
  nommer(name);
}

const Nom& Ch_input_uniforme::get_name() const
{
  return le_nom();
}

// Description
// Provides afield with a dummy geometry, a name, a time interval,
// components, and no field ownership.
void Ch_input_uniforme::getTemplate(TrioField& afield) const
{

  afield.dummy_geom();
  afield.setName(le_nom().getChar());

  afield._time1=mon_pb->presentTime();
  afield._time2=mon_pb->futureTime();

  afield._nb_field_components=nb_comp();
}

// Description
// Uses the first value in afield as uniform value, regardless of geometry.
void Ch_input_uniforme::setValue(const TrioField& afield)
{
  Champ_Input_Proto::setValueOnTab(afield, valeurs());
}
