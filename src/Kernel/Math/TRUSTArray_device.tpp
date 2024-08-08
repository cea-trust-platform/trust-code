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

#ifndef TRUSTArr_device_TPP_included
#define TRUSTArr_device_TPP_included

#include <TRUSTArray.h>

// Methodes de verification que le tableau est a jour sur le host:
// ToDo OpenMP :Appels couteux (car non inlines?) depuis operator()[int] mais comment faire mieux ?
template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::checkDataOnHost() const
{
#if defined(_OPENMP) && !defined(TRUST_USE_UVM)
  if (get_data_location()==DataLocation::Device)
    copyFromDevice(*this, "const detected with checkDataOnHost()");
#endif
}

template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::checkDataOnHost()
{
#if defined(_OPENMP) && !defined(TRUST_USE_UVM)
  const DataLocation& loc = get_data_location();
  if (loc==DataLocation::Host || loc==DataLocation::HostOnly || loc==DataLocation::PartialHostDevice) return;
  else if (loc==DataLocation::Device)
    copyFromDevice(*this, "non-const detected with checkDataOnHost()");
  // On va modifier le tableau (non const) sur le host:
  set_data_location(DataLocation::Host);
#endif
}

// Fonction pour connaitre la localisation du tableau
template<typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::isDataOnDevice() const
{
  DataLocation loc = get_data_location();
  return loc == DataLocation::Device || loc == DataLocation::HostDevice;
}

template<typename _TYPE_>
inline void TRUSTArray<_TYPE_>::printKernel(bool flag, const TRUSTArray<_TYPE_>& tab, std::string kernel_name) const
{
  if (kernel_name!="??" && tab.size_array()>100 && clock_on)
    {
      std::string clock(Process::is_parallel() ? "[clock]#"+std::to_string(Process::me()) : "[clock]  ");
      std::cout << clock << "            [" << (flag ? "Kernel] " : "Host]   ") << kernel_name
                << " with a loop on array [" << ptrToString(tab.data()).c_str() << "] of " << tab.size_array()
                << " elements" << std::endl ;
    }
}

// Fonctions checkDataOnDevice pour lancement conditionnel de kernels sur le device:
// -Si les tableaux passes en parametre sont sur a jour sur le device
// -Si ce n'est pas le cas, les tableaux sont copies sur le host via checkDataOnHost
template<typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::checkDataOnDevice(std::string kernel_name) const
{
#ifdef _OPENMP
  bool flag = isDataOnDevice() && computeOnDevice;
  if (!flag)
    checkDataOnHost();
  //else
  //  set_data_location(Device); // non const array will be computed on device
  printKernel(flag, *this, kernel_name);
  return flag;
#else
  return false;
#endif
}

template<typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::checkDataOnDevice(std::string kernel_name)
{
#ifdef _OPENMP
  bool flag = isDataOnDevice() && computeOnDevice;
  if (!flag)
    checkDataOnHost();
  else
    set_data_location(DataLocation::Device); // non const array will be computed on device
  printKernel(flag, *this, kernel_name);
  return flag;
#else
  return false;
#endif
}

template<typename _TYPE_>
inline bool TRUSTArray<_TYPE_>::checkDataOnDevice(const TRUSTArray<_TYPE_>& tab_const, std::string kernel_name)
{
#ifdef _OPENMP
  bool flag = isDataOnDevice() && tab_const.isDataOnDevice() && computeOnDevice;
  // Si un des deux tableaux n'est pas a jour sur le device alors l'operation se fera sur le host:
  if (!flag)
    {
      this->checkDataOnHost();
      tab_const.checkDataOnHost();
    }
  else
    set_data_location(DataLocation::Device); // non const array will be computed on device
  printKernel(flag, *this, kernel_name);
  return flag;
#else
  return false;
#endif
}


#endif
