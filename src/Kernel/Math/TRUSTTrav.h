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
// File:        TRUSTTrav.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTTrav_included
#define TRUSTTrav_included

#include <TRUSTTab.h>

// .DESCRIPTION : Tableau de travail a n entrees pour n<= 4
//  C'est un TRUSTTab avec allocation dans un pool de memoire gere par la_memoire. L'allocation initiale est realisee avec alloc_temp_storage
template<typename _TYPE_>
class TRUSTTrav : public TRUSTTab<_TYPE_>
{
protected:
  inline unsigned taille_memoire() const override { throw; }
  inline Sortie& printOn(Sortie& os) const override { return TRUSTTab<_TYPE_>::printOn(os); }
  inline Entree& readOn(Entree& is) override { return TRUSTTab<_TYPE_>::readOn(is); }

  inline int duplique() const override
  {
    TRUSTTrav* xxx = new  TRUSTTrav(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

public:
  inline TRUSTTrav() { TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE); }

  inline TRUSTTrav(int n)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::resize(n);
  }

  inline TRUSTTrav(int n1, int n2)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::resize(n1, n2);
  }

  inline TRUSTTrav(int n1, int n2, int n3)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::resize(n1, n2, n3);
  }

  inline TRUSTTrav(int n1, int n2, int n3, int n4)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::resize(n1, n2, n3, n4);
  }

  //  ATTENTION: construit un tableau de meme taill et de meme structure (espaces virtuels), mais initialise avec ZERO !!!
  inline TRUSTTrav(const TRUSTTab<_TYPE_>& tab)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::copy(tab, Array_base::NOCOPY_NOINIT);
    TRUSTTab<_TYPE_>::operator=(ZERO);
  }

  // Constructeur par copie
  //  ATTENTION: construit un tableau de meme taill et de meme structure (espaces virtuels), mais initialise avec ZERO !!!
  inline TRUSTTrav(const TRUSTVect<_TYPE_>& tab)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::set_line_size_(tab.line_size());
    TRUSTTab<_TYPE_>::resize(tab.size_array(), Array_base::NOCOPY_NOINIT);
    TRUSTTab<_TYPE_>::set_md_vector(tab.get_md_vector());
    TRUSTTab<_TYPE_>::operator=(ZERO);
  }

  // Constructeur par copie
  //  ATTENTION: construit un tableau de meme taill et de meme structure (espaces virtuels), mais initialise avec ZERO !!!
  inline TRUSTTrav(const TRUSTTrav& tab) : TRUSTTab<_TYPE_>(tab)
  {
    TRUSTTab<_TYPE_>::set_mem_storage(Array_base::TEMP_STORAGE);
    TRUSTTab<_TYPE_>::copy(tab, Array_base::NOCOPY_NOINIT);
    TRUSTTab<_TYPE_>::operator=(ZERO);
  }

  // Operateurs copie

  // Operateur copie (on ne veut pas l'operateur par defaut)
  inline TRUSTTrav& operator=(const TRUSTTrav& tab)
  {
    TRUSTTab<_TYPE_>::operator=(tab);
    return *this;
  }

  //  Operateur copie d'un tableau (copie structure ET contenu)
  inline TRUSTTrav& operator=(const TRUSTTab<_TYPE_>& tab)
  {
    // ATTENTION: note aux programmeurs
    //  La declaration de cet operateur est indispensable, sinon
    //   IntTab b;
    //   IntTrav a;
    //   a = b
    //  est traduit en
    //   IntTrav tmp(b); // copie la structure mais met les valeurs a zero
    //   a.operator=(tmp);
    TRUSTTab<_TYPE_>::operator=(tab);
    return *this;
  }

  //  Operateur copie d'un tableau (copie structure ET contenu)
  inline TRUSTTrav& operator=(const TRUSTVect<_TYPE_>& tab)
  {
    // ATTENTION: note aux programmeurs ...
    TRUSTTab<_TYPE_>::operator=(tab);
    return *this;
  }

  // Comme on surcharge l'operateur copie, il faut redefinir celui-la aussi.
  inline TRUSTTrav& operator=(_TYPE_ d)
  {
    TRUSTTab<_TYPE_>::operator=(d);
    return *this;
  }

private:
  static constexpr _TYPE_ ZERO = (_TYPE_)0;
};

using IntTrav = TRUSTTrav<int>;
using DoubleTrav = TRUSTTrav<double>;

#endif /* TRUSTTrav_included */
