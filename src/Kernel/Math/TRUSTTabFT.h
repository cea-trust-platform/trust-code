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
// File:        TRUSTTabFT.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTTabFT_included
#define TRUSTTabFT_included

#include <TRUSTTab.h>

// Pour Front-Tracking !!

template<typename _TYPE_>
class TRUSTTabFT : public TRUSTTab<_TYPE_>
{
public:

  TRUSTTabFT() { TRUSTTab<_TYPE_>::set_smart_resize(1);}

  TRUSTTabFT(const TRUSTTabFT&) = default;

  TRUSTTabFT(int i, int j)
  {
    TRUSTTab<_TYPE_>::set_smart_resize(1);
    TRUSTTab<_TYPE_>::resize(i, j);
  }

  TRUSTTabFT(const TRUSTTab<_TYPE_>& x)
  {
    TRUSTTab<_TYPE_>::set_smart_resize(1);
    TRUSTTab<_TYPE_>::operator=(x);
  }

  TRUSTTabFT& operator=(const TRUSTTab<_TYPE_>& x)
  {
    TRUSTTab<_TYPE_>::operator=(x);
    return *this;
  }

  TRUSTTabFT& operator=(const TRUSTTabFT& x)
  {
    TRUSTTab<_TYPE_>::operator=(x);
    return *this;
  }

  TRUSTTabFT& operator=(const double x)
  {
    TRUSTTab<_TYPE_>::operator=(x);
    return *this;
  }
};

using IntTabFT = TRUSTTabFT<int>;
using DoubleTabFT = TRUSTTabFT<double>;

template<typename _TYPE_>
class TRUSTArrayFT : public TRUSTArray<_TYPE_>
{
public:
  TRUSTArrayFT() { TRUSTArray<_TYPE_>::set_smart_resize(1); }
  inline TRUSTArrayFT(const TRUSTArrayFT& x);

  TRUSTArrayFT(int n)
  {
    TRUSTArray<_TYPE_>::set_smart_resize(1);
    TRUSTArray<_TYPE_>::resize_array(n);
  }

  TRUSTArrayFT& operator=(_TYPE_ i)
  {
    TRUSTArray<_TYPE_>::operator=(i);
    return *this;
  }

  // pour double seulement
  inline TRUSTArrayFT(const TRUSTArray<_TYPE_>& x);

  // pour int seulement
  inline TRUSTArrayFT& operator=(const TRUSTArray<_TYPE_>& x);
  inline TRUSTArrayFT& operator=(const TRUSTArrayFT& x);
};

using ArrOfIntFT = TRUSTArrayFT<int>;
using ArrOfDoubleFT = TRUSTArrayFT<double>;

/// \cond DO_NOT_DOCUMENT
template<>
inline TRUSTArrayFT<int>::TRUSTArrayFT(const TRUSTArrayFT<int>& x) = default;

template<>
inline TRUSTArrayFT<double>::TRUSTArrayFT(const TRUSTArrayFT<double>& x) : TRUSTArray<double>()
{
  TRUSTArray<double>::set_smart_resize(1);
  TRUSTArray<double>::operator=(x);
}

// pour double seulement
template<>
inline TRUSTArrayFT<int>::TRUSTArrayFT(const TRUSTArray<int>& x) = delete;

template<>
inline TRUSTArrayFT<double>::TRUSTArrayFT(const TRUSTArray<double>& x)
{
  TRUSTArray<double>::set_smart_resize(1);
  TRUSTArray<double>::operator=(x);
}

// pour int seulement
template<>
inline TRUSTArrayFT<double>& TRUSTArrayFT<double>::operator=(const TRUSTArray<double>& x) = delete;

template<>
inline TRUSTArrayFT<int>& TRUSTArrayFT<int>::operator=(const TRUSTArray<int>& x)
{
  TRUSTArray<int>::operator=(x);
  return *this;
}

template<>
inline TRUSTArrayFT<double>& TRUSTArrayFT<double>::operator=(const TRUSTArrayFT<double>& x) = delete;

template<>
inline TRUSTArrayFT<int>& TRUSTArrayFT<int>::operator=(const TRUSTArrayFT<int>& x)
{
  TRUSTArray<int>::operator=(x);
  return *this;
}
/// \endcond
#endif /* TRUSTTabFT_included */
