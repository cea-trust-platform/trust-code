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

#ifndef simd_tools_included
#define simd_tools_included

#include <Simd_Array_template.h>
#include <Simd_VectorArray_template.h>
#include <Simd_MatrixArray_template.h>

template<typename _TYPE_>
inline Simd_template<_TYPE_> max(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>& b)
{
  return SimdMax(a, b);
}

template<typename _TYPE_>
inline Simd_template<_TYPE_> min(const Simd_template<_TYPE_>& a, const Simd_template<_TYPE_>& b)
{
  return SimdMin(a, b);
}

inline Simd_float select_float(Simd_float x1, Simd_float x2, Simd_float value_if_x1_lower_than_x2, Simd_float value_otherwise)
{
  return SimdSelect(x1, x2, value_if_x1_lower_than_x2, value_otherwise);
}
inline float select_float(float x1, float x2, float value_if_x1_lower_than_x2, float value_otherwise)
{
  return (x1 < x2) ? value_if_x1_lower_than_x2 : value_otherwise;
}

inline Simd_double select_double(Simd_double x1, Simd_double x2, Simd_double value_if_x1_lower_than_x2, Simd_double value_otherwise)
{
  return SimdSelect(x1, x2, value_if_x1_lower_than_x2, value_otherwise);
}
inline double select_double(double x1, double x2, double value_if_x1_lower_than_x2, double value_otherwise)
{
  return (x1 < x2) ? value_if_x1_lower_than_x2 : value_otherwise;
}

#endif /* simd_tools_included */
