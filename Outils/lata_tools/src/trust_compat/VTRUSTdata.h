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

#ifndef VTRUSTdata_included
#define VTRUSTdata_included

#include <Array_base.h>
#include <simd_interface.h>
#include <assert.h>
#include <typeinfo>
#include <stdexcept>

template<typename _TYPE_>
class VTRUSTdata
{
public:
  using value_type = _TYPE_;

  VTRUSTdata(entier size, Array_base::Storage storage);
  ~VTRUSTdata();

  entier add_one_ref() { return ++ref_count_; }

  entier suppr_one_ref()
  {
    assert(ref_count_ > 0);
    return (--ref_count_);
  }

  _TYPE_* get_data() { return data_; }
  const _TYPE_* get_data() const { return data_; }
  inline entier ref_count() const { return ref_count_; }
  inline entier get_size() const { return size_; }

private:
  VTRUSTdata(const VTRUSTdata<_TYPE_>& v) = delete;
  VTRUSTdata& operator=(const VTRUSTdata<_TYPE_>& v) = delete;

  _TYPE_ *data_;
  entier ref_count_;
  entier size_;
  Array_base::Storage storage_;
};

using VDoubledata = VTRUSTdata<double>;
using VFloatdata = VTRUSTdata<float>;
using VIntdata = VTRUSTdata<entier>;

template<typename _TYPE_>
VTRUSTdata<_TYPE_>::VTRUSTdata(entier size, Array_base::Storage storage)
{
  if (size == 0)
    storage = Array_base::STANDARD;

  switch (storage)
    {
    case Array_base::STANDARD:
      {
#ifdef _EXCEPTION_
        try
          {
            data_ = new _TYPE_[size];
          }
        catch(...)
          {
            Cerr << "impossible d'allouer " << size << " " << typeid(_TYPE_).name() << finl;
            throw;
          }
#else
        data_ = new _TYPE_[size];
        if(!data_)
          {
            Cerr << "impossible d'allouer " << size << " " << typeid(_TYPE_).name() << finl;
            throw ;
          }
#endif
        break;
      }
    case Array_base::SIMD_ALIGNED:
      {
#ifdef SIMD_TOOLS_H
        data_ = (_TYPE_*) simd_malloc (sizeof(_TYPE_) * size);
#else
        Cerr<<"unable to allocate simd_aligned, version compiled without simd "<<finl;
        throw;
#endif
        break;
      }
    default:
      throw;
    }
  ref_count_ = 1;
  size_ = size;
  storage_ = storage;
  assert(data_ != 0);
}

template<typename _TYPE_>
VTRUSTdata<_TYPE_>::~VTRUSTdata() noexcept(false)
{
  assert(ref_count_ == 0);
  switch(storage_)
    {
    case Array_base::STANDARD:
      delete[] data_;
      break;
    case Array_base::SIMD_ALIGNED:
#ifdef SIMD_TOOLS_H
      simd_free(data_);
#else
      Cerr<<"unable to allocate simd_aligned, version compiled without simd "<<finl;
      throw;
#endif
      break;
    default:
      throw;
    }

  data_ = 0;  // paranoia: si size_==-1 c'est qu'on pointe sur un zombie
  size_ = -1; //  (pointeur vers un objet qui a ete detruit)
  storage_ = Array_base::STANDARD;
}

#endif /* VTRUSTdata_included */
