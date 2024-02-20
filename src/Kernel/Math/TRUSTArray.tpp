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

//  Change le nombre d'elements du tableau.
//  - Si smart_resize est non nul :
//    * Si la nouvelle taille est inferieure ou egale a la taille alouee (p->get_size()) on ne realloue pas de memoire
//    * sinon, on realloue et on copie les donnees existantes.
//    Astuce pour ne pas copier les anciennes donnees : resize(0); resize(n);
//  - Si smart_resize est nul, on realloue une nouvelle zone memoire uniquement si la nouvelle taille est differente de l'ancienne.
// Parametre: new_size
// Signification: nouvelle valeur demandee pour size_array() (doit etre >= 0)
// Parametre: opt
// Signification: quoi faire des anciennes case et des nouvelles cases (voir memory_resize())
//  Attention, pour un tableau smart_resize, l'option "INIT"
// Precondition :
//  - Si "new_size" est egal a la taille du tableau, aucune condition.
//  - Sinon,
//    * le tableau ne doit pas etre un type derive de TRUSTArray !!!
//    * le tableau doit etre "resizable", c'est a dire: soit detache, soit normal (pas de type "ref_data") avec ref_count egal a 1
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_array(int new_size, RESIZE_OPTIONS opt)
{
  // Si le tableau change de taille, il doit etre du type TRUSTArray
  assert(  ( mem_ == nullptr || (int)mem_->size() == new_size ) ||
           std::string(typeid(*this).name()).find("TRUSTArray") != std::string::npos );
  resize_array_(new_size, opt);
}

//  methode virtuelle (dans Array_base) identique a resize_array(), permet de traiter
//   de facon generique les ArrOf, Vect et Tab. Si l'objet est de type TRUSTArray, appel a resize_array(n)
// Prerequis: le tableau doit etre "resizable" (voir resize_array()). S'il est d'un type derive (Vect ou Tab),
//    il ne doit pas avoir de descripteur parallele si la taille est effectivement modifiee.
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

//
//    Fait pointer le tableau vers la zone de memoire "data_". On detache la zone de memoire existante.
//    Le tableau devient de type "ref_data". Attention : ptr doit etre non nul. La taille est initialisee avec size.
//   Attention: methode virtuelle: dans les classes derivee, cette methode initialise les structures pour creer
//    un tableau sequentiel. Pour faire une ref sur un tableau parallele utiliser DoubleVect::ref()
// Parametre: _TYPE_*
//    Signification: le tableau a recuperer. Si pointeur nul alors size doit etre nulle aussi et le tableau reste detache
// Parametre: int size
//    Signification: le nombre d'elements du tableau.
// Retour: TRUSTArray<_TYPE_>&
//    Signification: *this
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::ref_data(_TYPE_* ptr, int size)
{
  assert(ptr != 0 || size == 0);
  assert(size >= 0);
  detach_array(); // ToDo OpenMP revenir en arriere sur TRUSTArray.h
  span_ = Span_(ptr, size);
}

//
//    Fait pointer le tableau vers les memes donnees qu'un tableau existant. Le tableau sera du meme type que le tableau m ("detache", "normal").
//    Le tableau m ne doit pas etre de type "ref_data". Attention, le tableau source et *this sont ensuite figes (resize_array() interdit).
//   Attention: methode virtuelle: dans les classes derivee, cette methode initialise les structures pour creer un tableau sequentiel.
// Parametre: const TRUSTArray<_TYPE_>& m
//    Signification: le tableau a referencer (pas de type "ref_data" et different de *this !!!)
// Retour: TRUSTArray<_TYPE_>&
//    Signification: *this
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::ref_array(TRUSTArray& m, int start, int size)
{
  assert(&m != this);
  // La condition 'm n'est pas de type "ref_data"' est necessaire pour attach_array().
  detach_array();
  attach_array(m, start, size);
}

//  Copie les donnees du tableau m. Si "m" n'a pas la meme taille que "*this", on fait un resize_array.
//    Ensuite, on copie les valeurs de "m" dans "*this". Le type de tableau (methode d'allocation) n'est pas copie.
// Precondition: preconditions identiques a resize_array()
// Parametre: const TRUSTArray<_TYPE_>& m
//    Signification: la tableau a copier
// Retour:  TRUSTArray<_TYPE_>&
//    Signification: *this
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

//  operateur [] retourne le ieme element du tableau
// Parametre: int i
//    Signification: indice dans l'intervalle 0 <= i < size_array()
// Exception: assert si i n'est pas dans l'intervalle
template<typename _TYPE_>
inline _TYPE_& TRUSTArray<_TYPE_>::operator[](int i)
{
#ifdef _OPENMP
  this->checkDataOnHost();
#endif
  assert(i >= 0 && i < size_array());
  return span_[i];
}

//  operateur [] pour TRUSTArray<double> : retourne le ieme element du tableau
// Exception:
//    assert si la valeur sort de l'intervalle : [ -DMAXFLOAT,DMAXFLOAT ]
//    assert si i n'est pas dans l'intervalle
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
  assert(span_[i] > -DMAXFLOAT && span_[i] < DMAXFLOAT);
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

//  Renvoie un pointeur sur le premier element du tableau. Le pointeur est nul si le tableau est "detache".
//   Attention, l'adresse peut changer apres un appel a resize_array(), ref_data, ref_array, ...
// Retour: const _TYPE_*
//   Signification: pointeur sur le premier element du tableau
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
//  Renvoie la taille du tableau (nombre d'elements declares a la construction ou a resize_array()).
//    C'est le nombre d'elements accessibles a operator[]
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::size_array() const
{
  return (int)span_.size();
}

//  Retourne le nombre de references des donnees du tableau si le tableau est "normal", -1 s'il est "detache"
// Retour: int
//    Signification: ref_count_
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::ref_count() const
{
  return mem_ ? (int)mem_.use_count() : -1;
}

//  Ajoute une case en fin de tableau et y stocke la "valeur"
// Precondition:
//  Tableau doit etre de type "smart_resize" (sinon, ecroulement des performances). De plus, le tableau ne doit pas etre "ref_data",
//  et il ne doit pas y avoir plus d'une reference a la zone de memoire pointee (meme precondition que resize_array())
//  Le tableau doit etre de type TRUSTArray (pas un type derive)
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::append_array(_TYPE_ valeur)
{
  this->checkDataOnHost();
  if (mem_ == nullptr)
    mem_ = std::make_shared<Vector_>(Vector_(1, valeur));
  else
    mem_->push_back(valeur);
  span_ = Span_(*mem_);
}

//  Fonction de comparaison utilisee pour trier le tableau dans ArrOfDouble::trier(). Voir man qsort
template<typename _TYPE_ /* double ou float */ >
static True_int fonction_compare_arrofdouble_ordonner(const void * data1, const void * data2)
{
  const _TYPE_ x = *(const _TYPE_*)data1;
  const _TYPE_ y = *(const _TYPE_*)data2;
  if (x < y) return -1;
  else if (x > y) return 1;
  else return 0;
}

//  Fonction de comparaison utilisee pour trier le tableau dans ArrOfInt::trier(). Voir man qsort
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

//  Tri des valeurs du tableau dans l'ordre croissant. La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
/// \cond DO_NOT_DOCUMENT
template <typename _TYPE_ /* double ou float */ >
inline void TRUSTArray<_TYPE_>::ordonne_array()
{
  checkDataOnHost(*this);
  const int size = size_array();
  if (size > 1)
    {
      _TYPE_ * data = span_.data();
      qsort(data, size, sizeof(_TYPE_), fonction_compare_arrofdouble_ordonner<_TYPE_>);
    }
}
template <>
inline void TRUSTArray<int>::ordonne_array()
{
  checkDataOnHost(*this);
  const int size = size_array();
  if (size > 1)
    {
      int * data = span_.data();
      qsort(data, size, sizeof(int), fonction_compare_arrofint_ordonner);
    }
}
/// \endcond

//   Tri des valeurs du tableau dans l'ordre croissant et suppresion des doublons La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::array_trier_retirer_doublons()
{
  checkDataOnHost(*this);
  const int size_ = size_array();
  if (size_ <= 0) return;

  ordonne_array(); // Tri dans l'ordre croissant
  // Retire les doublons (codage optimise en pointeurs)
  _TYPE_ last_value = span_.back();
  _TYPE_ *src = span_.data() + 1;
  _TYPE_ *dest = span_.data() + 1;
  for (int i = 0; i < size_; i++)
    {
      _TYPE_ x = *(src++);
      if (x != last_value)
        {
          *(dest++) = x;
          last_value = x;
        }
    }
  int new_size_ = (int)(dest - span_.data());
  resize_array(new_size_);
}

//  Amene le tableau dans l'etat "normal", "detache" ou "ref_array" en associant une sous-zone de memoire du tableau m,
//    definie par start et size. Si size < 0, on prend le tableau m jusqu'a la fin.
// Precondition: Le tableau doit etre "detache"
// Parametre: const ArrOfDouble& m
//    Signification: tableau a utiliser. le tableau doit etre different de *this !!!
// Postcondition:
//    Si m est detache, le tableau reste detache,
//    si m est "ref_array", le tableau devient "ref_array",
//    sinon le tableau est "normal", avec ref_count > 1
//    Si m est de taille nulle, le tableau reste detache + Warning dans fichier .log
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::attach_array(const TRUSTArray& m, int start, int size)
{
  // Array must be detached
  assert(span_.empty() && mem_ == nullptr);
  // m might be detached too:
  assert(m.mem_ != nullptr || (start == 0 && size == -1));
  // Le tableau doit etre different de *this
  assert(&m != this);

  if (size < 0)
    size = m.size_array() - start;

  assert(start >= 0 && size >=0 && start + size <= m.size_array());

  mem_ = m.mem_;  // shared_ptr copy! One more owner for the underlying data

  // stupid enough, but we might have ref'ed a detached array ...
  if (mem_ != nullptr)
    span_ = Span_((_TYPE_ *)(m.mem_->data()+start), size);

//
//  if (m.size_array() > 0)
//    {
//      mem_ = m.mem_;
//      if (mem_) mem_->add_one_ref();
//      span_ = m.span_ + start;
//      size_array_ = size;
//      memory_size_ = m.memory_size_ - start;
//      smart_resize_ = m.smart_resize_;
//    }
//  else
//    {
//      // Cas particulier ou on attache un tableau de taille nulle: en theorie, c'est pareil qu'un tableau de taille non nulle, MAIS
//      //  dans les operateurs (ex:Op_Dift_VDF_Face_Axi), une ref est construite avant que le tableau ne prenne sa taille definitive. Donc, pour ne pas
//      //  empecher le resize, il ne faut pas attacher le tableau s'il n'a pas encore la bonne taille. Solution propre: reecrire les operateurs pour
//      //  qu'ils ne prennent pas une ref avant que le tableau ne soit valide et faire p_ = m.p_ dans tous les cas.
//      // Process::Journal() << "Warning TRUSTArray::attach_array(m), m.size_array()==0, on n attache pas le tableau" << finl;
//    }
}

//  Remplit "nb" cases consecutives du tableau a partir de la case "first" avec une valeur par defaut.
//  Cette fonction est appelee lors d'un resize pour initialiser les cases nouvellement creees.
//  Le comportement depend actuellement du type de tableau :
//  * Tableau de type "smart_resize":
//    * en mode debug (macro NDEBUG non definie) le tableau est initialise
//      avec une valeur invalide.
//    * en optimise, le tableau n'est pas initialise
//  * Tableau normal :
//    Le tableau est initialise avec la valeur 0. Ce comportement est choisi pour des raisons de compatibilite avec l'implementation precedente.
//    Cette specification pourrait etre modifiee prochainement pour des raisons de performances (pour ne pas avoir a initialiser inutilement les tableaux).
//    DONC: il faut supposer desormais que les nouvelles cases ne sont pas initialisees lors d'un resize.
// Parametre: first
//  Signification: premiere case a initialiser.
//  Contrainte:    (nb==0) ou (0 <= first < memory_size_)
// Parametre: nb
//  Signification: nombre de cases a initialiser.
//  Contrainte:    (nb==0) ou (0 < nb <= memory_size_ - first)
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::fill_default_value(RESIZE_OPTIONS opt, int first, int nb)
{
  checkDataOnHost(*this);
//  assert((nb == 0) || (first >= 0 && first < memory_size_));
//  assert((nb == 0) || (nb > 0 && nb <= memory_size_ - first));
//  _TYPE_ * data = span_;
//  assert(data!=0 || nb==0);
//  data += first;
//  if (opt != RESIZE_OPTIONS::COPY_INIT)
//    {
//#ifndef NDEBUG // On initialise uniquement en mode debug
//      static const _TYPE_  INVALIDE_ = (std::is_same<_TYPE_,double>::value) ? DMAXFLOAT*0.999 : INT_MIN;
//      for (int i = 0; i < nb; i++) data[i] = INVALIDE_;
//#endif
//    }
//  else // Comportement pour les tableaux normaux : compatibilite avec la version precedente : on initialise avec 0.
//    for (int i = 0; i < nb; i++) data[i] = (_TYPE_) 0;
}

//  methode protegee de changement de taille, appelable par les classes derivees
//   (idem que resize_array() mais sans condition sur le type derive de l'objet)
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_array_(int new_size, RESIZE_OPTIONS opt)
{
  assert(new_size >= 0);

  if (mem_ == nullptr)
    mem_ = std::make_shared<Vector_>(Vector_(new_size));
  else
    mem_->resize(new_size);
  span_ = Span_(*mem_);
//  if (new_size > 0)
//    {
//      if (mem_ == nullptr)
//        mem_ = std::make_shared<Vector_>(Vector_(new_size));
//      else
//        mem_->resize(new_size);
//      span_ = Span_(*mem_);
//    }
//  // Soit le tableau est detache (data_==0), soit il est normal (p_!=0)
//  // S'il est normal, il ne faut pas qu'il y ait d'autre reference au tableau, ou alors la taille ne doit pas changer.
//  assert(new_size == mem_.size() || span_ == 0 || (mem_ != 0 && ref_count() == 1));
//  // Le seul cas ou on n'appelle pas memory_resize est le suivant : e tableau est smart_resize
//  //  ET la taille reste inferieure ou egale a la taille allouee ET on ne veut pas initialiser les nouvelles cases
//  if ((opt == RESIZE_OPTIONS::COPY_INIT) || (new_size > memory_size_) || (smart_resize_ == 0))
//    // attention, il existe un cas ou memory_resize realloue un buffer meme si size_array_== new_size:
//    // si on est passe de smart_resize=1 a smart_resize=0, on realloue juste ce qu'il faut.
//    memory_resize(new_size, opt);
//  size_array_ = new_size;
}

//  Amene le tableau dans l'etat "detache". C'est a dire:
//  Si le tableau est "detache" :
//   * ne rien faire
//  Si le tableau est "normal" :
//   * decremente le nombre de references a *p
//   * detruit *p si p->ref_count==0
//   * annule p_, data_ et size_array_
//  Si le tableau est "ref_data" :
//   * annule data_ et size_array_
// Retour: int
//    Signification: 1 si les donnees du tableau ont ete supprimees
// Postcondition: On a p_==0, data_==0 et size_array_==0, memory_size_ = 0. L'attribut smart_resize_ est conserve.
template <typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::detach_array()
{
  mem_ = nullptr;
  span_ = Span_();
  return true;
}

//  Si besoin, alloue une nouvelle zone de memoire, copie les donnees et efface l'ancienne zone de memoire.
//  Attention, on suppose que cette methode est appelee par resize_array().
//  Attention: si ref_count_>1, l'appel a resize_array() est autorise uniquement si la nouvelle taille est identique a la precedente (auquel cas on ne fait rien)
//  Si ref_count_ == 1, l'appel est invalide si p_->data_ est different de data_ (le tableau a ete construit avec ref_array() avec start > 0)
// Precondition:
//  Le tableau doit etre de type "detache" ou "normal" avec ref_count==1, et il faut new_size >= 0
//  On suppose que size_array contient encore le nombre d'elements valides avant changement de taille.
// Parametre: new_size
//  Signification: nouvelle taille demandee pour le tableau.
// Parametre: options
//  Signification: voir ArrOfDouble::RESIZE_OPTIONS
// Postcondition:
//  p_ et data_ sont mis a jour, mais pas size_array_ !!! (on suppose que c'est fait dans resize_array()).
//  Si la nouvelle taille est nulle, on detache le tableau.
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::memory_resize(int new_size, RESIZE_OPTIONS opt)
{
}

#endif /* TRUSTArray_TPP_included */
