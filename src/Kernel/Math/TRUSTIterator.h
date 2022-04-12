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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTIterator.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTIterator_included
#define TRUSTIterator_included

#include<iostream>
#include<arch.h>

template<typename _TYPE_ARRAY_>
class TRUSTIterator
{
public:
  using value_type = typename _TYPE_ARRAY_::value_type;
  using pointer_type = value_type*;
  using reference_type = value_type& ;

  TRUSTIterator(pointer_type ptr) : ptr_(ptr) { }

  TRUSTIterator& operator++()
  {
    ptr_++;
    return *this;
  }

  TRUSTIterator operator++(True_int i)
  {
    TRUSTIterator it = *this; // copie !! faut pas modifier ...
    ++(*this);
    return it;
  }

  TRUSTIterator& operator--()
  {
    ptr_--;
    return *this;
  }

  TRUSTIterator operator--(True_int i)
  {
    TRUSTIterator it = *this; //copie !! faut pas modifier ...
    --(*this);
    return it;
  }

  reference_type operator[](True_int ind) { return *(ptr_ + ind); }
  reference_type operator*() { return *ptr_; }
  pointer_type operator->() { return ptr_; }

  bool operator==(const TRUSTIterator& it) const { return ptr_ == it.ptr_; }
  bool operator!=(const TRUSTIterator& it) const { return !(*this == it); }

private:
  pointer_type ptr_;
};

#endif /* TRUSTIterator_included */
