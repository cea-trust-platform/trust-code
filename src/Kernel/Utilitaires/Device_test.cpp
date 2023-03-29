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
#include <TRUSTTab_parts.h>
#include <Device.h>
#include <string>
#include <sstream>
#include <comm_incl.h>

// Device_test: teste intensivement les methodes de l'interface Device.h:
static bool self_tested_ = false;
bool self_test()
{
  if (!self_tested_ && Objet_U::computeOnDevice)
    {
      self_tested_ = true;
      // Test mapToDevice(arr)
      // Status
      // Status before		Status after		Copy ?
      // HostOnly	        HostDevice	        Yes
      // Host		        HostDevice	        Yes
      // HostDevice	        HostDevice	        No
      // Device		        Device		        No
      {
        DoubleTab a(10);
        assert(a.get_dataLocation() == HostOnly);
        mapToDevice(a);
        assert(a.get_dataLocation() == HostDevice);
        a.set_dataLocation(Host);
        mapToDevice(a);
        assert(a.get_dataLocation() == HostDevice);
        a.set_dataLocation(HostDevice);
        mapToDevice(a);
        assert(a.get_dataLocation() == HostDevice);
        a.set_dataLocation(Device);
        mapToDevice(a);
        assert(a.get_dataLocation() == Device);
      }

      int N = 10;
      // Teste les methodes d'acces sur le device:
      DoubleTab inco(N);
      inco = 1;
      mapToDevice(inco, "inco"); // copy
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

        const double *a_addr = mapToDevice(a, "a"); // up-to-date
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
        const double *a_addr = mapToDevice(a, "a"); // up-to-date

        DoubleTab b(N);
        double *b_addr = b.addrForDevice();
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

      // Mise a jour de l'inconnue sur le device:
      inco += 1;
      assert(inco.get_dataLocation() == Device);
      assert(inco.ref_count() == 1);
      {
        // Pas de temps suivant, nouvel operateur:
        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco
        assert(a.get_dataLocation() == Device);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        const double *a_addr = mapToDevice(a, "a"); // update

        DoubleTab b(N);
        double *b_addr = b.addrForDevice();
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
      // Mise a jour de l'inconnue sur le device
      {
        inco += 1;
        assert(inco.get_dataLocation() == Device);

        DoubleTab a;
        a.ref(inco); // Doit prendre l'etat de inco
        assert(a.get_dataLocation() == Device);
        assert(a.ref_count() == 2);
        assert(inco.ref_count() == 2);
        const double *a_addr = mapToDevice(a, "a"); // up-to-date

        DoubleTab b(N);
        double *b_addr = b.addrForDevice();
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
        mapToDevice(a, "a");
        mapToDevice(b, "b");
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
        copyFromDevice(b, "b");
        assert(const_b[0] == 3);
      }

      // Test de copyPartialFromDevice/copyPartialToDevice
      {
        ArrOfDouble a(4);
#ifndef NDEBUG
        const ArrOfDouble& const_a = a;
#endif
        a=0;
        mapToDevice(a, "a");
        a+=1; // Done on device
        copyPartialFromDevice(a, 1, 3, "a");
        assert(a.get_dataLocation() == PartialHostDevice);
        assert(const_a[0]==0);
        assert(const_a[1]==1);
        assert(const_a[2]==1);
        assert(const_a[3]==0);
        a[1]=2; // Done on host
        a[2]=2; // Done on host
        copyPartialToDevice(a, 1, 3, "a");
        assert(a.get_dataLocation() == Device);
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
        assert(a.get_dataLocation() == Device);
        a = 1; // Sur le device
        assert(a.get_dataLocation() == Device);
        DoubleTab b(a); // b doit etre aussi alloue sur le device et la copie faite sur le device
        assert(b.get_dataLocation() == Device);
        copyFromDevice(b, "b");
#ifndef NDEBUG
        const ArrOfDouble& const_b = b;
#endif
        assert(const_b[0] == 1);
        assert(const_b[b.size() - 1] == 1);
      }
      {
        // ToDo: Copies de tableau:
        DoubleTab a(10);
        a=1;
        mapToDevice(a, "a"); // a sur le device
        DoubleTab b;
        b = a; // b doit etre aussi alloue/rempli sur le device par copie de a:
        assert(b.get_dataLocation() == Device);
#ifndef NDEBUG
        const ArrOfDouble& const_b = b;
#endif
        copyFromDevice(b, "b");
        assert(const_b[0] == 1);
        assert(const_b[b.size() - 1] == 1);
      }
      // operator_vect_vect_generic pour DoubleTab::operator+-*/
      {
        DoubleTab a(10), b(10);
#ifndef NDEBUG
        const ArrOfDouble& const_a = a;
        const ArrOfDouble& const_b = b;
#endif
        a=1;
        b=10;
        mapToDevice(a,"a");
        assert(a.get_dataLocation() == HostDevice);
        mapToDevice(b, "b");
        assert(a.get_dataLocation() == HostDevice);
        a=b;  // TRUSTArray<_TYPE_>::inject_array(v) faite sur le device (a=10)
        assert(a.get_dataLocation() == Device);
        a+=b; // operator_vect_vect_generic(ADD_) faite sur le device (a=20)
        assert(a.get_dataLocation() == Device);
        b-=a; // operator_vect_vect_generic(SUB_) faite sur le device (b=-10)
        assert(b.get_dataLocation() == Device);
        // Retour sur le host pour verifier les resultats
        copyFromDevice(a,"a");
        copyFromDevice(b,"b");
        assert(a.get_dataLocation() == HostDevice);
        assert(b.get_dataLocation() == HostDevice);
        assert(const_a[0] == 20);
        assert(const_a[a.size()-1] == 20);
        assert(const_b[0] == -10);
        assert(const_b[b.size()-1] == -10);
      }
      // local_operations_vect_bis_generic
      {
        DoubleTab a(2);
        a(0)=1;
        a(1)=2;
        mapToDevice(a, "a");
        assert(est_egal(mp_norme_vect(a),sqrt(Process::nproc()*5)));
        assert(a.get_dataLocation() == HostDevice); // Operation faite sur le device
      }
      // DoubleTrav
      {
        DoubleTrav a(N);
        a = 1;
        mapToDevice(a, "a"); // copy
        assert(a.get_dataLocation() == HostDevice);
        assert(a.ref_count() == 1);
      }
      {
        DoubleTrav b(N);
        assert(b.get_dataLocation() == HostOnly); // ToDo: devrait etre en Host (recupere la zone memoire precedente)
        b = 2;
        assert(b.get_dataLocation() == HostOnly);
        mapToDevice(b, "b"); // copy ToDo cela devrait etre un update
      }
      // Constructeur par copie DoubleTab
      {
        DoubleTab a(N);
        a=-1;
        mapToDevice(a); // Sur le device
        DoubleTrav b(a); // b doit etre sur le device (=0)
        assert(b.get_dataLocation()==Device);
        b+=1; // Operation doit etre faite sur le device (=1)
        assert(b.get_dataLocation()==Device);
        copyFromDevice(b);
#ifndef NDEBUG
        const ArrOfDouble& const_b = b;
#endif
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
        a.addrForDevice()[0]=0;
        a.addrForDevice()[1]=0;
        a.addrForDevice()[2]=0;
        a.set_dataLocation(Device);
        assert(local_max_vect(a)==3);
        assert(local_min_vect(a)==-10);
        assert(local_max_abs_vect(a)==10);
        assert(local_min_abs_vect(a)==1);
        //assert(local_imax_vect(a)==1);
        //assert(local_imin_vect(a)==2);
        assert(a.isDataOnDevice());
        // On teste sur host les deux methodes imin,imax non portees sur GPU:
        copyFromDevice(a);
        assert(a.get_dataLocation()==HostDevice);
        assert(local_imax_vect(a)==1);
        assert(local_imin_vect(a)==2);
      }
      // ref_array
      {
        DoubleTab a(N);
        a=1;
        mapToDevice(a); // Sur le device
        assert(a.get_dataLocation()==HostDevice);
        DoubleTab b;
        b.ref_array(a);
        assert(b.get_dataLocation()==HostDevice); // b doit etre sur le device
      }
      // ToDo:Comment gerer les DoubleTab_Parts ? Pas facile donc pour le moment
      // le constructeur par copie fait un copyFromDevice du DoubleTab...
      /*
        {
            DoubleTab pression;
            mapToDevice(pression);
            DoubleTab_parts P0P1(pression); // P0P1 Device
            DoubleTab& P0 = P0P1[0];
            DoubleTab& P1 = P0P1[1];
            double moyenne_K = mp_moyenne_vect(P0); // P0 HostDevice mais pas P1 !
            P1 -= moyenne_K; // Gros pb car P1 toujours sur Device !
        }
        */
    }
  return self_tested_;
}


