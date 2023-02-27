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
#include <Device.h>
#include <string>
#include <sstream>
#include <comm_incl.h>

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

// Teste intensivement les methodes de l'interface Device.h:
static bool self_tested_ = false;
bool self_test()
{
  if (!self_tested_ && Objet_U::computeOnDevice)
    {
      self_tested_ = true;
      int N = 10;
      // Teste les methodes d'acces sur le device:
      DoubleTab inco(N);
      inco = 1;
      copyToDevice(inco, "inco"); // copy
      assert(inco.get_dataLocation() == HostDevice);
      assert(inco.ref_count() == 1);
      {
        // Exemple 1er operateur
        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco
        assert(a.get_dataLocation() == HostDevice);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        DoubleTab b(N);

        const double *a_addr = copyToDevice(a, "a"); // up-to-date
        double *b_addr = b.addr();
        #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice) map(tofrom:b_addr[0:b.size_array()])
        for (int i = 0; i < N; i++)
          b_addr[i] = a_addr[i];

#ifndef NDEBUG
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
#endif
        assert(const_b[5] == const_a[5]);
        assert(const_b[5] == 1);
        //assert(b[5] == a[5]); // Argh double& TRUSTArray<double>::operator[](int i) appele pour a et donc repasse sur host
        // Comment detecter que operator[](int i) est utilise en read ou write ? Possible ? Non, sauf si const utilise.
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostOnly);
        assert(inco.get_dataLocation() == HostDevice);
      }
      assert(inco.get_dataLocation() == HostDevice);
      assert(inco.ref_count() == 1);
      {
        // Exemple 2eme operateur
        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco qui est toujours HostDevice
        assert(a.get_dataLocation() == HostDevice);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        const double *a_addr = copyToDevice(a, "a"); // up-to-date

        DoubleTab b(N);
        double *b_addr = b.addr();
        #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice) map(tofrom:b_addr[0:b.size_array()])
        for (int i = 0; i < N; i++)
          b_addr[i] = a_addr[i];

#ifndef NDEBUG
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
#endif
        assert(const_b[5] == const_a[5]);
        assert(const_b[5] == 1);
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostOnly);
      }
      assert(inco.ref_count() == 1);

      // Mise a jour de l'inconnue sur le host:
      inco += 1;
      assert(inco.get_dataLocation() == Host);
      assert(inco.ref_count() == 1);
      {
        // Pas de temps suivant, nouvel operateur:
        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco
        assert(a.get_dataLocation() == Host);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        const double *a_addr = copyToDevice(a, "a"); // update

        DoubleTab b(N);
        double *b_addr = b.addr();
        #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice) map(tofrom:b_addr[0:b.size_array()])
        for (int i = 0; i < N; i++)
          b_addr[i] = a_addr[i];
#ifndef NDEBUG
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
#endif
        assert(const_b[5] == const_a[5]);
        assert(const_b[5] == 2);
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostOnly);
        assert(inco.get_dataLocation() == HostDevice); // Car a ref sur inco
      }
      assert(inco.get_dataLocation() == HostDevice);
      // Mise a jour de l'inconnue sur le host et mise a jour sur le device:
      {
        inco += 1;
        copyToDevice(inco, "inco"); // update
        assert(inco.get_dataLocation() == HostDevice);

        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco
        assert(a.get_dataLocation() == HostDevice);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        const double *a_addr = copyToDevice(a, "a"); // up-to-date

        DoubleTab b(N);
        double *b_addr = b.addr();
        #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice) map(tofrom:b_addr[0:b.size_array()])
        for (int i = 0; i < N; i++)
          b_addr[i] = a_addr[i];
#ifndef NDEBUG
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
#endif
        assert(const_b[5] == const_a[5]);
        assert(const_b[5] == 3);
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostOnly);
        assert(inco.get_dataLocation() == HostDevice);
      }
      assert(inco.get_dataLocation() == HostDevice);

      // Test d'operations ArrOfDouble sur GPU
      {
        ArrOfDouble a(10), b(10);
        a=1;
        b=2;
        copyToDevice(a);
        copyToDevice(b);
        b+=a; // TRUSTArray& operator+=(const TRUSTArray& y) sur le device
        b+=3; // TRUSTArray& operator+=(const _TYPE_ dy)
        b-=2; // TRUSTArray& operator-=(const _TYPE_ dy)
        b-=a; // TRUSTArray& operator-=(const TRUSTArray& y)
        // ToDo regler: Multiple definition of 'nvkernel__ZN10TRUST
        //b*=10; // TRUSTArray& operator*= (const _TYPE_ dy)
        //b/=2;  // TRUSTArray& operator/= (const _TYPE_ dy)
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == Device);
#ifndef NDEBUG
        const ArrOfDouble& const_b = b;
#endif
        // Operations sur le device:
        // Retour sur le host pour verifier le resultat
        copyFromDevice(b);
        assert(const_b[0] == 3);
      }

      // Test de copyPartialFromDevice/copyPartialToDevice
      {
        ArrOfDouble a(4);
#ifndef NDEBUG
        const ArrOfDouble& const_a = a;
#endif
        a=0;
        copyToDevice(a);
        a+=1; // Done on device
        copyPartialFromDevice(a, 1, 3);
        assert(a.get_dataLocation() == Host);
        assert(const_a[0]==0);
        assert(const_a[1]==1);
        assert(const_a[2]==1);
        assert(const_a[3]==0);
        a[1]=2; // Done on host
        a[2]=2; // Done on host
        copyPartialToDevice(a, 1, 3);
        assert(a.get_dataLocation() == Device);
        copyFromDevice(a);
        assert(const_a[0]==1);
        assert(const_a[1]==2);
        assert(const_a[2]==2);
        assert(const_a[3]==1);
      }
      // ToDo: porter operator_vect_single_generic sur GPU !!! DoubleTab a; a+=1;
      {
        /*
        DoubleTab a(10), b(10);
        a=1;
        b=2;
        copyToDevice(a);
        copyToDevice(b);
        b=a;  // operator_vect_vect_generic(IS_EGAL_)
        b+=a; // operator_vect_vect_generic(ADD_)
        b-=a; // operator_vect_vect_generic(SUB_)
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == Device);
        #ifndef NDEBUG
        const ArrOfDouble& const_b = b;
        #endif
        // Operations sur le device:
        // Retour sur le host pour verifier le resultat
        copyFromDevice(b);
        assert(const_b[0] == 3);
         */
      }
      // ToDo: Ameliorer DoubleTrav:
      {
        DoubleTrav a(N);
        a = 1;
        copyToDevice(a, "a"); // copy
        assert(a.get_dataLocation() == HostDevice);
        assert(a.ref_count() == 1);
      }
      {
        DoubleTrav b(N);
        assert(b.get_dataLocation() == HostOnly); // ToDo: devrait etre en Host (recupere la zone memoire precedente)
        b = 2;
        assert(b.get_dataLocation() == HostOnly);
        copyToDevice(b, "b"); // copy ToDo cela devrait etre un update
      }
    }
  return self_tested_;
}


