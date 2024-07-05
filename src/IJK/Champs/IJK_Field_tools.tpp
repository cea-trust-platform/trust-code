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

#ifndef IJK_Field_tools_TPP_H
#define IJK_Field_tools_TPP_H

#include <stat_counters.h>
#include <Statistiques.h>

template<typename _TYPE_, typename _TYPE_ARRAY_>
double norme_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  double somme = 0.;
  for (int k = 0; k < nk; k++)
    {
      double partial1 = 0.;
      for (int j = 0; j < nj; j++)
        {
          double partial2 = 0.;
          for (int i = 0; i < ni; i++)
            {
              double x = residu(i, j, k);
              partial2 += x * x;
            }
          partial1 += partial2;
        }
      somme += partial1;
    }
  somme = Process::mp_sum(somme);
  return sqrt(somme);
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ prod_scal_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& y)
{
  const int ni = x.ni();
  const int nj = x.nj();
  const int nk = x.nk();
  _TYPE_ somme = 0.;
  const _TYPE_ARRAY_& xd = x.data();
  const _TYPE_ARRAY_& yd = y.data();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              int index = x.linear_index(i,j,k);
              assert(index == y.linear_index(i,j,k));
              somme += xd[index] * yd[index];
            }
        }
    }
  somme = (_TYPE_)Process::mp_sum(somme); //!!!! WARNING: possible conversion to double to float!!!!!!!!
  return somme;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
double somme_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  double somme = 0.;
  for (int k = 0; k < nk; k++)
    {
      double partial1 = 0.;
      for (int j = 0; j < nj; j++)
        {
          double partial2 = 0.;
          for (int i = 0; i < ni; i++)
            {
              double x = residu(i, j, k);
              partial2 += x;
            }
          partial1 += partial2;
        }
      somme += partial1;
    }
  somme = Process::mp_sum(somme);
  return somme;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ max_ijk(const IJK_Field_template<_TYPE_, _TYPE_ARRAY_>& residu)
{
  const int ni = residu.ni();
  const int nj = residu.nj();
  const int nk = residu.nk();
  _TYPE_ res = (_TYPE_)-1.e30;
  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        {
          _TYPE_ x = residu(i,j,k);
          res = std::max(x,res);
        }
  res = (_TYPE_)Process::mp_max(res);
  return res;
}

#endif /* IJK_Field_tools_TPP_H */
