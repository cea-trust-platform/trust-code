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

#include <TRUSTTrav.h>
#include <Device.h>
#include <DeviceMemory.h>
#include <ctime>
#include <string>
#include <sstream>
#include <map>
#include <tuple>
#include <kokkos++.h>
#ifndef LATATOOLS
#include <comm_incl.h>
#include <Comm_Group_MPI.h>
#endif

bool init_device_ = false;
bool clock_on = false;
bool timer_on = true;
double clock_start;
int timer_counter=0;

std::string ptrToString(const void* adr)
{
  std::stringstream ss;
  ss << adr;
  return ss.str();
}

// Voir AmgXWrapper (src/init.cpp)
int AmgXWrapperScheduling(int rank, int nRanks, int nDevs)
{
  int devID;
  if (nRanks <= nDevs) // Less process than devices
    devID = rank;
  else // More processes than devices
    {
      int nBasic = nRanks / nDevs,
          nRemain = nRanks % nDevs;
      if (rank < (nBasic+1)*nRemain)
        devID = rank / (nBasic + 1);
      else
        devID = (rank - (nBasic+1)*nRemain) / nBasic + nRemain;
    }
  return devID;
}


#ifdef TRUST_USE_GPU
void init_device()
{
  if (init_device_) return;
  init_device_ = true;
  if (getenv("TRUST_CLOCK_ON")!= nullptr) clock_on = true;
  if (getenv("TRUST_DISABLE_TIMER")!= nullptr) timer_on = false;
  Process::imprimer_ram_totale(); // Impression avant copie des donnees sur GPU
}
#endif

#ifndef LATATOOLS
#ifdef TRUST_USE_CUDA
#include <cuda_runtime.h>
void init_cuda()
{
  // Necessaire sur JeanZay pour utiliser GPU Direct (http://www.idris.fr/jean-zay/gpu/jean-zay-gpu-mpi-cuda-aware-gpudirect.html)
  // mais performances moins bonnes (trust PAR_gpu_3D 2) donc desactive en attendant d'autres tests:
  // Absolument necessaire sur JeanZay (si OpenMPU-Cuda car sinon plantages lors des IO)
  // Voir: https://www.open-mpi.org/faq/?category=runcuda#mpi-cuda-aware-support pour activer ou non a la compilation !
#if defined(MPIX_CUDA_AWARE_SUPPORT) && MPIX_CUDA_AWARE_SUPPORT
  char* local_rank_env;
  cudaError_t cudaRet;
  /* Recuperation du rang local du processus via la variable d'environnement
     positionnee par Slurm, l'utilisation de MPI_Comm_rank n'etant pas encore
     possible puisque cette routine est utilisee AVANT l'initialisation de MPI */
  // ToDo pourrait etre appelee plus tard dans AmgX ou PETSc GPU...
  local_rank_env = getenv("SLURM_LOCALID");
  if (local_rank_env)
    {
      int rank = atoi(local_rank_env);
      int nRanks = atoi(getenv("SLURM_NTASKS"));
      if (rank==0) printf("The MPI library has CUDA-aware support and TRUST will try using this feature...\n");
      /* Definition du GPU a utiliser pour chaque processus MPI */
      True_int nDevs = 0;
      cudaGetDeviceCount(&nDevs);
      int devID = AmgXWrapperScheduling(rank, nRanks, nDevs);
      cudaRet = cudaSetDevice(devID);
      if(cudaRet != cudaSuccess)
        {
          printf("Error: cudaSetDevice failed\n");
          abort();
        }
      else
        {
          if (rank==0) printf("init_cuda() done!");
          cerr << "[MPI] Assigning rank " << rank << " to device " << devID << endl;
        }
    }
  else
    {
      printf("Error : can't guess the local rank of the task\n");
      abort();
    }
#endif     /* MPIX_CUDA_AWARE_SUPPORT */
}
#endif /* TRUST_USE_CUDA */
#endif /* LATATOOLS */

// Address on device (return host adress if no device):
template <typename _TYPE_>
_TYPE_* addrOnDevice(_TYPE_* ptr)
{
#ifdef TRUST_USE_GPU
  _TYPE_ *device_ptr = static_cast<_TYPE_*>(DeviceMemory::addrOnDevice(ptr));
  return device_ptr;
#else
  return ptr;
#endif
}
template <typename _TYPE_, typename _SIZE_>
_TYPE_* addrOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab)
{
#ifdef TRUST_USE_GPU
  if (tab.get_data_location()==DataLocation::HostOnly) return tab.data();
  else return addrOnDevice(tab.data());
#else
  return tab.data();
#endif
}

// Allocated ?
template <typename _TYPE_>
bool isAllocatedOnDevice(_TYPE_* tab_addr)
{
#ifdef TRUST_USE_GPU
  return DeviceMemory::isAllocatedOnDevice(tab_addr);
#else
  return false;
#endif
}

template <typename _TYPE_, typename _SIZE_>
bool isAllocatedOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab)
{
#ifdef TRUST_USE_GPU
  bool isAllocatedOnDevice1 = (tab.get_data_location() != DataLocation::HostOnly);
  bool isAllocatedOnDevice2 = isAllocatedOnDevice(tab.data());
  if (isAllocatedOnDevice1!=isAllocatedOnDevice2)
    Process::exit("isAllocatedOnDevice(TRUSTArray<_TYPE_>& tab) error! Seems tab.get_data_location() is not up-to-date !");
  return isAllocatedOnDevice2;
#else
  return false;
#endif
}

// Allocate on device:
template <typename _TYPE_, typename _SIZE_>
_TYPE_* allocateOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab, std::string arrayName)
{
  _TYPE_ *tab_addr = tab.data();
#ifdef TRUST_USE_GPU
  if (isAllocatedOnDevice(tab)) deleteOnDevice(tab);
  allocateOnDevice(tab_addr, tab.size_mem(), "an array "+arrayName);
  tab.set_data_location(DataLocation::Device);
#endif
  return tab_addr;
}

template <typename _TYPE_, typename _SIZE_>
_TYPE_* allocateOnDevice(_TYPE_* ptr, _SIZE_ size, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  assert(!isAllocatedOnDevice(ptr)); // Verifie que la zone n'est pas deja allouee
  clock_start = Statistiques::get_time_now();
  if (timer_on) statistiques().begin_count(gpu_mallocfree_counter_);
  size_t bytes = sizeof(_TYPE_) * size;
  size_t free_bytes  = DeviceMemory::deviceMemGetInfo(0);
  size_t total_bytes = DeviceMemory::deviceMemGetInfo(1);
  if (bytes>free_bytes)
    {
      Cerr << "Error ! Trying to allocate " << bytes << " bytes on GPU memory whereas only " << free_bytes << " bytes are available." << finl;
      Process::exit();
    }
  _TYPE_* device_ptr = static_cast<_TYPE_*>(Kokkos::kokkos_malloc(bytes));
  // Map host_ptr with device_ptr:
  DeviceMemory::add(ptr, device_ptr, size);
#ifndef NDEBUG
  const _TYPE_ INVALIDE_ = (std::is_same<_TYPE_,double>::value) ? DMAXFLOAT*0.999 : ( (std::is_same<_TYPE_,int>::value) ? INT_MIN : 0); // Identique a TRUSTArray<_TYPE_>::fill_default_value()
  Kokkos::View<_TYPE_*> ptr_v(device_ptr, size);
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), size, KOKKOS_LAMBDA(const int i)
  {
    ptr_v(i) = INVALIDE_;
  });
  end_gpu_timer(__KERNEL_NAME__);
#endif
  if (timer_on) statistiques().end_count(gpu_mallocfree_counter_);
  if (clock_on)
    {
      std::string clock(Process::is_parallel() ? "[clock]#"+std::to_string(Process::me()) : "[clock]  ");
      double ms = 1000 * (Statistiques::get_time_now() - clock_start);
      printf("%s %7.3f ms [Data]   Allocate %s on device [%9s] %6ld Bytes (%ld/%ldGB free) Currently allocated: %6ld\n", clock.c_str(), ms, arrayName.c_str(), ptrToString(ptr).c_str(), long(bytes), free_bytes/(1024*1024*1024), total_bytes/(1024*1024*1024), long(DeviceMemory::allocatedBytesOnDevice()));
    }
#endif
  return ptr;
}

// Delete on device:
template <typename _TYPE_, typename _SIZE_>
void deleteOnDevice(TRUSTArray<_TYPE_,_SIZE_>& tab)
{
#ifdef TRUST_USE_GPU
  _TYPE_ *tab_addr = tab.data();
  if (init_device_ && tab_addr && isAllocatedOnDevice(tab))
    {
      deleteOnDevice(tab_addr, tab.size_mem());
      tab.set_data_location(DataLocation::HostOnly);
    }
#endif
}

template <typename _TYPE_, typename _SIZE_>
void deleteOnDevice(_TYPE_* ptr, _SIZE_ size)
{
#ifdef TRUST_USE_GPU
  if (timer_on && statistiques_enabled()) statistiques().begin_count(gpu_mallocfree_counter_);
  std::string clock;
  if (PE_Groups::get_nb_groups()>0 && Process::is_parallel()) clock = "[clock]#"+std::to_string(Process::me());
  else
    clock = "[clock]  ";
  _SIZE_ bytes = sizeof(_TYPE_) * size;
  if (clock_on)
    cout << clock << "            [Data]   Delete on device array [" << ptrToString(ptr).c_str() << "] of " << bytes << " Bytes. It remains " << DeviceMemory::getMemoryMap().size()-1 << " arrays." << endl << flush;
  Kokkos::kokkos_free(addrOnDevice(ptr));
  DeviceMemory::del(ptr);
  if (timer_on && statistiques_enabled()) statistiques().end_count(gpu_mallocfree_counter_);
#endif
}

// Update const array on device if necessary
// Before		After		Copy ?
// HostOnly	    HostDevice	Yes
// Host		    HostDevice	Yes
// HostDevice	HostDevice	No
// Device		Device		No
template <typename _TYPE_, typename _SIZE_>
const _TYPE_* mapToDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab, std::string arrayName)
{
  // Update data on device if necessary
  DataLocation loc = tab.isDataOnDevice() ? tab.get_data_location() : DataLocation::HostDevice;
  const _TYPE_ *tab_addr = mapToDevice_(const_cast<TRUSTArray<_TYPE_,_SIZE_> &>(tab),
                                        loc, arrayName);
  return tab_addr;
}

template <typename _TYPE_, typename _SIZE_>
_TYPE_* mapToDevice_(TRUSTArray<_TYPE_,_SIZE_>& tab, DataLocation nextLocation, std::string arrayName)
{
  _TYPE_ *tab_addr = tab.data();
#ifdef TRUST_USE_GPU
  DataLocation currentLocation = tab.get_data_location();
  tab.set_data_location(nextLocation); // Important de specifier le nouveau status avant la recuperation du pointeur:
  // Important for ref_array/ref_tab support, we take the size of the memory allocated, not the size of the array (tab.size_array()):
  //int memory_size = tab.size_array();
  int memory_size = tab.size_mem();
  if (currentLocation==DataLocation::HostOnly)
    {
      // Not a Trav which is already allocated on device:
      if (!(tab.get_mem_storage() == STORAGE::TEMP_STORAGE && isAllocatedOnDevice(tab_addr)))
        allocateOnDevice(tab_addr, memory_size);
      copyToDevice(tab_addr, memory_size, "array "+arrayName);
    }
  else if (currentLocation==DataLocation::Host)
    {
      copyToDevice(tab_addr, memory_size, "array " + arrayName);
      if (DeviceMemory::warning(memory_size)) // Warning for large array only:
        ToDo_Kokkos("H2D update of large array! Add a breakpoint to find the reason.");
    }
  else if (currentLocation==DataLocation::PartialHostDevice)
    Process::exit("Error, can't map on device an array with PartialHostDevice status!");
#endif
  return tab_addr;
}

template <typename _TYPE_, typename _SIZE_>
void copyToDevice(_TYPE_* ptr, _SIZE_ size, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (size>0)
    {
      assert(isAllocatedOnDevice(ptr));
      _SIZE_ bytes = sizeof(_TYPE_) * size;
      start_gpu_timer("copyToDevice",bytes);
      if (timer_on) statistiques().begin_count(gpu_copytodevice_counter_);
      Kokkos::View<_TYPE_*> host_view(ptr, size);
      Kokkos::View<_TYPE_*> device_view(addrOnDevice(ptr), size);
      Kokkos::deep_copy(device_view, host_view);
      if (timer_on) statistiques().end_count(gpu_copytodevice_counter_, bytes);
      std::stringstream message;
      message << "Copy to device " << arrayName << " [" << ptrToString(ptr) << "]";
      end_gpu_timer(message.str(), bytes);
    }
#endif
}

// Copy non-const array on device if necessary for computation on device
template <typename _TYPE_, typename _SIZE_>
_TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_,_SIZE_>& tab, std::string arrayName)
{
  // non-const array will be modified on device:
  _TYPE_ *tab_addr = mapToDevice_(tab, DataLocation::Device, arrayName);
  return tab_addr;
}

// ToDo OpenMP: rename copy -> update or map ?
// Copy non-const array to host from device
template <typename _TYPE_, typename _SIZE_>
void copyFromDevice(TRUSTArray<_TYPE_,_SIZE_>& tab, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (tab.get_data_location() == DataLocation::Device)
    {
      copyFromDevice(tab.data(), tab.size_mem(), " array " + arrayName);
      tab.set_data_location(DataLocation::HostDevice);
    }
#endif
}
template <typename _TYPE_, typename _SIZE_>
void copyFromDevice(_TYPE_* ptr, _SIZE_ size, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (size>0)
    {
      assert(isAllocatedOnDevice(ptr));
      _SIZE_ bytes = sizeof(_TYPE_) * size;
      start_gpu_timer("copyFromDevice",bytes);
      if (timer_on) statistiques().begin_count(gpu_copyfromdevice_counter_);
      Kokkos::View<_TYPE_*> host_view(ptr, size);
      Kokkos::View<_TYPE_*> device_view(addrOnDevice(ptr), size);
      Kokkos::deep_copy(host_view, device_view);
      if (timer_on) statistiques().end_count(gpu_copyfromdevice_counter_, bytes);
      std::stringstream message;
      message << "Copy from device" << arrayName << " [" << ptrToString(ptr) << "] " << size << " items ";
      end_gpu_timer(message.str(), bytes);
      if (clock_on) printf("\n");
      if (DeviceMemory::warning(size)) // Warning for large array only:
        ToDo_Kokkos("D2H update of large array! Add a breakpoint to find the reason if not IO.");
    }
#endif
}

// Copy const array to host from device
template <typename _TYPE_, typename _SIZE_>
void copyFromDevice(const TRUSTArray<_TYPE_,_SIZE_>& tab, std::string arrayName)
{
  copyFromDevice(const_cast<TRUSTArray<_TYPE_,_SIZE_>&>(tab), arrayName);
}

// Partial copy of an array (from deb to fin element) from host to device
// Typical example: Deal with boundary condition (small loop) on the host
// copyPartialFromDevice(resu, 0, premiere_face_int);   // Faces de bord
// copyPartialFromDevice(resu, nb_faces, nb_faces_tot); // Pour ajouter les faces de bord virtuelles si necessaire
template <typename _TYPE_>
void copyPartialFromDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (tab.get_data_location()==DataLocation::Device || tab.get_data_location()==DataLocation::PartialHostDevice)
    {
      int bytes = sizeof(_TYPE_) * (fin-deb);
      _TYPE_ *tab_addr = tab.data();
      start_gpu_timer("copyPartialFromDevice",bytes);
      if (timer_on) statistiques().begin_count(gpu_copyfromdevice_counter_);
      Kokkos::View<_TYPE_*> host_view(tab_addr+deb, fin-deb);
      Kokkos::View<_TYPE_*> device_view(addrOnDevice(tab_addr)+deb, fin-deb);
      Kokkos::deep_copy(host_view, device_view);
      if (timer_on) statistiques().end_count(gpu_copyfromdevice_counter_, bytes);
      std::string message;
      message = "Partial update from device of array "+arrayName+" ["+ptrToString(tab_addr)+"]";
      end_gpu_timer(message, bytes);
      tab.set_data_location(DataLocation::PartialHostDevice);
    }
#endif
}

// Partial copy of an array (from deb to fin element) from host to device
template <typename _TYPE_>
void copyPartialToDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (tab.get_data_location()==DataLocation::PartialHostDevice)
    {
      int bytes = sizeof(_TYPE_) * (fin-deb);
      _TYPE_ *tab_addr = tab.data();
      start_gpu_timer("copyPartialToDevice",bytes);
      if (timer_on) statistiques().begin_count(gpu_copytodevice_counter_);
      Kokkos::View<_TYPE_*> host_view(tab_addr+deb, fin-deb);
      Kokkos::View<_TYPE_*> device_view(addrOnDevice(tab_addr)+deb, fin-deb);
      Kokkos::deep_copy(device_view, host_view);
      if (timer_on) statistiques().end_count(gpu_copytodevice_counter_, bytes);
      std::string message;
      message = "Partial update to device of array "+arrayName+" ["+ptrToString(tab_addr)+"]";
      end_gpu_timer(message, bytes);
      tab.set_data_location(DataLocation::Device);
    }
#endif
}

template <typename _TYPE_>
void copyPartialToDevice(const TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef TRUST_USE_GPU
  if (tab.get_data_location()==DataLocation::PartialHostDevice)
    {
      // ToDo OpenMP par de recopie car si le tableau est const il n'a ete modifie sur le host
      const_cast<TRUSTArray<_TYPE_>&>(tab).set_data_location(DataLocation::Device);
    }
#endif
}

//
//  Explicit template instanciations
//
template char* addrOnDevice<char>(char* ptr);
template double* addrOnDevice<double>(TRUSTArray<double>& tab);
template int* addrOnDevice<int>(TRUSTArray<int>& tab);
template float* addrOnDevice<float>(TRUSTArray<float>& tab);

template bool isAllocatedOnDevice<double>(double* tab_addr);
template bool isAllocatedOnDevice<int>(int* tab_addr);
template bool isAllocatedOnDevice<float>(float* tab_addr);
template bool isAllocatedOnDevice<double>(TRUSTArray<double>& tab);
template bool isAllocatedOnDevice<int>(TRUSTArray<int>& tab);
template bool isAllocatedOnDevice<float>(TRUSTArray<float>& tab);

template double* allocateOnDevice<double>(TRUSTArray<double>& tab, std::string arrayName);
template int* allocateOnDevice<int>(TRUSTArray<int>& tab, std::string arrayName);
template float* allocateOnDevice<float>(TRUSTArray<float>& tab, std::string arrayName);
template char* allocateOnDevice<char>(char* ptr, int size, std::string arrayName);

template const double* allocateOnDevice<double>(const TRUSTArray<double>& tab, std::string arrayName);
template const int* allocateOnDevice<int>(const TRUSTArray<int>& tab, std::string arrayName);
template const float* allocateOnDevice<float>(const TRUSTArray<float>& tab, std::string arrayName);

template void deleteOnDevice<double>(TRUSTArray<double>& tab);
template void deleteOnDevice<int>(TRUSTArray<int>& tab);
template void deleteOnDevice<float>(TRUSTArray<float>& tab);
template void deleteOnDevice<char>(char* ptr, int size);
template void deleteOnDevice<int>(int* ptr, int size);
template void deleteOnDevice<float>(float* ptr, int size);
template void deleteOnDevice<double>(double* ptr, int size);

template const double* mapToDevice<double>(const TRUSTArray<double>& tab, std::string arrayName);
template const int* mapToDevice<int>(const TRUSTArray<int>& tab, std::string arrayName);
template const float* mapToDevice<float>(const TRUSTArray<float>& tab, std::string arrayName);
template void copyToDevice<char>(char* ptr, int size, std::string arrayName);

template double* mapToDevice_<double>(TRUSTArray<double>& tab, DataLocation nextLocation, std::string arrayName);
template int* mapToDevice_<int>(TRUSTArray<int>& tab, DataLocation nextLocation, std::string arrayName);
template float* mapToDevice_<float>(TRUSTArray<float>& tab, DataLocation nextLocation, std::string arrayName);

template double* computeOnTheDevice<double>(TRUSTArray<double>& tab, std::string arrayName);
template int* computeOnTheDevice<int>(TRUSTArray<int>& tab, std::string arrayName);
template float* computeOnTheDevice<float>(TRUSTArray<float>& tab, std::string arrayName);

template void copyFromDevice<double>(TRUSTArray<double>& tab, std::string arrayName);
template void copyFromDevice<int>(TRUSTArray<int>& tab, std::string arrayName);
template void copyFromDevice<float>(TRUSTArray<float>& tab, std::string arrayName);
template void copyFromDevice<char>(char* ptr, int size, std::string arrayName);

template void copyFromDevice<double>(const TRUSTArray<double>& tab, std::string arrayName);
template void copyFromDevice<int>(const TRUSTArray<int>& tab, std::string arrayName);
template void copyFromDevice<float>(const TRUSTArray<float>& tab, std::string arrayName);

template void copyPartialFromDevice<double>(TRUSTArray<double>& tab, int deb, int fin, std::string arrayName);
template void copyPartialFromDevice<int>(TRUSTArray<int>& tab, int deb, int fin, std::string arrayName);
template void copyPartialFromDevice<float>(TRUSTArray<float>& tab, int deb, int fin, std::string arrayName);

template void copyPartialToDevice<double>(TRUSTArray<double>& tab, int deb, int fin, std::string arrayName);
template void copyPartialToDevice<int>(TRUSTArray<int>& tab, int deb, int fin, std::string arrayName);
template void copyPartialToDevice<float>(TRUSTArray<float>& tab, int deb, int fin, std::string arrayName);

template void copyPartialToDevice<double>(const TRUSTArray<double>& tab, int deb, int fin, std::string arrayName);
template void copyPartialToDevice<int>(const TRUSTArray<int>& tab, int deb, int fin, std::string arrayName);
template void copyPartialToDevice<float>(const TRUSTArray<float>& tab, int deb, int fin, std::string arrayName);

#if INT_is_64_ == 2

// The ones needed for 64 bits:
template double* addrOnDevice<double>(TRUSTArray<double,trustIdType>& tab);
template int* addrOnDevice<int>(TRUSTArray<int,trustIdType>& tab);
template trustIdType* addrOnDevice<trustIdType>(TRUSTArray<trustIdType,trustIdType>& tab);
template trustIdType* addrOnDevice<trustIdType>(TRUSTArray<trustIdType,int>& tab);
template float* addrOnDevice<float>(TRUSTArray<float,trustIdType>& tab);

template bool isAllocatedOnDevice<trustIdType>(trustIdType* tab_addr);
template bool isAllocatedOnDevice<double>(TRUSTArray<double,trustIdType>& tab);
template bool isAllocatedOnDevice<float>(TRUSTArray<float,trustIdType>& tab);
template bool isAllocatedOnDevice<int>(TRUSTArray<int,trustIdType>& tab);
template bool isAllocatedOnDevice<trustIdType>(TRUSTArray<trustIdType,trustIdType>& tab);
template bool isAllocatedOnDevice<trustIdType>(TRUSTArray<trustIdType,int>& tab);

template double* allocateOnDevice<double>(TRUSTArray<double,trustIdType>& tab, std::string arrayName);
template int* allocateOnDevice<int>(TRUSTArray<int,trustIdType>& tab, std::string arrayName);
template trustIdType* allocateOnDevice<trustIdType>(TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template trustIdType* allocateOnDevice<trustIdType>(TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template float* allocateOnDevice<float>(TRUSTArray<float,trustIdType>& tab, std::string arrayName);

template const double* allocateOnDevice<double>(const TRUSTArray<double,trustIdType>& tab, std::string arrayName);
template const int* allocateOnDevice<int>(const TRUSTArray<int,trustIdType>& tab, std::string arrayName);
template const trustIdType* allocateOnDevice<trustIdType>(const TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template const trustIdType* allocateOnDevice<trustIdType>(const TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template const float* allocateOnDevice<float>(const TRUSTArray<float,trustIdType>& tab, std::string arrayName);

template void deleteOnDevice<double>(TRUSTArray<double,trustIdType>& tab);
template void deleteOnDevice<int>(TRUSTArray<int,trustIdType>& tab);
template void deleteOnDevice<trustIdType>(TRUSTArray<trustIdType,trustIdType>& tab);
template void deleteOnDevice<trustIdType>(TRUSTArray<trustIdType,int>& tab);
template void deleteOnDevice<float>(TRUSTArray<float,trustIdType>& tab);

template void deleteOnDevice<int>(int* ptr, long size);
template void deleteOnDevice<int>(int* ptr, long long size);
template void deleteOnDevice<trustIdType>(trustIdType* ptr, long size);
template void deleteOnDevice<trustIdType>(trustIdType* ptr, long long size);
template void deleteOnDevice<trustIdType>(trustIdType* ptr, int size);
template void deleteOnDevice<float>(float* ptr, long size);
template void deleteOnDevice<double>(double* ptr, long size);
template void deleteOnDevice<float>(float* ptr, long long size);
template void deleteOnDevice<double>(double* ptr, long long size);

template const double* mapToDevice<double>(const TRUSTArray<double,trustIdType>& tab, std::string arrayName);
template const int* mapToDevice<int>(const TRUSTArray<int,trustIdType>& tab, std::string arrayName);
template const trustIdType* mapToDevice<trustIdType>(const TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template const trustIdType* mapToDevice<trustIdType>(const TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template const float* mapToDevice<float>(const TRUSTArray<float,trustIdType>& tab, std::string arrayName);

template int* computeOnTheDevice(TRUSTArray<int,trustIdType>& tab, std::string arrayName, bool enabled);
template trustIdType* computeOnTheDevice(TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template trustIdType* computeOnTheDevice(TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template float* computeOnTheDevice(TRUSTArray<float,trustIdType>& tab, std::string arrayName);
template double* computeOnTheDevice(TRUSTArray<double,trustIdType>& tab, std::string arrayName);

template void copyFromDevice<int, trustIdType>(TRUSTArray<int,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<trustIdType, int>(TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template void copyFromDevice<trustIdType, trustIdType>(TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<float, trustIdType>(TRUSTArray<float,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<double, trustIdType>(TRUSTArray<double,trustIdType>& tab, std::string arrayName);

// With const:
template void copyFromDevice<int, trustIdType>(const TRUSTArray<int,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<trustIdType, int>(const TRUSTArray<trustIdType,int>& tab, std::string arrayName);
template void copyFromDevice<trustIdType, trustIdType>(const TRUSTArray<trustIdType,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<float, trustIdType>(const TRUSTArray<float,trustIdType>& tab, std::string arrayName);
template void copyFromDevice<double, trustIdType>(const TRUSTArray<double,trustIdType>& tab, std::string arrayName);


#endif

