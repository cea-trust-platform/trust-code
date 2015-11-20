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
// File:        ICoCoTrioFieldUtil.cpp
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

#include <ICoCoTrioFieldUtil.h>
// pour ajouter une dependance pour avoir le .o dans ICoCo
#include <ICoCoField.h>
#include <string.h>
#include <iostream>
#include <iomanip>

using ICoCo::TrioField;


// Ajoute 1 a 1 les valeurs du TrioField donnee en parametre a celles de this
// Verifie la compatibilite des dimensionnements

void ajoute_field(ICoCo::TrioField& a , const ICoCo::TrioField& OtherField)
{

  bool compat=true;
  compat &= (a._type==OtherField._type);
  compat &= (a._mesh_dim==OtherField._mesh_dim);
  compat &= (a._space_dim==OtherField._space_dim);
  compat &= (a._nbnodes==OtherField._nbnodes);
  compat &= (a._nodes_per_elem==OtherField._nodes_per_elem);
  compat &= (a._nb_elems==OtherField._nb_elems);
  compat &= (a._itnumber==OtherField._itnumber);
  compat &= (a._time1==OtherField._time1);
  compat &= (a._time2==OtherField._time2);
  compat &= (a.nb_values()==OtherField.nb_values());
  compat &= (a._nb_field_components==OtherField._nb_field_components);
  if (!compat)
    throw 0;

  for (int i=0; i<a.nb_values()*a._nb_field_components; i++)
    a._field[i]+=OtherField._field[i];


}

