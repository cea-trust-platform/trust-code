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
// File:        Noms.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Noms.h>
Implemente_vect(Nom);
Implemente_instanciable(Noms,"Noms",VECT(Nom));


// Description:
//    Ecriture d'un tableau de noms sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Noms::printOn(Sortie& s) const
{
  return VECT(Nom)::printOn(s);
}


// Description:
//    Lecture d'un tableau de nomss sur un flot d'entree
// Precondition:
// Parametre: Entree& s
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Noms::readOn(Entree& s)
{
  return VECT(Nom)::readOn(s);
}

int Noms::search(const Nom& t ) const
{
  assert(size()>=0);
  int i=size();
  while(i--)
    if (operator()(i)==t)
      {
        return i;
      }
  return -1;
}

int Noms::search_without_checking_Motcle(const Nom& t ) const
{
  assert(size()>=0);
  int i=size();
  while(i--)
    {
      const Nom& unnom=operator()(i);
      if (unnom.getString().compare(t)==0)
      {
        return i;
      }
    }
  return -1;
}

int Noms::contient_(const char* const ch) const
{
  return (rang(ch)!=-1);
}
/* Returns the VECT position number of a string (-1 if not found) */
int Noms::rang(const char* const ch) const
{
  Nom nom(ch);
  return search(nom);
}
