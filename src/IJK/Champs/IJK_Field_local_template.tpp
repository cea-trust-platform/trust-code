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

#ifndef IJK_Field_local_template_TPP_H
#define IJK_Field_local_template_TPP_H

#include <Simd_template.h>

// External storage: false=>normal behaviour, true=>the field will not allocate memory in data_, the user must
//  provide the storage via set_external_storage() method (used to map memory from elsewhere).
//  Example: field.allocate(... , true); int size=field.get_allocated_size(); Array T(size); field.set_external_storage(T);
// IMPORTANT: when "external_storage==false", an appropriate offset will be set in order to align the data on cache lines,
//  (this offset varies from field to field even if the field shape (ni,nj,nk,compo,ghost) is exactly the same
//  plus extra padding will be added to align all i=0,j,k on SIMD aligned blocks...
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::allocate(int Ni, int Nj, int Nk, int ghosts, int additional_k_layers, int nb_compos, bool external_storage)
{
  // WARNING !!!
  // for the sympy_to_trust system, mem alignment must be disabled otherwise we have serious trouble
  // with fields of same "kind" that have not the same storage pattern (offset, stride, etc...), and
  // we heavily rely on "same pattern" to optimize fetch operations in the fields...
  bool disable_memalign = true;
  if (ghosts > Ni || ghosts > Nj || ghosts > Nk)
    {
      Cerr << "Error in IJK_Field_local_template::allocate: ghostsize=" << ghosts << " is larger than the local mesh size " << Ni << " " << Nj << " " << Nk << finl;
      Process::exit();
    }
  ni_ = Ni;
  nj_ = Nj;
  nk_ = Nk;
  ghost_size_ = ghosts;
  nb_compo_ = nb_compos;
  j_stride_ = ni_ + 2 * ghosts;
  if (!external_storage || disable_memalign)
    // Align beginning of line on SIMD type...
    while (j_stride_ % Simd_template<_TYPE_>::size() != 0)
      j_stride_++;
  compo_stride_ = j_stride_ * (nj_ + 2 * ghosts);
  offset_ = ghost_size_ * (1 + j_stride_ + compo_stride_ * nb_compo_);
  k_layer_shift_ = 0;
  additional_k_layers_ = additional_k_layers;
  int sz = (nk_ + 2 * ghosts + additional_k_layers) * compo_stride_ * nb_compo_ + 1;
  allocated_size_ = sz;

  if (!external_storage || disable_memalign)
    {
      // Ensure that there is some padding data at beginning:
      offset_ += 8;

      // Align origin on cache line boundary
      const int CacheLineSizeBytes = 64;
      sz += (int)(CacheLineSizeBytes/sizeof(_TYPE_));
      // Add supplemental data at end for the last SIMD instruction of the loop that might go
      // beyond the end of the usefull data
      sz += (int)(CacheLineSizeBytes/sizeof(_TYPE_));

      allocated_size_ = sz;
      _TYPE_ *ptr = data_.addr() + offset_;
      char *cptr = (char *) ptr;
      long long iptr = (long long) cptr;
      long long decal = iptr & (CacheLineSizeBytes - 1);
      offset_ += (int)((CacheLineSizeBytes - decal) / sizeof(_TYPE_));
    }
  if (!external_storage)
    data_.resize_array(sz);

}

// Adds n * compo_stride_ * nb_compo_ to the offset (shifts the data by n layers in the k direction without moving memory)
// Used by the jacobi "in place" algorithm: the result replaces the source data but is shifted in k.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::shift_k_origin(int n)
{
  k_layer_shift_ += n;
  // Check that the data still fits into the allocated memory block
  assert(k_layer_shift_ >= 0 && k_layer_shift_ <= additional_k_layers_);

  offset_ += compo_stride_ * nb_compo_ * n;
}

// Creates a field with nk=1, that points to the data than src(...,...,k_layer) (uses ref_array() to create the reference).
// ghostsize is identical to source array.
template<>
inline void IJK_Field_local_template<float,ArrOfFloat>::ref_ij(IJK_Field_local_float& src, int k_lay)
{
  Cerr << "Error: must implement ArrOfFloat::ref_array() and reset()" << endl;
  Process::exit();
}

template<>
inline void IJK_Field_local_template<double, ArrOfDouble>::ref_ij(IJK_Field_local_double& src, int k_lay)
{
  data_.reset();
  ni_ = src.ni_;
  nj_ = src.nj_;
  nk_ = 1;
  ghost_size_ = src.ghost_size_;
  j_stride_ = src.j_stride_;
  nb_compo_ = 1;
  compo_stride_ = 0;
  k_layer_shift_ = 0;
  additional_k_layers_ = 0;

  const int i_first = src.linear_index(-ghost_size_, -ghost_size_, k_lay);
  const int i_last = src.linear_index(ni_ + ghost_size_ - 1, nj_ + ghost_size_ - 1, k_lay);
  offset_ = src.linear_index(0, 0, k_lay) - i_first;
  data_.ref_array(src.data_, i_first, i_last - i_first + 1);
}



#endif /* IJK_Field_local_template_TPP_H */
