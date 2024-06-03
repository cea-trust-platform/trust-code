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

#ifndef SSE_Kernels_included
#define SSE_Kernels_included

#include <IJK_Field.h>

namespace SSE_Kernels
{
constexpr int GENERIC_STRIDE = -1;

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Multipass_Jacobigeneric_template(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& x,
                                      IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& residue,
                                      const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& coeffs,
                                      const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& secmem,
                                      const int npass,
                                      const bool last_pass_is_residue,
                                      const _TYPE_ relax_coefficient);
template <typename _TYPE_, typename _TYPE_ARRAY_>
void reference_kernel_template(const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& tab,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& coeffs,
                               const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& secmem,
                               IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& resu,
                               const _TYPE_ relax, const bool residue);
}

#include <SSE_kernels.tpp>
#endif
