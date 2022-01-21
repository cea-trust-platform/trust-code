/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Trier.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Trier.h>
#include <DoubleVect.h>

void QuickSort(const DoubleVect& a,int g, int d, ArrOfInt& c)
{
  int t,i,j;
  double v;
  if (g < d)
    {
      if(a[c[g]]>a[c[d]])
        {
          t=c[g];
          c[g]=c[d];
          c[d]=t;
        }
      v = a[c[d]];
      i = g-1;
      j = d;
      do
        {
          do i++;
          while (a[c[i]] < v);
          do j--;
          while (a[c[j]] > v);
          t = c[i];
          c[i] = c[j];
          c[j] = t;
        }
      while (j > i);
      c[j] = c[i];
      c[i] = c[d];
      c[d] = t;
      QuickSort (a, g, i-1,c);
      QuickSort (a, i+1, d,c);
    }
}

void trier_abs(const DoubleVect& a, ArrOfInt& c)
{
  DoubleVect a2(a.size_array());
  c.resize_array(a.size_array());
  int i;

  for (i=0; i<a2.size_array(); i++) a2[i]=std::fabs(a[i]);
  for ( i=0; i<c.size_array(); i++) c[i]=i;

  QuickSort(a2, 0, a2.size_array()-1, c);
}

void trier(const DoubleVect& a, ArrOfInt& c)
{
  c.resize_array(a.size_array());
  int i;
  for (i=0; i<c.size_array(); i++) c[i]=i;
  QuickSort(a, 0, a.size_array()-1, c);
}

void essai()
{
  DoubleVect a(5);
  ArrOfInt c(5);
  int i;
  a[0]=-10.1;
  a[1]=8.3;
  a[2]=7.31;
  a[3]=7.3;
  a[4]=7.1;
  Cerr << "a before selecting "  << finl;
  for (i=0; i<5; i++) Cerr << a[i] << " ";
  Cerr << finl;
  trier (a,c);
  Cerr << "a after selecting " << finl;
  for ( i=0; i<5; i++) Cerr << a[c[i]] << " ";
  Cerr << finl;
  trier_abs (a,c);
  Cerr << "a after selecting abs " << finl;
  for ( i=0; i<5; i++) Cerr << a[c[i]] << " ";
  Cerr << finl;
}


