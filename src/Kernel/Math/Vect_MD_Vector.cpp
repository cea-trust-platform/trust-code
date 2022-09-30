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

#include <Vect_MD_Vector.h>

Vect_MD_Vector::Vect_MD_Vector(const Vect_MD_Vector& vect)
{
  sz = 0;
  data = 0;
  operator=(vect);
}

Vect_MD_Vector::Vect_MD_Vector() : sz(0), data(0) { }

Vect_MD_Vector::~Vect_MD_Vector() { delete[] data; }

const MD_Vector& Vect_MD_Vector::operator[](int i) const
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}

MD_Vector& Vect_MD_Vector::operator[](int i)
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}

Vect_MD_Vector& Vect_MD_Vector::operator=(const Vect_MD_Vector& vect)
{
  if (this == &vect) return *this;

  delete[] data;
  data = 0;
  sz = vect.sz;
  if (sz)
    {
      data = new MD_Vector[sz];
      for (int i = 0; i < sz; i++) data[i] = vect[i];
    }
  return *this;
}

Vect_MD_Vector& Vect_MD_Vector::add(const MD_Vector& data_to_add)
{
  MD_Vector *data2 = new MD_Vector[sz + 1];

  for (int i = 0; i < sz; i++) data2[i] = data[i];

  data2[sz] = data_to_add;
  delete[] data;
  sz++;
  data = data2;
  return *this;
}
