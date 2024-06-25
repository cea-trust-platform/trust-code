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

#ifndef Device_included
#define Device_included

#include <TRUSTTabs_forward.h>
#include <Nom.h>
#include <stat_counters.h>
#include <kokkos++.h>

#ifdef _OPENMP
#include <omp.h>
#ifdef TRUST_USE_CUDA
#include <nvtx3/nvToolsExt.h>
#endif
// See https://nvidia.github.io/NVTX/
// See https://stackoverflow.com/questions/23230003/something-between-func-and-pretty-function/29856690#29856690
#endif

// TODO - scope all this, global vars are bad.
extern bool init_openmp_, clock_on, timer_on;
extern double clock_start;

void self_test();
void exit_on_copy_condition(int size);
void set_exit_on_copy_condition(int size);
void init_openmp();
void init_cuda();
int allocatedBytesOnDevice();
std::string ptrToString(const void* adr);

#ifdef _OPENMP
#define ToDo_Kokkos(str)                              \
        printf("[Kokkos %s] Warning, code running slow cause not ported yet: line %d in %s \n", str, __LINE__, __FILE__);
#else
#define ToDo_Kokkos(str)
#endif

// Macro Kernel_Name
#ifdef _OPENMP
inline const std::string methodName(const std::string& prettyFunction, const int line)
{
  size_t colons = prettyFunction.find("::");
  size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
  size_t end = prettyFunction.rfind("(") - begin;
  return prettyFunction.substr(begin,end)+":"+std::to_string(line);
}
#define __KERNEL_NAME__ methodName(__PRETTY_FUNCTION__,__LINE__)
#else
#define __KERNEL_NAME__ ""
#endif

// Timers GPU avec OpenMP (renommer?)
inline std::string start_gpu_timer(std::string str="kernel", int bytes=-1)
{
#ifdef _OPENMP
  if (init_openmp_ && timer_on)
    {
      if (clock_on) clock_start = Statistiques::get_time_now();
      if (bytes == -1) statistiques().begin_count(gpu_kernel_counter_);
#ifdef TRUST_USE_CUDA
      nvtxRangePush(str.c_str());
#endif
    }
#endif
  return str;
}

inline void end_gpu_timer(int onDevice, const std::string& str, int bytes=-1) // Return in [ms]
{
#ifdef _OPENMP
  if (init_openmp_ && timer_on)
    {
      if (onDevice)
        {
#ifdef TRUST_USE_UVM
          cudaDeviceSynchronize();
#endif
#ifdef KOKKOS
          Kokkos::fence();  // Barrier for real time
#endif
        }
      if (bytes == -1) statistiques().end_count(gpu_kernel_counter_, 0, onDevice);
      if (clock_on) // Affichage
        {
          std::string clock(Process::is_parallel() ? "[clock]#" + std::to_string(Process::me()) : "[clock]  ");
          double ms = 1000 * (Statistiques::get_time_now() - clock_start);
          if (bytes == -1)
            if (onDevice)
              printf("%s %7.3f ms [Kernel] %15s\n", clock.c_str(), ms, str.c_str());
            else
              printf("%s %7.3f ms [Host]   %15s\n", clock.c_str(), ms, str.c_str());
          else
            {
              double mo = (double) bytes / 1024 / 1024;
              if (ms == 0 || bytes == 0)
                printf("%s            [Data]   %15s\n", clock.c_str(), str.c_str());
              else
                printf("%s %7.3f ms [Data]   %15s %6ld Bytes %5.1f Go/s\n", clock.c_str(), ms, str.c_str(),
                       long(bytes), mo / ms);
              //printf("%s %7.3f ms [Data]   %15s %6ld Mo %5.1f Go/s\n", clock.c_str(), ms, str.c_str(), long(mo), mo/ms);
            }
          fflush(stdout);
        }
#ifdef TRUST_USE_CUDA
      nvtxRangePop();
#endif
    }
#endif
}

template <typename _TYPE_>
extern _TYPE_* addrOnDevice(TRUSTArray<_TYPE_>& tab);

template <typename _TYPE_>
inline const _TYPE_* addrOnDevice(const TRUSTArray<_TYPE_>& tab)
{
  return addrOnDevice(const_cast<TRUSTArray<_TYPE_>&>(tab));
}

template <typename _TYPE_>
extern _TYPE_* allocateOnDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
inline const _TYPE_* allocateOnDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??")
{
  return allocateOnDevice(const_cast<TRUSTArray<_TYPE_>&>(tab), arrayName);
}

template <typename _TYPE_>
void allocateOnDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??")
{
  allocateOnDevice(const_cast<TRUSTArray<_TYPE_>&>(tab), arrayName);
}

template <typename _TYPE_>
bool isAllocatedOnDevice(_TYPE_* tab_addr)
{
  // Routine omp_target_is_present pour existence d'une adresse sur le device
  // https://www.openmp.org/spec-html/5.0/openmpse34.html#openmpsu168.html
#ifdef _OPENMP
  return omp_target_is_present(tab_addr, omp_get_default_device())==1;
#else
  return false;
#endif
}

template <typename _TYPE_>
bool isAllocatedOnDevice(TRUSTArray<_TYPE_>& tab)
{
#ifdef _OPENMP
  bool isAllocatedOnDevice1 = (tab.get_data_location() != DataLocation::HostOnly);
  bool isAllocatedOnDevice2 = isAllocatedOnDevice(tab.data());
  if (isAllocatedOnDevice1!=isAllocatedOnDevice2) Process::exit("isAllocatedOnDevice(TRUSTArray<_TYPE_>& tab) error! Seems tab.get_data_location() is not up-to-date !");
  return isAllocatedOnDevice2;
#else
  return false;
#endif
}

template <typename _TYPE_>
extern void deleteOnDevice(TRUSTArray<_TYPE_>& tab);

template <typename _TYPE_>
extern const _TYPE_* mapToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??", const bool enabled=true);

template <typename _TYPE_>
extern _TYPE_* mapToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName);

template <typename _TYPE_>
extern _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??", const bool enabled=true);

template <typename _TYPE_>
extern void copyFromDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern void copyFromDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern void copyPartialFromDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??");

template <typename _TYPE_>
void copyPartialFromDevice(const TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??")
{
  copyPartialFromDevice(const_cast<TRUSTArray<_TYPE_>&>(tab), deb, fin, arrayName);
}

template <typename _TYPE_>
extern void copyPartialToDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??");

template <typename _TYPE_>
extern void copyPartialToDevice(const TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??");

// ToDo OpenMP implemente methods for pointer (used only for the moment in Schema_Comm_Vecteurs for buffer communication with _TYPE_=char):
template <typename _TYPE_>
extern _TYPE_* allocateOnDevice(_TYPE_* ptr, int size, std::string arrayName="??");

template <typename _TYPE_>
extern void deleteOnDevice(_TYPE_* ptr, int size);

template <typename _TYPE_>
extern void copyToDevice(_TYPE_* ptr, int size, std::string arrayName="??");

template <typename _TYPE_>
extern void copyFromDevice(_TYPE_* ptr, int size, std::string arrayName="??");

#endif
