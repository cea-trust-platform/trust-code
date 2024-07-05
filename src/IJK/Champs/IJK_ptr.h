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

#ifndef IJK_ptr_included
#define IJK_ptr_included

#include <ConstIJK_ptr.h>

// We can automaticaly cast an IJK_ptr to a constIJK_ptr but not reversed.
template <typename _TYPE_, typename _TYPE_ARRAY_ >
class IJK_ptr : public ConstIJK_ptr<_TYPE_, _TYPE_ARRAY_>
{
public:
  IJK_ptr(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& field, int i, int j, int k): ConstIJK_ptr<_TYPE_, _TYPE_ARRAY_>(field, i, j, k)
  {
  }
  /*! @brief Performs the assignment: field(i+i_offset,j,k) = val
   *
   */
  void put_val(int i_offset, const _TYPE_ & val)
  {
    assert(this->i_ + i_offset >= this->i_min_ && this->i_ + i_offset < this->i_max_);
    // cast en non const ok car on avait un IJK_Field non const au depart
    const _TYPE_ *ptr = this->ptr_;
    ((_TYPE_*)ptr)[i_offset] = val;
  }
  void put_val(int i_offset, const Simd_template<_TYPE_>& val)
  {
    assert(this->i_ + i_offset >= this->i_min_ && this->i_ + i_offset < this->i_max_);
    const _TYPE_ *ptr = this->ptr_;
    SimdPut((_TYPE_*)ptr + i_offset, val);
  }
};

using IJK_float_ptr = IJK_ptr<float, ArrOfFloat>;
using IJK_double_ptr = IJK_ptr<double, ArrOfDouble>;


#endif
