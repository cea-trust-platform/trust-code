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
// File:        TRUSTArray.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTArray_TPP_included
#define TRUSTArray_TPP_included

#include <string>

// Description : Change le nombre d'elements du tableau.
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
inline void TRUSTArray<_TYPE_>::resize_array(int new_size, Array_base::Resize_Options opt)
{
  // Si le tableau change de taille, il doit etre du type TRUSTArray
  // XXX : Elie & Adrien : on fait comme ca... a voir si ca marche quand ArrOfXXX deviennent de typedef ...
  assert(new_size == size_array_ || std::string(typeid(*this).name()).find("TRUSTArray") != std::string::npos );
  resize_array_(new_size, opt);
}

// Description: methode virtuelle (dans Array_base) identique a resize_array(), permet de traiter
//   de facon generique les ArrOf, Vect et Tab. Si l'objet est de type TRUSTArray, appel a resize_array(n)
// Prerequis: le tableau doit etre "resizable" (voir resize_array()). S'il est d'un type derive (Vect ou Tab),
//    il ne doit pas avoir de descripteur parallele si la taille est effectivement modifiee.
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_tab(int n, Array_base::Resize_Options opt)
{
  resize_array(n, opt);
}

// Description: Change le mode l'allocation memoire: reallocation d'un tableau a chaque changement de taille (flag = 0) ou reallocation
// uniquement si la taille augmente et par doublement de la taille du tableau (flag = 1).
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::set_smart_resize(int flag)
{
  assert(flag == 0 || flag == 1);
  smart_resize_ = flag;
}

// Description: Change le mode d'allocation memoire lors des resize (voir VTRUSTdata et TRUST_ptr_trav)
//   Exemple pour creer un tableau avec allocation temporaire:
//    DoubleTab tab; // Creation d'un tableau vide
//    tab.set_mem_storage(TEMP_STORAGE); // Changement de mode d'allocation
//    tab.resize(n); // Allocation memoire
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::set_mem_storage(const Storage storage)
{
  storage_type_ = storage;
}

// Description:
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
  detach_array();
  data_ = ptr;
  size_array_ = size;
  memory_size_ = size; // Pour passer les tests si on resize a la meme taille
}

// Description:
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

// Description: Copie les donnees du tableau m. Si "m" n'a pas la meme taille que "*this", on fait un resize_array.
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
      resize_array(new_size, NOCOPY_NOINIT);
      inject_array(m);
    }
  return *this;
}

// Description: operateur [] retourne le ieme element du tableau
// Parametre: int i
//    Signification: indice dans l'intervalle 0 <= i < size_array()
// Exception: assert si i n'est pas dans l'intervalle
template<typename _TYPE_>
inline _TYPE_& TRUSTArray<_TYPE_>::operator[](int i)
{
  assert(i >= 0 && i < size_array_);
  return data_[i];
}

// Description: operateur [] pour TRUSTArray<double> : retourne le ieme element du tableau
// Exception:
//    assert si la valeur sort de l'intervalle : [ -DMAXFLOAT,DMAXFLOAT ]
//    assert si i n'est pas dans l'intervalle
template<>
inline double& TRUSTArray<double>::operator[](int i)
{
  assert(i >= 0 && i < size_array_);
  assert(data_[i] > -DMAXFLOAT && data_[i] < DMAXFLOAT);
  return data_[i];
}

template<typename _TYPE_>
inline const _TYPE_& TRUSTArray<_TYPE_>::operator[](int i) const
{
  assert(i >= 0 && i < size_array_);
  return data_[i];
}

template<>
inline const double& TRUSTArray<double>::operator[](int i) const
{
  assert(i >= 0 && i < size_array_);
  assert(data_[i] > -DMAXFLOAT && data_[i] < DMAXFLOAT);
  return data_[i];
}

// Description: Renvoie un pointeur sur le premier element du tableau. Le pointeur est nul si le tableau est "detache".
//   Attention, l'adresse peut changer apres un appel a resize_array(), ref_data, ref_array, ...
// Retour: const _TYPE_*
//   Signification: pointeur sur le premier element du tableau
template <typename _TYPE_>
inline _TYPE_* TRUSTArray<_TYPE_>::addr()
{
  return data_;
}

template <typename _TYPE_>
inline const _TYPE_* TRUSTArray<_TYPE_>::addr() const
{
  return data_;
}

// Description: Renvoie la taille du tableau (nombre d'elements declares a la construction ou a resize_array()).
//    C'est le nombre d'elements accessibles a operator[]
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::size_array() const
{
  return size_array_;
}

// Description: Retourne le nombre de references des donnees du tableau si le tableau est "normal", -1 s'il est "detache" ou "ref_data"
// Retour: int
//    Signification: ref_count_
template <typename _TYPE_>
inline int TRUSTArray<_TYPE_>::ref_count() const
{
  return p_ ? p_->ref_count() : -1;
}

// Description: Ajoute une case en fin de tableau et y stocke la "valeur"
// Precondition:
//  Tableau doit etre de type "smart_resize" (sinon, ecroulement des performances). De plus, le tableau ne doit pas etre "ref_data",
//  et il ne doit pas y avoir plus d'une reference a la zone de memoire pointee (meme precondition que resize_array())
//  Le tableau doit etre de type TRUSTArray (pas un type derive)
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::append_array(_TYPE_ valeur)
{
  assert(smart_resize_);
  const int n = size_array_;
  resize_array(size_array_+1, COPY_NOINIT);
  data_[n] = valeur;
}

// Description: Copie les elements source[first_element_source + i] dans les elements  (*this)[first_element_dest + i] pour 0 <= i < nb_elements
//    Les autres elements de (*this) sont inchanges.
// Precondition:
// Parametre:       const ArrOfDouble& m
//  Signification:   le tableau a utiliser, doit etre different de *this !
// Parametre:       int nb_elements
//  Signification:   nombre d'elements a copier, nb_elements >= -1. Si nb_elements==-1, on copie tout le tableau m.
//  Valeurs par defaut: -1
// Parametre:       int first_element_dest
//  Valeurs par defaut: 0
// Parametre:       int first_element_source
//  Valeurs par defaut: 0
// Retour: ArrOfDouble&
//    Signification: *this
// Exception: Sort en erreur si la taille du tableau m est plus grande que la taille de tableau this.
template <typename _TYPE_>
inline TRUSTArray<_TYPE_>& TRUSTArray<_TYPE_>::inject_array(const TRUSTArray& source, int nb_elements, int first_element_dest, int first_element_source)
{
  assert(&source != this && nb_elements >= -1);
  assert(first_element_dest >= 0 && first_element_source >= 0);

  if (nb_elements < 0) nb_elements = source.size_array();

  assert(first_element_source + nb_elements <= source.size_array());
  assert(first_element_dest + nb_elements <= size_array());

  if (nb_elements > 0)
    {
      _TYPE_ * addr_dest = data_ + first_element_dest;
      const _TYPE_ * addr_source = source.addr() + first_element_source;
      // PL: On utilise le memcpy car c'est VRAIMENT plus rapide (10% +vite sur RNR_G20)
      memcpy(addr_dest , addr_source, nb_elements * sizeof(_TYPE_));
      /*
        int i;
        for (i = 0; i < nb_elements; i++) {
        addr_dest[i] = addr_source[i];
        } */
    }
  return *this;
}

// Description: Fonction de comparaison utilisee pour trier le tableau dans ArrOfDouble::trier(). Voir man qsort
static True_int fonction_compare_arrofdouble_ordonner(const void * data1, const void * data2)
{
  const double x = *(const double*)data1;
  const double y = *(const double*)data2;
  if (x < y) return -1;
  else if (x > y) return 1;
  else return 0;
}

// Description: Fonction de comparaison utilisee pour trier le tableau dans ArrOfInt::trier(). Voir man qsort
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

// Description: Tri des valeurs du tableau dans l'ordre croissant. La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
template <>
inline void TRUSTArray<double>::ordonne_array()
{
  const int size = size_array_;
  if (size > 1)
    {
      double * data = data_;
      qsort(data, size, sizeof(double), fonction_compare_arrofdouble_ordonner);
    }
}

template <>
inline void TRUSTArray<int>::ordonne_array()
{
  const int size = size_array_;
  if (size > 1)
    {
      int * data = data_;
      qsort(data, size, sizeof(int), fonction_compare_arrofint_ordonner);
    }
}

// Description:  Tri des valeurs du tableau dans l'ordre croissant et suppresion des doublons La fonction utilisee est qsort de stdlib (elle est en n*log(n)).
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::array_trier_retirer_doublons()
{
  const int size_ = size_array_;
  if (size_ <= 0) return;

  ordonne_array(); // Tri dans l'ordre croissant
  // Retire les doublons (codage optimise en pointeurs)
  _TYPE_ last_value = *data_;
  _TYPE_ *src = data_ + 1;
  _TYPE_ *dest = data_ + 1;
  for (int i = size_ - 1; i != 0; i--)
    {
      _TYPE_ x = *(src++);
      if (x != last_value)
        {
          *(dest++) = x;
          last_value = x;
        }
    }
  int new_size_ = dest - data_;
  resize_array(new_size_);
}

// Description: Amene le tableau dans l'etat "normal", "detache" ou "ref_array" en associant une sous-zone de memoire du tableau m,
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
  // Le tableau doit etre detache
  assert(data_ == 0 && p_ == 0);
  // Le tableau doit etre different de *this
  assert(&m != this);
  if (size < 0) size = m.size_array_ - start;

  assert(start >= 0 && size >=0 && start + size <= m.size_array_);
  if (m.size_array() > 0)
    {
      p_ = m.p_;
      if (p_) p_->add_one_ref();
      data_ = m.data_ + start;
      size_array_ = size;
      memory_size_ = m.memory_size_ - start;
      smart_resize_ = m.smart_resize_;
    }
  else
    {
      // Cas particulier ou on attache un tableau de taille nulle: en theorie, c'est pareil qu'un tableau de taille non nulle, MAIS
      //  dans les operateurs (ex:Op_Dift_VDF_Face_Axi), une ref est construite avant que le tableau ne prenne sa taille definitive. Donc, pour ne pas
      //  empecher le resize, il ne faut pas attacher le tableau s'il n'a pas encore la bonne taille. Solution propre: reecrire les operateurs pour
      //  qu'ils ne prennent pas une ref avant que le tableau ne soit valide et faire p_ = m.p_ dans tous les cas.
      // Process::Journal() << "Warning TRUSTArray::attach_array(m), m.size_array()==0, on n attache pas le tableau" << finl;
    }
}

// Description: Remplit "nb" cases consecutives du tableau a partir de la case "first" avec une valeur par defaut.
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
inline void TRUSTArray<_TYPE_>::fill_default_value(Array_base::Resize_Options opt, int first, int nb)
{
  assert((nb == 0) || (first >= 0 && first < memory_size_));
  assert((nb == 0) || (nb > 0 && nb <= memory_size_ - first));
  _TYPE_ * data = data_;
  assert(data!=0 || nb==0);
  data += first;
  if (opt != COPY_INIT)
    {
#ifndef NDEBUG // On initialise uniquement en mode debug
      static const _TYPE_  INVALIDE_ = (std::is_same<_TYPE_,double>::value) ? DMAXFLOAT*0.999 : INT_MIN;
      for (int i = 0; i < nb; i++) data[i] = INVALIDE_;
#endif
    }
  else // Comportement pour les tableaux normaux : compatibilite avec la version precedente : on initialise avec 0.
    for (int i = 0; i < nb; i++) data[i] = (_TYPE_) 0;
}

// Description: methode protegee de changement de taille, appelable par les classes derivees
//   (idem que resize_array() mais sans condition sur le type derive de l'objet)
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::resize_array_(int new_size, Array_base::Resize_Options opt)
{
  assert(new_size >= 0);
  // Soit le tableau est detache (data_==0), soit il est normal (p_!=0)
  // S'il est normal, il ne faut pas qu'il y ait d'autre reference au tableau, ou alors la taille ne doit pas changer.
  assert(new_size == size_array_ || data_ == 0 || (p_ != 0 && ref_count() == 1));
  // Le seul cas ou on n'appelle pas memory_resize est le suivant : e tableau est smart_resize
  //  ET la taille reste inferieure ou egale a la taille allouee ET on ne veut pas initialiser les nouvelles cases
  if ((opt == COPY_INIT) || (new_size > memory_size_) || (smart_resize_ == 0))
    // attention, il existe un cas ou memory_resize realloue un buffer meme si size_array_== new_size:
    // si on est passe de smart_resize=1 a smart_resize=0, on realloue juste ce qu'il faut.
    memory_resize(new_size, opt);
  size_array_ = new_size;
}

// Description: Amene le tableau dans l'etat "detache". C'est a dire:
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
inline int TRUSTArray<_TYPE_>::detach_array()
{
  int retour = 0;
  if (p_)
    {
      // Le tableau est de type "normal". Si la zone de memoire n'est plus utilisee par personne, on la detruit.
      if ((p_->suppr_one_ref()) == 0)
        {
          delete p_;
          retour = 1;
        }
      p_ = 0;
    }
  data_ = 0;
  size_array_ = 0;
  memory_size_ = 0;
  return retour;
}

// Description: Si besoin, alloue une nouvelle zone de memoire, copie les donnees et efface l'ancienne zone de memoire.
//  Attention, on suppose que cette methode est appelee par resize_array().
//  Attention: si ref_count_>1, l'appel a resize_array() est autorise uniquement si la nouvelle taille est identique a la precedente (auquel cas on ne fait rien)
//  Si ref_count_ == 1, l'appel est invalide si p_->data_ est different de data_ (le tableau a ete construit avec ref_array() avec start > 0)
// Precondition:
//  Le tableau doit etre de type "detache" ou "normal" avec ref_count==1, et il faut new_size >= 0
//  On suppose que size_array contient encore le nombre d'elements valides avant changement de taille.
// Parametre: new_size
//  Signification: nouvelle taille demandee pour le tableau.
// Parametre: options
//  Signification: voir ArrOfDouble::Resize_Options
// Postcondition:
//  p_ et data_ sont mis a jour, mais pas size_array_ !!! (on suppose que c'est fait dans resize_array()).
//  Si la nouvelle taille est nulle, on detache le tableau.
template <typename _TYPE_>
inline void TRUSTArray<_TYPE_>::memory_resize(int new_size, Array_base::Resize_Options opt)
{
  assert(new_size >= 0);

  // Si new_size==size_array_, on ne fait rien, c'est toujours autorise
  if (new_size == size_array_) return;

  // Sinon, il faut que le resize soit autorise, c'est a dire
  //  - soit le tableau est detache
  //  - soit le tableau est normal et il n'y a pas d'autre reference au tableau
  assert((data_ == 0) || ((p_ != 0) && (p_->get_data() == data_) && (ref_count() == 1)));

  // Occupation memoire de l'ancien tableau:
  int old_mem_size = memory_size_;
  assert((!p_) || (p_->get_size() == memory_size_));
  assert(memory_size_ >= size_array_);

  // Occupation memoire du nouveau tableau : Si smart_resize, on prend au moins deux fois la taille precedente, ou new_size
  int new_mem_size = new_size;
  if (smart_resize_)
    {
      if (new_size <= old_mem_size) new_mem_size = old_mem_size;
      else if (new_size < old_mem_size * 2) new_mem_size = old_mem_size * 2;
      else if (new_size > old_mem_size && old_mem_size > INT_MAX / 2) new_mem_size = INT_MAX;
    }

  if (new_mem_size != old_mem_size)
    {
      // detach_array() efface le contenu de size_array_. On le met de cote:
      const int old_size_array = size_array_;
      if (new_mem_size == 0) detach_array(); // La nouvelle taille est nulle, on cree un tableau "detache"
      else
        {
          // Allocation d'une nouvelle zone
          VTRUSTdata<_TYPE_> * new_p = new VTRUSTdata<_TYPE_>(new_mem_size, storage_type_);
          _TYPE_ * new_data = new_p->get_data();
          // Raccourci si le tableau etait "detache", inutile de copier les anciennes donnees. On copie si COPY_OLD est demande
          int copy_size = 0;
          if (data_ != 0)
            {
              // Calcul du nombre d'elements a copier vers la nouvelle zone de memoire : c'est le min de l'ancienne et de la nouvelle taille.
              if (opt != NOCOPY_NOINIT)
                {
                  copy_size = size_array_;
                  if (new_size < copy_size) copy_size = new_size;
                  // Copie des valeurs dans le nouveau tableau
                  for (int i = 0; i < copy_size; i++) new_data[i] = data_[i];
                }
              detach_array(); // Destruction de l'ancienne zone (si plus aucune reference)
            }
          // On attache la nouvelle zone de memoire
          p_ = new_p;
          data_ = new_data;
          memory_size_ = new_mem_size;
          // Initialisation des cases supplementaires avec une valeur par defaut
          fill_default_value(opt, copy_size, new_mem_size - copy_size);
          // Restaure l'ancienne valeur de size_array_
          size_array_ = old_size_array;
        }
    }
  else
    {
      // Pas de reallocation, initialisation si besoin
      if (opt == COPY_INIT && new_size > size_array_) fill_default_value(opt, size_array_, new_size - size_array_);
    }
}

#endif /* TRUSTArray_TPP_included */
