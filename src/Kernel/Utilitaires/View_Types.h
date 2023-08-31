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

#ifndef View_Types_included
#define View_Types_included

#include <kokkos++.h>

#ifdef KOKKOS_

#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

// The DualView type allowing semi-automatic sync between host and device.
//  !WARNING! --> I choose 'LayoutRight' to stick to default data layout in TRUST
//   but this is not good for GPU ... hopefully second dimension is never too big in most cases ...
template<typename T>
using DualViewTab = Kokkos::DualView<T **, Kokkos::LayoutRight>;

// The execution space (=where code is run): on the device if compiled for GPU, else CPU.
using execution_space = DualViewTab<double>::execution_space;

// The memory space (=where data is stored): on the device if compiled for GPU, or on CPU otherwise:
typedef std::conditional< \
std::is_same<execution_space, Kokkos::DefaultExecutionSpace>::value , \
DualViewTab<double>::memory_space, DualViewTab<double>::host_mirror_space>::type \
memory_space;

// Whatever the compilation type, the host memory space:
using host_mirror_space = DualViewTab<double>::host_mirror_space;

// The actual view type that will be manipulated everywhere in the kernels (a *device* view)
template<typename T>
using ViewTab = Kokkos::View<T **, typename DualViewTab<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;

// Its const version:
template<typename T>
using ConstViewTab = Kokkos::View<const T **, typename DualViewTab<T>::array_layout, memory_space, Kokkos::MemoryRandomAccess>;

using IntTabView = ViewTab<int>;
using DoubleTabView = ViewTab<double>;

using CIntTabView = ConstViewTab<int>;
using CDoubleTabView = ConstViewTab<double>;

#endif // KOKKOS_

#endif
