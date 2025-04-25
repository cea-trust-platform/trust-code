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

#ifndef Device_included
#define Device_included

#include <TRUSTTabs_forward.h>
#include <Nom.h>
#include <stat_counters.h>
#include <kokkos++.h>

#ifdef TRUST_USE_CUDA
// See https://nvidia.github.io/NVTX/
// See https://stackoverflow.com/questions/23230003/something-between-func-and-pretty-function/29856690#29856690
#include <nvtx3/nvToolsExt.h>
#endif

// TODO - scope all this, global vars are bad.
extern bool init_device_, clock_on, timer;
extern double clock_start;
extern int timer_counter;

void self_test();
void init_device();
void init_cuda();
std::string ptrToString(const void* adr);

#ifdef TRUST_USE_GPU
#define ToDo_Kokkos(str)                              \
        if (Process::je_suis_maitre()) fprintf(stderr, "[Kokkos %s] Warning, code running slow cause not ported yet: line %d in %s \n", str, __LINE__, __FILE__);
#else
#define ToDo_Kokkos(str)
#endif

// Macro Kernel_Name
#ifdef TRUST_USE_GPU
inline const std::string methodName(const std::string& prettyFunction, const int line)
{
  size_t colons = prettyFunction.find("::");
  if (colons != std::string::npos)
    {
      // method::class:line
      size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;
      size_t end = prettyFunction.rfind("(") - begin;
      return prettyFunction.substr(begin, end) + ":" + std::to_string(line);
    }
  else
    {
      // return_type function:line
      colons = prettyFunction.find('(');
      return prettyFunction.substr(0, colons) + ":" + std::to_string(line);
    }
}
#define __KERNEL_NAME__ methodName(__PRETTY_FUNCTION__,__LINE__)
#else
#define __KERNEL_NAME__ ""
#endif

// Timers GPU (avec possibilite de desactiver avec if (timer) dans certains Kernels critiques sur CPU):
inline std::string start_gpu_timer(std::string str="kernel", int bytes=-1)
{
#ifdef TRUST_USE_GPU
  if (init_device_)
    {
      timer_counter++;
#ifndef NDEBUG
      if (timer_counter>1)
        Cerr << "[Kokkos] timer_counter=" << timer_counter << " : start_gpu_timer() not closed by end_gpu_timer() !" << finl;
      //Process::exit("Error, start_gpu_timer() not closed by end_gpu_timer() !");
#endif
      if (clock_on) clock_start = Statistiques::get_time_now();
      if (bytes == -1) statistiques().begin_count(gpu_kernel_counter_, false);
#ifdef TRUST_USE_CUDA
      if (!str.empty()) nvtxRangePush(str.c_str());
#endif
    }
#endif
  return str;
}

inline void end_gpu_timer(const std::string& str, int onDevice=1, int bytes=-1) // Return in [ms]
{
#ifdef TRUST_USE_GPU
  if (init_device_)
    {
      timer_counter--;
#ifndef NDEBUG
      if (timer_counter!=0)
        Cerr << "[Kokkos] timer_counter=" << timer_counter << " : end_gpu_timer() not opened by start_gpu_timer() !" << finl;
      //Process::exit("Error, start_gpu_timer() not closed by end_gpu_timer() !");
#endif
      if (onDevice)
        {
#ifdef TRUST_USE_UVM
          cudaDeviceSynchronize();
#endif
#ifdef KOKKOS
          Kokkos::fence();  // Barrier for real time
#endif
        }
      if (bytes == -1) statistiques().end_count(gpu_kernel_counter_, 0, onDevice, false);
      if (clock_on) // Affichage
        {
          std::string clock(Process::is_parallel() ? "[clock]#" + std::to_string(Process::me()) : "[clock]  ");
          double ms = 1000 * (Statistiques::get_time_now() - clock_start);
          if (bytes == -1)
            {
              if (!str.empty())
                printf("%s %7.3f ms [%s %15s\n", clock.c_str(), ms, onDevice ? "Device]" : "Host]  ", str.c_str());
            }
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
      if (!str.empty()) nvtxRangePop();
#endif
    }
#endif
}

template <typename _TYPE_>
extern _TYPE_* addrOnDevice(_TYPE_* ptr);

template <typename _TYPE_, typename _SIZE_=int>
extern _TYPE_* addrOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
inline const _TYPE_* addrOnDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab)
{
  return addrOnDevice(const_cast<TRUSTArray<_TYPE_,_SIZE_>&>(tab));
}

template <typename _TYPE_, typename _SIZE_=int>
extern _TYPE_* allocateOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
inline const _TYPE_* allocateOnDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab)
{
  return allocateOnDevice(const_cast<TRUSTArray<_TYPE_,_SIZE_>&>(tab));
}

template <typename _TYPE_>
extern bool isAllocatedOnDevice(_TYPE_* tab_addr);

template <typename _TYPE_, typename _SIZE_=int>
extern bool isAllocatedOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern void deleteOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern const _TYPE_* mapToDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern _TYPE_* mapToDevice_(TRUSTArray<_TYPE_,_SIZE_>& tab, DataLocation nextLocation);

template <typename _TYPE_, typename _SIZE_=int>
extern _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern void copyFromDevice(TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern void copyFromDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab);

template <typename _TYPE_, typename _SIZE_=int>
extern _TYPE_* allocateOnDevice(_TYPE_* ptr, _SIZE_ size);

template <typename _TYPE_, typename _SIZE_=int>
extern void deleteOnDevice(_TYPE_* ptr, _SIZE_ size);

template <typename _TYPE_, typename _SIZE_=int>
extern void copyToDevice(_TYPE_* ptr, _SIZE_ size);

template <typename _TYPE_, typename _SIZE_=int>
extern void copyFromDevice(_TYPE_* ptr, _SIZE_ size);

#endif
