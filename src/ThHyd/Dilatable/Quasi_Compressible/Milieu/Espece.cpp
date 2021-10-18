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
// File:        Espece.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Milieu
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Espece.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Espece,"Espece",Fluide_Quasi_Compressible);
// XD espece interprete nul 1 not_set

Espece::Espece() : Masse_mol_(-1.) { }

Sortie& Espece::printOn(Sortie& os) const { return os; }

Entree& Espece::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  return is;
}

void Espece::set_param(Param& param)
{
  param.ajouter("mu",&mu,Param::REQUIRED); // XD_ADD_P field_base Species dynamic viscosity value (kg.m-1.s-1).
  param.ajouter("Cp",&Cp,Param::REQUIRED); // XD_ADD_P field_base Species specific heat value (J.kg-1.K-1).
  param.ajouter("Masse_molaire",&Masse_mol_,Param::REQUIRED); // XD_ADD_P double Species molar mass.
}

void Espece::verifier_coherence_champs(int& err,Nom& msg)
{
  Milieu_base::verifier_coherence_champs(err,msg);
}
