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

template<typename T, int _SHAPE_> struct InnerType { using TYPE = void;  };
template<typename T> struct InnerType<T,1> { using TYPE = T*;  };
template<typename T> struct InnerType<T,2> { using TYPE = T**;  };
template<typename T> struct InnerType<T,3> { using TYPE = T***;  };
template<typename T> struct InnerType<T,4> { using TYPE = T****;  };

//I had to do this otherwise ConstViews becomes view of pointers to consts T...
template<typename T, int _SHAPE_> struct ConstInnerType { using TYPE = void;  };
template<typename T> struct ConstInnerType<T,1> { using TYPE = const T*;  };
template<typename T> struct ConstInnerType<T,2> { using TYPE = const T**;  };
template<typename T> struct ConstInnerType<T,3> { using TYPE = const T***;  };
template<typename T> struct ConstInnerType<T,4> { using TYPE = const T****;  };

template<typename T, int _SHAPE_>
using DeviceView = Kokkos::View<typename InnerType<T, _SHAPE_>::TYPE, Kokkos::LayoutRight>;

// Whatever the compilation type, the host memory space:
using host_mirror_space = Kokkos::HostSpace;

// The execution space (=where code is run): on the device if compiled for GPU, else CPU.
using execution_space = DeviceView<double, 1>::execution_space;

// Typedefs for range policies in kernels
using range_1D = Kokkos::RangePolicy<execution_space>;
using range_2D = Kokkos::MDRangePolicy<execution_space, Kokkos::Rank<2>>;
using range_3D = Kokkos::MDRangePolicy<execution_space, Kokkos::Rank<3>>;

// The memory space (=where data is stored): on the device if compiled for GPU, or on CPU otherwise:
typedef std::conditional< \
std::is_same<execution_space, Kokkos::DefaultExecutionSpace>::value , \
DeviceView<double,1>::memory_space, host_mirror_space>::type \
memory_space;

//for host views:
//"You do not need to explicitly specify host_execution_space because host_mirror_space already implies that you are using the host execution space."
//memory space implies execution space

using unmanaged_memory = Kokkos::MemoryTraits<Kokkos::Unmanaged>;

// The actual view type that will be manipulated everywhere in the kernels (a *device* view)
template<typename T, int _SHAPE_>
using View = Kokkos::View<typename InnerType<T, _SHAPE_>::TYPE, typename DeviceView<T,_SHAPE_>::array_layout, memory_space>;

// Views on the host that allow conditional execution of loop that are not fully ported to device. They are unmanaged to avoid new allocation
template<typename T, int _SHAPE_>
using HostView = Kokkos::View<typename InnerType<T, _SHAPE_>::TYPE, typename DeviceView<T,_SHAPE_>::array_layout, host_mirror_space, unmanaged_memory>;
template<typename T, int _SHAPE_>
using ConstView = Kokkos::View<typename ConstInnerType<T, _SHAPE_>::TYPE, typename DeviceView<T,_SHAPE_>::array_layout, memory_space>;
template<typename T, int _SHAPE_>
using ConstHostView = Kokkos::View<typename ConstInnerType<T, _SHAPE_>::TYPE, typename DeviceView<T,_SHAPE_>::array_layout, host_mirror_space, unmanaged_memory>;

// Handy aliases
using IntArrView = View<int, 1>;
using DoubleArrView = View<double, 1>;

using CIntArrView = ConstView<int, 1>;
using CDoubleArrView = ConstView<double, 1>;

using IntTabView = View<int, 2>;
using IntTabView3 = View<int, 3>;
using IntTabView4 = View<int, 4>;
using DoubleTabView = View<double, 2>;
using DoubleTabView3 = View<double, 3>;
using DoubleTabView4 = View<double, 4>;

using CIntTabView = ConstView<int, 2>;
using CIntTabView3 = ConstView<int, 3>;
using CIntTabView4 = ConstView<int, 4>; // Changed from double to int
using CDoubleTabView = ConstView<double, 2>;
using CDoubleTabView3 = ConstView<double, 3>;
using CDoubleTabView4 = ConstView<double, 4>;

// Host views
using IntArrHostView = HostView<int, 1>;
using DoubleArrHostView = HostView<double, 1>;

using CIntArrHostView = ConstHostView<int, 1>;
using CDoubleArrHostView = ConstHostView<double, 1>;

using IntTabHostView = HostView<int, 2>;
using IntTabHostView3 = HostView<int, 3>;
using IntTabHostView4 = HostView<int, 4>;
using DoubleTabHostView = HostView<double, 2>;
using DoubleTabHostView3 = HostView<double, 3>;
using DoubleTabHostView4 = HostView<double, 4>;

using CIntTabHostView = ConstHostView<int, 2>;
using CIntTabHostView3 = ConstHostView<int, 3>;
using CIntTabHostView4 = ConstHostView<int, 4>; // Changed from double to int
using CDoubleTabHostView = ConstHostView<double, 2>;
using CDoubleTabHostView3 = ConstHostView<double, 3>;
using CDoubleTabHostView4 = ConstHostView<double, 4>;

//Wrapper functions to build a unmanaged view from a pointer and a set of dimensions
template <typename ViewType, typename _TYPE_, int _SHAPE_, typename _SIZE_>
inline ViewType createView(_TYPE_* ptr, const std::array<_SIZE_, 4>& dims)
{
  return ViewType(
           (_TYPE_*)(ptr),dims[0],
           1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
           2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
           3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG
         );
}
template <typename ViewType, typename _TYPE_, int _SHAPE_, typename _SIZE_>
inline ViewType createView(const _TYPE_* ptr, const std::array<_SIZE_, 4>& dims)
{
  return ViewType(
           (_TYPE_*)(ptr),dims[0],
           1 < _SHAPE_ ? dims[1] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
           2 < _SHAPE_ ? dims[2] : KOKKOS_IMPL_CTOR_DEFAULT_ARG,
           3 < _SHAPE_ ? dims[3] : KOKKOS_IMPL_CTOR_DEFAULT_ARG
         );
}
extern void kokkos_self_test();

#else // Kokkos not defined

template<int _SHAPE_> using IntView = IntTab;
template<int _SHAPE_> using DoubleView = DoubleTab;

#define KOKKOS_INLINE_FUNCTION inline
#endif

#endif
