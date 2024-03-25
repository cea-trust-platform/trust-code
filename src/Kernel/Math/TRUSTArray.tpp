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

#ifndef TRUSTArray_TPP_included
#define TRUSTArray_TPP_included

#include <string.h>
#include <algorithm>
#include <TRUSTTravPool.h>


/*! Destroy array, potentially making block available again if this was a Trav
 */
template <typename _TYPE_>
inline TRUSTArray<_TYPE_>::~TRUSTArray()
{
  if (storage_type_ == STORAGE::TEMP_STORAGE && mem_ != nullptr)
    // Give it back to the pool of free blocks - this means a shared_ptr copy, memory will be preserved
    TRUSTTravPool<_TYPE_>::ReleaseBlock(mem_);
  // Deletion on device if last (shared) instance and not a Trav:
  if (isDataOnDevice() && ref_count() == 1 && storage_type_ != STORAGE::TEMP_STORAGE)
    deleteOnDevice(*this);
}


/**
 * Change the size of the array.
 *
 * This updates the underlying std::vector size.
 *
 * Conditions:
 *  - if the size changes, type must not be a derived class of TRUSTArray.
 *  - the array can not be resized if data is shared (ref_array)
 *  - the array can not be resized if it is a 'ref_data'.
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_array(int new_size, RESIZE_OPTIONS opt)
{
  // Si le tableau change de taille, il doit etre du type TRUSTArray
  assert(  ( mem_ == nullptr || (int)mem_->size() == new_size ) ||
           std::string(typeid(*this).name()).find("TRUSTArray") != std::string::npos );
  // ref_arrays can not be resized:
  assert( ref_count() <= 1 );
  // ref_data can not be resized:
  assert( span_.empty() || mem_ != nullptr );
  resize_array_(new_size, opt);
}

/**  Methode virtuelle (dans Array_base) identique a resize_array(), permet de traiter
 *   de facon generique les ArrOf, Vect et Tab. Si l'objet est de type TRUSTArray, appel a resize_array(n)
 *
 *   Prerequis: le tableau doit etre "resizable" (voir resize_array()). S'il est d'un type derive (Vect ou Tab),
 *    il ne doit pas avoir de descripteur parallele si la taille est effectivement modifiee.
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_tab(int n, RESIZE_OPTIONS opt)
{
  resize_array(n, opt);
}

//  Change le mode d'allocation memoire lors des resize (voir VTRUSTdata et TRUST_ptr_trav)
//   Exemple pour creer un tableau avec allocation temporaire:
//    DoubleTab tab; // Creation d'un tableau vide
//    tab.set_mem_storage(TEMP_STORAGE); // Changement de mode d'allocation
//    tab.resize(n); // Allocation memoire
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::set_mem_storage(const STORAGE storage)
{
  storage_type_ = storage;
}


/** Make the array point to the memory zone indicated by data_. The array is detached from its current underlying
 * data ('mem_' is released).
 *
 * Warning: virtual method. In derived classes this method initializes the structures to create a sequential array.
 * To create a ref on a parallel array, see DoubleVect::ref()
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::ref_data(_TYPE_* ptr, int size)
{
  assert(ptr != 0 || size == 0);
  assert(size >= 0);
  detach_array(); // ToDo OpenMP revenir en arriere sur TRUSTArray.h
  span_ = Span_(ptr, size);
}

/** Make the current array point to the data of another existing array. Ownership of the data is hence shared.
 *
 * The current array is first detached (see detach_array()), and then attached to the provided data (see attach_array())
 * Wanring: virtual -> in derived classes this method initializes structures to create a sequential array.
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::ref_array(TRUSTArray& m, int start, int size)
{
  assert(&m != this);
  // La condition 'm n'est pas de type "ref_data"' est necessaire pour attach_array().
  detach_array();
  attach_array(m, start, size);
}

/** Copy the data from another array, potentially resizing.
 * The storage type is not copied.
 */
template <typename _TYPE_>
inline TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::operator=(const TRUSTArray& m)
{
  if (&m != this)
    {
      const int new_size = m.size_array();
      // On utilise la methode resize_array() qui teste le type derive de l'objet (resize interdit sur un type derive)
      resize_array(new_size, RESIZE_OPTIONS::NOCOPY_NOINIT);
      inject_array(m);
    }
  return *this;
}

/**  operateur [] retourne le ieme element du tableau
* Parametre: int i
*    Signification: indice dans l'intervalle 0 <= i < size_array()
* Exception: assert si i n'est pas dans l'intervalle
*/
template<typename _TYPE_>
inline _TYPE_& TRUSTArray<_TYPE_>::operator[](int i)
{
#ifdef _OPENMP
  this->checkDataOnHost();
#endif
  assert(i >= 0 && i < size_array());
  return span_[i];
}

/**  operateur [] pour TRUSTArray<double> : retourne le ieme element du tableau
* Exception:
*    assert si la valeur sort de l'intervalle : [ -DMAXFLOAT,DMAXFLOAT ]
*    assert si i n'est pas dans l'intervalle
*/
/// \cond DO_NOT_DOCUMENT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
template<>
inline double& TRUSTArray<double>::operator[](int i)
{
#ifdef _OPENMP
  this->checkDataOnHost();
#endif
  assert(i >= 0 && i < size_array());
  // [ABN] We can not perform this check here, since we might be *setting* the value from an un-initialized state.
  //       And an uninitialized state might well be completly off the bounds!
  //  assert(span_[i] > -DMAXFLOAT && span_[i] < DMAXFLOAT);
  return span_[i];
}
/// \endcond

template<typename _TYPE_>
inline const _TYPE_& TRUSTArray<_TYPE_>::operator[](int i) const
{
#ifdef _OPENMP
  this->checkDataOnHost();
#endif
  assert(i >= 0 && i < size_array());
  return span_[i];
}

/// \cond DO_NOT_DOCUMENT
template<>
inline const double& TRUSTArray<double>::operator[](int i) const
{
#ifdef _OPENMP
  this->checkDataOnHost();
#endif
  assert(i >= 0 && i < size_array());
  assert(span_[i] > -DMAXFLOAT && span_[i] < DMAXFLOAT);
  return span_[i];
}
#pragma GCC diagnostic pop
/// \endcond

/** Returns a pointer on the first element of the array. nullptr will be returned if array is detached.
 * Careful, address might change after a resize_array(), ref_data(), etc.
 */
template <typename _TYPE_>
inline _TYPE_* TRUSTArray<_TYPE_>::addr()
{
  checkDataOnHost();
  return span_.data();
}

template <typename _TYPE_>
inline const _TYPE_* TRUSTArray<_TYPE_>::addr() const
{
  checkDataOnHost();
  return span_.data();
}

template <typename _TYPE_>
inline _TYPE_ *TRUSTArray<_TYPE_>::data()
{
  return span_.data();
}

template <typename _TYPE_>
inline const _TYPE_ *TRUSTArray<_TYPE_>::data() const
{
  return span_.data();
}

/** Returns the size of the array.
 */
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::size_array() const
{
  return (int)span_.size();
}

/** Returns the number of shared owners of the data underlying the array. If -1 detached array.
  * This is simply the ref count of the shared pointer 'mem_'
  */
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::ref_count() const
{
  return mem_ ? (int)mem_.use_count() : -1;
}

/**  Ajoute une case en fin de tableau et y stocke la "valeur"
 * Precondition:
 *  Le tableau ne doit pas etre "ref_data",
 *  et il ne doit pas y avoir plus d'une reference a la zone de memoire pointee (meme precondition que resize_array())
 *  Le tableau doit etre de type TRUSTArray (pas un type derive)
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::append_array(_TYPE_ valeur)
{
  this->checkDataOnHost();
  if (mem_ == nullptr)
    {
      assert(span_.empty()); // not ref_data
      mem_ = std::make_shared<Vector_>(Vector_(1, valeur));
    }
  else
    mem_->push_back(valeur);
  span_ = Span_(*mem_);
}

/**  Fonction de comparaison utilisee pour trier le tableau dans ArrOfDouble::trier(). Voir man qsort
 */
template<typename _TYPE_ /* double ou float */ >
static True_int fonction_compare_arrofdouble_ordonner(const void * data1, const void * data2)
{
  const _TYPE_ x = *(const _TYPE_*)data1;
  const _TYPE_ y = *(const _TYPE_*)data2;
  if (x < y) return -1;
  else if (x > y) return 1;
  else return 0;
}

/**  Fonction de comparaison utilisee pour trier le tableau dans ArrOfInt::trier(). Voir man qsort
 */
static True_int fonction_compare_arrofint_ordonner(const void * data1, const void * data2)
{
  const int x = *(const int*)data1;
  const int y = *(const int*)data2;
#ifndef INT_is_64_
  return x - y;
#else
  if (x < y) return -1;
  else if (x > y) return 1;
  else return 0;
#endif
}

/**  Tri des valeurs du tableau dans l'ordre croissant. La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
 */
/// \cond DO_NOT_DOCUMENT
template <typename _TYPE_ /* double ou float */ >
inline void TRUSTArray<_TYPE_>::ordonne_array()
{
  checkDataOnHost();
  const int size = size_array();
  if (size > 1)
    {
      _TYPE_ * data = span_.data();
      //  TODO: what's the difference with std::sort  (also in N.log(N))?
      qsort(data, size, sizeof(_TYPE_), fonction_compare_arrofdouble_ordonner<_TYPE_>);
    }
}

template <>
inline void TRUSTArray<int>::ordonne_array()
{
  checkDataOnHost();
  const int size = size_array();
  if (size > 1)
    {
      int * data = span_.data();
      qsort(data, size, sizeof(int), fonction_compare_arrofint_ordonner);
    }
}
/// \endcond

/**  Tri des valeurs du tableau dans l'ordre croissant et suppresion des doublons La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::array_trier_retirer_doublons()
{
  checkDataOnHost();
  const int size_ = size_array();
  if (size_ <= 0) return;

  // Sort ascending
  ordonne_array();

  auto new_end = std::unique(span_.begin(), span_.end());

  int new_size_ = (int)std::distance(span_.begin(), new_end);
  resize_array(new_size_);
}

/** Attach the array to (part of) an existing block of data stored in another array.
 * The data is then shared between that other array and this.
 *
 * If size < 0, we take the data from the specified start till the end of the block.
 * Array must be detached first before invoking this method.
 * It is forbidden to attach to a ref_data.
 */
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::attach_array(const TRUSTArray& m, int start, int size)
{
  // Array must be detached
  assert(span_.empty() && mem_ == nullptr);
  // we might attach to an already detached array ... make sure that start and size are coherent
  assert(m.mem_ != nullptr || (start == 0 && size <= 0));
  // we don't attach to ourself:
  assert(&m != this);
  // we don't attach to a ref_data:
  assert(! (m.mem_ == nullptr && !m.span_.empty()) );

  if (size < 0)
    size = m.size_array() - start;

  assert(start >= 0 && size >=0 && start + size <= m.size_array());

  // shared_ptr copy! One more owner for the underlying data - note we migth copy nullptr here ...
  mem_ = m.mem_;

  // Copy (shared) data location from m - locations will be synchronized automatically between ref arrays
  data_location_ = m.data_location_;

  // stupid enough, but we might have ref'ed a detached array ...
  if (mem_ != nullptr)
    span_ = Span_((_TYPE_ *)(m.mem_->data()+start), size);
  else
    span_ = Span_();
}


/** Bring the current array in a detached state, i.e. both 'mem_' and 'span_' are cleared, whatever the current state.
*/
template <typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::detach_array()
{
  mem_ = nullptr;
  span_ = Span_();
  if (isDataOnDevice())
    deleteOnDevice(*this);
  return true;
}

#endif /* TRUSTArray_TPP_included */
