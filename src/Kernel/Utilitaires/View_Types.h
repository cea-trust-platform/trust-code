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

#ifndef View_Types_included
#define View_Types_included

#include <kokkos++.h>

#ifdef KOKKOS
// The DualView type allowing semi-automatic sync between host and device.
// By default, host is 'LayoutRight' and device is 'LayoutLeft' -> very important
// We keep Kokkos::LayoutRight for OpenMP now
template<typename T>
using DualViewArr = Kokkos::DualView<T *, Kokkos::LayoutRight>;
template<typename T>
using DualViewTab = Kokkos::DualView<T **, Kokkos::LayoutRight>;
template<typename T>
using DualViewTab3 = Kokkos::DualView<T ***, Kokkos::LayoutRight>;
template<typename T>
using DualViewTab4 = Kokkos::DualView<T ****, Kokkos::LayoutRight>;

// The execution space (=where code is run): on the device if compiled for GPU, else CPU.
using execution_space = DualViewArr<double>::execution_space;

// Typedefs for range policies in kernels
using range_1D = Kokkos::RangePolicy<execution_space>;
using range_2D = Kokkos::MDRangePolicy<execution_space, Kokkos::Rank<2>>;
using range_3D = Kokkos::MDRangePolicy<execution_space, Kokkos::Rank<3>>;

// The memory space (=where data is stored): on the device if compiled for GPU, or on CPU otherwise:
typedef std::conditional< \
std::is_same<execution_space, Kokkos::DefaultExecutionSpace>::value , \
DualViewArr<double>::memory_space, DualViewArr<double>::host_mirror_space>::type \
memory_space;

// Whatever the compilation type, the host memory space:
using host_mirror_space = DualViewArr<double>::host_mirror_space;

//for host views:
//"You do not need to explicitly specify host_execution_space because host_mirror_space already implies that you are using the host execution space."
//memory space implies execution space

using random_unmanaged_memory = Kokkos::MemoryTraits<Kokkos::RandomAccess | Kokkos::Unmanaged>;
// The actual view type that will be manipulated everywhere in the kernels (a *device* view)
template<typename T>
using ViewArr = Kokkos::View<T *, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ViewTab = Kokkos::View<T **, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ViewTab3 = Kokkos::View<T ***, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ViewTab4 = Kokkos::View<T ****, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;

// Views on the host that allow conditional execution of loop that are not fully ported to device. They are unmanaged to avoid new allocation
template<typename T>
using HostViewArr = Kokkos::View<T *, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using HostViewTab = Kokkos::View<T **, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using HostViewTab3 = Kokkos::View<T ***, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using HostViewTab4 = Kokkos::View<T ****, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;

// Its const version (const disabled for OpenMP, weird bug)
#ifdef _OPENMP_TARGET
template<typename T>
using ConstViewArr = Kokkos::View</* const */T *, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab = Kokkos::View</* const */T **, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab3 = Kokkos::View</* const */T ***, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab4 = Kokkos::View</* const */T ****, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
// Host views
template<typename T>
using ConstHostViewArr = Kokkos::View<const T *, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab = Kokkos::View<const T **, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab3 = Kokkos::View<const T ***, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab4 = Kokkos::View<const T ****, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;

#else //else openmp

template<typename T>
using ConstViewArr = Kokkos::View<const T *, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab = Kokkos::View<const T **, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab3 = Kokkos::View<const T ***, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
template<typename T>
using ConstViewTab4 = Kokkos::View<const T ****, typename DualViewArr<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;
// Host views
template<typename T>
using ConstHostViewArr = Kokkos::View<const T *, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab = Kokkos::View<const T **, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab3 = Kokkos::View<const T ***, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
template<typename T>
using ConstHostViewTab4 = Kokkos::View<const T ****, typename DualViewArr<T>::array_layout, host_mirror_space,  random_unmanaged_memory>;
#endif

// Handy aliases:
using IntArrView = ViewArr<int>;
using DoubleArrView = ViewArr<double>;

using CIntArrView = ConstViewArr<int>;
using CDoubleArrView = ConstViewArr<double>;

using IntTabView = ViewTab<int>;
using DoubleTabView = ViewTab<double>;
using DoubleTabView3 = ViewTab3<double>;
using DoubleTabView4 = ViewTab4<double>;

using CIntTabView = ConstViewTab<int>;
using CIntTabView3 = ConstViewTab3<int>;
using CDoubleTabView = ConstViewTab<double>;
using CDoubleTabView3 = ConstViewTab3<double>;
using CDoubleTabView4 = ConstViewTab4<double>;

// Host views
using IntArrHostView = HostViewArr<int>;
using DoubleArrHostView = HostViewArr<double>;

using CIntArrHostView = ConstHostViewArr<int>;
using CDoubleArrHostView = ConstHostViewArr<double>;

using IntTabHostView = HostViewTab<int>;
using DoubleTabHostView = HostViewTab<double>;
using DoubleTabHostView3 = HostViewTab3<double>;
using DoubleTabHostView4 = HostViewTab4<double>;

using CIntTabHostView = ConstHostViewTab<int>;
using CIntTabHostView3 = ConstHostViewTab3<int>;
using CDoubleTabHostView = ConstHostViewTab<double>;
using CDoubleTabHostView3 = ConstHostViewTab3<double>;
using CDoubleTabHostView4 = ConstHostViewTab4<double>;

extern void kokkos_self_test();
#else

using IntArrView = IntVect;
using DoubleArrView = DoubleVect;

using CIntArrView = const IntVect;
using CDoubleArrView = const DoubleVect;

using IntTabView = IntTab;
using DoubleTabView = DoubleTab;
using DoubleTabView3 = DoubleTab;
using DoubleTabView4 = DoubleTab;

using CIntTabView = IntTab& ;
using CIntTabView3 = IntTab& ;
using CDoubleTabView = const DoubleTab& ;
using CDoubleTabView3 = const DoubleTab;
using CDoubleTabView4 = const DoubleTab;
//Host
using IntArrHostView = IntVect;
using DoubleArrHostView = DoubleVect;

using CIntArrHostView = const IntVect;
using CDoubleArrHostView = const DoubleVect;

using IntTabHostView = IntTab;
using DoubleTabHostView = DoubleTab;
using DoubleTabHostView3 = DoubleTab;
using DoubleTabHostView4 = DoubleTab;

using CIntTabHostView = IntTab& ;
using CIntTabHostView3 = IntTab& ;
using CDoubleTabHostView = const DoubleTab& ;
using CDoubleTabHostView3 = const DoubleTab;
using CDoubleTabHostView4 = const DoubleTab;

#define KOKKOS_INLINE_FUNCTION inline
#endif

#endif
