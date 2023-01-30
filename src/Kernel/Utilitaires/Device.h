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

#ifndef Device_included
#define Device_included

// Timer:
#include <ctime>
#include <string>
#include <cstring>
#include <Array_base.h>
#include <stat_counters.h>
extern void init_openmp();
extern bool self_test();
static double clock_start;
static char* clock_on=NULL;
inline void start_timer()
{
  clock_on = getenv ("TRUST_CLOCK_ON");
  if (clock_on!=NULL) clock_start = Statistiques::get_time_now();
  statistiques().begin_count(gpu_kernel_counter_);
}
inline void end_timer(const std::string& str, int size=-1) // Return in [ms]
{
  if (clock_on!=NULL)
    {
      double ms = 1000 * (Statistiques::get_time_now() - clock_start);
      if (size==-1)
        {
          statistiques().end_count(gpu_kernel_counter_);
          printf("[clock] %7.3f ms [Kernel] %15s\n", ms, str.c_str());
        }
      else
        {
          double mo = (double)size / 1024 / 1024;
          if (ms == 0 || size==0)
            printf("[clock]            [Data]   %15s\n", str.c_str());
          else
            printf("[clock] %7.3f ms [Data]   %15s %6ld Bytes %5.1f Go/s\n", ms, str.c_str(), long(size), mo/ms);
          //printf("[clock] %7.3f ms [Data]   %15s %6ld Mo %5.1f Go/s\n", ms, str.c_str(), long(mo), mo/ms);
        }
      fflush(stdout);
    }
}

template <typename _TYPE_>
inline const _TYPE_* copyToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??")
{
  // const array will matches on host and device
  const _TYPE_ *tab_addr = copyToDevice_(const_cast<TRUSTArray <_TYPE_>&>(tab), HostDevice, arrayName);
  return tab_addr;
}
#include <sstream>
inline std::string toString(const void* adr)
{
  std::stringstream ss;
  ss << adr;
  return ss.str();
}
template <typename _TYPE_>
inline _TYPE_* copyToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName)
{
#ifdef _OPENMP
#ifndef NDEBUG
  if (self_test()) self_test();
#endif
  tab.nommer(arrayName); // ToDo implementer nom dans Array_base car ici Objet_U appele
  DataLocation currentLocation = tab.get_dataLocation();
  tab.set_dataLocation(nextLocation); // Important de specifier le nouveau status avant la recuperation du pointeur:
#endif
  _TYPE_* tab_addr = tab.addr(); // Car addr() contient un mecanisme de verification
#ifdef _OPENMP
  std::string message;
  start_timer();
  int size=0;
  if (currentLocation==HostOnly)
    {
      //#pragma omp target enter data if (Objet_U::computeOnDevice) map(to:tab_addr[0:tab.size_array()])
      //Argh le bug trouve: si plusieurs appels de copy (DoubleTrav?), l'update ne se fait pas car tableau deja alloue
      //donc on alloue puis on update pour etre certain de la copie:
      size = sizeof(_TYPE_) * tab.size_array();
      statistiques().begin_count(gpu_copytodevice_counter_);
      #pragma omp target enter data if (Objet_U::computeOnDevice) map(alloc:tab_addr[0:tab.size_array()])
      #pragma omp target update if (Objet_U::computeOnDevice) to(tab_addr[0:tab.size_array()])
      statistiques().end_count(gpu_copytodevice_counter_, size);
      message = "Copy to device array "+arrayName+" ["+toString(tab.addr())+"]";
    }
  else if (currentLocation==Host)
    {
      size = sizeof(_TYPE_) * tab.size_array();
      statistiques().begin_count(gpu_copytodevice_counter_);
      #pragma omp target update if (Objet_U::computeOnDevice) to(tab_addr[0:tab.size_array()])
      statistiques().end_count(gpu_copytodevice_counter_, size);
      message = "Update on device array "+arrayName+" ["+toString(tab.addr())+"]";
    }
  else
    {
      if (arrayName!="??")
        {
          message = "No change on device array "+arrayName+" ["+toString(tab.addr())+"]";
          size = 0;
        }
    }
  if (message!="") end_timer(message, size);
#endif
  return tab_addr;
}

template <typename _TYPE_>
inline _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??")
{
  // non-const array will be modified on device:
  _TYPE_ *tab_addr = copyToDevice_(tab, Device, arrayName);
  return tab_addr;
}

template <typename _TYPE_>
inline void copyFromDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??")
{
#ifdef _OPENMP
  _TYPE_* tab_addr = tab.addr();
  if (tab.get_dataLocation()==Device)
    {
      start_timer();
      int size = sizeof(_TYPE_) * tab.size_array();
      statistiques().begin_count(gpu_copyfromdevice_counter_, size);
      #pragma omp target update if (Objet_U::computeOnDevice) from(tab_addr[0:tab.size_array()])
      statistiques().end_count(gpu_copyfromdevice_counter_, size);
      std::string message;
      message = "Copy from device of array "+arrayName+" ["+toString(tab.addr())+"]";
      end_timer(message, size);
      if (clock_on) printf("\n");
      tab.set_dataLocation(HostDevice);
    }
#endif
}

#endif
