/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef TRUSTArr_kokkos_TPP_included
#define TRUSTArr_kokkos_TPP_included

// TODO: this file should ultimately be moved / merged with TRUSTArr.tpp?

#ifdef KOKKOS_
#include <View_Types.h>

// Create internal DualView member, and populate it with current host data
template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::init_view_arr() const
{
  long ze_dim = this->size_array();

  // Do we need to re-init?
  bool is_init = dual_view_init_;
  if(is_init && dual_view_arr_.h_view.is_allocated())
    // change of alloc or resize triggers re-init (for now - resize could be done better)
    if (dual_view_arr_.h_view.data() != this->addr() || (long)dual_view_arr_.extent(0) != ze_dim)
      is_init = false;

  if (is_init) return;
  dual_view_init_ = true;

  using t_host = typename DualViewArr<_TYPE_>::t_host;  // Host type
  using t_dev = typename DualViewArr<_TYPE_>::t_dev;    // Device type
  using size_type = typename DualViewArr<_TYPE_>::size_type;

  const std::string& nom = this->le_nom().getString();

  // Re-use already TRUST allocated data:
  t_host host_view = t_host((_TYPE_ *)this->addr(), ze_dim);
  // Empty view on device - just a memory allocation:
  t_dev device_view = t_dev(nom, ze_dim);

  // Dual view is made as an assembly of the two views:
  dual_view_arr_ = DualViewArr<_TYPE_>(device_view, host_view);

  // Mark data modified on host so it will be sync-ed to device later on:
  dual_view_arr_.template modify<host_mirror_space>();
}

template<typename _TYPE_>
inline ConstViewArr<_TYPE_> TRUSTArray<_TYPE_>::view_ro() const
{
  // Init if necessary
  init_view_arr();
  // Copy to device if needed (i.e. if modify() was called):
  dual_view_arr_.template sync<memory_space>();
  // return *device* view:
  return dual_view_arr_.template view<memory_space>();
}

template<typename _TYPE_>
inline ViewArr<_TYPE_> TRUSTArray<_TYPE_>::view_wo()
{
  // Init if necessary
  init_view_arr();
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_arr_.template modify<memory_space>();
  // return *device* view:
  return dual_view_arr_.template view<memory_space>();
}

template<typename _TYPE_>
inline ViewArr<_TYPE_> TRUSTArray<_TYPE_>::view_rw()
{
  // Init if necessary
  init_view_arr();
  // Copy to device (if needed) ...
  dual_view_arr_.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_arr_.template modify<memory_space>();
  // return *device* view:
  return dual_view_arr_.template view<memory_space>();
}

template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::sync_to_host() const
{
  // Copy to host (if needed) ...
  dual_view_arr_.template sync<host_mirror_space>();
}

template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::modified_on_host() const
{
  // Mark modified on host side:
  if(dual_view_init_)
    dual_view_arr_.template modify<host_mirror_space>();
}

#endif // KOKKOS_

#endif
