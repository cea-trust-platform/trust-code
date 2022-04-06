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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Memoire.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Memoire_included
#define Memoire_included

#include <Memoire_ptr.h>
class Sortie;
class Objet_U;
class Nom;

// .DESCRIPTION La memoire de Trio-U
class Memoire
{
public :
  static Memoire& Instance();
  int add (Objet_U* );
  int suppr(int );
  Objet_U& objet_u(int);
  const Objet_U& objet_u(int) const;
  Objet_U* objet_u_ptr(int);
  const Objet_U* objet_u_ptr(int) const;
  int rang(const Nom& type, const Nom& nom) const;
  int rang(const Nom& nom) const;
  friend Sortie& operator <<(Sortie&, const Memoire&);
  void compacte();
  ~Memoire();

  inline Double_ptr_trav* add_trav_double(int n);
  inline Int_ptr_trav* add_trav_int(int n);

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,double>::value, TRUST_ptr_trav<_TYPE_>*>::type
  add_trav(int n) { return add_trav_double(n); }

  template<typename _TYPE_>
  inline typename std::enable_if<std::is_same<_TYPE_,int>::value, TRUST_ptr_trav<_TYPE_>*>::type
  add_trav(int n) { return add_trav_int(n); }

  int verifie() const;
  int imprime() const;

private :

  int size;
  Memoire_ptr* data;
  static int step;
  static int prems;
  Double_ptr_trav* trav_double;
  Int_ptr_trav* trav_int;
  static Memoire* _instance;
protected :
  Memoire();
};


// Description:
//    Ajoute une zone de travail "double" dans la memoire
// Precondition:
// Parametre: int n
//    Signification: le nombre d'elements dans la zone a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Double_ptr_trav*
//    Signification: pointeur sur la zone creee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Double_ptr_trav* Memoire::add_trav_double(int n)
{
  return trav_double->add(n);
}

// Description:
//    Ajoute une zone de travail "int" dans la memoire
// Precondition:
// Parametre: int n
//    Signification: le nombre d'elements dans la zone a creer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Int_ptr_trav*
//    Signification: pointeur sur la zone creee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Int_ptr_trav* Memoire::add_trav_int(int n)
{
  return trav_int->add(n);
}
Sortie& operator <<(Sortie& , const Memoire&);
#endif
