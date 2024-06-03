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

#ifndef Simd_MatrixArray_template_included
#define Simd_MatrixArray_template_included

#include <Simd_template.h>
#include <FixedVector.h>

// Simd_floatMatrixArray<N,Nlines,Ncolumns> stores N matrices of size Nlines * Ncolumns,
// arranged for Simd operation (one simd load or store accesses one particular
// (i,j) element of the matrix for n consecutive matrices).
// N must be a multiple of the simd vector size
// It is a fixed size storage that should be used for efficient processing
// of local data (that fit in the L1 cache)
template<typename _TYPE_,int N, int Nlines, int Ncolumns>
class Simd_MatrixArray_template
{
public:
  _TYPE_& operator()(int matrix_index, int line, int col)
  {
    assert(line >= 0 && line < Nlines && col >= 0 && col < Ncolumns && matrix_index >= 0 && matrix_index < N);
    return data_[line][col][matrix_index];
  }

  const _TYPE_& operator()(int matrix_index, int line, int col) const
  {
    assert(line >= 0 && line < Nlines && col >= 0 && col < Ncolumns && matrix_index >= 0 && matrix_index < N);
    return data_[line][col][matrix_index];
  }

  Simd_template<_TYPE_> SimdGet(int matrix_index, int line, int col) const
  {
    assert(line >= 0 && line < Nlines && col >= 0 && col < Ncolumns && matrix_index >= 0 && matrix_index < N);
    return SimdGet(data_[line][col] + matrix_index);
  }

  void SimdPut(int matrix_index, int line, int col, const Simd_template<_TYPE_>& x)
  {
    assert(line >= 0 && line < Nlines && col >= 0 && col < Ncolumns && matrix_index >= 0 && matrix_index < N);
    SimdPut(data_[line][col] + matrix_index, x);
  }

protected:
  _TYPE_ data_[Nlines][Ncolumns][N];
};

template<int N, int Nlines, int Ncolumns> using Simd_floatMatrixArray = Simd_MatrixArray_template<float,N,Nlines,Ncolumns>;
template<int N, int Nlines, int Ncolumns> using Simd_doubleMatrixArray = Simd_MatrixArray_template<double,N,Nlines,Ncolumns>;

// Computes the inverse of all matrices in the array with simd instructions tab and resu must be properly aligned for simd.
// Determinant of the matrices must not be zero (undefined behavior or crash...)
template<typename _TYPE_,int N>
inline void Simd_Matrix33_inverse_template(const Simd_MatrixArray_template<_TYPE_,N,3,3>& tab, Simd_MatrixArray_template<_TYPE_,N,3,3>& resu)
{
  // 51 x vector size operations (count 4 x vector size operations for division)
  // Runs at 13 Gflops on Nehalem 3Ghz with icc -O
  const int vsize = Simd_template<_TYPE_>::size();
  assert(N%vsize == 0);
  for (int i = 0; i < N; i += vsize)
    {
      const Simd_template<_TYPE_> a00 = tab.SimdGet(i,0,0), a01 = tab.SimdGet(i,0,1), a02 = tab.SimdGet(i,0,2);
      const Simd_template<_TYPE_> a10 = tab.SimdGet(i,1,0), a11 = tab.SimdGet(i,1,1), a12 = tab.SimdGet(i,1,2);
      const Simd_template<_TYPE_> a20 = tab.SimdGet(i,2,0), a21 = tab.SimdGet(i,2,1), a22 = tab.SimdGet(i,2,2);
      // calcul de valeurs temporaires pour optimisation
      const Simd_template<_TYPE_> t4 = a00*a11, t6 = a00*a12, t8 = a01*a10;
      const Simd_template<_TYPE_> t10 = a02*a10, t12 = a01*a20, t14 = a02*a20;
      const Simd_template<_TYPE_> t = t4*a22-t6*a21-t8*a22+t10*a21+t12*a12-t14*a11;
      const Simd_template<_TYPE_> t17 = SimdReciprocalMed(t);

      //calcul de la matrice inverse
      resu.SimdPut(i,0,0, (a11*a22-a12*a21)*t17);
      resu.SimdPut(i,0,1, (a02*a21-a01*a22)*t17);
      resu.SimdPut(i,0,2, (a01*a12-a02*a11)*t17);
      resu.SimdPut(i,1,0, (a12*a20-a10*a22)*t17);
      resu.SimdPut(i,1,1, (a00*a22-t14)*t17);
      resu.SimdPut(i,1,2, (t10-t6)*t17);
      resu.SimdPut(i,2,0, (a10*a21-a11*a20)*t17);
      resu.SimdPut(i,2,1, (t12-a00*a21)*t17);
      resu.SimdPut(i,2,2, (t4-t8)*t17);
    }
}


#endif /* Simd_MatrixArray_template_included */
