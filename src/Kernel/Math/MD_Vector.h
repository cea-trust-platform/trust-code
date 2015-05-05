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
// File:        MD_Vector.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MD_Vector_included
#define MD_Vector_included

#include <Deriv_MD_Vector_base.h>
class MD_Vector_base;

// Options for arithmetic operations on vectors (mp_min_vect_local, apply_operator, etc...)
//  VECT_SEQUENTIAL_ITEMS: compute requested operation only on sequential items (real items that are not received from another processor)
//   (this is generally slower than VECT_REAL_ITEMS)
//  VECT_REAL_ITEMS: compute requested operation on real items if size_reelle_ok(), otherwise on all items
//  VECT_ALL_ITEMS: compute requested operation on all items (this is equivalent to a call to the Array class operator)
enum Mp_vect_options { VECT_SEQUENTIAL_ITEMS, VECT_REAL_ITEMS, VECT_ALL_ITEMS };

// .DESCRIPTION:
//  Cette classe est un DERIV mais l'objet pointe est partage entre plusieurs
//  instances de cette classe. L'objet pointe ne peut etre accede qu'en "const"
//   et n'est accessible que par des instances de MD_Vector. Donc
//   il n'existe pas de moyen d'y acceder en "non const" autrement qu'avec un cast.
//  La methode attach() et le constructeur par copie rattachent le pointeur a une
//   instance existante deja attachee a un pointeur.
//  La methode attach_detach() s'approprie l'objet pointe par le DERIV et detache
//   l'objet du DERIV. C'est la seule facon de "construire" les objets MD_Vector
//   (evite une copie, et permet d'assurer que le MD_Vect ne peut plus etre modifie
//    une fois que qu'il a ete attache a un MD_Vector)
//  ATTENTION: la securite de la methode repose
//   sur le fait que l'instance pointee par MD_Vector n'est accessible nulle part
//   ailleurs que par des objets MD_Vector. NE PAS AJOUTER de methode
//    attach(const MD_Vector_base &), cela casse la securite de la classe !!! (B.Mathieu)
//  inline d'un maximum de methodes pour ne pas penaliser les tableaux non distribues,
//   tout en evitant d'inclure MD_Vector_base.h
class MD_Vector
{
public:
  MD_Vector() : ptr_(0) {};
  inline MD_Vector(const MD_Vector&);
  inline ~MD_Vector();
  inline MD_Vector& operator=(const MD_Vector&);
  inline void attach(const MD_Vector&);

  void copy(const MD_Vector_base&);

  int non_nul() const
  {
    return (ptr_ != 0);
  }
  const MD_Vector_base& valeur() const
  {
    assert(ptr_);
    return *ptr_;
  }
  inline void detach();
  int operator==(const MD_Vector&) const;
  int operator!=(const MD_Vector&) const;
private:
  void detach_();
  void attach_(const MD_Vector&);
  const MD_Vector_base *ptr_;
};

// Description: constructeur par copie, associe le pointeur au meme objet que
//  la source et incremente le compteur de references, partie inline pour traiter
//  le cas ou la source est nulle
inline MD_Vector::MD_Vector(const MD_Vector& src) :
  ptr_(0)
{
  if (src.ptr_)
    attach_(src);
}

// Description: Detache le pointeur de l'objet pointe et decremente le compteur de ref.
//  Si le compteur est nul, detruit l'objet.
inline void MD_Vector::detach()
{
  if (ptr_)
    detach_();
}

// Description: Detache le pointeur et attache au meme objet que src,
//  puis incremente le compteur de ref (si le pointeur est non nul).
inline void MD_Vector::attach(const MD_Vector& src)
{
  if (this == &src)
    return; // sinon a.attach(a) risque de detruire le pointeur !
  if (ptr_)
    detach_();
  if (src.ptr_)
    attach_(src);
}

// Description: idem que attach(src)
inline MD_Vector& MD_Vector::operator=(const MD_Vector& src)
{
  attach(src);
  return *this;
}

// Description: detache le pointeur s'il n'est pas nul.
//  inline pour ne pas penaliser les tableaux non distribues.
inline MD_Vector::~MD_Vector()
{
  if (ptr_)
    detach_();
}

#endif
