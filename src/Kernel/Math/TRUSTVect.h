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
// File:        TRUSTVect.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTVect_included
#define TRUSTVect_included

#include <MD_Vector_tools.h>
#include <MD_Vector_base.h>
#include <communications.h>
#include <DescStructure.h>
#include <TRUSTArray.h>
#include <MD_Vector.h>
#include <limits.h>
#include <math.h>

template<typename _TYPE_>
class TRUSTVect : public TRUSTArray<_TYPE_>
{
protected:
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    TRUSTVect* xxx = new  TRUSTVect(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override
  {
    if (TRUSTArray<_TYPE_>::nproc() > 1 && md_vector_.non_nul())
      {
        Cerr << "Error in TRUSTVect::printOn: try to print a parallel vector" << finl;
        Process::exit();
      }
    TRUSTArray<_TYPE_>::printOn(os);
    return os;
  }

  // Description: Lecture d'un vecteur sequentiel (comme un ArrOfDouble)
  //  Attention: appel invalide si le vecteur a un MD_Vector non nul. (pour les vecteurs paralleles, utiliser une methode de sauvegarde/reprise)
  Entree& readOn(Entree& is) override
  {
    if (md_vector_.non_nul())
      {
        // Que veut-on faire si on lit dans un vecteur ayant deja une structure parallele ?
        Cerr << "Error in TRUSTVect::readOn: vector has a parallel structure" << finl;
        Process::exit();
      }
    TRUSTArray<_TYPE_>::readOn(is);
    size_reelle_ = TRUSTArray<_TYPE_>::size_array();
    line_size_ = 1;
    return is;
  }

public:
  virtual ~TRUSTVect() { }

  TRUSTVect() : size_reelle_(0), line_size_(1) { }

  // Description: construction d'un vecteur de taille n. Les elements du vecteur sont initialises a zero par defaut. Pour ne pas initialiser les valeurs, utiliser ceci:
  //   DoubleVect toto;
  //   toto.resize(n, NOCOPY_NOINIT);
  TRUSTVect(int n) :   TRUSTArray<_TYPE_>(n), size_reelle_(n), line_size_(1) { }

  // Description: Constructeur par copie. Il s'agit d'un "deep copy" voir ArrOfDouble::ArrOfDouble(const ArrOfDouble &)
  //  Remarque: il n'y a pas de constructeur par copie a partir de ArrOfDouble
  //   Ceci est volontaire, sinon on risque de grosses pertes de performances par creation implicite d'objets, difficile a trouver.
  //   (exemple: appel d'une methode toto(const IntVect &) avec un ArrOfInt produit une copie du tableau !)
  //  Utiliser copy() pour copier un ArrOfDouble dans un DoubleVect
  TRUSTVect(const TRUSTVect& v) : TRUSTArray<_TYPE_>(v), md_vector_(v.md_vector_), size_reelle_(v.size_reelle_), line_size_(v.line_size_) { }

  inline TRUSTVect& operator=(const TRUSTVect&);
  inline TRUSTVect& operator=(_TYPE_);
  inline int size() const;
  inline int size_totale() const;
  inline int size_reelle() const;
  inline int size_reelle_ok() const;
  inline int line_size() const;
  inline void resize(int, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void copy(const TRUSTArray<_TYPE_>&, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void copy(const TRUSTVect&, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void operator+=(const TRUSTVect& v) { operator_add(*this, v); }
  inline void operator+=(const _TYPE_ x) { operator_add(*this, x); }
  inline void operator-=(const TRUSTVect& v) { operator_sub(*this, v); }
  inline void operator-=(const _TYPE_ x) { operator_sub(*this, x); }
  inline void operator*=(const TRUSTVect& v) { operator_multiply(*this, v); }
  inline void operator*= (const _TYPE_ x) { operator_multiply(*this, x); }

  /********************** */
  // FIXME : PAS POUR INT
  inline void operator/=(const TRUSTVect& v) { operator_divide(*this, v); }
  inline void operator/= (const _TYPE_ x) { operator_divide(*this, x); }
  // Options par defaut choisies pour compatibilite avec la version precedente. Attention: il y avait un echange_espace_virtuel avant, ce n'est pas strictement equivalent
  inline void abs(Mp_vect_options opt = VECT_ALL_ITEMS) { operator_abs(*this, opt); }
  inline void carre(Mp_vect_options opt = VECT_ALL_ITEMS) { carre(*this, opt); }
  inline void racine_carree(Mp_vect_options opt = VECT_ALL_ITEMS) { racine_carree_(*this, opt); }
  inline void ajoute(double alpha, const TRUSTVect& y, Mp_vect_options opt = VECT_ALL_ITEMS);// x+=alpha*y
  inline void ajoute_sans_ech_esp_virt(double alpha, const TRUSTVect& y, Mp_vect_options opt = VECT_REAL_ITEMS) { ajoute_alpha_v(*this, alpha, y, opt); } // x+=alpha*y sans echange_espace_virtuel
  inline void ajoute_produit_scalaire(double alpha, const TRUSTVect& x, const TRUSTVect& y, Mp_vect_options opt = VECT_ALL_ITEMS) { ajoute_produit_scalaire(*this, alpha, x, y, opt); } // z+=alpha*x*y;
  inline void ajoute_carre(double alpha, const TRUSTVect& y, Mp_vect_options opt = VECT_ALL_ITEMS) { ajoute_carre_(*this, alpha, y, opt); }
  // FIXME : PAS POUR INT
  /********************** */

  // par defaut: min et max sur items reels (compat. 1.5.6):
  inline _TYPE_ local_max_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return local_max_vect_(*this, opt); }
  inline _TYPE_ local_min_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return local_min_vect_(*this, opt); }
  inline _TYPE_ local_max_abs_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return local_max_abs_vect_(*this, opt); }
  inline _TYPE_ local_min_abs_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return local_min_abs_vect_(*this, opt); }
  inline _TYPE_ mp_max_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return mp_max_vect_(*this, opt); }
  inline _TYPE_ mp_min_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return mp_min_vect_(*this, opt); }
  inline _TYPE_ mp_max_abs_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return mp_max_abs_vect_(*this, opt); }
  inline _TYPE_ mp_min_abs_vect(Mp_vect_options opt = VECT_REAL_ITEMS) const { return mp_min_abs_vect_(*this, opt); }
  inline _TYPE_ mp_norme_vect() const { return mp_norme_vect(*this); } // FIXME : PAS POUR INT

  // methodes virtuelles

  inline virtual void ref(const TRUSTVect&);
  inline virtual void echange_espace_virtuel();
  inline virtual void set_md_vector(const MD_Vector&);
  inline virtual void jump(Entree&);
  inline virtual void lit(Entree&, int resize_and_read=1);
  inline virtual void ecrit(Sortie&) const;
  inline virtual void detach_vect() { md_vector_.detach(); }
  inline virtual const MD_Vector& get_md_vector() const { return md_vector_; }
  inline void resize_tab(int n, Array_base::Resize_Options opt = Array_base::COPY_INIT) override;
  inline void ref_data(_TYPE_* ptr, int new_size) override;
  inline void ref_array(TRUSTArray<_TYPE_>&, int start = 0, int sz = -1) override;

  // Description: met l'objet dans l'etat obtenu par le constructeur par defaut.
  inline void reset() override
  {
    md_vector_.detach();
    line_size_ = 1;
    size_reelle_ = 0;
    TRUSTArray<_TYPE_>::reset();
  }

protected:
  inline void set_line_size_(int n);
  inline void resize_vect_(int n, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  inline void copy_(const TRUSTVect& v, Array_base::Resize_Options opt = Array_base::COPY_INIT);

private:
  // Un DoubleVect est un ArrOfDouble qui possede eventuellement une structure de tableau distribue. Ce pointeur peut etre nul.
  MD_Vector md_vector_;

  // Propriete size_reelle du tableau (fournie par scattered_vect_data). -1 => l'appel a size_reelle() et size() est invalide pour ce vecteur.
  int size_reelle_;

  // Facteur multiplicatif a appliquer entre md_vector_.nb_items_tot() et size_array() et entre md_vector_.nb_items_reels() et size_reelle_.
  // Si l'objet est un tableau, ce facteur est generalement egal au produit des dimension(i) pour i>1 (une ligne du tableau par item geometrique du descripteur)
  // Attention, line_size_ peut etre nul pour un tableau a zero colonnes mais pas s'il y a un descripteur attache.
  int line_size_;
};

using IntVect = TRUSTVect<int>;
using DoubleVect = TRUSTVect<double>;

/* ********************************** *
 * FONCTIONS NON MEMBRES DE TRUSTVect *
 * ********************************** */

#include <TRUSTVect_tools.tpp> // external templates function specializations ici ;)

/* ****************************** *
 * FONCTIONS MEMBRES DE TRUSTVect *
 * ****************************** */

#include <TRUSTVect.tpp> // templates specializations ici ;)

#endif /* TRUSTVect_included */
