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

#ifndef TRUSTVect_TPP_included
#define TRUSTVect_TPP_included

#include <string>
#include <TRUSTVect.h>

//  Taille de l'espace "reel" du vecteur. (si md_vector_ est nul, cette valeur est identique a size_array(),
//   sinon, soit elle est egale a md_vector_....get_nb_items(),
//          soit l'appel est invalide (en particulier si les items reels ne sont pas regroupes en debut de tableau)
//  On peut interroger le vecteur pour savoir si size_reelle() est valide avec size_reelle_ok()
template<typename _TYPE_, typename _SIZE_>
inline _SIZE_ TRUSTVect<_TYPE_,_SIZE_>::size_reelle() const
{
  // Si cet assert plante, c'est que l'appel a ete declare invalide par le MD_Vect associe a ce vecteur.
  assert(size_reelle_ >= 0);
  // Si cet assert plante, c'est que le tableau a ete redimensionne avec resize_array() au lieu de resize(). (invalide pour un Vect ou un Tab).
  assert( (TRUSTArray<_TYPE_,_SIZE_>::size_array() == size_reelle_ || md_vector_.non_nul()) );
  return size_reelle_;
}

//  renvoie 1 si l'appel a size() et size_reelle() est valide, 0 sinon
template<typename _TYPE_, typename _SIZE_>
inline _SIZE_ TRUSTVect<_TYPE_,_SIZE_>::size_reelle_ok() const
{
  return size_reelle_ >= 0;
}

//  Identique a size_reelle()
template<typename _TYPE_, typename _SIZE_>
inline _SIZE_ TRUSTVect<_TYPE_,_SIZE_>::size() const
{
  return size_reelle();
}

template<typename _TYPE_, typename _SIZE_>
template<typename _SCALAR_TYPE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ajoute(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt)
{
  assert ( (std::is_scalar<_SCALAR_TYPE_>::value) );
  ajoute_alpha_v(*this, alpha, y, opt);
  if (opt == VECT_ALL_ITEMS) echange_espace_virtuel();
}

//  Identique a size_array()
template<typename _TYPE_, typename _SIZE_>
inline _SIZE_ TRUSTVect<_TYPE_,_SIZE_>::size_totale() const
{
  return TRUSTArray<_TYPE_,_SIZE_>::size_array();
}

template<typename _TYPE_, typename _SIZE_>
inline int TRUSTVect<_TYPE_,_SIZE_>::line_size() const
{
  // Si line_size_ est nulle, size_array doit etre nul aussi
  assert( (line_size_ > 0 || TRUSTArray<_TYPE_,_SIZE_>::size_array() == 0) );
  return line_size_;
}

//  change l'attribut line_size_ du tableau avec n >= 1. n == 0 est autorise uniquement si size_array_ == 0
// Precondition: le md_vector_ doit etre nul (il faut attribuer md_vector_ apres la line_size_ car lorsqu'on attribue md_vector_ on teste
//  la validite des tailles de tableaux en fonction de line_size_) ou la line_size_ ne doit pas changer (cas d'un resize qui ne change rien)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::set_line_size_(int n)
{
  assert(!md_vector_.non_nul() || line_size_ == n);
  assert(n >= 0);
  line_size_ = n;
}

//  Change la taille du vecteur (identique a resize_array() pour le traitement des anciennes valeurs et de nouvelles cases).
//  Attention: Cette methode n'est pas virtuelle, et afin d'eviter d'amener un DoubleTab/Int dans un etat invalide, l'appel est interdit si l'objet
//  est de ce type. Dans ce cas, voir resize_tab.
// Precondition: l'appel est interdit si le vecteur a une structure parallele. Le vecteur doit etre "resizable" (voir preconditions de ArrOfDouble::resize_array()).
//  Appel interdit si l'objet n'est pas un DoubleVect (sinon mauvaise initialisation des dimensions du tableau)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::resize(_SIZE_ n, RESIZE_OPTIONS opt)
{
  // Verifie que l'objet est bien du type DoubleVect
  assert( (n == TRUSTArray<_TYPE_,_SIZE_>::size_array() || std::string(typeid(*this).name()).find("TRUSTVect") != std::string::npos) );
  resize_vect_(n, opt);
}

//  Methode interne de resize (appellee par DoubleTab/Int::resize(...)) sans precondition sur le type de l'objet.
// Precondition: l'appel est interdit si le vecteur a une structure parallele.
//  Le vecteur doit etre "resizable" (voir preconditions de ArrOfDouble/Int::resize_array()). n doit etre un multiple de line_size_
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::resize_vect_(_SIZE_ n, RESIZE_OPTIONS opt)
{
  if (md_vector_.non_nul())
    {
      Cerr << "Resize of a distributed array is forbidden!" << finl;
      Process::exit();
    }
  assert(n == 0 || (n > 0 && line_size_ > 0 && n % line_size_ == 0));
  TRUSTArray<_TYPE_,_SIZE_>::resize_array_(n, opt);
  size_reelle_ = n;
  // ne pas mettre line_size_ a 1 ici, voir DoubleTab::resize_dim0()
}

//  copie la structure et les valeurs du tableau v dans *this avec ArrOfDouble/Int::operator=()
//  (attention, si le tableau est de type ref_data ou ref, restrictions et cas particuliers !!!)
//  Attention: si on ne veut pas copier les structures paralleles, utiliser inject_array()
// Precondition: si le tableau *this doit etre resize, il doit etre de type DoubleVect (et pas d'un type derive !)
//  Si le tableau *this a deja une structure parallele, l'appel n'est autorise que les md_vector
//  sont deja identiques, sinon il faut d'abord faire un reset() du tableau (pour copier la structure), ou utiliser inject_array() (pour ne pas copier la structure).
//  (ceci pour eviter d'ecraser accidentellement une structure parallele alors qu'on ne veut que copier les valeurs).
template<typename _TYPE_, typename _SIZE_>
inline TRUSTVect<_TYPE_,_SIZE_>& TRUSTVect<_TYPE_,_SIZE_>::operator=(const TRUSTVect& v)
{
  copy(v);
  return *this;
}

template<typename _TYPE_, typename _SIZE_>
inline TRUSTVect<_TYPE_,_SIZE_>& TRUSTVect<_TYPE_,_SIZE_>::operator=(_TYPE_ x)
{
  TRUSTArray<_TYPE_,_SIZE_>::operator=(x);
  return *this;
}

//  detache le tableau et l'attache a v (sauf si v==*this, ne fait rien)
//  Fait pointer le tableau sur la meme domaine de memoire que v et copie le MD_Vector (utilise ArrOfDouble/Int::attach_array())
//  Attention, il devient alors interdit de resizer le tableau v ainsi que *this
//  Methode virtuelle reimplementee dans DoubleTab
// Precondition:
//  L'objet ne doit pas etre un sous-type de DoubleVect (sinon mauvaise initialisation  des dimensions.
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ref(const TRUSTVect& v)
{
  if (&v != this)
    {
      TRUSTArray<_TYPE_,_SIZE_>::detach_array();
      TRUSTArray<_TYPE_,_SIZE_>::attach_array(v);
      md_vector_ = v.md_vector_;
      size_reelle_ = v.size_reelle_;
      line_size_ = v.line_size_;
    }
}

//  copie de la structure et des valeurs (si opt==COPY_INIT) du tableau v.
//  Attention, v doit vraiment etre de type ArrOfDouble/Int, pas d'un type derive (sinon ambiguite: faut-il copier ou pas le MD_Vector ?)
// Precondition: Le vecteur ne doit pas avoir de structure de tableau distribue et il doit vraiment etre de type Double/IntVect.
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::copy(const TRUSTArray<_TYPE_,_SIZE_>& v, RESIZE_OPTIONS opt)
{
  assert(std::string(typeid(*this).name()).find("TRUSTVect") != std::string::npos);
  assert(std::string(typeid(v).name()).find("TRUSTArray") != std::string::npos);
  assert(!md_vector_.non_nul());
  resize(v.size_array(), opt);
  if (opt != RESIZE_OPTIONS::NOCOPY_NOINIT) inject_array(v);
}

//  copie de la structure du vecteur v et des valeurs si opt==COPY_INIT.
// Precondition: idem que operator=(const DoubleVect &)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::copy(const TRUSTVect& v, RESIZE_OPTIONS opt)
{
  if (&v != this)
    {
      // Interdiction de resizer si l'objet est d'un type derive de DoubleVect (sinon mauvaise dimension(0) !)
      assert( (v.size_array() == TRUSTArray<_TYPE_,_SIZE_>::size_array() || std::string(typeid(*this).name()).find("TRUSTVect") != std::string::npos) );
      copy_(v, opt);
    }
}

//  methode protegee appelable depuis une classe derivee (pas de precondition sur le type derive de *this)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::copy_(const TRUSTVect& v, RESIZE_OPTIONS opt)
{
  assert(&v != this); // Il faut avoir fait le test avant !
  // Si le vecteur a deja une structure parallele, la copie n'est autorisee que si
  // le vecteur source a la meme structure. Si ce n'est pas le cas, utiliser inject_array() pour copier uniquement les valeurs, ou faire d'abord reset() si on veut ecraser la structure.
#ifndef LATATOOLS
  assert((!md_vector_.non_nul()) || (md_vector_ == v.md_vector_));
#endif
  TRUSTArray<_TYPE_,_SIZE_>::resize_array_(v.size_array(), RESIZE_OPTIONS::NOCOPY_NOINIT);
  if (v.isDataOnDevice()) allocateOnDevice(*this); // Alloue de la memoire sur le device si v est deja alloue sur le device
  if (opt != RESIZE_OPTIONS::NOCOPY_NOINIT)
    TRUSTArray<_TYPE_,_SIZE_>::inject_array(v);
  md_vector_ = v.md_vector_; // Pour le cas ou md_vector_ est nul et pas v.md_vector_
  size_reelle_ = v.size_reelle_;
  line_size_ = v.line_size_;
}

//  methode virtuelle identique a resize_array(), permet de traiter de facon generique les ArrOf, Vect et Tab.
//   Cree un tableau sequentiel... Si l'objet est de type Int/DoubleVect, appel a resize(n)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::resize_tab(_SIZE_ n, RESIZE_OPTIONS opt)
{
  resize(n, opt);
}

/*! See TRUSTArray::ref_data().
 * Create an array without any parallel structure.
 */
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ref_data(_TYPE_* ptr, _SIZE_ size)
{
  md_vector_.detach();
  TRUSTArray<_TYPE_,_SIZE_>::ref_data(ptr, size);
  size_reelle_ = size;
  line_size_ = 1;
}

//  voir ArrOfDouble/Int::ref_array(). (cree un tableau sans structure parallele)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ref_array(TRUSTArray<_TYPE_,_SIZE_>& m, _SIZE_ start, _SIZE_ new_size)
{
  md_vector_.detach();
  TRUSTArray<_TYPE_,_SIZE_>::ref_array(m, start, new_size);
  size_reelle_ = TRUSTArray<_TYPE_,_SIZE_>::size_array(); // pas size qui peut valoir -1
  line_size_ = 1;
}

//  associe le md_vector au vecteur et initialise l'attribut size_reelle_ (voir methode size_reelle())
//  Si md_vector est nul, detache simplement le md_vector existant.
// Precondition: le vecteur doit deja avoir la taille appropriee au nouveau md_vector, c'est a dire md_vector...get_nb_items_tot() * line_size_
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::set_md_vector(const MD_Vector& md_vector)
{
#ifndef LATATOOLS
  _SIZE_ size_r = TRUSTArray<_TYPE_,_SIZE_>::size_array();
  if (md_vector.non_nul())
    {
      size_r = md_vector->get_nb_items_reels();
      if (size_r >= 0) size_r *= line_size_;
      else size_r = -1; // Cas particulier ou la size_reelle ne veut rien dire

      _SIZE_ size_tot = md_vector->get_nb_items_tot() * line_size_;
      if (size_tot != TRUSTArray<_TYPE_,_SIZE_>::size_array())
        {
          Cerr << "Internal error in TRUSTVect::set_md_vector(): wrong array size\n"
               << " Needed size = " << md_vector->get_nb_items_tot() << " x " << line_size_
               << "\n Actual size = " << TRUSTArray<_TYPE_,_SIZE_>::size_array() << finl;
          Process::exit();
        }
      if (line_size_ == 0)
        {
          Cerr << "Internal error in TRUSTVect::set_md_vector():\n"
               << " cannot attach descriptor to empty array (line_size_ is zero)" << finl;
          Process::exit();
        }
    }
  size_reelle_ = size_r;
  md_vector_ = md_vector;
#endif
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::echange_espace_virtuel()
{
#ifndef LATATOOLS
  if(Process::is_sequential()) return;
#if INT_is_64_ == 2
  // echange_espace_virtuel() should not be called on big array or array of TID:
  assert( (!std::is_same<_TYPE_, trustIdType>::value && !std::is_same<_SIZE_, trustIdType>::value) );
#endif
  MD_Vector_tools::echange_espace_virtuel(*this);
#endif
}

#if INT_is_64_ == 2
// We should never have to do MPI on big arrays:
// [ABN] do not know why, constexpr test in generic version not working ...
template<> inline void TRUSTVect<trustIdType, int>::echange_espace_virtuel()
{
  assert(false);
  Process::exit("echange_espace_virtuel() called on big array or array of TID!");
}
template<> inline void TRUSTVect<int, trustIdType>::echange_espace_virtuel()
{
  assert(false);
  Process::exit("echange_espace_virtuel() called on big array or array of int!");
}
template<> inline void TRUSTVect<trustIdType, trustIdType>::echange_espace_virtuel()
{
  assert(false);
  Process::exit("echange_espace_virtuel() called on big array or array of TID!");
}
template<> inline void TRUSTVect<double, trustIdType>::echange_espace_virtuel()
{
  assert(false);
  Process::exit("echange_espace_virtuel() called on big array or array of double!");
}
template<> inline void TRUSTVect<float, trustIdType>::echange_espace_virtuel()
{
  assert(false);
  Process::exit("echange_espace_virtuel() called on big array or array of float!");
}
#endif

//  ecriture des valeurs du tableau "raw" sans structure parallele
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ecrit(Sortie& os) const
{
#ifndef LATATOOLS
  TRUSTArray<_TYPE_,_SIZE_>::printOn(os);
  os << (int)-1 << finl; // le marqueur -1 indique que c'est le nouveau format "ecrit", sans structure parallele
#endif
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::jump(Entree& is)
{
  TRUSTVect::lit(is, 0 /* Do not resize&read the array */);
}

//  lecture d'un tableau pour reprise de calcul. On lit les valeurs "raw".
//  Attention, si le tableau n'est pas vide, il doit deja avoir la bonne taille et la bonne structure, sinon erreur !
// Parameter resize_and_read if the array is sized AND read (by default, yes)
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::lit(Entree& is, bool resize_and_read)
{
#ifndef LATATOOLS
  _SIZE_ sz = -1;
  is >> sz;
  if (resize_and_read)
    {
      if (TRUSTArray<_TYPE_,_SIZE_>::size_array() == 0 && (!get_md_vector().non_nul()))
        resize(sz, RESIZE_OPTIONS::NOCOPY_NOINIT);
      else if (sz != TRUSTArray<_TYPE_,_SIZE_>::size_array())
        {
          // Si on cherche a relire un tableau de taille inconnue, le tableau doit etre reset() a l'entree. On n'aura pas la structure parallele du tableau !
          Cerr << "Error in TRUSTVect::lit(Entree & is): array has already a structure with incorrect size" << finl;
          Process::exit();
        }
      is.get(TRUSTArray<_TYPE_,_SIZE_>::addr(), sz);
    }
  else
    {
      // May be slow if large chunks are read:
      // Double tmp;
      //for (_SIZE_ i=0;i<sz;i++) is >> tmp;
      // So we bufferize:
      const _SIZE_ sz_max = 128000;
      _SIZE_ buffer_size = std::min(sz,sz_max);
      TRUSTArray<_TYPE_,_SIZE_> tmp(buffer_size);
      while(sz>buffer_size)
        {
          is.get(tmp.addr(), buffer_size);
          sz-=buffer_size;
        }
      is.get(tmp.addr(), sz);
    }
  _SIZE_ sz_reel = -2;
  is >> sz_reel;
  if (sz_reel >= 0)
    {
      // Lecture de l'ancien format. Ignore les valeurs lues.
      _SIZE_ sz_virt;
      is >> sz_virt;
      DescStructure toto;
      is >> toto;
      if (!std::is_same<_TYPE_,int>::value) /* double ou float */
        {
          TRUSTArray<int,_SIZE_> it_communs;
          is >> it_communs;
          TRUSTArray<int,_SIZE_> it_communs_tot;
          is >> it_communs_tot;
        }
    }
#endif
}

template<typename _TYPE_, typename _SIZE_>
template<typename _SCALAR_TYPE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ajoute_sans_ech_esp_virt(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt)
{
  ajoute_alpha_v(*this, alpha, y, opt); // x+=alpha*y sans echange_espace_virtuel
}

template<typename _TYPE_, typename _SIZE_>
template<typename _SCALAR_TYPE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ajoute_produit_scalaire(_SCALAR_TYPE_ alpha, const TRUSTVect& x, const TRUSTVect& y, Mp_vect_options opt)
{
  ajoute_produit_scalaire(*this, alpha, x, y, opt);  // z+=alpha*x*y;
}

template<typename _TYPE_, typename _SIZE_>
template<typename _SCALAR_TYPE_>
inline void TRUSTVect<_TYPE_,_SIZE_>::ajoute_carre(_SCALAR_TYPE_ alpha, const TRUSTVect& y, Mp_vect_options opt)
{
  ajoute_operation_speciale_generic<TYPE_OPERATION_VECT_SPEC::CARRE_,_TYPE_>(*this,alpha,y,opt);
}


#endif /* TRUSTVect_TPP_included */
