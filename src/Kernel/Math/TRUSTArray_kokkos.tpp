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

/*! Access the correct dual view member, according to SHAPE
 */
// get_dual_view for _SHAPE_ == 1
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
typename std::enable_if<_SHAPE_ == 1, DualView<_TYPE_, 1>>::type&
                                                        TRUSTArray<_TYPE_, _SIZE_>::get_dual_view() const
{
  return dual_view_1_;
}

// get_dual_view for _SHAPE_ == 2
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
typename std::enable_if<_SHAPE_ == 2, DualView<_TYPE_, 2>>::type&
                                                        TRUSTArray<_TYPE_, _SIZE_>::get_dual_view() const
{
  return dual_view_2_;
}

// get_dual_view for _SHAPE_ == 3
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
typename std::enable_if<_SHAPE_ == 3, DualView<_TYPE_, 3>>::type&
                                                        TRUSTArray<_TYPE_, _SIZE_>::get_dual_view() const
{
  return dual_view_3_;
}

// get_dual_view for _SHAPE_ == 4
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
typename std::enable_if<_SHAPE_ == 4, DualView<_TYPE_, 4>>::type&
                                                        TRUSTArray<_TYPE_, _SIZE_>::get_dual_view() const
{
  return dual_view_4_;
}

// Create internal DualView member, and populate it with current host data
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
inline void TRUSTArray<_TYPE_,_SIZE_>::init_view() const
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  const auto& dual_view = get_dual_view<_SHAPE_>();

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(dual_view.h_view.is_allocated() &&
      dual_view.h_view.data() == this->data() &&
      dual_view.view_device().data() == addrOnDevice(*this) &&
      (long) dual_view.extent(0) == this->dimension_tot(0) &&
      (_SHAPE_ >= 2 && (long) dual_view.extent(1) == this->dimension_tot(1)) &&
      (_SHAPE_ >= 3 && (long) dual_view.extent(2) == this->dimension_tot(2)) &&
      (_SHAPE_ >= 4 && (long) dual_view.extent(3) == this->dimension_tot(3))    )
    return;

  // Re-use data already allocated on host to create host-view:
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!  WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //          This heavily relies on the LayoutRight defined for the DualView (which is not optimal
  //          for GPU processing, but avoids having to explicitely copying the data ...)

  using t_host = typename DualView<_TYPE_,_SHAPE_>::t_host;  // Host type
  using t_dev = typename DualView<_TYPE_,_SHAPE_>::t_dev;    // Device type

  t_host host_view = t_host(const_cast<_TYPE_ *>(this->data()), this->dimension_tot(0),
                            1 < _SHAPE_ ? this->dimension_tot(1) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                            2 < _SHAPE_ ? this->dimension_tot(2) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                            3 < _SHAPE_ ? this->dimension_tot(3) : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
  t_dev device_view;
#ifdef _OPENMP_TARGET
  // Device memory is allocated with OpenMP: ToDo replace by allocate ?
  mapToDevice(*this, "Kokkos init_view()");
  device_view = t_dev(const_cast<_TYPE_ *>(addrOnDevice(*this)), this->dimension_tot(0),
                      1 < _SHAPE_ ? this->dimension_tot(1) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                      2 < _SHAPE_ ? this->dimension_tot(2) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                      3 < _SHAPE_ ? this->dimension_tot(3) : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
#else
  device_view = create_mirror_view_and_copy(Kokkos::DefaultExecutionSpace::memory_space(), host_view);
#endif

  // Create a mutable pointer to the view so we can assemble & mark it
  auto& mutable_dual_view = const_cast<DualView<_TYPE_, _SHAPE_>&>(dual_view);

// Dual view is made as an assembly of the two views:
  mutable_dual_view = DualView<_TYPE_, _SHAPE_>(device_view, host_view);

// Mark data modified on host so it will be sync-ed to device later on:
  mutable_dual_view.template modify<host_mirror_space>();

}

///////////// Read-Only ////////////////////////////
//// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape
  // Init if necessary
  this->template init_view<_SHAPE_>();
  auto& view = get_dual_view<_SHAPE_>();
#ifdef _OPENMP_TARGET
  mapToDevice(*this, "Kokkos TRUSTTab::view_ro()");
#else
  // Copy to device if needed (i.e. if modify() was called):
  //Create a mutable pointer to the view so we can sync it
  auto& mutable_view = const_cast<DualView<_TYPE_, _SHAPE_>&>(view);
  mutable_view.template sync<memory_space>();
#endif
  // return *device* view:
  return view.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  return ConstHostView<_TYPE_,_SHAPE_>(this->addr(), this->dimension_tot(0),
                                       1 < _SHAPE_ ? this->dimension_tot(1) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                       2 < _SHAPE_ ? this->dimension_tot(2) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                       3 < _SHAPE_ ? this->dimension_tot(3) : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

//////////// Write-only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  // Init if necessary
  this->template init_view<_SHAPE_>();
  auto& view = get_dual_view<_SHAPE_>();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTArray<_TYPE_,_SIZE_>::view_wo()"); // ToDo allouer sans copie ?
#else
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  view.template modify<memory_space>();
#endif
  // return *device* view:
  return view.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  return HostView<_TYPE_,_SHAPE_>(this->addr(), this->dimension_tot(0),
                                  1 < _SHAPE_ ? this->dimension_tot(1) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  2 < _SHAPE_ ? this->dimension_tot(2) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  3 < _SHAPE_ ? this->dimension_tot(3) : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

//////////// Read-Write ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  // Init if necessary
  this->template init_view<_SHAPE_>();
  auto& view = get_dual_view<_SHAPE_>();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos view_rw()");
#else
  // Copy to device (if needed) ...
  view.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  view.template modify<memory_space>();
#endif
  // return *device* view:
  return view.view_device();
}
// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  assert(nb_dim_==_SHAPE_); //The accessors should never be called with the wrong shape

  return HostView<_TYPE_,_SHAPE_>(this->addr(), this->dimension_tot(0),
                                  1 < _SHAPE_ ? this->dimension_tot(1) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  2 < _SHAPE_ ? this->dimension_tot(2) : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  3 < _SHAPE_ ? this->dimension_tot(3) : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

// Methode de debug
template<typename _TYPE_, typename _SIZE_, int _SHAPE_>
void debug_device_view(const View<_TYPE_,_SHAPE_> view_tab, TRUSTArray<_TYPE_,_SIZE_>& tab, _SIZE_ max_size=-1)
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

