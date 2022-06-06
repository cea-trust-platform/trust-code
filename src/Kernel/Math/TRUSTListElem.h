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

#ifndef TRUSTListElem_included
#define TRUSTListElem_included

#include <assert.h>
#include <arch.h>

template<typename _TYPE_> class TRUSTList;

// .DESCRIPTION : Classe qui sert a representer une liste de reels int/double precision. On ne peut pas utiliser la classe container
//  List avec des objets du type int/double car int/double est un type predefini du C++ qui ne possede pas les fonctions exigees par List< >.
template<typename _TYPE_>
class TRUSTListElem
{
public :
  friend class TRUSTList<_TYPE_>;

  TRUSTListElem() { suivant_ = this; } // Construit une liste vide.
  TRUSTListElem(const _TYPE_ x) : data(x), suivant_(0) { } // Construit le singleton (x).
  TRUSTListElem(const TRUSTListElem&);
  ~TRUSTListElem();

  inline int est_vide() const { return suivant_ == this; } // renvoie 1 si la liste est vide. 0 sinon
  inline int est_dernier() const { return ((est_vide()) || (suivant_ == 0)); } // renvoie 1 si il n'y a pas de suivant. 0 sinon

  inline _TYPE_& valeur() { return data; }
  inline _TYPE_ valeur() const { return data; }

  inline TRUSTListElem& add(_TYPE_ ) ;
  inline TRUSTListElem& suivant() { return *suivant_; }
  inline const TRUSTListElem& suivant() const { return *suivant_; }

  template<typename T>
  friend int operator ==(const TRUSTListElem<T>& , const TRUSTListElem<T>& );

protected:
  _TYPE_ data;
  TRUSTListElem* suivant_;
};

using IntListElem = TRUSTListElem<int>;
using DoubleListElem = TRUSTListElem<double>;

// Description: Constructeur par copie
// Parametre: const TRUSTListElem& list (la liste a copier)
template<typename _TYPE_>
TRUSTListElem<_TYPE_>::TRUSTListElem(const TRUSTListElem<_TYPE_>& list)
{
  if (list.est_vide()) suivant_ = this;
  else
    {
      data = list.data;
      if (list.suivant_)
        {
          TRUSTListElem<_TYPE_> *next = new TRUSTListElem<_TYPE_>(*list.suivant_); //Recursif !!
          suivant_ = next;
        }
      else suivant_ = 0;
    }
}

template<typename _TYPE_>
TRUSTListElem<_TYPE_>::~TRUSTListElem()
{
  if (est_vide()) suivant_ = 0;

  if (suivant_)
    {
      // On ne garde pas la version delete suivant_ car sinon on est limite par le nombre d'appel recursif possible
      // delete suivant_;
      TRUSTListElem<_TYPE_> *poignee, *pr;
      pr = suivant_;
      suivant_ = 0;
      while (pr)
        {
          poignee = pr->suivant_;
          pr->suivant_ = 0;
          delete pr;
          pr = poignee;
        }
    }
}

// Description: insertion en queue
// Parametre: int/double value_to_add (element a ajouter)
template<typename _TYPE_>
inline TRUSTListElem<_TYPE_>& TRUSTListElem<_TYPE_>::add(_TYPE_ value_to_add)
{
  assert(est_dernier());
  TRUSTListElem<_TYPE_> *next = new TRUSTListElem<_TYPE_>(value_to_add);
  suivant_ = next;
  return *this;
}

// Description:  Operateur de comparaison de deux listes
// Retour:  1 si les listes sont egales, 0 sinon
template<typename _TYPE_>
int operator ==(const TRUSTListElem<_TYPE_>& list1 , const TRUSTListElem<_TYPE_>& list2)
{
  int retour = 1;
  if (list1.data != list2.data) retour = 0;
  if ((!list1.est_dernier()) && (list2.est_dernier())) retour = 0;
  if ((list1.est_dernier()) && (!list2.est_dernier())) retour = 0;
  if ((!list1.est_dernier()) && (!list2.est_dernier()))  retour = (*list1.suivant_ == *list2.suivant_);
  return retour;
}

#endif /* TRUSTListElem_included */
