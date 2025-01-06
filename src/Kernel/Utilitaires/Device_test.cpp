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
#include <TRUSTTab_parts.h>
#include <Device.h>
#include <string>
#include <sstream>
#include <comm_incl.h>
#include <DeviceMemory.h>

// Device_test: teste intensivement les methodes de l'interface Device.h:
bool self_tested_ = false;
void self_test()
{
  if (self_tested_)
    return;
  else
    self_tested_ = true;
  {
    // local_operations_vect_bis_generic
    // Cas test unitaire critique:
    {
      DoubleTab a(2);
      a(0) = 1;
      a(1) = 2;
      mapToDevice(a, "a");
      double mp = mp_norme_vect(a);
      double sol = sqrt(Process::nproc() * 5);
      if (mp != sol)
        {
          Cerr << "What! " << mp << " != " << sol << finl;
          Cerr
              << "Weird bug seen during a sum reduction with Kokkos::parallel_reduce for TRUST production build on Cuda 11.6. Fixed with Cuda 12.1 (or before...)."
              << finl;
          Cerr << "Update Cuda please !" << finl;
          Process::exit();
        }
    }
  }
#ifndef NDEBUG
  // Test mapToDevice(arr)
  // Status
  // Status before		Status after		Copy ?
  // DataLocation::HostOnly	        DataLocation::HostDevice	        Yes
  // Host		        DataLocation::HostDevice	        Yes
  // DataLocation::HostDevice	        DataLocation::HostDevice	        No
  // Device		        Device		        No
  {
    DoubleTab a(10);
    assert(a.get_data_location() == DataLocation::HostOnly);
    mapToDevice(a);
    assert(a.get_data_location() == DataLocation::HostDevice);
    a[1]=0;
    assert(a.get_data_location() == DataLocation::Host);
    mapToDevice(a);
    assert(a.get_data_location() == DataLocation::HostDevice);
  }

  {
    DoubleTab a(10);
    a = 23;
    assert(a.get_data_location() == DataLocation::HostOnly);
    {
      DoubleTab b;
      b.ref(a);
      mapToDevice(b);
      assert(b.get_data_location() == DataLocation::HostDevice);
      assert(a.get_data_location() == DataLocation::HostDevice);
    }
  }

  int N = 10;
  // Teste les methodes d'acces sur le device:
  DoubleTab inco(N);
  inco = 1;
  mapToDevice(inco, "inco"); // copy
  assert(inco.get_data_location() == DataLocation::HostDevice);
  assert(inco.ref_count() == 1);
  {
    // Exemple 1er operateur
    DoubleTab a;
    a.ref(inco); // Doit prendre l'etat de inco
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(a.ref_count() == 2);
    assert(inco.ref_count() == 2);
    DoubleTab b(N);

    CDoubleArrView a_v = static_cast<const ArrOfDouble&>(a).view_ro();
    DoubleArrView b_v = static_cast<ArrOfDouble&>(b).view_wo();
    Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int i)
    {
      b_v[i] = a_v[i];
    });
    Kokkos::fence();

    const DoubleTab& const_b = b;
    const DoubleTab& const_a = a;
    assert(const_b[5] == const_a[5]);
    assert(const_b[5] == 1);
    //assert(b[5] == a[5]); // Argh double& TRUSTArray<double>::operator[](int i) appele pour a et donc repasse sur host
    // Comment detecter que operator[](int i) est utilise en read ou write ? Possible ? Non, sauf si const utilise.
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::HostDevice);
    assert(inco.get_data_location() == DataLocation::HostDevice);
  }
  assert(inco.get_data_location() == DataLocation::HostDevice);
  assert(inco.ref_count() == 1);
  {
    // Exemple 2eme operateur
    DoubleTab a;
    a.ref(inco); // Doit prendre l'etat de inco qui est toujours DataLocation::HostDevice
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(a.ref_count() == 2);
    assert(inco.ref_count() == 2);

    DoubleTab b(N);
    CDoubleArrView a_v = static_cast<const ArrOfDouble&>(a).view_ro();
    DoubleArrView b_v = static_cast<ArrOfDouble&>(b).view_wo();
    Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int i)
    {
      b_v[i] = a_v[i];
    });
    Kokkos::fence();

    const DoubleTab& const_b = b;
    const DoubleTab& const_a = a;
    assert(const_b[5] == const_a[5]);
    assert(const_b[5] == 1);
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::HostDevice);
  }
  assert(inco.ref_count() == 1);

  // Mise a jour de l'inconnue sur le device:
  inco += 1;
  assert(inco.get_data_location() == DataLocation::Device);
  assert(inco.ref_count() == 1);
  {
    // Pas de temps suivant, nouvel operateur:
    DoubleTab a;
    a.ref(inco); // Doit prendre l'etat de inco
    assert(a.get_data_location() == DataLocation::Device);
    assert(a.ref_count() == 2);
    assert(inco.ref_count() == 2);

    DoubleTab b(N);
    CDoubleArrView a_v = static_cast<const ArrOfDouble&>(a).view_ro();
    DoubleArrView b_v = static_cast<ArrOfDouble&>(b).view_wo();
    Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int i)
    {
      b_v[i] = a_v[i];
    });
    Kokkos::fence();
    const DoubleTab& const_b = b;
    const DoubleTab& const_a = a;
    assert(const_b[5] == const_a[5]);
    assert(const_b[5] == 2);
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::HostDevice);
    assert(inco.get_data_location() == DataLocation::HostDevice); // Car a ref sur inco
  }
  assert(inco.get_data_location() == DataLocation::HostDevice);
  // Mise a jour de l'inconnue sur le device
  {
    inco += 1;
    assert(inco.get_data_location() == DataLocation::Device);

    DoubleTab a;
    a.ref(inco); // Doit prendre l'etat de inco
    assert(a.get_data_location() == DataLocation::Device);
    assert(a.ref_count() == 2);
    assert(inco.ref_count() == 2);

    DoubleTab b(N);
    CDoubleArrView a_v = static_cast<const ArrOfDouble&>(a).view_ro();
    DoubleArrView b_v = static_cast<ArrOfDouble&>(b).view_wo();
    Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int i)
    {
      b_v[i] = a_v[i];
    });
    Kokkos::fence();
    const DoubleTab& const_b = b;
    const DoubleTab& const_a = a;
    assert(const_b[5] == const_a[5]);
    assert(const_b[5] == 3);
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::HostDevice);
    assert(inco.get_data_location() == DataLocation::HostDevice);
  }
  assert(inco.get_data_location() == DataLocation::HostDevice);

  // Test d'operations ArrOfDouble sur GPU
  {
    ArrOfDouble a(10), b(10);
    a=1;
    b=2;
    mapToDevice(a, "a");
    mapToDevice(b, "b");
    b+=a; // TRUSTArray& operator+=(const TRUSTArray& y) sur le device
    b+=3; // TRUSTArray& operator+=(const _TYPE_ dy)
    b-=2; // TRUSTArray& operator-=(const _TYPE_ dy)
    b-=a; // TRUSTArray& operator-=(const TRUSTArray& y)
    // ToDo regler: Multiple definition of 'nvkernel__ZN10TRUST
    //b*=10; // TRUSTArray& operator*= (const _TYPE_ dy)
    //b/=2;  // TRUSTArray& operator/= (const _TYPE_ dy)
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::Device);
    const ArrOfDouble& const_b = b;
    // Operations sur le device:
    // Retour sur le host pour verifier le resultat
    copyFromDevice(b, "b");
    assert(const_b[0] == 3);
  }

  // Test de copyPartialFromDevice/copyPartialToDevice
  {
    ArrOfDouble a(4);
    const ArrOfDouble& const_a = a;
    a=0;
    mapToDevice(a, "a");
    a+=1; // Done on device
    copyPartialFromDevice(a, 1, 3, "a");
    assert(a.get_data_location() == DataLocation::PartialHostDevice);
    assert(const_a[0]==0);
    assert(const_a[1]==1);
    assert(const_a[2]==1);
    assert(const_a[3]==0);
    a[1]=2; // Done on host
    a[2]=2; // Done on host
    copyPartialToDevice(a, 1, 3, "a");
    assert(a.get_data_location() == DataLocation::Device);
    copyFromDevice(a, "a");
    assert(const_a[0]==1);
    assert(const_a[1]==2);
    assert(const_a[2]==2);
    assert(const_a[3]==1);
  }

  // Constructeurs sur device:
  {
    DoubleTab a(10);
    allocateOnDevice(a);
    assert(a.get_data_location() == DataLocation::Device);
    a = 1; // Sur le device
    assert(a.get_data_location() == DataLocation::Device);
    DoubleTab b(a); // b doit etre aussi alloue sur le device et la copie faite sur le device
    assert(b.get_data_location() == DataLocation::Device);
    copyFromDevice(b, "b");
    const ArrOfDouble& const_b = b;
    assert(const_b[0] == 1);
    assert(const_b[b.size() - 1] == 1);
  }
  {
    // Copies de tableau:
    DoubleTab a(10);
    a=1;
    mapToDevice(a, "a"); // a sur le device
    DoubleTab b;
    b = a; // b doit etre aussi alloue/rempli sur le device par copie de a:
    assert(b.get_data_location() == DataLocation::Device);
    const ArrOfDouble& const_b = b;
    copyFromDevice(b, "b");
    assert(const_b[0] == 1);
    assert(const_b[b.size() - 1] == 1);
  }
  // operator_vect_vect_generic pour DoubleTab::operator+-*/
  {
    DoubleTab a(10), b(10);
    const ArrOfDouble& const_a = a;
    const ArrOfDouble& const_b = b;
    a=1;
    b=10;
    mapToDevice(a,"a");
    assert(a.get_data_location() == DataLocation::HostDevice);
    mapToDevice(b, "b");
    assert(b.get_data_location() == DataLocation::HostDevice);
    a=b;  // TRUSTArray<_TYPE_>::inject_array(v) faite sur le device (a=10)
    assert(a.get_data_location() == DataLocation::Device);
    a+=b; // operator_vect_vect_generic(ADD_) faite sur le device (a=20)
    assert(a.get_data_location() == DataLocation::Device);
    b-=a; // operator_vect_vect_generic(SUB_) faite sur le device (b=-10)
    assert(b.get_data_location() == DataLocation::Device);
    // Retour sur le host pour verifier les resultats
    copyFromDevice(a,"a");
    copyFromDevice(b,"b");
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(b.get_data_location() == DataLocation::HostDevice);
    assert(const_a[0] == 20);
    assert(const_a[a.size()-1] == 20);
    assert(const_b[0] == -10);
    assert(const_b[b.size()-1] == -10);
  }
  // DoubleTrav
  {
    DoubleTrav a(10*N);
    a = 1;
    mapToDevice(a, "a"); // copy
    assert(a.get_data_location() == DataLocation::HostDevice);
    assert(a.ref_count() == 1);
    DeviceMemory::printMemoryMap();
  }
  // Second DoubleTrav
  {
    DeviceMemory::printMemoryMap();
    DoubleTrav a(10*N); // a is initialized to 0 on the device cause previous DoubleTrav was HostDevice !
    assert(a.get_data_location() == DataLocation::Device);
    const ArrOfDouble& const_a = a;
    assert(const_a[0] == 0);
  }
  // Constructeur par copie DoubleTab
  {
    DoubleTab a(N);
    a=-1;
    mapToDevice(a); // Sur le device
    DoubleTrav b(a); // b doit etre sur le device (=0)
    assert(b.get_data_location()==DataLocation::Device);
    b+=1; // Operation doit etre faite sur le device (=1)
    assert(b.get_data_location()==DataLocation::Device);
    copyFromDevice(b);
    const ArrOfDouble& const_b = b;
    assert(const_b[0] == 1);
    assert(const_b[b.size()-1] == 1);
  }
  // max/min methods
  {
    DoubleTab a(3);
    a(0)=1;
    a(1)=3;
    a(2)=-10;
    mapToDevice(a);
    // Change sur le host pour test:
    a.data()[0]=0;
    a.data()[1]=0;
    a.data()[2]=0;
    a.set_data_location(DataLocation::Device);
    assert(local_max_vect(a)==3);
    assert(local_min_vect(a)==-10);
    assert(local_max_abs_vect(a)==10);
    assert(local_min_abs_vect(a)==1);
    //assert(local_imax_vect(a)==1);
    //assert(local_imin_vect(a)==2);
    assert(a.isDataOnDevice());
    // On teste sur host les deux methodes imin,imax non portees sur GPU:
    copyFromDevice(a);
    assert(a.get_data_location()==DataLocation::HostDevice);
    assert(local_imax_vect(a)==1);
    assert(local_imin_vect(a)==2);
  }
  // ref_array
  {
    DoubleTab a(N);
    a=1;
    mapToDevice(a, "a"); // Sur le device
    assert(a.get_data_location()==DataLocation::HostDevice);
    DoubleTab b;
    b.ref_array(a);
    assert(b.get_data_location()==DataLocation::HostDevice); // b doit etre sur le device
  }
  // ref_tab/ref_array on a chunk array
  {
    DoubleTab a(2*N);
    a=1;
    DoubleTab b;
    b.ref_tab(a, 0, N); // reference partielle sur a
    mapToDevice(b, "b"); // Sur le device
    assert(b.get_data_location()==DataLocation::HostDevice); // b doit etre sur le device
    assert(a.get_data_location()==DataLocation::HostDevice); // a doit etre sur le device

    DoubleArrView a_v = static_cast<ArrOfDouble&>(a).view_wo();
    Kokkos::parallel_for(2*N, KOKKOS_LAMBDA(const int i)
    {
      a_v[i] = 2;
    });
    Kokkos::fence();
    // Retour sur le device et verification que a etait completement sur le device:
    assert(a.get_data_location()==DataLocation::Device);
    assert(a(0)==2);
    assert(a.get_data_location()==DataLocation::Host);
    assert(a(2*N-1)==2);
  }
  double * ptr_host;
  {
    DoubleTab a(N);
    a=1;
    {
      DoubleTab b;
      b.ref_array(a);
      mapToDevice(b, "b"); // Sur le device
      assert(a.data()==b.data());
      assert(b.get_data_location() == DataLocation::HostDevice);
      assert(a.get_data_location() == DataLocation::HostDevice); // a est considere sur le device egalement
    }
    assert(a.get_data_location() == DataLocation::HostDevice);
    ptr_host = a.data();
    assert(isAllocatedOnDevice(ptr_host)); // Verifie que le tableau possede une zone memoire sur le device
    a.resize(2*N);
    a=2;
    {
      DoubleTab b;
      b.ref_array(a);
      mapToDevice(b, "b"); // Sur le device
    }
  }
  assert(!isAllocatedOnDevice(ptr_host)); // Verifie que le tableau ne possede plus une zone memoire sur le device
  // ToDo:Comment gerer les DoubleTab_Parts ? Pas facile donc pour le moment
  // le constructeur par copie fait un copyFromDevice du DoubleTab...
  /*
    {
        DoubleTab pression;
        mapToDevice(pression);
        DoubleTab_parts P0P1(pression); // P0P1 Device
        DoubleTab& P0 = P0P1[0];
        DoubleTab& P1 = P0P1[1];
        double moyenne_K = mp_moyenne_vect(P0); // P0 DataLocation::HostDevice mais pas P1 !
        P1 -= moyenne_K; // Gros pb car P1 toujours sur Device !
    }
    */
  //if (Process::me()==0) std::cerr << ptr_host << std::endl;

  // DoubleTrav copy Constructor:
  {
    DoubleTrav b(100);
    b = 123;
    mapToDevice(b);
    DoubleVect b2(b);
    assert(b2.get_mem_storage() == STORAGE::TEMP_STORAGE);
    assert(b2.get_data_location() == DataLocation::Device);

    DoubleVect b3(b2);
    assert(b3.get_mem_storage() == STORAGE::TEMP_STORAGE);
    assert(b3.get_data_location() == DataLocation::Device);
  }

  {
    // Provisoire reproduire le blocage ?
    //DoubleTab a(10);
    //allocateOnDevice(a);
    //allocateOnDevice(a);
  }
#endif
}


