/****************************************************************************
* Copyright (c) 2022, CEA
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
#include <Array_base.h>
#include <Statistiques.h>
static double clock_start;
static char* clock_on=NULL;
inline void start_timer()
{
  clock_on = getenv ("TRUST_CLOCK_ON");
  if (clock_on!=NULL) clock_start = Statistiques::get_time_now();
}
inline void end_timer(const std::string& str) // Return in [ms]
{
  if (clock_on!=NULL)
    {
      double ms = 1000 * (Statistiques::get_time_now() - clock_start);
      printf("[clock] %7.3f ms %15s\n", ms, str.c_str());
      fflush(stdout);
    }
}
inline void end_timer(const std::string& str, int size) // Return in [ms]
{
  if (clock_on!=NULL)
    {
      double ms = 10000 * (Statistiques::get_time_now() - clock_start);
      int mo = size/1024/1024;
      if (ms==0)
          printf("[clock]            %15s %6d Mo\n", str.c_str(), mo);
      else
      {
#ifndef INT_is_64_
      printf("[clock] %7.3f ms %15s %6d Mo %5.1f Go/s\n", ms ,str.c_str(), mo, mo/ms);
#else
      printf("[clock] %7.3f ms %15s %6ld Mo %5.1f Go/s\n", ms ,str.c_str(), mo, mo/ms);
#endif
      }
      fflush(stdout);
    }
}

// Pour disabler
inline bool computeOnDevice()
{
  return getenv("TRUST_DISABLE_DEVICE") == NULL ? true : false;
}

template <typename _TYPE_>
inline const _TYPE_* copyToDevice(const TRUSTArray<_TYPE_>& tab)
{
  // const array will matches on host and device
  const _TYPE_ *tab_addr = copyToDevice_(const_cast<TRUSTArray <_TYPE_>&>(tab), Array_base::HostDevice);
  return tab_addr;
}

template <typename _TYPE_>
inline _TYPE_* copyToDevice_(TRUSTArray<_TYPE_>& tab, Array_base::dataLocation nextLocation)
{
#ifdef _OPENMP
  Array_base::dataLocation currentLocation = tab.get_dataLocation();
  tab.set_dataLocation(nextLocation); // Important de specifier le nouveau status avant la recuperation du pointeur:
#endif
  _TYPE_* tab_addr = tab.addr(); // Car addr() contient un mecanisme de verification
#ifdef _OPENMP
  start_timer();
  if (currentLocation==Array_base::HostOnly)
    {
      #pragma omp target enter data if (computeOnDevice()) map(to:tab_addr[0:tab.size_array()])
      end_timer((std::string) "copyToDevice Array ", sizeof(_TYPE_) * tab.size_array());
    }
  else if (currentLocation==Array_base::Host)
    {
      #pragma omp target update if (computeOnDevice()) to(tab_addr[0:tab.size_array()])
      end_timer((std::string) "updateToDevice Array ", sizeof(_TYPE_) * tab.size_array());
    }
  else
      end_timer((std::string) "upToDateDevice Array ", sizeof(_TYPE_) * tab.size_array());
#endif
  return tab_addr;
}

template <typename _TYPE_>
inline _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab)
{
  // non-const array will be modified on device:
  _TYPE_ *tab_addr = copyToDevice_(tab, Array_base::Device);
  return tab_addr;
}

template <typename _TYPE_>
inline void copyFromDevice(TRUSTArray<_TYPE_>& tab)
{
#ifdef _OPENMP
  _TYPE_* tab_addr = tab.addr();
  if (tab.get_dataLocation()==Array_base::Device)
    {
      start_timer();
      #pragma omp target update if (computeOnDevice()) from(tab_addr[0:tab.size_array()])
      end_timer((std::string) "copyFromDevice Array ", sizeof(_TYPE_) * tab.size_array());
      tab.set_dataLocation(Array_base::HostDevice);
    }
#endif
}
#endif
