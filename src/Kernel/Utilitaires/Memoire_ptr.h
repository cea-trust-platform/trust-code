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

#ifndef Memoire_ptr_included
#define Memoire_ptr_included

#include <Objet_U.h>
#include <assert.h>


// .DESCRIPTION Pointeur dans la Memoire de TRUST pour un Objet_U
// .SECTION voir aussi Objet_U Memoire
class Memoire_ptr
{
public :

  int next;

  Memoire_ptr(Objet_U* ptr=0) ;
  inline int libre() const;
  inline void set(Objet_U* ptr);
  inline Objet_U& obj();
  inline Memoire_ptr& operator=(const Memoire_ptr&);
private :
  Objet_U* o_ptr;
};

// .DESCRIPTION  Une zone de travail de la Memoire TRUST composee d'elements de type int/double/float
// .SECTION voir aussi
//    Memoire Int_ptr_trav Double_ptr_trav
template<typename _TYPE_>
class TRUST_ptr_trav
{
public :
  typedef _TYPE_ value_type;

  inline TRUST_ptr_trav() : sz(-1), data(0), lock_(0), next(0) { }

  // Supprime les donnees attachees a la zone de travail
  ~TRUST_ptr_trav()
  {
    if(sz!=-1)
      {
        if (next) delete next;
        delete[] data;
      }
  }

  inline int unlock() { return lock_ = 0; } // Deverrouille la zone de travail
  inline _TYPE_* ptr_() const { return data ; } // Retourne un pointeur sur les donnees de la zone de travail
  inline TRUST_ptr_trav* add(int n);

private :
  int        sz;
  _TYPE_*    data;
  int        lock_;
  TRUST_ptr_trav*         next;
} ;

typedef TRUST_ptr_trav<double> Double_ptr_trav;
typedef TRUST_ptr_trav<int> Int_ptr_trav;

// Description: Ajout d'elements dans la zone de travail
// Parametre: int n
//    Signification: le nombre d'element a creer dans la zone de travail
// Retour: Double_ptr_trav*
//    Signification: pointeur sur le zone de travail contenant les elements crees
template<typename _TYPE_>
inline TRUST_ptr_trav<_TYPE_>* TRUST_ptr_trav<_TYPE_>::add(int n)
{
  if(sz==-1)
    {
      sz=n;
      data=new _TYPE_[sz];
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
  TRUST_ptr_trav* curseur=this;
  while(curseur->next)
    {
      curseur=curseur->next;
      if ( (!curseur->lock_) && (curseur->sz==n))
        {
          curseur->lock_=1;
          return curseur;
        }
    }
  curseur->next= new TRUST_ptr_trav();
  if(!curseur->next)
    {
      Cerr << "Not enough memory " << finl;
      Process::exit();
    }
  curseur=curseur->next;
  curseur->sz=n;
  curseur->data=new _TYPE_[n];
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
//    Indique si le pointeur memoire est libre, c'est-a-dire s'il pointe sur un
//    Objet_U non nul
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le pointeur est libre
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Memoire_ptr::libre() const
{
  return o_ptr==0;
}

// Description:
//    Affecte un Objet_U a un pointeur memoire
// Precondition:
// Parametre: Objet_U* ptr
//    Signification: pointeur sur un Objet_U
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Memoire_ptr::set(Objet_U* ptr)
{
  o_ptr=ptr;
}

// Description:
//    Retourne une reference sur l'Objet_U pointe par le pointeur memoire
// Precondition:
//    le pointeur memoire ne doit pas etre libre
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: reference sur l'Objet_U pointe
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Objet_U& Memoire_ptr::obj()
{
  assert(o_ptr!=0);
  return *o_ptr;
}


// Description:
//    Operateur d'affectation entre pointeurs memoire
//    Dans le cas A=B, l'Objet_U pointe par A est l'Objet_U pointe par B
// Precondition:
// Parametre: const Memoire_ptr& mptr
//    Signification: le pointeur memoire B
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Memoire_ptr&
//    Signification: le pointeur memoire A
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Memoire_ptr& Memoire_ptr::operator=(const Memoire_ptr& mptr)
{
  o_ptr=mptr.o_ptr;
  next=mptr.next;
  return *this;
}


#endif
