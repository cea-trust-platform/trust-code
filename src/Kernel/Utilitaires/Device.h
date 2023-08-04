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

#include <Array_base.h>
#include <Nom.h>
#ifdef _OPENMP
#include <omp.h>
#endif

extern bool self_test();
extern void init_openmp();
extern void init_cuda();
extern std::string toString(const void* adr);
// Si OpenMP implementation
#ifdef _OPENMP
extern void start_timer(int bytes=-1);
extern void end_timer(int onDevice, const std::string& str, int bytes=-1);
#else // Sinon inlining vide:
inline void start_timer(int bytes=-1) {};
inline void end_timer(int onDevice, const std::string& str, int bytes=-1) {};
#endif

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
  // omp_target_is_present buggee ? Renvoie 0 sur le device 1 meme si alloue...
  if (omp_get_default_device()!=0) Process::exit((Nom)"Do not use isAllocatedOnDevice(tab_addr) on device other than 0 ! Contact TRUST support.");
  return omp_target_is_present(tab_addr, omp_get_default_device())==1;
#else
  return false;
#endif
}

template <typename _TYPE_>
bool isAllocatedOnDevice(TRUSTArray<_TYPE_>& tab)
{
#ifdef _OPENMP
  if (omp_get_default_device()==0)
    return isAllocatedOnDevice(tab.addrForDevice());
  else
#endif
    return tab.get_dataLocation()!=HostOnly;
}

template <typename _TYPE_>
extern void deleteOnDevice(TRUSTArray<_TYPE_>& tab);

template <typename _TYPE_>
extern const _TYPE_* mapToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern _TYPE_* mapToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName);

template <typename _TYPE_>
extern _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

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
