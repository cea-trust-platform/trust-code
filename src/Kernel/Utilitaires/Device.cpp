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

#ifdef _OPENMP
#include <omp.h>
#include <comm_incl.h>
void init_openmp()
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int device = rank % omp_get_num_devices();
  omp_set_default_device(device);
  if (Process::je_suis_maitre())
    {
      Cerr << "Initializing OpenMP offload on devices..."  << finl;
      Cerr << "Detecting " << omp_get_num_devices() << " devices." << finl;
      Cerr << "Assigning rank " << rank << " to device " << device << finl;
    }
  // Dummy target region, so as not to measure startup time later:
  #pragma omp target
  { ; }
}
#endif

// Teste intensivement les methodes de l'interface Device.h:
static bool self_tested_ = false;
bool self_test()
{
  if (!self_tested_)
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

        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
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

        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
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
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
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
        const DoubleTab& const_b = b;
        const DoubleTab& const_a = a;
        assert(const_b[5] == const_a[5]);
        assert(const_b[5] == 3);
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostOnly);
        assert(inco.get_dataLocation() == HostDevice);
      }
      assert(inco.get_dataLocation() == HostDevice);

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


