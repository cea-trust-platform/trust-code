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
#ifndef FixedVector_included
#define FixedVector_included
#include <assert.h>
class IJK_Splitting;
// Generic vector of fixed size
template<class T, int N>
class FixedVector
{
public:
  static int size()
  {
    return N;
  }
  T& operator[](int i)
  {
    assert(i>=0 && i<N);
    return data_[i];
  }
  const T& operator[](int i) const
  {
    assert(i>=0 && i<N);
    return data_[i];
  }
#if 1
  void echange_espace_virtuel()
  {
    for (int i = 0; i < N; i++)
      data_[i].echange_espace_virtuel(data_[i].ghost());
  }
  const IJK_Splitting& get_splitting() const
  {
    return data_[0].get_splitting();
  }
#endif
protected:
  T data_[N];
};

template<class T, int N>
inline FixedVector<T,N> operator-(const FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] - v2[i];
  return resu;
}

template<class T, int N>
inline FixedVector<T,N> operator+(const FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] + v2[i];
  return resu;
}

template<class T, int N>
inline FixedVector<T,N> operator*(const FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] * v2[i];
  return resu;
}

template<class T, int N>
inline FixedVector<T,N> operator*(const FixedVector<T,N>& v1, const T& x)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] * x;
  return resu;
}

template<class T, int N>
inline const FixedVector<T,N>& operator*=(FixedVector<T,N>& v1, const T& x)
{
  for (int i = 0; i < N; i++)
    v1[i] *= x;
  return v1;
}

//GAB 02 dec 2020 : on definit le produit_scalaire
// /!\ ATTENTION les deux fixed vectors doivent etre du meme type T !!
// /!\ On a fait la surcharge pour un IJK_FT_double : on peut faire le
//     le prod scal pour un IJK_FT_double du coup
template<class T, int N>
inline const T& produit_scalaire(FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  T resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2[i];
  return resu;
}

template<class T, int N>
inline const FixedVector<T,N>& produit_scalaire(FixedVector<T,N>& v1, const FixedVector<T,1>& v2)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2[0];
  return resu;
}


template<class T, int N>
inline const FixedVector<T,N>& produit_scalaire(FixedVector<T,N>& v1, const double v2)
{
  FixedVector<T,N> resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2;
  return resu;
}
////////////////////////////////////////////////////////////////////////////////


template<class T, int N>
inline const FixedVector<T,N>& operator-=(FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  for (int i = 0; i < N; i++)
    v1[i] -= v2[i];
  return v1;
}

template<class T, int N>
inline const FixedVector<T,N>& operator+=(FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
{
  for (int i = 0; i < N; i++)
    v1[i] += v2[i];
  return v1;
}

template<class T, int N, int M>
inline FixedVector<FixedVector<T,N>,M> operator*(const FixedVector<FixedVector<T,N>,M>& m, const T& v)
{
  FixedVector<FixedVector<T,N>,M> resu;
  for (int j = 0; j < M; j++)
    for (int i = 0; i < N; i++)
      resu[j][i] = m[j][i] * v;
  return resu;
}

template<class T, int N, int M>
inline FixedVector<FixedVector<T,N>,M> operator+(const FixedVector<FixedVector<T,N>,M>& m1,
                                                 const FixedVector<FixedVector<T,N>,M>& m2)
{
  FixedVector<FixedVector<T,N>,M> resu;
  for (int j = 0; j < M; j++)
    for (int i = 0; i < N; i++)
      resu[j][i] = m1[j][i] + m2[j][i];
  return resu;
}


// GAB SURCHARGE OPERATEURS
//template<class T, int N>
//inline FixedVector<T,N>& operator=(FixedVector<T,N>& v1, const FixedVector<T,N>& v2)
//{
//  for (int i = 0; i < N; i++)
//    {
//      v1[i] = v2[i];
//    }
//  return v1;
//}
////////////////////////////////////////////////////////////////////////////////

#endif
