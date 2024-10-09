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
#ifndef IJK_Field_vector_included
#define IJK_Field_vector_included
#include <assert.h>
#include <FixedVector.h>
#include <IJK_Field.h>

/*! @brief The class IJK_Field_vector is a fixed array of polymorphic IJK fields.
 *
 * For example, the velocity is a fixed vector with 3 components.
 * Each component is a (smart) pointer, thus allowing any derived class of IJK_Field_template to be used (this is used for example in cut-cell methods).
 *
 * The bracket '[]' operator allows the direct retrieval of a reference to one of the components.
 *
 * The function IJK_Navier_stokes_tools.h::allocate_velocity() gives an example on how the object is instanciated:
 *
 *       // Construction of the IJK_Field_template objects
 *       v.get_ptr(0) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
 *       v.get_ptr(1) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
 *       v.get_ptr(2) = std::make_shared<IJK_Field_template<T,TRUSTArray<T>>>();
 *
 *       // Allocation of the fields
 *       v[0].allocate(s, IJK_Splitting::FACES_I, ghost);
 *       v[1].allocate(s, IJK_Splitting::FACES_J, ghost);
 *       v[2].allocate(s, IJK_Splitting::FACES_K, ghost);
 */

template<class T, int N>
class IJK_Field_vector : FixedVector<std::shared_ptr<IJK_Field_template<T,TRUSTArray<T>>>, N>
{
public:
  IJK_Field_vector() { }

  static int size()
  {
    return N;
  }
  IJK_Field_template<T,TRUSTArray<T>>& operator[](int i)
  {
    assert(i>=0 && i<N);
    return *this->data_[i];
  }
  const IJK_Field_template<T,TRUSTArray<T>>& operator[](int i) const
  {
    assert(i>=0 && i<N);
    return *this->data_[i];
  }
  std::shared_ptr<IJK_Field_template<T,TRUSTArray<T>>>& get_ptr(int i)
  {
    assert(i>=0 && i<N);
    return this->data_[i];
  }
  const std::shared_ptr<IJK_Field_template<T,TRUSTArray<T>>>& get_ptr(int i) const
  {
    assert(i>=0 && i<N);
    return this->data_[i];
  }
#if 1
  void echange_espace_virtuel()
  {
    for (int i = 0; i < N; i++)
      this->data_[i]->echange_espace_virtuel(this->data_[i]->ghost());
  }
  const IJK_Splitting& get_splitting() const
  {
    return this->data_[0]->get_splitting();
  }
#endif
protected:
};

template<class T, int N>
inline IJK_Field_vector<double, N> operator-(const IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] - v2[i];
  return resu;
}

template<class T, int N>
inline IJK_Field_vector<double, N> operator+(const IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] + v2[i];
  return resu;
}

template<class T, int N>
inline IJK_Field_vector<double, N> operator*(const IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] * v2[i];
  return resu;
}

template<class T, int N>
inline IJK_Field_vector<double, N> operator*(const IJK_Field_vector<double, N>& v1, const IJK_Field_template<T,TRUSTArray<T>>& x)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu[i] = v1[i] * x;
  return resu;
}

template<class T, int N>
inline const IJK_Field_vector<double, N>& operator*=(IJK_Field_vector<double, N>& v1, const IJK_Field_template<T,TRUSTArray<T>>& x)
{
  for (int i = 0; i < N; i++)
    v1[i] *= x;
  return v1;
}

template<class T, int N>
inline const IJK_Field_template<T,TRUSTArray<T>>& produit_scalaire(IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  IJK_Field_template<T,TRUSTArray<T>> resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2[i];
  return resu;
}

template<class T, int N>
inline const IJK_Field_vector<double, N>& produit_scalaire(IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, 1>& v2)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2[0];
  return resu;
}


template<class T, int N>
inline const IJK_Field_vector<double, N>& produit_scalaire(IJK_Field_vector<double, N>& v1, const double v2)
{
  IJK_Field_vector<double, N> resu;
  for (int i = 0; i < N; i++)
    resu += v1[i] * v2;
  return resu;
}


template<class T, int N>
inline const IJK_Field_vector<double, N>& operator-=(IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  for (int i = 0; i < N; i++)
    v1[i] -= v2[i];
  return v1;
}

template<class T, int N>
inline const IJK_Field_vector<double, N>& operator+=(IJK_Field_vector<double, N>& v1, const IJK_Field_vector<double, N>& v2)
{
  for (int i = 0; i < N; i++)
    v1[i] += v2[i];
  return v1;
}

using IJK_Field_vector3_double = IJK_Field_vector<double, 3>;
using IJK_Field_vector3_int = IJK_Field_vector<int, 3>;

#endif
