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

#include <Value_Input_Int.h>
#include <iostream>
#include <Probleme_base.h>
#include <Interprete.h>
#include <Param.h>

using std::string;

Implemente_instanciable(Value_Input_Int,"Value_Input_Int",Objet_U);

Sortie& Value_Input_Int::printOn(Sortie& s ) const
{
  return s;
}

Entree& Value_Input_Int::readOn(Entree& is )
{
  Param param(que_suis_je());
  set_param(param);

  Nom nom_pb;
  param.ajouter("probleme",&nom_pb,Param::REQUIRED);

  int value = 0;
  param.ajouter("initial_value",&value); //useful for Navier-Stokes equation initialization

  param.lire_avec_accolades_depuis(is);

  //complete Probleme_base register
  mon_pb=ref_cast(Probleme_base,Interprete::objet(nom_pb));
  mon_pb.valeur().setInputIntValue(name,value);

  return is;
}

void Value_Input_Int::set_param(Param& param)
{
  param.ajouter("nom",&name, Param::REQUIRED);
}


