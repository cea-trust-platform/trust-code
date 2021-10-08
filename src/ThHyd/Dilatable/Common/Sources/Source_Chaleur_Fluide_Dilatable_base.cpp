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
// File:        Source_Chaleur_Fluide_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Chaleur_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>

Implemente_base(Source_Chaleur_Fluide_Dilatable_base,"Source_Chaleur_Fluide_Dilatable_base",Source_base);

Sortie& Source_Chaleur_Fluide_Dilatable_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Source_Chaleur_Fluide_Dilatable_base::readOn(Entree& is)
{
  return is;
}

void Source_Chaleur_Fluide_Dilatable_base::completer()
{
  Cerr<<"Source_Chaleur_Fluide_Dilatable_base::completer()"<<finl;
  Source_base::completer();
  le_fluide = ref_cast(Fluide_Dilatable_base,mon_equation->milieu());
}

DoubleTab& Source_Chaleur_Fluide_Dilatable_base::calculer(DoubleTab& resu) const
{
  return ajouter(resu);
}
