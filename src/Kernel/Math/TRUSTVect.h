/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef TRUSTVect_included
#define TRUSTVect_included

#include <type_traits>
#include <MD_Vector_tools.h>
#include <MD_Vector_base.h>
#include <DescStructure.h>
#include <TRUSTArray.h>
#include <limits.h>
#include <math.h>

#include <MD_Vector.h>
#ifndef LATATOOLS   // Lata tools does not use parallelism
#include <communications.h>
#endif

template<typename _TYPE_, typename _SIZE_>
class TRUSTVect : public TRUSTArray<_TYPE_,_SIZE_>
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUSTVect* xxx = new  TRUSTVect(*this);
    if(!xxx) Process::exit("Not enough memory ");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override
  {
#ifndef LATATOOLS
    if (TRUSTArray<_TYPE_,_SIZE_>::nproc() > 1 && md_vector_.non_nul())
      Process::exit("Error in TRUSTVect::printOn: try to print a parallel vector");
    TRUSTArray<_TYPE_,_SIZE_>::printOn(os);
#endif
    return os;
  }

  /*! @brief Lecture d'un vecteur sequentiel (comme un ArrOfDouble) Attention: appel invalide si le vecteur a un MD_Vector non nul.
   *
   * (pour les vecteurs paralleles, utiliser une methode de sauvegarde/reprise)
   *
   */
  Entree& readOn(Entree& is) override
  {
#ifndef LATATOOLS
    // Que veut-on faire si on lit dans un vecteur ayant deja une structure parallele ?
    if (md_vector_.non_nul())
      Process::exit("Error in TRUSTVect::readOn: vector has a parallel structure");
    TRUSTArray<_TYPE_,_SIZE_>::readOn(is);
    size_reelle_ = TRUSTArray<_TYPE_,_SIZE_>::size_array();
    line_size_ = 1;
#endif
    return is;
  }

public:

  using Span_ = tcb::span<_TYPE_>;

  // One instanciation with given template parameter may see all other template versions (useful in ref_as_xxx())
  template<typename _TYPE2_, typename _SIZE2_> friend class TRUSTVect;

  virtual ~TRUSTVect() { }

  TRUSTVect() : size_reelle_(0), line_size_(1) { }

  /*! @brief construction d'un vecteur de taille n.
   *
   * Les elements du vecteur sont initialises a zero par defaut. Pour ne pas initialiser les valeurs, utiliser ceci: DoubleVect toto;
   *    toto.resize(n, RESIZE_OPTIONS::NOCOPY_NOINIT);
   *
   */
  TRUSTVect(_SIZE_ n) :   TRUSTArray<_TYPE_,_SIZE_>(n), size_reelle_(n), line_size_(1) { }

  /*! @brief Constructeur par copie.
   *
   * Il s'agit d'un "deep copy" voir ArrOfDouble::ArrOfDouble(const ArrOfDouble &) Remarque: il n'y a pas de constructeur par copie a partir de ArrOfDouble
   *    Ceci est volontaire, sinon on risque de grosses pertes de performances par creation implicite d'objets, difficile a trouver.
   *    (exemple: appel d'une methode toto(const IntVect &) avec un ArrOfInt produit une copie du tableau !)
   *   Utiliser copy() pour copier un ArrOfDouble dans un DoubleVect
   *
   */
  TRUSTVect(const TRUSTVect& v) : TRUSTArray<_TYPE_,_SIZE_>(v), md_vector_(v.md_vector_), size_reelle_(v.size_reelle_), line_size_(v.line_size_) { }

  inline TRUSTVect& operator=(const TRUSTVect&);
  inline TRUSTVect& operator=(_TYPE_);
  inline _SIZE_ size() const;
  inline _SIZE_ size_totale() const;
  inline _SIZE_ size_reelle() const;
  inline _SIZE_ size_reelle_ok() const;
  inline int line_size() const;   // Even in 64b we suppose line_size_ is always sufficiently small to fit in an int.
  inline void resize(_SIZE_, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void copy(const TRUSTArray<_TYPE_,_SIZE_>&, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void copy(const TRUSTVect&, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void operator+=(const TRUSTVect& v) { operator_add(*this, v); }
  inline void operator+=(const _TYPE_ x) { operator_add(*this, x); }
  inline void operator-=(const TRUSTVect& v) { operator_sub(*this, v); }
  inline void operator-=(const _TYPE_ x) { operator_sub(*this, x); }
  inline void operator*=(const TRUSTVect& v) { operator_multiply(*this, v); }
  inline void operator*= (const _TYPE_ x) { operator_multiply(*this, x); }

  template<typename _T_ /* double ou float */>
  inline void operator/=(const TRUSTVect<_T_, _SIZE_>& v) { operator_divide(*this, v); }
  inline void operator/= (const _TYPE_ x) { operator_divide(*this, x); }

  inline virtual const MD_Vector& get_md_vector() const { return md_vector_; }
  inline void resize_tab(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT) override;

  // See same methods as in TRUSTArray - CAREFUL, this is not an override because arg types are different (vect vs arr)
  inline void from_tid_to_int(TRUSTVect<int, int>& out) const;
  inline void ref_as_big(TRUSTVect<_TYPE_, trustIdType>& out) const;
  inline void ref_as_small(TRUSTVect<_TYPE_, int>& out) const;

  // Options par defaut choisies pour compatibilite avec la version precedente. Attention: il y avait un echange_espace_virtuel avant, ce n'est pas strictement equivalent
  inline void abs(Mp_vect_options opt=VECT_ALL_ITEMS) { operator_abs(*this, opt); }
  inline void carre(Mp_vect_options opt=VECT_ALL_ITEMS) { carre_(*this, opt); }
  inline void racine_carree(Mp_vect_options opt=VECT_ALL_ITEMS) { racine_carree_(*this, opt); }

  template<typename _SCALAR_TYPE_>
  inline void ajoute(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt=VECT_ALL_ITEMS);

  template<typename _SCALAR_TYPE_>
  inline void ajoute_sans_ech_esp_virt(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt=VECT_REAL_ITEMS);

  template<typename _SCALAR_TYPE_>
  inline void ajoute_produit_scalaire(_SCALAR_TYPE_ alpha, const TRUSTVect& x, const TRUSTVect& y, Mp_vect_options opt=VECT_ALL_ITEMS);

  template<typename _SCALAR_TYPE_>
  inline void ajoute_carre(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt=VECT_ALL_ITEMS);

#ifndef LATATOOLS
  //
  //    All the methods below involve parallelism or are not used in lata_tools
  //

  // par defaut: min et max sur items reels (compat. 1.5.6):
  inline _TYPE_ local_max_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return local_max_vect_(*this, opt); }
  inline _TYPE_ local_min_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return local_min_vect_(*this, opt); }
  inline _TYPE_ local_max_abs_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return local_max_abs_vect_(*this, opt); }
  inline _TYPE_ local_min_abs_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return local_min_abs_vect_(*this, opt); }
  inline _TYPE_ mp_max_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return mp_max_vect_(*this, opt); }
  inline _TYPE_ mp_min_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return mp_min_vect_(*this, opt); }
  inline _TYPE_ mp_max_abs_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return mp_max_abs_vect_(*this, opt); }
  inline _TYPE_ mp_min_abs_vect(Mp_vect_options opt=VECT_REAL_ITEMS) const { return mp_min_abs_vect_(*this, opt); }
  inline _TYPE_ mp_norme_vect() const { return mp_norme_vect_(*this); }
#endif  // LATATOOLS

  // methodes virtuelles

  inline virtual void ref(const TRUSTVect&);
  inline virtual void echange_espace_virtuel();
  inline virtual void set_md_vector(const MD_Vector&);
  inline virtual void jump(Entree&);
  inline virtual void lit(Entree&, bool resize_and_read=1);
  inline virtual void ecrit(Sortie&) const;
  inline virtual void detach_vect() { md_vector_.detach(); }

  inline void ref_data(_TYPE_* ptr, _SIZE_ new_size) override;
  inline void ref_array(TRUSTArray<_TYPE_,_SIZE_>&, _SIZE_ start = 0, _SIZE_ sz = -1) override;

  inline Span_ get_span() override { return Span_(TRUSTArray<_TYPE_,_SIZE_>::addr(),size_reelle()); }
  inline Span_ get_span_tot() override { return Span_(TRUSTArray<_TYPE_,_SIZE_>::addr(),TRUSTArray<_TYPE_,_SIZE_>::size_array()); }
  inline const Span_ get_span() const override { return Span_((_TYPE_*)TRUSTArray<_TYPE_,_SIZE_>::addr(), size_reelle()); }
  inline const Span_ get_span_tot() const override { return Span_((_TYPE_*)TRUSTArray<_TYPE_,_SIZE_>::addr(), TRUSTArray<_TYPE_,_SIZE_>::size_array()); }

  /*! @brief met l'objet dans l'etat obtenu par le constructeur par defaut.
   *
   */
  inline void reset() override
  {
    md_vector_.detach();
    line_size_ = 1;
    size_reelle_ = 0;
    TRUSTArray<_TYPE_,_SIZE_>::reset();
  }

protected:
  inline void set_line_size_(int n);   // Even in 64b we suppose line_size_ is always sufficiently small to fit in an int.
  inline void resize_vect_(_SIZE_ n, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);
  inline void copy_(const TRUSTVect& v, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  // Un DoubleVect est un ArrOfDouble qui possede eventuellement une structure de tableau distribue. Ce pointeur peut etre nul.
  MD_Vector md_vector_;

  // Propriete size_reelle du tableau (fournie par scattered_vect_data). -1 => l'appel a size_reelle() et size() est invalide pour ce vecteur.
  _SIZE_ size_reelle_;

  // Facteur multiplicatif a appliquer entre md_vector_.nb_items_tot() et size_array() et entre md_vector_.nb_items_reels() et size_reelle_.
  // Si l'objet est un tableau, ce facteur est generalement egal au produit des dimension(i) pour i>1 (une ligne du tableau par item geometrique du descripteur)
  // Attention, line_size_ peut etre nul pour un tableau a zero colonnes mais pas s'il y a un descripteur attache.
  // Even in 64b we suppose line_size_ is always sufficiently small to fit in an int.
  int line_size_;
};

using DoubleVect = TRUSTVect<double, int>;
using FloatVect = TRUSTVect<float, int>;
using IntVect = TRUSTVect<int, int>;
using TIDVect = TRUSTVect<trustIdType, int>;

template <typename _TYPE_>
using BigTRUSTVect = TRUSTVect<_TYPE_, trustIdType>;

using BigDoubleVect = BigTRUSTVect<double>;
using BigIntVect = BigTRUSTVect<int>;
using BigTIDVect = BigTRUSTVect<trustIdType>;

/* ********************************** *
 * FONCTIONS NON MEMBRES DE TRUSTVect *
 * ********************************** */

#include <TRUSTVect_tools.tpp> // external templates function specializations ici ;)

/* ****************************** *
 * FONCTIONS MEMBRES DE TRUSTVect *
 * ****************************** */

#include <TRUSTVect.tpp> // templates specializations ici ;)

#endif /* TRUSTVect_included */
