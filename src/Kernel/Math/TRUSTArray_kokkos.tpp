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

#ifndef TRUSTArr_kokkos_TPP_included
#define TRUSTArr_kokkos_TPP_included

#include <TRUSTArray.h>
#ifdef KOKKOS
// Create internal DualView member, and populate it with current host data
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTArray<_TYPE_,_SIZE_>::init_view_arr() const
{
  long ze_dim = this->size_array();

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(dual_view_arr_.h_view.is_allocated() &&
      dual_view_arr_.h_view.data() == this->data() &&
      dual_view_arr_.view_device().data() == addrOnDevice(*this) &&
      (long) dual_view_arr_.extent(0) == ze_dim)
    return ;

  using t_host = typename DualViewArr<_TYPE_>::t_host;  // Host type
  using t_dev = typename DualViewArr<_TYPE_>::t_dev;    // Device type
  //using size_type = typename DualViewArr<_TYPE_>::size_type;

  //const std::string& nom = this->le_nom().getString();

  // Re-use already TRUST allocated data:
  t_host host_view = t_host(const_cast<_TYPE_ *>(this->data()), ze_dim);
  // Empty view on device - just a memory allocation:
  //t_dev device_view = t_dev(nom, ze_dim);
  t_dev device_view;
#ifdef _OPENMP_TARGET
  // Device memory is allocated with OpenMP: ToDo replace by allocate ?
  mapToDevice(*this, "Kokkos init_view_arr()");
  device_view = t_dev(const_cast<_TYPE_ *>(addrOnDevice(*this)), ze_dim);
#else
  device_view = create_mirror_view_and_copy(Kokkos::DefaultExecutionSpace::memory_space(), host_view);
#endif

  // Dual view is made as an assembly of the two views:
  dual_view_arr_ = DualViewArr<_TYPE_>(device_view, host_view);

  // Mark data modified on host so it will be sync-ed to device later on:
  dual_view_arr_.template modify<host_mirror_space>();
}


//////////// Read-Only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  // Init if necessary
  init_view_arr();
#ifdef _OPENMP_TARGET
  mapToDevice(*this, "Kokkos TRUSTArray::view_ro()");
#else
  // Copy to device if needed (i.e. if modify() was called):
  dual_view_arr_.template sync<memory_space>();
#endif
  // return *device* view:
  return dual_view_arr_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  return ConstHostViewArr<_TYPE_>(addr(), size_array());
}

//////////// Write-only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  // Init if necessary
  init_view_arr();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTArray<_TYPE_,_SIZE_>::view_wo()"); // ToDo allouer sans copie ?
#else
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_arr_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_arr_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  return HostViewArr<_TYPE_>(addr(), size_array());
}

//////////// Read-Write ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  // Init if necessary
  init_view_arr();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTArray::view_rw()");
#else
  // Copy to device (if needed) ...
  dual_view_arr_.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_arr_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_arr_.view_device();
}

template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewArr<_TYPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  return HostViewArr<_TYPE_>(addr(), size_array());
}


/////////////////////////

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTArray<_TYPE_,_SIZE_>::sync_to_host() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Copy to host (if needed) ...
  dual_view_arr_.template sync<host_mirror_space>();
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTArray<_TYPE_,_SIZE_>::modified_on_host() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Mark modified on host side:
  if(dual_view_arr_.h_view.is_allocated())
    dual_view_arr_.template modify<host_mirror_space>();
}

// Methode de debug
template<typename _TYPE_, typename _SIZE_>
void debug_device_view(const ViewArr<_TYPE_> view_tab, TRUSTArray<_TYPE_,_SIZE_>& tab, _SIZE_ max_size=-1)
{
  assert(view_tab.data()==addrOnDevice(tab)); // Verifie meme adress
  Cout << "View size=" << view_tab.size() << finl;
  _SIZE_ size = max_size;
  if (size==-1) size = view_tab.extent(0);
  Kokkos::parallel_for(size, KOKKOS_LAMBDA(const _SIZE_ i)
  {
    printf("[Kokkos]: %p [%2ld]=%e\n", view_tab.data(), i, view_tab(i));
  });
  Cout << "Tab size=" << tab.size_array() << finl;
  assert(view_tab.size()==tab.size_array());
//  _TYPE_ *ptr = tab.data();
//  #pragma omp target teams distribute parallel for
//  for (_SIZE_ i=0; i<size; i++)
//    printf("[OpenMP]: %p [%2ld]=%e\n", ptr, i, ptr[i]);
}
#endif
#endif

