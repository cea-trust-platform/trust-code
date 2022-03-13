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
// File:        TRUSTList.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTList_included
#define TRUSTList_included

#include <TRUSTListElem.h>
#include <Double.h>
#include <Nom.h>

#define IMAX 1000000000

template<typename _TYPE_> class TRUSTList_Curseur;

// .DESCRIPTION : Classe qui sert a representer une liste de reels int/double precision. On ne peut pas utiliser la classe container
//  List avec des objets du type int/double car int/double est un type predefini du C++ qui ne possede pas les fonctions exigees par List< >.
template<typename _TYPE_>
class TRUSTList : public TRUSTListElem<_TYPE_>
{
public :
  friend class TRUSTList_Curseur<_TYPE_>;

  TRUSTList() : TRUSTListElem<_TYPE_>()
  {
    min_data = std::is_same<_TYPE_, double>::value ? DMAXFLOAT : IMAX;
    max_data = std::is_same<_TYPE_, double>::value ? -DMAXFLOAT : -IMAX;
    dernier_ = this;
  }

  // Constructeur par copie
  TRUSTList(const TRUSTList& a_list) : TRUSTListElem<_TYPE_>()
  {
    min_data = a_list.min_data;
    max_data = a_list.max_data;
    if (a_list.est_vide()) this->suivant_ = this;
    else
      {
        this->data = a_list.data;
        dernier_ = this;
        if (a_list.suivant_)
          {
            TRUSTListElem<_TYPE_> *next = new TRUSTListElem<_TYPE_>(*a_list.suivant_); //Recursif !!
            this->suivant_ = next;
          }
        else this->suivant_ = 0;
      }
  }

  inline Sortie& printOn(Sortie& os) const;
  inline Entree& readOn(Entree& is);

  inline int size() const;
  inline int contient(_TYPE_) const;
  inline int rang(_TYPE_) const;

  // Description: Renvoie le dernier element de la liste
  inline TRUSTListElem<_TYPE_>& dernier() { return *dernier_; }
  inline const TRUSTListElem<_TYPE_>& dernier() const { return *dernier_; }

  inline _TYPE_& operator[](int );
  inline const _TYPE_& operator[](int ) const;

  inline TRUSTList& add(_TYPE_ ) ;
  inline TRUSTList& add_if_not(_TYPE_ ) ;
  inline TRUSTList& operator=(const TRUSTList& );

  template<typename T>
  friend int operator ==(const TRUSTList<T>& , const TRUSTList<T>& );

  inline void suppr(_TYPE_ );
  inline void vide();
  inline void calcule_min_max();

private:
  TRUSTListElem<_TYPE_>* dernier_;
  _TYPE_ min_data, max_data;
};

// .DESCRIPTION : List_Curseur de reels int/double precision
template<typename _TYPE_>
class TRUSTList_Curseur
{
public :
  TRUSTList_Curseur(const TRUSTList<_TYPE_>& a_list) : curseur(&(TRUSTListElem<_TYPE_>&) a_list) { if (a_list.est_vide()) curseur = 0; }
  TRUSTList_Curseur(const TRUSTListElem<_TYPE_>& a_list) : curseur(&(TRUSTListElem<_TYPE_>&) a_list) { if (a_list.est_vide()) curseur = 0; }

  inline operator bool() const {  return (curseur != 0); } // Teste si le curseur est non nul

  // Avance le curseur dans la liste. Si le curseur est sur le dernier element, il devient nul
  inline void operator++() { curseur = curseur->est_dernier() ? 0 : &(curseur->suivant()); }

  // retourne la valeur du curseur
  inline _TYPE_ valeur() const { return curseur->valeur(); }
  inline _TYPE_& valeur() { return curseur->valeur(); }

  // Affectation d'une liste a une liste_curseur
  inline void operator=(const TRUSTList<_TYPE_>& a_list)
  {
    curseur = (&(TRUSTListElem<_TYPE_>&) a_list);
    if (a_list.est_vide()) curseur = 0;
  }

  // Retourne la liste associee a la liste_curseur
  inline const TRUSTListElem<_TYPE_>& list() const { return *curseur; }
  inline TRUSTListElem<_TYPE_>& list() { return *curseur; }

private :
  TRUSTListElem<_TYPE_>* curseur;
};

// typedefs !!!
using IntList = TRUSTList<int>;
using DoubleList = TRUSTList<double>;
using IntList_Curseur = TRUSTList_Curseur<int>;
using DoubleList_Curseur = TRUSTList_Curseur<double>;

#include <TRUSTList.tpp> // templates specializations ici ;)

#endif /* TRUSTList_included */
