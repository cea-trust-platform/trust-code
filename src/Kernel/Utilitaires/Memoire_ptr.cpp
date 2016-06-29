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
// File:        Memoire_ptr.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Memoire_ptr.h>
#include <DoubleVect.h>

// Description:
//    Constructeur
// Precondition:
// Parametre: Objet_U* ptr
//    Signification: l'Objet_U sur lequel pointer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Memoire_ptr::Memoire_ptr(Objet_U* ptr) : o_ptr(ptr)
{
  next=-1;
}



// Description:
//    Ajout d'elements dans la zone de travail
// Precondition:
// Parametre: int n
//    Signification: le nombre d'element a creer dans la zone de travail
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Double_ptr_trav*
//    Signification: pointeur sur le zone de travail contenant les elements crees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Double_ptr_trav* Double_ptr_trav::add(int n)
{
  if(sz==-1)
    {
      sz=n;
      data=new double[sz];
      if(!data)
        {
          Cerr << "Not enough memory " << finl;
          Process::exit();
        }
      lock_=1;
      next=0;
      return this;
    }
  if ( (!lock_) && (sz==n))
    {
      lock_=1;
      return this;
    }
  Double_ptr_trav* curseur=this;
  while(curseur->next)
    {
      curseur=curseur->next;
      if ( (!curseur->lock_) && (curseur->sz==n))
        {
          curseur->lock_=1;
          return curseur;
        }
    }
  curseur->next= new Double_ptr_trav();
  if(!curseur->next)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  curseur=curseur->next;
  curseur->sz=n;
  curseur->data=new double[n];
  if(!data)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  curseur->lock_=1;
  curseur->next=0;
  return curseur;
}

// Description:
//    Destructeur
//    Supprime les donnees attachees a la zone de travail
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
Double_ptr_trav::~Double_ptr_trav()
{
  if(sz!=-1)
    {
      if (next) delete next;
      delete[] data;
    }
}



// Description:
//    Ajout d'elements dans la zone de travail
// Precondition:
// Parametre: int n
//    Signification: le nombre d'element a creer dans la zone de travail
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Int_ptr_trav*
//    Signification: pointeur sur le zone de travail contenant les elements crees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Int_ptr_trav* Int_ptr_trav::add(int n)
{
  if(sz==-1)
    {
      sz=n;
      data=new int[sz];
      if(!data)
        {
          Cerr << "Not enough memory " << finl;
          Process::exit();
        }
      lock_=1;
      next=0;
      return this;
    }
  if ( (!lock_) && (sz==n))
    {
      lock_=1;
      return this;
    }
  Int_ptr_trav* curseur=this;
  while(curseur->next)
    {
      curseur=curseur->next;
      if ( (!curseur->lock_) && (curseur->sz==n))
        {
          curseur->lock_=1;
          return curseur;
        }
    }
  curseur->next= new Int_ptr_trav();
  if(!curseur->next)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  curseur=curseur->next;
  curseur->sz=n;
  curseur->data=new int[n];
  if(!data)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  curseur->lock_=1;
  curseur->next=0;
  return curseur;
}

// Description:
//    Destructeur
//    Supprime les donnees attachees a la zone de travail
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
Int_ptr_trav::~Int_ptr_trav()
{
  if(sz!=-1)
    {
      if (next) delete next;
      delete[] data;
    }
}
