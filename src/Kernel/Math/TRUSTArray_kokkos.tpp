/****************************************************************************
* Copyright (c) 2025, CEA
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

// Create internal DeviceView member
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
inline void TRUSTArray<_TYPE_,_SIZE_>::init_device_view() const
{
  bool flattened = check_flattened<_SHAPE_>(); //The accessors should never be called with the wrong _SHAPE_
  _SIZE_ dimension_tot_0 = flattened ? this->size_array() : this->dimension_tot(0);

  const auto& device_view = get_device_view<_SHAPE_>();

  //Useful when casting a 1D Tab into a multi-D View !
  _SIZE_ dims[4] = {dimension_tot_0, nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0, nb_dim_>3 ? this->dimension_tot(3) : 0};

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(device_view.data() == addrOnDevice(*this) &&
      (long) device_view.extent(0) == dims[0] &&
      (_SHAPE_ >= 2 && (long) device_view.extent(1) == dims[1]) &&
      (_SHAPE_ >= 3 && (long) device_view.extent(2) == dims[2]) &&
      (_SHAPE_ >= 4 && (long) device_view.extent(3) == dims[3]) )
    return;

  mapToDevice(*this); // Device memory is allocated
  auto& mutable_device_view = const_cast<DeviceView<_TYPE_, _SHAPE_>&>(device_view);
  mutable_device_view = DeviceView<_TYPE_,_SHAPE_>(const_cast<_TYPE_ *>(addrOnDevice(*this)), dims[0],
                                                   1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                                   2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                                   3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

//Check if the internal value of nb_dim_ (that can be >1 if the Array is a Tab) is compatible with the _SHAPE_
//argument of the accessors. Morevover, it returns true if you are trying to flatten a Tab into an array, or false otherwise
template<typename _TYPE_, typename _SIZE_>
template<int _SHAPE_>
bool TRUSTArray<_TYPE_,_SIZE_>::check_flattened() const
{
  //Trying to represent a TRUSTArray with a multi-D View
#ifndef NDEBUG
  bool is_array = std::string(typeid(*this).name()).find("TRUSTArray") != std::string::npos;;
  assert(not(is_array && _SHAPE_>1));
#endif

  //Mismatch in multi-D Tab dimension and accessor _SHAPE_ value !
  assert((not(this->nb_dim_>1 && _SHAPE_>1 && _SHAPE_ != this->nb_dim_)));

  // The Tab accessor can sometime be called with _SHAPE_ == 1.
  // For instance, this can happen when a vect operation is called on a Tab
  // In this case, we want the View to be flattened with the first dimension as the total size of the tab
  // Otherwise, this would give a 1D View of dimension equal to the first dimension of the Tab (typically <)
  // When true is returned, we do a 1D view with dimension size_array(). This is always what we want with _SHAPE_=1
  return (_SHAPE_==1);
}

///////////// Read-Only ////////////////////////////
// Device version (GPU / CPU compiled)
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  this->template init_device_view<_SHAPE_>();
  mapToDevice(*this);
  return get_device_view<_SHAPE_>();
}

// GPU compiled, host view version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<gpu_enabled_is_host_exec_space<EXEC_SPACE>, ConstHostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_ro() const
{
  bool flattened = check_flattened<_SHAPE_>(); //The accessors should never be called with the wrong _SHAPE_

  _SIZE_ dimension_tot_0 = flattened ? this->size_array() : this->dimension_tot(0);

  //Useful when casting a 1D Tab into a multi-D View !
  _SIZE_ dims[4] = {dimension_tot_0, nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0, nb_dim_>3 ? this->dimension_tot(3) : 0};

  return ConstHostView<_TYPE_,_SHAPE_>(this->addr(), dims[0],
                                       1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                       2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                       3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}


//////////// Write-only ////////////////////////////
// Device version (GPU / CPU compiled)
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  this->template init_device_view<_SHAPE_>();
  computeOnTheDevice(*this);
  return get_device_view<_SHAPE_>();
}

// GPU compiled, host view version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<gpu_enabled_is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_wo()
{
  bool flattened = check_flattened<_SHAPE_>(); //The accessors should never be called with the wrong _SHAPE_

  _SIZE_ dimension_tot_0 = flattened ? this->size_array() : this->dimension_tot(0);

  //Useful when casting a 1D Tab into a multi-D View !
  _SIZE_ dims[4] = {dimension_tot_0, nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0, nb_dim_>3 ? this->dimension_tot(3) : 0};

  return HostView<_TYPE_,_SHAPE_>(this->addr(), dims[0],
                                  1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

//////////// Read-Write ////////////////////////////
// Device version (GPU / CPU compiled)
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, View<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  this->template init_device_view<_SHAPE_>();
  computeOnTheDevice(*this);
  return get_device_view<_SHAPE_>();
}
// GPU compiled, host view version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<int _SHAPE_, typename EXEC_SPACE>
inline std::enable_if_t<gpu_enabled_is_host_exec_space<EXEC_SPACE>, HostView<_TYPE_,_SHAPE_> >
TRUSTArray<_TYPE_,_SIZE_>::view_rw()
{
  bool flattened = check_flattened<_SHAPE_>(); //The accessors should never be called with the wrong _SHAPE_

  _SIZE_ dimension_tot_0 = flattened ? this->size_array() : this->dimension_tot(0);

  //Useful when casting a 1D Tab into a multi-D View !
  _SIZE_ dims[4] = {dimension_tot_0, nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0, nb_dim_>3 ? this->dimension_tot(3) : 0};

  return HostView<_TYPE_,_SHAPE_>(this->addr(), dims[0],
                                  1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
                                  3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG);
}

#endif
#endif

