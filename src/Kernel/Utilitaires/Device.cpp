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
#include <stat_counters.h>

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
#include <omp.h>
// Set MPI processes to devices
void init_openmp()
{
  bool round_robin=false;
  int devID;
  int nDevs = omp_get_num_devices();
  int rank = Process::me();
  int nRanks = Process::nproc();
  if (round_robin)
    devID = rank % nDevs;
  else
    devID = AmgXWrapperScheduling(rank, nRanks, nDevs);
  if (Process::je_suis_maitre())
    {
      Cerr << "Initializing OpenMP offload on devices..."  << finl;
      Cerr << "Detecting " << nDevs << " devices." << finl;
    }
  cerr << "[OpenMP] Assigning rank " << rank << " to device " << devID << endl;
  omp_set_default_device(devID);
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
      int nDevs = 0;
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
inline std::string toString(const void* adr)
{
  std::stringstream ss;
  ss << adr;
  return ss.str();
}

// Timers pour OpenMP:
void start_timer(int size)
{
  clock_on = getenv ("TRUST_CLOCK_ON");
  if (clock_on!=NULL) clock_start = Statistiques::get_time_now();
  if (size==-1) statistiques().begin_count(gpu_kernel_counter_);
}
void end_timer(const std::string& str, int size) // Return in [ms]
{
  if (size==-1) statistiques().end_count(gpu_kernel_counter_);
  if (clock_on!=NULL) // Affichage
    {
      double ms = 1000 * (Statistiques::get_time_now() - clock_start);
      if (size==-1)
        printf("[clock] %7.3f ms [Kernel] %15s\n", ms, str.c_str());
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

// Copy const array on device if necessary
template <typename _TYPE_>
const _TYPE_* copyToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  // const array will matches on host and device
  const _TYPE_ *tab_addr = copyToDevice_(const_cast<TRUSTArray <_TYPE_>&>(tab), HostDevice, arrayName);
  return tab_addr;
}

template <typename _TYPE_>
_TYPE_* copyToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName)
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
  int size=0;
  start_timer(size);
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

// Copy non-const array on device if necessary for computation on device
template <typename _TYPE_>
_TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
  // non-const array will be modified on device:
  _TYPE_ *tab_addr = copyToDevice_(tab, Device, arrayName);
  return tab_addr;
}

// Copy non-const array to host from device
template <typename _TYPE_>
void copyFromDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName)
{
#ifdef _OPENMP
  _TYPE_* tab_addr = tab.addr();
  if (tab.get_dataLocation()==Device)
    {
      int size = sizeof(_TYPE_) * tab.size_array();
      start_timer(size);
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

// Partial copy of an array (from deb to fin element) from host to device
template <typename _TYPE_>
void copyPartialFromDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef _OPENMP
  if (tab.get_dataLocation()==Device)
    {
      int size = sizeof(_TYPE_) * (fin-deb);
      _TYPE_* tab_addr = tab.addr();
      start_timer(size);
      statistiques().begin_count(gpu_copyfromdevice_counter_, size);
      #pragma omp target update if (Objet_U::computeOnDevice) from(tab_addr[deb:fin-deb])
      statistiques().end_count(gpu_copyfromdevice_counter_, size);
      std::string message;
      message = "Partial update from device of array "+arrayName+" ["+toString(tab.addr())+"]";
      end_timer(message, size);
      if (clock_on) printf("\n");
      tab.set_dataLocation(PartialHostDevice);
    }
#endif
}

// Partial copy of an array (from deb to fin element) from host to device
template <typename _TYPE_>
void copyPartialToDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName)
{
#ifdef _OPENMP
  if (tab.get_dataLocation()==PartialHostDevice)
    {
      int size = sizeof(_TYPE_) * (fin-deb);
      _TYPE_* tab_addr = tab.addr();
      start_timer(size);
      statistiques().begin_count(gpu_copytodevice_counter_, size);
      #pragma omp target update if (Objet_U::computeOnDevice) to(tab_addr[deb:fin-deb])
      statistiques().end_count(gpu_copytodevice_counter_, size);
      std::string message;
      message = "Partial update to device of array "+arrayName+" ["+toString(tab.addr())+"]";
      end_timer(message, size);
      if (clock_on) printf("\n");
      tab.set_dataLocation(Device);
    }
#endif
}

void instantiate_template_functions()
{
  TRUSTArray<int> i;
  TRUSTArray<double> d;
  TRUSTArray<float> f;
  copyToDevice(i, "");
  copyToDevice(d, "");
  computeOnTheDevice(i, "");
  computeOnTheDevice(d, "");
  copyFromDevice(i, "");
  copyFromDevice(d, "");
  copyFromDevice(f, "");
  copyPartialFromDevice(i, 0, 0, "");
  copyPartialFromDevice(d, 0, 0, "");
  copyPartialToDevice(i, 0, 0, "");
  copyPartialToDevice(d, 0, 0, "");
}
