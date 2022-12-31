/****************************************************************************
* Copyright (c) 2023, CEA
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
#include <Vc/Vc>
#include <cstdlib>
#include <cmath>

#if defined(VC_VC_) && (defined(WITH_SSE) || defined(WITH_AVX))
#define OPTIM_AVX_
#endif

/*! @brief This class provides a generic access to simd operations on x86, x86 AMD and ARM architectures.
 *
 * Functionalities provided by the class are :
 *   - load vector size aligned data from memory (SimdGet)
 *   - getting x[i-1] and x[i+1] for finite difference algorithms
 *     (SimdGetAtLeft, SimdGetAtRight, etc)
 *   - arithmetic operations (+ - * /)
 *   - conditional affectation (SimdSelect)
 *  See simd_malloc() and simd_free() to allocate aligned blocs of memory.
 *
 */
template<typename _TYPE_>
class Simd_template
{
public:
  using value_type = _TYPE_;

  Simd_template() { data_ = 0.; };
  // Commodity default constructor (provides implicit conversion)
#ifdef OPTIM_AVX_
  Simd_template(Vc::Vector<_TYPE_> x) : data_(x) {};
#endif
  Simd_template(_TYPE_ x) : data_(x) {};

  // Size of the vector
  static int size()
  {
#ifdef OPTIM_AVX_
    return Vc::Vector<_TYPE_>::Size ;
#else
    return 1;
#endif
  }

  void operator+=(Simd_template a) { data_ += a.data_; }
  void operator*=(Simd_template a) { data_ *= a.data_; }
  void operator-=(Simd_template a) { data_ -= a.data_; }

#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> data_;
#else
  _TYPE_ data_;
#endif

};

using Simd_float = Simd_template<float>;
using Simd_double = Simd_template<double>;

/*! @brief returns the size in bytes of SIMD vectors on the current architecture
 *
 */
inline static constexpr int simd_getalign()
{
#ifdef OPTIM_AVX_
  return Vc_1::MemoryAlignment;
#else
  return 1;
#endif
}

/*! @brief allocates a memory bloc of given size with proper alignment for SIMD.
 *
 */
template<typename T>
inline T* simd_malloc (std::size_t size)
{
#ifdef OPTIM_AVX_
  return Vc::malloc<T, Vc::AlignOnCacheline>(size);
#else
  T* res_ptr = (T*)malloc(size * sizeof(T));
  return res_ptr;
#endif
}

/*! @brief frees a memory bloc previously allocated with simd_malloc()
 *
 */
inline void simd_free(void* ptr)
{
#ifdef OPTIM_AVX_
  Vc::free(ptr);
#else
  free(ptr);
#endif
}

// uintptr_t should be defined in stdint.h
//  (this type is the result of pointer operations like ptr1-ptr2)
typedef uintptr_t uintptr_type;

/*! @brief returns 1 if pointer is aligned on size bytes, 0 otherwise
 *
 * Warn: size must be a power of 2.
 *
 */
inline int aligned(const void *ptr, int size)
{
  return ((uintptr_type)ptr & (uintptr_type)(size-1)) == 0;
}

/*! @brief Returns the vector found at address data.
 *
 * data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGet(const _TYPE_ *data)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> v(data, Vc::Aligned);
  return v;
#else
  return *data;
#endif
}

/*! @brief Stores vector x at address data.
 *
 * data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline void SimdPut(_TYPE_ *data, Simd_template<_TYPE_> x)
{
#ifdef OPTIM_AVX_
  x.data_.store(data, Vc::Aligned);
#else
  *data = x.data_;
#endif
}

/*! @brief Returns the vector x starting at adress data+1 data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGetAtRight(const _TYPE_ *data)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data+Vc::Vector<_TYPE_>::Size, Vc::Aligned);

  //first_vector and second_vector are concatenated and then shifted by 1 to the left
  Vc::Vector<_TYPE_> data_plus_1 = first_vector.shifted(1, second_vector);
  return data_plus_1;
#else
  return data[1];
#endif
}

/*! @brief Returns the vector x starting at adress data-1 data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdGetAtLeft(const _TYPE_ *data)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data - Vc::Vector<_TYPE_>::Size, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data, Vc::Aligned);

  //first_vector and second_vector are concatenated and then shifted by size_of_vector-1 to the left
  Vc::Vector<_TYPE_> data_minus_1 = first_vector.shifted(Vc::Vector<_TYPE_>::Size - 1, second_vector);
  return data_minus_1;
#else
  return data[-1];
#endif
}

/*! @brief Returns the vector left and center starting at adress data-1 and data data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftCenter(const _TYPE_ *data, Simd_template<_TYPE_>& left, Simd_template<_TYPE_>& center)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data - Vc::Vector<_TYPE_>::Size, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data, Vc::Aligned);

  //first_vector and second_vector are concatenated and then shifted by size_of_vector-1 to the left
  Vc::Vector<_TYPE_> data_minus_1 = first_vector.shifted(Vc::Vector<_TYPE_>::Size - 1, second_vector);

  left =  data_minus_1;
  center =  second_vector;
#else
  left = data[-1];
  center = data[0];
#endif
}

/*! @brief Returns the vector center and right starting at adress data and data+1 data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline void SimdGetCenterRight(const _TYPE_ *data,
                               Simd_template<_TYPE_>& center,
                               Simd_template<_TYPE_>& right)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data+Vc::Vector<_TYPE_>::Size, Vc::Aligned);

  //first_vector and second_vector are concatenated and then shifted by 1 to the left
  Vc::Vector<_TYPE_> data_plus_1 = first_vector.shifted(1, second_vector);
  center = first_vector;
  right = data_plus_1;
#else
  center = data[0];
  right = data[1];
#endif

}

/*! @brief Returns the vectors left, center and right starting at adress data-1, data and data+1 data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftCenterRight(const _TYPE_ *data,
                                   Simd_template<_TYPE_>& left,
                                   Simd_template<_TYPE_>& center,
                                   Simd_template<_TYPE_>& right)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data-Vc::Vector<_TYPE_>::Size, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data, Vc::Aligned);
  Vc::Vector<_TYPE_> third_vector(data+Vc::Vector<_TYPE_>::Size, Vc::Aligned);

  //shifting second_vector by 1 to the left and padding in elements of third_vector
  Vc::Vector<_TYPE_> data_plus_1 = second_vector.shifted(1, third_vector);
  //shifting first_vector by size_of_vector-1 to the left and padding in elements of second_vector
  Vc::Vector<_TYPE_> data_minus_1 = first_vector.shifted(Vc::Vector<_TYPE_>::Size - 1, second_vector);


  left =  data_minus_1;
  center =  second_vector;
  right = data_plus_1;

#else
  left = data[-1];
  center = data[0];
  right = data[1];
#endif

}

/*! @brief Returns the vectors leftleft, left, center and right starting at adress data-2, data-1, data and data+1 data must be aligned for the architecture (see simd_malloc())
 *
 */
template<typename _TYPE_>
inline void SimdGetLeftleftLeftCenterRight(const _TYPE_ *data,
                                           Simd_template<_TYPE_>& leftleft,
                                           Simd_template<_TYPE_>& left,
                                           Simd_template<_TYPE_>& center,
                                           Simd_template<_TYPE_>& right)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> first_vector(data-Vc::Vector<_TYPE_>::Size, Vc::Aligned);
  Vc::Vector<_TYPE_> second_vector(data, Vc::Aligned);
  Vc::Vector<_TYPE_> third_vector(data+Vc::Vector<_TYPE_>::Size, Vc::Aligned);

  //shifting second_vector by 1 to the left and padding in elements of third_vector
  Vc::Vector<_TYPE_> data_plus_1 = second_vector.shifted(1, third_vector);
  //shifting first_vector by size_of_vector-1 to the left and padding in elements of second_vector
  Vc::Vector<_TYPE_> data_minus_1 = first_vector.shifted(Vc::Vector<_TYPE_>::Size - 1, second_vector);
  //shifting first_vector by size_of_vector-2 to the left and padding in elements of second_vector
  Vc::Vector<_TYPE_> data_minus_2 = first_vector.shifted(Vc::Vector<_TYPE_>::Size - 2, second_vector);


  leftleft = data_minus_2;
  left =  data_minus_1;
  center =  second_vector;
  right = data_plus_1;
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
inline Simd_template<_TYPE_> Simd_absolute_value(Simd_template<_TYPE_> a)
{
#ifdef OPTIM_AVX_
  return Vc::abs(a.data_);
#else
  return std::abs(a.data_);
#endif
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
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> compare = x1.data_ - x2.data_; //comparing x1 and x2

  Vc::Vector<_TYPE_> resu;
  //using compare as a maks to determine resu values
  resu(compare < 0) = value_if_x1_lower_than_x2.data_;
  resu(compare >= 0) = value_otherwise.data_;

  return resu;
#else
  return (x1.data_ < x2.data_) ? value_if_x1_lower_than_x2 : value_otherwise;
#endif
}

// Returns a vector built with min(a[i],b[i]) (element wise)
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdMin(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>&   b)
{
#ifdef OPTIM_AVX_
  return Vc::min(a.data_, b.data_);
#else
  return (a.data_ < b.data_) ? a : b;
#endif
}

// Returns a vector built with max(a[i],b[i]) (element wise)
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdMax(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>&   b)
{
#ifdef OPTIM_AVX_
  return Vc::max(a.data_, b.data_);
#else
  return (a.data_ > b.data_) ? a : b;
#endif
}

// Returns approximate result of a/b
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdDivideMed(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>& b)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> x = Vc::reciprocal(b.data_); // x = approximation de 1/b
  Vc::Vector<_TYPE_> y = a.data_ * x; // y = a * x
  // resu = (a.data_ - b.data_ * y) * x + y
  Vc::Vector<_TYPE_> resu = (a.data_ - b.data_ * y) * x + y;
  return resu;
#else
  return a.data_ / b.data_;
#endif

}

// Returns approximate result of 1/b
template<typename _TYPE_>
inline Simd_template<_TYPE_> SimdReciprocalMed(const Simd_template<_TYPE_>& b)
{
#ifdef OPTIM_AVX_
  Vc::Vector<_TYPE_> x = Vc::reciprocal(b.data_); // x = approximation de 1/b
  // resu = (2 - b * x) * x
  Vc::Vector<_TYPE_> two(2.);
  Vc::Vector<_TYPE_> resu = (two - b.data_ * x) * x;
  return resu;
#else
  return 1. / b.data_;
#endif
}


class Simd_int
{
public:
  using value_type = int;

  Simd_int() {};
  // Commodity default constructor (provides implicit conversion)
  Simd_int(int x)
  {
#ifdef OPTIM_AVX_
    //on parcourt chaque vecteur d'entiers qui composent data_, et on les initialise a x
    for(size_t j = 0; j < data_.vectorsCount(); j++)
      {
        Vc::int_v value(x);
        data_.vector(j) = value;
      }
#else
    data_ = x;
#endif
  };

#ifdef OPTIM_AVX_
  Simd_int(Vc::Memory<Vc::int_v,  Vc::float_v::Size> x) : data_(x) {};
#endif

  //on souhaite que la taille d'un vecteur d'entiers soit la meme que celle d'un vecteur de flottants
  static int size()
  {
#ifdef OPTIM_AVX_
    return Vc::float_v::Size;
#else
    return 1;
#endif
  }

  void operator|=(Simd_int a)
  {
#ifdef OPTIM_AVX_
    //on parcourt chaque vecteur d'entiers qui composent data_
    for(size_t j = 0; j < data_.vectorsCount(); j++)
      {
        data_.vector(j) =  data_.vector(j) | a.data_.vector(j);
      }
#else
    data_ |= a.data_;
#endif
  }

#ifdef OPTIM_AVX_
  // dans certains cas, un vecteur de flottants n'a pas la meme taille qu'un vecteur d'entiers
  // on aimerait que ce soit le cas pour pouvoir facilement les comparer
  // on utilise donc la structure de donnees Vc:::Memory qui est compose d'un certain nombre de vecteurs d'entiers
  // mais dont la taille totale est celle d'un vecteur de flottants
  // par exemple, si la taille d'un vecteur de flottants est 8 et la taille d'un vecteur d'entiers est 4
  // data_ sera compose de 2 vecteurs d'entiers de taille 4
  Vc::Memory<Vc::int_v,  Vc::float_v::Size> data_;
#else
  int data_ = 0;
#endif

};

/*! @brief Returns the vector found at address data.
 *
 * data must be aligned for the architecture (see simd_malloc())
 *
 */
inline Simd_int SimdGet(const int *data)
{
#ifdef OPTIM_AVX_
  Vc::Memory<Vc::int_v,  Vc::float_v::Size> res;

  //on parcourt chaque vecteur d'entiers qui composent res
  //et on charge les valeurs de data correspondantes
  for(size_t j = 0; j < res.vectorsCount(); j++)
    {
      Vc::int_v res_vector_i;
      res_vector_i.load(&data[j * Vc::int_v::Size], Vc::Aligned);
      res.vector(j) = res_vector_i;
    }
  return res;
#else
  return *data;
#endif
}

/*! @brief Stores vector x at address data.
 *
 * data must be aligned for the architecture (see simd_malloc())
 *
 */
inline void SimdPut(int *data, Simd_int x)
{
#ifdef OPTIM_AVX_
  //on parcourt chaque vecteur d'entiers qui composent x
  //et on stocke les valeurs qu'elles contiennt dans data
  for(size_t j = 0; j < x.data_.vectorsCount(); j++)
    {
      Vc::int_v x_vector_i = x.data_.vector(j);
      x_vector_i.store(&data[j * Vc::int_v::Size], Vc::Aligned);
    }
#else
  *data = x.data_;
#endif

}

inline Simd_int SimdSelect(Simd_float x1,
                           Simd_float x2,
                           Simd_int value_if_x1_lower_than_x2,
                           Simd_int value_otherwise)
{
#ifdef OPTIM_AVX_
  //on compare x1 et x2
  Vc::Mask<float> mask = (x1.data_ < x2.data_);

  // on veut que le masque ait la meme structure que le membre data_ de Simd_int
  // par exemple si data_ de Simd_int contient 2 vecteurs d'entiers,
  // le masque doit aussi etre compose de 2 vecteurs d'entiers
  Vc::Memory<Vc::Mask<int>, Vc::Mask<float>::Size> mask_int;
  for (size_t i= 0; i < Vc::Mask<float>::Size; ++i) { mask_int[i] = mask[i]; }

  Vc::Memory<Vc::int_v,  Vc::float_v::Size> res;

  //on parcourt chaque vecteur d'entiers qui compose res
  //et on applique la selection sur chacun d'entre eux
  for (size_t i= 0; i < res.vectorsCount(); ++i)
    {

      //on recupere le vecteur i de mask et le vecteur i de res
      Vc::Mask<int> mask_i = mask_int.vector(i);
      Vc::int_v res_i = res.vector(i);

      //on determine la valeur du vecteur i de res en fonction du masque
      res_i(mask_i) = value_if_x1_lower_than_x2.data_.vector(i);
      res_i(!mask_i) = value_otherwise.data_.vector(i);

      //on stocke le resultat dans le vecteur i de res
      res.vector(i) = res_i;

    }

  return res;
#else
  return (x1.data_ < x2.data_) ? value_if_x1_lower_than_x2.data_ : value_otherwise.data_;
#endif
}

inline void SimdCompareAndSetIfLower(const Simd_float x_new, Simd_float x,
                                     const Simd_int i_new, Simd_int i)
{
#ifdef OPTIM_AVX_
  //on compare x1 et x2
  Vc::Mask<float> mask = (x_new.data_ < x.data_);

  //on determine le nouveau x
  x.data_(mask) = x_new.data_;

  Vc::Memory<Vc::Mask<int>, Vc::Mask<float>::Size> mask_int;
  for (size_t j= 0; j < Vc::Mask<float>::Size; ++j) { mask_int[j] = mask[j]; }

  //on parcourt chaque vecteur qui compose i et on applique la selection dessus
  for (size_t j= 0; j < i.data_.vectorsCount(); ++j)
    {

      Vc::Mask<int> mask_j = mask_int.vector(j);
      //on determine la valeur du vecteur i de res en fonction du masque
      Vc::int_v i_at_j = i.data_.vector(j);
      i_at_j(mask_j) = i_new.data_.vector(j);
      i.data_.vector(j) = i_at_j;
    }
#else
  if (x_new.data_ < x.data_)
    {
      x = x_new;
      i = i_new;
    }
#endif
}


#endif


