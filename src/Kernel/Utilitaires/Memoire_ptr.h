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
// File:        Memoire_ptr.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Memoire_ptr_included
#define Memoire_ptr_included

#include <assert.h>
#include <arch.h>

class Objet_U;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Pointeur dans la Memoire de TRUST pour un Objet_U
// .SECTION voir aussi
//    Objet_U Memoire
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Une zone de travail de la Memoire TRUST composee d'elements de type double
// .SECTION voir aussi
//    Memoire Int_ptr_trav
//////////////////////////////////////////////////////////////////////////////
class Double_ptr_trav
{
public :
  inline Double_ptr_trav();
  Double_ptr_trav* add(int n);
  inline int unlock();
  ~Double_ptr_trav();
  inline double* d_ptr_() const;
private :
  int        sz;
  double*    data;
  int        lock_;
  Double_ptr_trav*         next;
} ;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Une zone de travail de la Memoire TRUST composee d'elements de type int
// .SECTION voir aussi
//    Memoire Double_ptr_trav
//////////////////////////////////////////////////////////////////////////////
class Int_ptr_trav
{
public :
  inline Int_ptr_trav();
  Int_ptr_trav* add(int n);
  inline int unlock();
  ~Int_ptr_trav();
  inline int* i_ptr_() const;
private :
  int        sz;
  int*    data;
  int        lock_;
  Int_ptr_trav*         next;
} ;

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


// Description:
//    Constructeur
//    La zone de travaille cree est vide
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
inline Double_ptr_trav::Double_ptr_trav() : sz(-1), data(0),
  lock_(0), next(0)
{
}

// Description:
//    Deverrouille la zone de travail
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
inline int Double_ptr_trav::unlock()
{
  return lock_=0;
}

// Description:
//    Retourne un pointeur sur les donnees de la zone de travail
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double*
//    Signification: pointeur sur le tableau de donnees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double* Double_ptr_trav::d_ptr_() const
{
  return data;
}


// Description:
//    Constructeur
//    La zone de travail cree est vide
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
inline Int_ptr_trav::Int_ptr_trav() : sz(-1), data(0),
  lock_(0), next(0)
{
}

// Description:
//    Deverrouille la zone de travail
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
inline int Int_ptr_trav::unlock()
{
  return lock_=0;
}

// Description:
//    Retourne un pointeur sur les donnees de la zone de travail
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int*
//    Signification: pointeur sur le tableau des donnees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int* Int_ptr_trav::i_ptr_() const
{
  return data;
}




#endif
