/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        VectMD_Vector.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////
#ifndef VectMD_Vector_h
#define VectMD_Vector_h
#include <Vect.h>
#define declare_vect_no_obj(_TYPE_)                        \
  class VECT(_TYPE_)                                        \
  {                                                        \
  public:                                                \
    VECT(_TYPE_)();                                        \
    inline int size() const { return sz; };                \
    VECT(_TYPE_)(int);                                \
    const _TYPE_& operator[](int) const;                \
    _TYPE_& operator[](int);                                \
    VECT(_TYPE_)(const VECT(_TYPE_)&);                        \
    virtual ~VECT(_TYPE_)();                                \
    VECT(_TYPE_)& operator=(const VECT(_TYPE_)& );        \
    VECT(_TYPE_)& add(const _TYPE_&);                        \
  protected :                                                \
    int sz;                                                \
    _TYPE_* data;                                        \
  };

#define implemente_vect_no_obj(_TYPE_)                                        \
  VECT(_TYPE_)::VECT(_TYPE_)(const VECT(_TYPE_)& vect)                        \
  {                                                                        \
    sz = 0;                                                                \
    data = 0;                                                                \
    operator=(vect);                                                        \
  }                                                                        \
  VECT(_TYPE_)::VECT(_TYPE_)() : sz(0), data(0){}                        \
  VECT(_TYPE_)::~VECT(_TYPE_)()                                                \
  {                                                                        \
    delete [] data;                                                        \
  }                                                                        \
  VECT(_TYPE_)& VECT(_TYPE_)::operator=(const VECT(_TYPE_)& vect)        \
  {                                                                        \
    if (this == &vect) return *this;                                        \
    delete [] data;                                                        \
    data = 0;                                                                \
    sz = vect.sz;                                                        \
    if (sz) {                                                                \
      data = new _TYPE_[sz];                                                \
      for (int i = 0; i < sz; i++)                                        \
        data[i] = vect[i];                                                \
    }                                                                        \
    return *this;                                                        \
  }                                                                        \
  const _TYPE_& VECT(_TYPE_)::operator[](int i) const                \
  {                                                                        \
    assert( (i>=0) && (i<sz) );                                                \
    return data[i];                                                        \
  }                                                                        \
  _TYPE_& VECT(_TYPE_)::operator[](int i)                                \
  {                                                                        \
    assert( (i>=0) && (i<sz) );                                                \
    return data[i];                                                        \
  }                                                                        \
  VECT(_TYPE_) & VECT(_TYPE_)::add(const _TYPE_ & data_to_add)                \
  {                                                                        \
    _TYPE_ *data2 = new _TYPE_[sz+1];                                        \
    for (int i = 0; i < sz; i++)                                        \
      data2[i] = data[i];                                                \
    data2[sz] = data_to_add;                                                \
    delete [] data;                                                        \
    sz++;                                                                \
    data = data2;                                                        \
    return *this;                                                        \
  }

#include <MD_Vector.h>

declare_vect_no_obj(MD_Vector)

#endif
