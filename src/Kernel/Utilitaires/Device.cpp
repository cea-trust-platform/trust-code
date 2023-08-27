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

#include <TRUSTTab.h>
#include <TRUSTTrav.h>
#include <TRUSTArray.h>
#include <Array_base.h>
#include <Device.h>
#include <ctime>
#include <string>
#include <sstream>
#include <comm_incl.h>
#include <Comm_Group_MPI.h>

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
#ifdef _OPENMP
// Set MPI processes to devices
void init_openmp()
{
  // ToDo: OMP_TARGET_OFFLOAD=DISABLED equivaut a TRUST_DISABLE_DEVICE=1
  // donc peut etre supprimer cette derniere variable (qui disable aussi rocALUTION sur GPU dans le code mais pas AmgX encore)...
  // https://www.openmp.org/spec-html/5.0/openmpse65.html
  if (init_openmp_ || getenv("TRUST_DISABLE_DEVICE")!=NULL)
    return;
  char const* var = getenv("OMP_TARGET_OFFLOAD");
  if (var!=NULL && std::string(var)=="DISABLED")
    return;
  init_openmp_ = true;
  if (getenv("TRUST_CLOCK_ON")!= NULL) clock_on = true;
#ifdef MPI_
  MPI_Comm localWorld;
  MPI_Comm globalWorld;
  if (sub_type(Comm_Group_MPI,PE_Groups::current_group()))
    globalWorld = ref_cast(Comm_Group_MPI,PE_Groups::current_group()).get_mpi_comm();
  else
    globalWorld = MPI_COMM_WORLD;
  MPI_Comm_split_type(globalWorld, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &localWorld);
  True_int rank; // Local rank
  MPI_Comm_rank(localWorld, &rank);
  True_int nRanks; // Local number of ranks
  MPI_Comm_size(localWorld, &nRanks);
  // Node name:
  True_int len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(name, &len);
  std::string nodeName = name;
  int nDevs = omp_get_num_devices(); // Local number of devices
  if (nDevs==0)
    {
      Cerr << "Error, no device detected during OpenMP initialization." << finl;
      Process::exit();
    }
  int devID = AmgXWrapperScheduling(rank, nRanks, nDevs);
  Cerr << "Initializing OpenMP offload on devices..."  << finl;
  cerr << "[OpenMP] Assigning local rank " << rank << " (global rank " << Process::me() << ") of node " << nodeName.c_str() << " to its device " << devID << "/" << nDevs-1 << endl;
  omp_set_default_device(devID);
#endif
  // Dummy target region, so as not to measure startup time later:
  #pragma omp target
  { ; }
}
#endif

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
#endif

#include <sstream>
std::string toString(const void* adr)
{
  std::stringstream ss;
  ss << adr;
  return ss.str();
}

// Allocate on device:
template <typename _TYPE_>
_TYPE_* allocateOnDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  _TYPE_* tab_addr = tab.addrForDevice();
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      if (isAllocatedOnDevice(tab)) deleteOnDevice(tab);
      allocateOnDevice(tab_addr, tab.size_array(), "an array "+arrayName);
      tab.set_dataLocation(Device);
    }
#endif
  return tab_addr;
}

template <typename _TYPE_>
_TYPE_* allocateOnDevice(_TYPE_* ptr, int size, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      assert(!isAllocatedOnDevice(ptr)); // Verifie que la zone n'est pas deja allouee
      clock_start = Statistiques::get_time_now();
      int bytes = sizeof(_TYPE_) * size;
      size_t free_bytes=0, total_bytes=0;
      // ToDo OpenMP use hipMemGetInfo on AMD...
#ifdef TRUST_USE_CUDA
      cudaMemGetInfo(&free_bytes, &total_bytes);
      if (bytes>free_bytes)
        {
          Cerr << "Error ! Trying to allocate " << bytes << " bytes on GPU memory whereas only " << free_bytes << " bytes are available." << finl;
          Process::exit();
        }
#endif
      #pragma omp target enter data map(alloc:ptr[0:size])
      if (clock_on)
        {
          std::string clock(Process::nproc()>1 ? "[clock]#"+std::to_string(Process::me()) : "[clock]  ");
          double ms = 1000 * (Statistiques::get_time_now() - clock_start);
          printf("%s %7.3f ms [Data]   Allocate %s on device [%9s] %6ld Bytes (%ld/%ldGB free)\n", clock.c_str(), ms, arrayName.c_str(), toString(ptr).c_str(), long(bytes), free_bytes/(1024*1024*1024), total_bytes/(1024*1024*1024));
        }
    }
#endif
  return ptr;
}

// Delete on device:
template <typename _TYPE_>
void deleteOnDevice(TRUSTArray<_TYPE_>& tab)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      _TYPE_ * tab_addr = tab.addrForDevice();
      if (init_openmp_ && tab_addr && isAllocatedOnDevice(tab))
        {
          deleteOnDevice(tab_addr, tab.size_array());
          tab.set_dataLocation(HostOnly);
        }
    }
#endif
}

template <typename _TYPE_>
void deleteOnDevice(_TYPE_* ptr, int size)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      std::string clock;
      if (PE_Groups::get_nb_groups()>0 && Process::nproc()>1) clock = "[clock]#"+std::to_string(Process::me());
      else
        clock = "[clock]  ";
      int bytes = sizeof(_TYPE_) * size;
      if (clock_on)
        cout << clock << "            [Data]   Delete on device array [" << toString(ptr).c_str() << "] of " << bytes << " Bytes" << endl << flush;
      #pragma omp target exit data map(delete:ptr[0:size])
    }
#endif
}

// Update const array on device if necessary
// Before		After		Copy ?
// HostOnly	    HostDevice	Yes
// Host		    HostDevice	Yes
// HostDevice	HostDevice	No
// Device		Device		No
template <typename _TYPE_>
const _TYPE_* mapToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  // Update data on device if necessary
  const _TYPE_ *tab_addr = mapToDevice_(const_cast<TRUSTArray <_TYPE_>&>(tab), tab.isDataOnDevice() ? tab.get_dataLocation() : HostDevice, arrayName);
  return tab_addr;
}

template <typename _TYPE_>
_TYPE_* mapToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName)
{
  _TYPE_* tab_addr = tab.addrForDevice();
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      init_openmp();
#ifndef NDEBUG
      if (self_test()) self_test();
#endif
      DataLocation currentLocation = tab.get_dataLocation();
      tab.set_dataLocation(nextLocation); // Important de specifier le nouveau status avant la recuperation du pointeur:
      if (currentLocation==HostOnly)
        {
          allocateOnDevice(tab_addr, tab.size_array());
          copyToDevice(tab_addr, tab.size_array(), "array "+arrayName);
        }
      else if (currentLocation==Host)
        copyToDevice(tab_addr, tab.size_array(), "array "+arrayName);
      else if (currentLocation==PartialHostDevice)
        Process::exit("Error, can't map on device an array with PartialHostDevice status!");
    }
#endif
  return tab_addr;
}

template <typename _TYPE_>
void copyToDevice(_TYPE_* ptr, int size, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      assert(isAllocatedOnDevice(ptr) || size==0);
      int bytes = sizeof(_TYPE_) * size;
      start_timer(bytes);
      statistiques().begin_count(gpu_copytodevice_counter_);
      #pragma omp target update to(ptr[0:size])
      statistiques().end_count(gpu_copytodevice_counter_, bytes);
      std::stringstream message;
      message << "Copy to device " << arrayName << " [" << toString(ptr) << "]";
      end_timer(Objet_U::computeOnDevice, message.str(), bytes);
    }
#endif
}

// Copy non-const array on device if necessary for computation on device
template <typename _TYPE_>
_TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  // non-const array will be modified on device:
  _TYPE_ *tab_addr = mapToDevice_(tab, Device, arrayName);
  return tab_addr;
}

// ToDo OpenMP: rename copy -> update or map ?
// Copy non-const array to host from device
template <typename _TYPE_>
void copyFromDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice && tab.get_dataLocation()==Device)
    {
      copyFromDevice(tab.addrForDevice(), tab.size_array(), " array "+arrayName);
      tab.set_dataLocation(HostDevice);
    }
#endif
}
template <typename _TYPE_>
void copyFromDevice(_TYPE_* ptr, int size, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      assert(isAllocatedOnDevice(ptr));
      int bytes = sizeof(_TYPE_) * size;
      start_timer(bytes);
      statistiques().begin_count(gpu_copyfromdevice_counter_);
      #pragma omp target update from(ptr[0:size])
      statistiques().end_count(gpu_copyfromdevice_counter_, bytes);
      std::stringstream message;
      message << "Copy from device" << arrayName << " [" << toString(ptr) << "]";
      end_timer(Objet_U::computeOnDevice, message.str(), bytes);
      if (clock_on) printf("\n");
    }
#endif
}

// Copy const array to host from device
template <typename _TYPE_>
void copyFromDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  copyFromDevice(const_cast<TRUSTArray<_TYPE_>&>(tab), arrayName);
}

// Partial copy of an array (from deb to fin element) from host to device
// Typical example: Deal with boundary condition (small loop) on the host
// copyPartialFromDevice(resu, 0, premiere_face_int);   // Faces de bord
// copyPartialFromDevice(resu, nb_faces, nb_faces_tot); // Pour ajouter les faces de bord virtuelles si necessaire
template <typename _TYPE_>
void copyPartialFromDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice)
    {
      if (tab.get_dataLocation()==Device || tab.get_dataLocation()==PartialHostDevice)
        {
          int bytes = sizeof(_TYPE_) * (fin-deb);
          _TYPE_* tab_addr = tab.addrForDevice();
          start_timer(bytes);
          statistiques().begin_count(gpu_copyfromdevice_counter_);
          #pragma omp target update from(tab_addr[deb:fin-deb])
          statistiques().end_count(gpu_copyfromdevice_counter_, bytes);
          std::string message;
          message = "Partial update from device of array "+arrayName+" ["+toString(tab_addr)+"]";
          end_timer(Objet_U::computeOnDevice, message, bytes);
          tab.set_dataLocation(PartialHostDevice);
        }
    }
#endif
}

// Partial copy of an array (from deb to fin element) from host to device
template <typename _TYPE_>
void copyPartialToDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice && tab.get_dataLocation()==PartialHostDevice)
    {
      int bytes = sizeof(_TYPE_) * (fin-deb);
      _TYPE_* tab_addr = tab.addrForDevice();
      start_timer(bytes);
      statistiques().begin_count(gpu_copytodevice_counter_);
      #pragma omp target update to(tab_addr[deb:fin-deb])
      statistiques().end_count(gpu_copytodevice_counter_, bytes);
      std::string message;
      message = "Partial update to device of array "+arrayName+" ["+toString(tab_addr)+"]";
      end_timer(Objet_U::computeOnDevice, message, bytes);
      tab.set_dataLocation(Device);
    }
#endif
}

template <typename _TYPE_>
void copyPartialToDevice(const TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef _OPENMP
  if (Objet_U::computeOnDevice && tab.get_dataLocation()==PartialHostDevice)
    {
      // ToDo OpenMP par de recopie car si le tableau est const il n'a ete modifie sur le host
      const_cast<TRUSTArray<_TYPE_>&>(tab).set_dataLocation(Device);
    }
#endif
}

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

