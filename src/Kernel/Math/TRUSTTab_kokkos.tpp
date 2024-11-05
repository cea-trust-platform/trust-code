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

#ifndef TRUSTTab_kokkos_TPP_included
#define TRUSTTab_kokkos_TPP_included

#include <TRUSTTab.h>
#ifdef KOKKOS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create internal DualView member, and populate it with current host data
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::init_view_tab2() const
{
  kokkos_self_test();
  long trail_dim = 1;
  for(int d=1; d < this->nb_dim(); d++)
    trail_dim *= this->dimension_tot(d);
  long dims[2] = {this->dimension_tot(0), trail_dim};

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(dual_view_tab2_.h_view.is_allocated() &&
      dual_view_tab2_.h_view.data() == this->data() &&
      dual_view_tab2_.view_device().data() == addrOnDevice(*this) &&
      (long) dual_view_tab2_.extent(0) == dims[0] &&
      (long) dual_view_tab2_.extent(1) == dims[1])
    return;

//  if(nb_dim() != 2)
//    Process::exit("Wrong dim number in view init!");

  using t_host = typename DualViewTab<_TYPE_>::t_host;  // Host type
  using t_dev = typename DualViewTab<_TYPE_>::t_dev;    // Device type
  //using size_type = typename DualViewTab<_TYPE_>::size_type;

  //const std::string& nom = this->le_nom().getString();

  // Re-use data already allocated on host to create host-view:
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!  WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //          This heavily relies on the LayoutRight defined for the DualView (which is not optimal
  //          for GPU processing, but avoids having to explicitely copying the data ...)
  t_host host_view = t_host(const_cast<_TYPE_ *>(this->data()), dims[0], dims[1]);
  // Empty view on device - just a memory allocation:
  //t_dev device_view = t_dev(nom, dims[0], dims[1]);
  t_dev device_view;
#ifdef _OPENMP_TARGET
  // Device memory is allocated with OpenMP: ToDo replace by allocate ?
  mapToDevice(*this, "Kokkos init_view_tab2()");
  device_view = t_dev(const_cast<_TYPE_ *>(addrOnDevice(*this)), dims[0], dims[1]);
  // Example of transpose which will be used to connect Cuda (AmgX) to Kokkos:
  /*
  device_view = t_dev("", dims[0], dims[1]);
  Kokkos::View<const _TYPE_ **, Kokkos::LayoutRight, memory_space, Kokkos::MemoryUnmanaged> device_view_LayoutRight(
    const_cast<_TYPE_ *>(addrOnDevice(*this)), dims[0], dims[1]);
  start_gpu_timer();
  Nom name("[KOKKOS] Deep_copy items= ");
  name += (Nom) (int) dims[0];
  // Kokkos::deep_copy(device_view, device_view_LayoutRight); Slower 100-200% ?
  Kokkos::parallel_for("[KOKKOS] Manual transpose ", Kokkos::RangePolicy<>(0, dims[0]), KOKKOS_LAMBDA(
                         const int i)
  {
    for (int j = 0; j < dims[1]; j++)
      device_view(i, j) = device_view_LayoutRight(j, i);
  });
  end_gpu_timer(Objet_U::computeOnDevice, name.getString());
   */
#else
  device_view = create_mirror_view_and_copy(Kokkos::DefaultExecutionSpace::memory_space(), host_view);
#endif

  // Dual view is made as an assembly of the two views:
  dual_view_tab2_ = DualViewTab<_TYPE_>(device_view, host_view);

  // Mark data modified on host so it will be sync-ed to device later on:
  dual_view_tab2_.template modify<host_mirror_space>();
}

//////////// Read-Only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_ro() const
{
  // Init if necessary
  init_view_tab2();
#ifdef _OPENMP_TARGET
  mapToDevice(*this, "Kokkos TRUSTTab::view_ro()");
#else
  // Copy to device if needed (i.e. if modify() was called):
  dual_view_tab2_.template sync<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab2_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_ro() const
{
  return ConstHostViewTab<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1));
}

//////////// Write-only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_wo()
{
  // Init if necessary
  init_view_tab2();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTTab<_TYPE_,_SIZE_>::view_wo()"); // ToDo allouer sans copie ?
#else
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab2_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab2_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_wo()
{
  return HostViewTab<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1));
}

//////////// Read-Write ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_rw()
{
  // Init if necessary
  init_view_tab2();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos view_rw()");
#else
  // Copy to device (if needed) ...
  dual_view_tab2_.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab2_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab2_.view_device();
}


// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view_rw()
{
  return HostViewTab<_TYPE_>(this->addr(), this->size());
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::sync_to_host() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Copy to host (if needed) ...
  dual_view_tab2_.template sync<host_mirror_space>();
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::modified_on_host() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Mark modified on host side:
  if(dual_view_tab2_.h_view.is_allocated())
    dual_view_tab2_.template modify<host_mirror_space>();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 3D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create internal DualView member, and populate it with current host data
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::init_view_tab3() const
{
  long dims[3] = {this->dimension_tot(0), nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0};

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(dual_view_tab3_.h_view.is_allocated() &&
      dual_view_tab3_.h_view.data() == this->data() &&
      dual_view_tab3_.view_device().data() == addrOnDevice(*this) &&
      (long) dual_view_tab3_.extent(0) == dims[0] &&
      (long) dual_view_tab3_.extent(1) == dims[1] &&
      (long) dual_view_tab3_.extent(2) == dims[2])
    return;

  //if(nb_dim() != 3)
  //  Process::exit("Wrong dim number in view init!");

  using t_host = typename DualViewTab3<_TYPE_>::t_host;  // Host type
  using t_dev = typename DualViewTab3<_TYPE_>::t_dev;    // Device type
  //using size_type = typename DualViewTab3<_TYPE_>::size_type;

  //const std::string& nom = this->le_nom().getString();

  // Re-use data already allocated on host to create host-view:
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!  WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //          This heavily relies on the LayoutRight defined for the DualView (which is not optimal
  //          for GPU processing, but avoids having to explicitely copying the data ...)
  t_host host_view = t_host(const_cast<_TYPE_ *>(this->data()), dims[0], dims[1], dims[2]);
  // Empty view on device - just a memory allocation:
  //t_dev device_view = t_dev(nom, dims[0], dims[1], dims[2]);
  t_dev device_view;
#ifdef _OPENMP_TARGET
  // Device memory is allocated with OpenMP: ToDo replace by allocate ?
  mapToDevice(*this, "Kokkos init_view_tab3()");
  device_view = t_dev(const_cast<_TYPE_ *>(addrOnDevice(*this)), dims[0], dims[1], dims[2]);
#else
  device_view = create_mirror_view_and_copy(Kokkos::DefaultExecutionSpace::memory_space(), host_view);
#endif

  // Dual view is made as an assembly of the two views:
  dual_view_tab3_ = DualViewTab3<_TYPE_>(device_view, host_view);

  // Mark data modified on host so it will be sync-ed to device later on:
  dual_view_tab3_.template modify<host_mirror_space>();
}

/////////// Read-Only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_ro() const
{
  // Init if necessary
  init_view_tab3();
#ifdef _OPENMP_TARGET
  mapToDevice(*this, "Kokkos TRUSTTab3::view3_ro()");
#else
  // Copy to device if needed (i.e. if modify() was called):
  dual_view_tab3_.template sync<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab3_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_ro() const
{
  return ConstHostViewTab3<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1), this->dimension_tot(2));
}

//////////// Write-only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_wo()
{
  // Init if necessary
  init_view_tab3();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTTab<_TYPE_,_SIZE_>::view3_wo()"); // ToDo allouer sans copie ?
#else
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab3_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab3_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_wo()
{
  return HostViewTab3<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1), this->dimension_tot(2));
}

//////////// Read-Write ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_rw()
{
  // Init if necessary
  init_view_tab3();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos view3_rw()");
#else
  // Copy to device (if needed) ...
  dual_view_tab3_.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab3_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab3_.view_device();
}
// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab3<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view3_rw()
{
  return HostViewTab3<_TYPE_>(this->addr(), this->size());
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::sync_to_host3() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Copy to host (if needed) ...
  dual_view_tab3_.template sync<host_mirror_space>();
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::modified_on_host3() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Mark modified on host side:
  if(dual_view_tab3_.h_view.is_allocated())
    dual_view_tab3_.template modify<host_mirror_space>();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 4D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Create internal DualView member, and populate it with current host data
template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::init_view_tab4() const
{
  long dims[4] = {this->dimension_tot(0), nb_dim_>1 ? this->dimension_tot(1) : 0, nb_dim_>2 ? this->dimension_tot(2) : 0 , nb_dim_>3 ? this->dimension_tot(3) : 0};

  // change of alloc or resize triggers re-init (for now - resize could be done better)
  if(dual_view_tab4_.h_view.is_allocated() &&
      dual_view_tab4_.h_view.data() == this->data() &&
      dual_view_tab4_.view_device().data() == addrOnDevice(*this) &&
      (long)dual_view_tab4_.extent(0) == dims[0] &&
      (long)dual_view_tab4_.extent(1) == dims[1] &&
      (long)dual_view_tab4_.extent(2) == dims[2] &&
      (long)dual_view_tab4_.extent(3) == dims[3])
    return;

  //if(nb_dim() != 4)
  //  Process::exit("Wrong dim number in view init!");

  using t_host = typename DualViewTab4<_TYPE_>::t_host;  // Host type
  using t_dev = typename DualViewTab4<_TYPE_>::t_dev;    // Device type
  //using size_type = typename DualViewTab4<_TYPE_>::size_type;

  //const std::string& nom = this->le_nom().getString();

  // Re-use data already allocated on host to create host-view:
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!  WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //          This heavily relies on the LayoutRight defined for the DualView (which is not optimal
  //          for GPU processing, but avoids having to explicitely copying the data ...)
  t_host host_view = t_host(const_cast<_TYPE_ *>(this->data()), dims[0], dims[1], dims[2], dims[3]);
  // Empty view on device - just a memory allocation:
  //t_dev device_view = t_dev(nom, dims[0], dims[1], dims[2], dims[3]);
  t_dev device_view;
#ifdef _OPENMP_TARGET
  // Device memory is allocated with OpenMP: ToDo replace by allocate ?
  mapToDevice(*this, "Kokkos init_view_tab4()");
  device_view = t_dev(const_cast<_TYPE_ *>(addrOnDevice(*this)), dims[0], dims[1], dims[2], dims[3]);
#else
  device_view = create_mirror_view_and_copy(Kokkos::DefaultExecutionSpace::memory_space(), host_view);
#endif

  // Dual view is made as an assembly of the two views:
  dual_view_tab4_ = DualViewTab4<_TYPE_>(device_view, host_view);

  // Mark data modified on host so it will be sync-ed to device later on:
  dual_view_tab4_.template modify<host_mirror_space>();
}

/////////// Read-Only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ConstViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_ro() const
{
  // Init if necessary
  init_view_tab4();
#ifdef _OPENMP_TARGET
  mapToDevice(*this, "Kokkos TRUSTTab::view4_ro()");
#else
  // Copy to device if needed (i.e. if modify() was called):
  dual_view_tab4_.template sync<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab4_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, ConstHostViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_ro() const
{
  return ConstHostViewTab4<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1), this->dimension_tot(2), this->dimension_tot(3));
}

//////////// Write-only ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_wo()
{
  // Init if necessary
  init_view_tab4();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos TRUSTTab4<_TYPE_,_SIZE_>::view4_wo()"); // ToDo allouer sans copie ?
#else
  // Mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab4_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab4_.view_device();
}

// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_wo()
{
  return HostViewTab4<_TYPE_>(this->addr(), this->dimension_tot(0), this->dimension_tot(1), this->dimension_tot(2), this->dimension_tot(3));
}

//////////// Read-Write ////////////////////////////
// Device version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_default_exec_space<EXEC_SPACE>, ViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_rw()
{
  // Init if necessary
  init_view_tab4();
#ifdef _OPENMP_TARGET
  computeOnTheDevice(*this, "Kokkos view4_rw()");
#else
  // Copy to device (if needed) ...
  dual_view_tab4_.template sync<memory_space>();
  // ... and mark the (device) data as modified, so that the next sync() (to host) will copy:
  dual_view_tab4_.template modify<memory_space>();
#endif
  // return *device* view:
  return dual_view_tab4_.view_device();
}
// Host version
template<typename _TYPE_, typename _SIZE_>  // this one first!!
template<typename EXEC_SPACE>
inline std::enable_if_t<is_host_exec_space<EXEC_SPACE>, HostViewTab4<_TYPE_> >
TRUSTTab<_TYPE_,_SIZE_>::view4_rw()
{
  return HostViewTab4<_TYPE_>(this->addr(), this->size());
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::sync_to_host4() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Copy to host (if needed) ...
  dual_view_tab4_.template sync<host_mirror_space>();
}

template<typename _TYPE_, typename _SIZE_>
inline void TRUSTTab<_TYPE_,_SIZE_>::modified_on_host4() const
{
#ifdef _OPENMP_TARGET
  Process::exit("ToDo");
#endif
  // Mark modified on host side:
  if(dual_view_tab4_.h_view.is_allocated())
    dual_view_tab4_.template modify<host_mirror_space>();
}

// Methode de debug:
template<typename _TYPE_, typename _SIZE_>
void debug_device_view(const ViewTab<_TYPE_> view_tab, TRUSTTab<_TYPE_,_SIZE_>& tab, _SIZE_ max_size=-1)
{
  assert(view_tab.data()==addrOnDevice(tab)); // Verifie meme adress
  Cout << "View size=" << view_tab.size() << finl;
  _SIZE_ size = max_size;
  if (size==-1) size = (int)view_tab.extent(0);
  int nb_compo = (int)view_tab.extent(1);
  Kokkos::parallel_for(size, KOKKOS_LAMBDA(const int i)
  {
    for (int j=0; j<nb_compo; j++)
      printf("[Kokkos]: %p [%2d,%2d]=%e\n", (void*)view_tab.data(), i, j, view_tab(i,j));
  });
  Cout << "Tab size=" << tab.size_array() << finl;
  assert((int)view_tab.size()==tab.size_array());
//  nb_compo = tab.dimension(1);
//  _TYPE_ *ptr = tab.data();
//  #pragma omp target teams distribute parallel for
//  for (int i=0; i<size; i++)
//    {
//      for (int j=0; j<nb_compo; j++)
//        printf("[OpenMP]: %p [%d,%d]=%e\n", (void*)ptr,  i, j, ptr[i*nb_compo+j]);
//    }
}
#endif
#endif
