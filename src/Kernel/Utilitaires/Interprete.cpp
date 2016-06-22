/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Interprete.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Interprete.h>
#include <Interprete_bloc.h>
#include <List_Nom.h>

Implemente_instanciable(Interprete, "Interprete", Objet_U);
Implemente_ref(Interprete);
Implemente_liste(Nom);

Entree& Interprete::readOn(Entree& is)
{
  Cerr << "Error in Interprete::readOn" << finl;
  exit();
  return is;
}

Sortie& Interprete::printOn(Sortie& os) const
{
  Cerr << "Error in Interprete::printOn" << finl;
  exit();
  return os;
}

// Description: Voir Interprete_bloc::objet_global()
//  BM: la classe Interprete n'est pas le meilleur endroit pour cette
//  methode mais on verra ca plus tard...
Objet_U& Interprete::objet(const Nom& nom)
{
  return Interprete_bloc::objet_global(nom);
}

// Description: Renvoie 1 si l'objet existe, 0 sinon
//   voir Interprete_bloc::objet_global_existant()
int Interprete::objet_existant(const Nom& nom)
{
  return Interprete_bloc::objet_global_existant(nom);
}
