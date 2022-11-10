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

#ifndef Simd_VectorArray_template_included
#define Simd_VectorArray_template_included

#include <Simd_template.h>
#include <FixedVector.h>

// Simd_VectorArray_template stores N vectors of size VectorSize arranged for Simd operation (one simd load or store accesses one
// particular component of the vector for n consecutive vectors).
// It is a fixed size storage that should be used for efficient processing of local data (that fit in the L1 cache)
template<typename _TYPE_, int N, int VectorSize>
class Simd_VectorArray_template
{
public:
  _TYPE_& operator()(int vector_index, int component)
  {
    assert(vector_index >= 0 && vector_index < N && component >= 0 && component < VectorSize);
    return data_[component][vector_index];
  }

  const _TYPE_& operator()(int vector_index, int component) const
  {
    assert(vector_index >= 0 && vector_index < N && component >= 0 && component < VectorSize);
    return data_[component][vector_index];
  }

  Simd_template<_TYPE_> SimdGet(int vector_index, int component) const
  {
    assert(vector_index >= 0 && vector_index < N && component >= 0 && component < VectorSize);
    return SimdGet(data_[component] + vector_index);
  }

  void SimdPut(int vector_index, int component, const Simd_template<_TYPE_>& x)
  {
    assert(vector_index >= 0 && vector_index < N && component >= 0 && component < VectorSize);
    SimdPut(data_[component] + vector_index, x);
  }

  FixedVector<Simd_template<_TYPE_>, VectorSize> SimdGetVector(int i)
  {
    assert(i >= 0 && i < N);
    FixedVector<Simd_template<_TYPE_>, VectorSize> v;
    for (int j = 0; j < VectorSize; j++) v[j] = ::SimdGet(data_[j] + i);
    return v;
  }

protected:
  // Fortran type storage to allow vectorized operation on n consecutive vectors:
  _TYPE_ data_[VectorSize][N];
};

template <int N, int VectorSize> using Simd_intVectorArray = Simd_VectorArray_template<int,N,VectorSize>;
template <int N, int VectorSize> using Simd_floatVectorArray = Simd_VectorArray_template<float,N,VectorSize>;
template <int N, int VectorSize> using Simd_doubleVectorArray = Simd_VectorArray_template<double,N,VectorSize>;

#endif /* Simd_VectorArray_template_included */
