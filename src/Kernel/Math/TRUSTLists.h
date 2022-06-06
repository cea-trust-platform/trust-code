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

#ifndef TRUSTLists_included
#define TRUSTLists_included

#include <TRUSTList.h>

inline void throw_()
{
  Cerr << "Unable to carry out allocation " << finl;
  Process::exit();
}

// .DESCRIPTION : Un tableau de listes de type IntList
template<typename _TYPE_>
class TRUSTLists
{
public :
  virtual ~TRUSTLists()
  {
    if (sz)
      {
        assert(data);
        delete[] data;
      }
    else
      assert(data == 0);
  }

  TRUSTLists() : sz(0), data(0) { } // Construit un tableau vide de listes

  // Construit un tableau de i listes
  TRUSTLists(int i) : sz(i)
  {
    if (i == 0) data = 0;
    else
      {
        data = new TRUSTList<_TYPE_>[i];
        if (!data) throw_();
      }
  }

  // Constructeur par copie
  TRUSTLists(const TRUSTLists& vect) : sz(vect.sz), data(new TRUSTList<_TYPE_>[vect.sz])
  {
    if (!data) throw_();
    int i = sz;
    if (i == 0) data = 0;
    else while (i--) data[i] = vect[i];
  }

  inline Sortie& printOn(Sortie& s) const
  {
    int i = sz;
    while (i--) data[i].printOn(s);
    return s << finl;
  }

  inline Entree& readOn(Entree& is) { return is; }

  inline TRUSTLists& operator=(const TRUSTLists& );

  inline const TRUSTList<_TYPE_>& operator[](int) const;
  inline TRUSTList<_TYPE_>& operator[](int);

  inline void dimensionner(int );
  inline int size() const { return sz; } // retourne le nombre de listes dans le tableau
  inline int search(const TRUSTList<_TYPE_>&) const;

protected :
  int sz;
  TRUSTList<_TYPE_>* data;
};

using IntLists = TRUSTLists<int>;
using DoubleLists = TRUSTLists<double>;

// Description: Copie et affectation. Les anciennes donnees sont perdues
template<typename _TYPE_>
inline TRUSTLists<_TYPE_>& TRUSTLists<_TYPE_>::operator=(const TRUSTLists<_TYPE_>& vect)
{
  sz = vect.sz;
  if (data) delete[] data;

  if (sz)
    {
      data = new TRUSTList<_TYPE_>[sz];
      if (!data) throw_();
      int i = sz;
      while (i--) data[i] = vect[i];
    }
  else data = 0;
  return *this;
}

// Description: Recherche d'une liste dans le tableau
template<typename _TYPE_>
inline int TRUSTLists<_TYPE_>::search(const TRUSTList<_TYPE_>& t) const
{
  int i = sz, retour = -1;
  while (i--)
    if (data[i] == t)
      {
        retour = i;
        i = 0;
      }
  return retour;
}

// Description: Acces a la ieme liste du tableau
template<typename _TYPE_>
inline const TRUSTList<_TYPE_>& TRUSTLists<_TYPE_>::operator[](int i) const
{
  assert((i >= 0) && (i < sz));
  return data[i];
}

template<typename _TYPE_>
inline TRUSTList<_TYPE_>& TRUSTLists<_TYPE_>::operator[](int i)
{
  assert((i >= 0) && (i < sz));
  return data[i];
}

// Description: Redimensionne un tableau de listes
template<typename _TYPE_>
inline void TRUSTLists<_TYPE_>::dimensionner(int i)
{
  assert(sz == 0 && data == 0);
  sz = i;
  if (i == 0) data = 0;
  else
    {
      data = new TRUSTList<_TYPE_>[i];
      if (!data) throw_();
    }
}

#endif /* TRUSTLists_included */
