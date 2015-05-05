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
// File:        DoubleLists.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <DoubleLists.h>


// Description:
//    Ecriture sur un flot de sortie
//    Ecrit successivement toutes les listes du tableau
// Precondition:
// Parametre: Sortie& s
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& DoubleLists::printOn(Sortie& s) const
{
  int i=sz;
  while(i--)
    data[i].printOn(s);
  return s<<finl;
}

// Description:
//    Lecture dans un flot d'entree
//    ne fait rien
// Precondition:
// Parametre: Entree& s
//    Signification: flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: flot d'entree inchange
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& DoubleLists::readOn(Entree& s)
{
  return s;
}

// Description:
//    Constructeur par copie
// Precondition:
// Parametre: const DoubleLists& vect
//    Signification: la tableau de listes a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleLists::DoubleLists(const DoubleLists& vect)
  : sz(vect.sz), data(new DoubleList[vect.sz])
{
  if(!data)
    {
      Cerr << "Unable to carry out allocation " << finl;
      Process::exit();
    }
  int i=sz;
  if(i==0) data=0;
  else while(i--)
      data[i]=vect[i];
}


// Description:
//    Construit un tableau vide de listes
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleLists::DoubleLists() : sz(0), data(0) {}

// Description:
//    Construit un tableau de i listes
// Precondition:
// Parametre: int i
//    Signification: nombre de listes
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleLists::DoubleLists(int i): sz(i)
{
  if(i==0) data=0;
  else
    {
      data= new DoubleList[i];
      if(!data)
        {
          Cerr << "Unable to carry out allocation " << finl;
          Process::exit();
        }
    }
}


// Description:
//    Destructeur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleLists::~DoubleLists()
{
  if(sz)
    {
      assert(data);
      delete[] data;
    }
  else
    assert (data==0) ;
}


// Description:
//    Copie et affectation.
//    Les anciennes donnees sont perdues
// Precondition:
// Parametre: const DoubleLists& vect
//    Signification: le tableau de listes a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: IntLists&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleLists& DoubleLists::operator=(const DoubleLists& vect)
{
  sz=vect.sz;
  if(data)
    delete[] data;
  if(sz)
    {
      data = new DoubleList[sz];
      if(!data)
        {
          Cerr << "Unable to carry out allocation " << finl;
          Process::exit();
        }
      int i=sz;
      while(i--)
        data[i]=vect[i];
    }
  else data=0;
  return *this;
}

// Description:
//    Recherche d'une liste dans le tableau
// Precondition:
// Parametre: const IntList& t
//    Signification: la liste a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: indice de la liste ou -1 si elle est introuvable
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int DoubleLists::search(const DoubleList& t) const
{
  int i=sz;
  int retour=-1;
  while(i--)
    if( data[i] == t )
      {
        retour=i;
        i=0;
      }
  return retour;
}

// Description:
//    Acces a la ieme liste du tableau
// Precondition:
// Parametre: int i
//    Signification: indice de la liste
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const DoubleList&
//    Signification: la ieme liste du tableau
//    Contraintes:
// Exception:
//    Sort en erreur si i depasse les bornes du tableau
// Effets de bord:
// Postcondition:
const DoubleList& DoubleLists::operator[](int i) const
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}

// Description:
//    Acces a la ieme liste du tableau
// Precondition:
// Parametre: int i
//    Signification: indice de la liste
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:  DoubleList&
//    Signification: la ieme liste du tableau
//    Contraintes:
// Exception:
//    Sort en erreur si i depasse les bornes du tableau
// Effets de bord:
// Postcondition:
DoubleList& DoubleLists::operator[](int i)
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}

// Description:
//    Redimensionne un tableau de listes
// Precondition:
//    Le tablea doit etre vide
// Parametre: int i
//    Signification: taille du tableau
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DoubleLists::dimensionner(int i)
{
  assert(sz==0);
  assert(data==0);
  sz =i;
  if(i==0)
    data=0;
  else
    {
      data = new DoubleList[i];
      if(!data)
        {
          Cerr << "Unable to carry out allocation " << finl;
          Process::exit();
        }
    }
}
