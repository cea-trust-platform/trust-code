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

#ifndef Simd_template_included
#define Simd_template_included

#include <assert.h>
#include <stdint.h>
#include <cstdlib>
#include <cmath>

#include <kokkos++.h>
#ifdef _KOKKOS_AVX
#include <Kokkos_SIMD.hpp>
#endif

/*! @brief This class provides a generic access to simd operations on x86, x86 AMD and ARM architectures.
 *
 * Functionalities provided by the class are :
 *   - load vector size aligned data from memory (SimdGet)
 *   - getting x[i-1] and x[i+1] for finite difference algorithms
 *     (SimdGetAtLeft, SimdGetAtRight, etc)
 *   - arithmetic operations (+ - * /)
 *   - conditional affectation (SimdSelect)
 *
 */
template<typename _TYPE_>
class Simd_template
{
public:
  using value_type = _TYPE_;
#ifdef _KOKKOS_AVX
  using simd_type = Kokkos::Experimental::native_simd<_TYPE_>;
#endif
  Simd_template() { data_ = 0.; };
  // Commodity default constructor (provides implicit conversion)
  Simd_template(_TYPE_ x) : data_(x) {};
#ifdef _KOKKOS_AVX
  Simd_template(simd_type x) : data_(x) {};
#endif
  // Size of the vector
  static int size()
  {
#ifdef _KOKKOS_AVX
    return int(simd_type::size());
#else
    return 1;
#endif
  }
  void operator+=(Simd_template a) { data_ += a.data_; }
  void operator*=(Simd_template a) { data_ *= a.data_; }
  void operator-=(Simd_template a) { data_ -= a.data_; }

  _TYPE_& operator[](int idx)
  {
    assert(idx>=0 && idx<size());
#ifdef _KOKKOS_AVX
    return data_[idx];
#else
    return data_;
#endif
  }

  _TYPE_ operator[](int idx) const
  {
    assert(idx>=0 && idx<size());
#ifdef _KOKKOS_AVX
    return data_[idx];
#else
    return data_;
#endif
  }

  _TYPE_ sumReduction()
  {
#ifdef _KOKKOS_AVX
    _TYPE_ res(0.);
    for(int ivec = 0; ivec < size(); ivec++)
      res += data_[ivec];
    return res;
#else
    return data_;
#endif
  }

#ifdef _KOKKOS_AVX
  simd_type data_;
#else
  _TYPE_ data_;
#endif
};

using Simd_float = Simd_template<float>;
using Simd_double = Simd_template<double>;
using Simd_int = Simd_template<int>;
using RightSimdArrayView = Kokkos::View<Simd_double*,Kokkos::LayoutRight>;
using RightSimdTabView = Kokkos::View<Simd_double**,Kokkos::LayoutRight>;

#ifdef _KOKKOS_AVX
using tag_type = Kokkos::Experimental::element_aligned_tag;
#endif

/*! @brief returns 1 if pointer is aligned on size bytes, 0 otherwise
 *
 * Warn: size must be a power of 2.
 *
 */
inline int aligned(const void *ptr, int size)
{
  // uintptr_t is the result of pointer operations like ptr1-ptr2
  return ((uintptr_t)ptr & (uintptr_t)(size-1)) == 0;
}

/*! @brief Returns the vector found at address data.
 *
 * data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGet(const _TYPE_ *data)
{
#ifdef _KOKKOS_AVX
  Simd_template<_TYPE_> v;
  v.data_.copy_from(data,tag_type());
  return v;
#else
  return *data;
#endif
}

/*! @brief Stores vector x at address data.
 *
 * data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline void SimdPut(_TYPE_ *data, Simd_template<_TYPE_> x)
{
#ifdef _KOKKOS_AVX
  x.data_.copy_to(data, tag_type());
#else
  *data = x.data_;
#endif
}

/*! @brief Returns the vector x starting at adress data+1, data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGetAtRight(const _TYPE_ *data)
{
#ifdef _KOKKOS_AVX
  Simd_template<_TYPE_> v;
  v.data_.copy_from(data+1,tag_type());
  return v;
#else
  return data[1];
#endif
}

/*! @brief Returns the vector x starting at adress data-1 data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGetAtLeft(const _TYPE_ *data)
{
#ifdef _KOKKOS_AVX
  Simd_template<_TYPE_> v;
  v.data_.copy_from(data-1,tag_type());
  return v;
#else
  return data[-1];
#endif
}

/*! @brief Returns the vector left and center starting at adress data-1 and data data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftCenter(const _TYPE_ *data, Simd_template<_TYPE_>& left, Simd_template<_TYPE_>& center)
{
#ifdef _KOKKOS_AVX
  left.data_.copy_from(data-1, tag_type());
  center.data_.copy_from(data, tag_type());
#else
  left = data[-1];
  center = data[0];
#endif
}

/*! @brief Returns the vector center and right starting at adress data and data+1 data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline void SimdGetCenterRight(const _TYPE_ *data,
                               Simd_template<_TYPE_>& center,
                               Simd_template<_TYPE_>& right)
{
#ifdef _KOKKOS_AVX
  center.data_.copy_from(data, tag_type());
  right.data_.copy_from(data+1, tag_type());
#else
  center = data[0];
  right = data[1];
#endif
}

/*! @brief Returns the vectors left, center and right starting at adress data-1, data and data+1 data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftCenterRight(const _TYPE_ *data,
                                   Simd_template<_TYPE_>& left,
                                   Simd_template<_TYPE_>& center,
                                   Simd_template<_TYPE_>& right)
{
#ifdef _KOKKOS_AVX
  left.data_.copy_from(data-1, tag_type());
  center.data_.copy_from(data, tag_type());
  right.data_.copy_from(data+1, tag_type());
#else
  left = data[-1];
  center = data[0];
  right = data[1];
#endif
}

/*! @brief Returns the vectors leftleft, left, center and right starting at adress data-2, data-1, data and data+1 data must be aligned for the architecture
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftleftLeftCenterRight(const _TYPE_ *data,
                                           Simd_template<_TYPE_>& leftleft,
                                           Simd_template<_TYPE_>& left,
                                           Simd_template<_TYPE_>& center,
                                           Simd_template<_TYPE_>& right)
{
#ifdef _KOKKOS_AVX
  leftleft.data_.copy_from(data-2, tag_type());
  left.data_.copy_from(data-1, tag_type());
  center.data_.copy_from(data, tag_type());
  right.data_.copy_from(data+1, tag_type());
#else
  leftleft = data[-2];
  left = data[-1];
  center = data[0];
  right = data[1];
#endif
}

/*! @brief returns a+b
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator+(Simd_template<_TYPE_> a, Simd_template<_TYPE_> b) { return a.data_ + b.data_; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator+(Simd_template<_TYPE_> a, double b) { return a.data_ + b; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator+(double a, Simd_template<_TYPE_> b) { return a + b.data_; }
/*! @brief returns a-b
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator-(Simd_template<_TYPE_> a, Simd_template<_TYPE_> b) { return a.data_ - b.data_; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator-(Simd_template<_TYPE_> a, double b) { return a.data_ - b; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator-(double a, Simd_template<_TYPE_> b) { return a - b.data_; }


/*! @brief returns a*b
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator*(Simd_template<_TYPE_> a, Simd_template<_TYPE_> b) { return a.data_ * b.data_; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator*(Simd_template<_TYPE_> a, double b) { return a.data_ * b; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator*(double a, Simd_template<_TYPE_> b) { return a * b.data_; }

template<typename _TYPE_>
inline Simd_template<_TYPE_> operator/(Simd_template<_TYPE_> a, Simd_template<_TYPE_> b) { return a.data_ / b.data_; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator/(Simd_template<_TYPE_> a, double b) { return a.data_ / b; }
template<typename _TYPE_>
inline Simd_template<_TYPE_> operator/(double a, Simd_template<_TYPE_> b) { return a / b.data_; }

template<typename _TYPE_>
inline Simd_template<_TYPE_> Simd_absolute_value(Simd_template<_TYPE_> a)
{
#ifdef _KOKKOS_AVX
  return Kokkos::abs(a.data_);
#else
  return std::abs(a.data_);
#endif
}

template<typename _TYPE_>
inline Simd_template<_TYPE_> Simd_exp(Simd_template<_TYPE_> a)
{
#ifdef _KOKKOS_AVX
  return Kokkos::exp(a.data_);
#else
  return std::exp(a.data_);
#endif
}

template<typename _TYPE_>
inline Simd_template<_TYPE_> Simd_sqrt(Simd_template<_TYPE_> a)
{
#ifdef _KOKKOS_AVX
  return Kokkos::sqrt(a.data_);
#else
  return std::sqrt(a.data_);
#endif
}

// Returns a vector built with min(a[i],b[i]) (element wise)
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdMin(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>&   b)
{
#ifdef _KOKKOS_AVX
  return Kokkos::min(a.data_, b.data_);
#else
  return (a.data_ < b.data_) ? a : b;
#endif
}

// Returns a vector built with max(a[i],b[i]) (element wise)
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdMax(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>&   b)
{
#ifdef _KOKKOS_AVX
  return Kokkos::max(a.data_, b.data_);
#else
  return (a.data_ > b.data_) ? a : b;
#endif
}

// Approximation of a/b not available with Kokkos... classic division
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdDivideMed(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>& b)
{
  return a.data_ / b.data_;
}

// // Approximation of 1/b not available with Kokkos... classic reciprocal
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdReciprocalMed(const Simd_template<_TYPE_>& b)
{
  return 1. / b.data_;
}


/*! @brief This function performs the following operation on the vectors for (i=0; i<size())
 *
 *    if (x1[i] < x2[i])
 *      result[i] = value_if_x1_lower_than_x2[i]
 *    else
 *      result[i] = value_otherwise[i]
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdSelect(Simd_template<_TYPE_> x1,
                                        Simd_template<_TYPE_> x2,
                                        Simd_template<_TYPE_> value_if_x1_lower_than_x2,
                                        Simd_template<_TYPE_> value_otherwise)
{
#ifdef _KOKKOS_AVX
  Simd_template<_TYPE_> res(value_otherwise.data_);
  where(x1.data_ < x2.data_, res.data_) = value_if_x1_lower_than_x2.data_;
  return res;
#else
  return (x1.data_ < x2.data_) ? value_if_x1_lower_than_x2.data_ : value_otherwise.data_;
#endif
}

#endif


