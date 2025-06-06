/****************************************************************************
* Copyright (c) 2024, CEA
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
#ifndef MD_Vector_included
#define MD_Vector_included

#include <MD_Vector_base.h>
#include <memory>

// Options for arithmetic operations on vectors (mp_min_vect_local, apply_operator, etc...)
//  VECT_SEQUENTIAL_ITEMS: compute requested operation only on sequential items (real items that are not received from another processor)
//   (this is generally slower than VECT_REAL_ITEMS)
//  VECT_REAL_ITEMS: compute requested operation on real items if size_reelle_ok(), otherwise on all items
//  VECT_ALL_ITEMS: compute requested operation on all items (this is equivalent to a call to the Array class operator)
enum Mp_vect_options { VECT_SEQUENTIAL_ITEMS, VECT_REAL_ITEMS, VECT_ALL_ITEMS };

/*! @brief : Cette classe est un OWN_PTR mais l'objet pointe est partage entre plusieurs
 *
 *   instances de cette classe. L'objet pointe ne peut etre accede qu'en "const"
 *    et n'est accessible que par des instances de MD_Vector. Donc
 *    il n'existe pas de moyen d'y acceder en "non const" autrement qu'avec un cast.
 *   La methode attach() et le constructeur par copie rattachent le pointeur a une
 *    instance existante deja attachee a un pointeur.
 *   La methode attach_detach() s'approprie l'objet pointe par le OWN_PTR et detache
 *    l'objet du OWN_PTR. C'est la seule facon de "construire" les objets MD_Vector
 *    (evite une copie, et permet d'assurer que le MD_Vect ne peut plus etre modifie
 *     une fois que qu'il a ete attache a un MD_Vector)
 *   ATTENTION: la securite de la methode repose
 *    sur le fait que l'instance pointee par MD_Vector n'est accessible nulle part
 *    ailleurs que par des objets MD_Vector. NE PAS AJOUTER de methode
 *     attach(const MD_Vector_base &), cela casse la securite de la classe !!! (B.Mathieu)
 *   inline d'un maximum de methodes pour ne pas penaliser les tableaux non distribues,
 *    tout en evitant d'inclure MD_Vector_base.h
 *
 */
class MD_Vector
{
public:
  MD_Vector() {}
  inline MD_Vector(const MD_Vector&);
  inline MD_Vector& operator=(const MD_Vector&);
  inline void attach(const MD_Vector&);
  inline void detach();

  int non_nul() const
  {
#ifndef LATATOOLS
    return (ptr_ != 0);
#else
    return 0;
#endif
  }

#ifndef LATATOOLS
  void copy(const MD_Vector_base&);

  const MD_Vector_base& valeur() const
  {
    assert(ptr_);
    return *ptr_;
  }

  inline const MD_Vector_base* operator ->() const { assert(ptr_); return ptr_.get(); }

  int operator==(const MD_Vector&) const;
  int operator!=(const MD_Vector&) const;

private:
  std::shared_ptr<MD_Vector_base> ptr_;
#endif
};

/*! @brief constructeur par copie, associe le pointeur au meme objet que la source
 */
inline MD_Vector::MD_Vector(const MD_Vector& src)
{
  attach(src);
}

/*! @brief Detache le pointeur de l'objet pointe
 */
inline void MD_Vector::detach()
{
#ifndef LATATOOLS
  ptr_ = nullptr;
#endif
}

/*! @brief Detache le pointeur et attache au meme objet que src.
 */
inline void MD_Vector::attach(const MD_Vector& src)
{
#ifndef LATATOOLS
  if (this == &src)
    return; // sinon ligne suivante detruit le pointeur ?
  ptr_ = src.ptr_;
#endif
}

/*! @brief idem que attach(src)
 */
inline MD_Vector& MD_Vector::operator=(const MD_Vector& src)
{
  attach(src);
  return *this;
}

#endif
