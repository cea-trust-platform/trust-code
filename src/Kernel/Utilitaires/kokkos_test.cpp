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

#include <View_Types.h>
#include <TRUSTTab.h>
#include <TRUSTTab_kokkos.tpp>
#include <TRUSTArray_kokkos.tpp>
#include <iostream>
#include <Device.h>

// Teste les methodes de l'interface Kokkos utilisees dans TRUST:
bool kokkos_self_tested_ = false;
void kokkos_self_test()
{
#ifndef NDEBUG
#ifdef _OPENMP
  if (kokkos_self_tested_) return;
  kokkos_self_tested_=true;

  // Tab dimensions:
  int nb_elem = 3;
  int nb_compo = 2;
  DoubleTab a(nb_elem, nb_compo);
  a=1.0;
  DoubleTab b(nb_elem, nb_compo);
  b=2.0;

  // Check DoubleTabView:
  {
    DoubleTabView b_v = b.view_rw();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
    {
      assert(b_v(i, 0) == 2);
      for (int j = 0; j < nb_compo; j++)
        b_v(i, j) = 2 * b_v(i, j);
    });
    Kokkos::fence();
    assert(b(0, 0) == 4); // Retour sur le host et verification
  }
  // Check const DoubleTabView:
  {
    const DoubleTabView a_v = a.view_ro();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
    {
      assert(a_v(i, 0) == 1);
    });
    Kokkos::fence();
    assert(a(0, 0) == 1); // Verification sur le host
  }
  // Check basic loop:
  {
    a = 1;
    b = 2;
    const DoubleTabView a_v = a.view_ro();
    DoubleTabView b_v = b.view_rw();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(
                           const int i)
    {
      for (int j = 0; j < nb_compo; j++)
        {
          b_v(i, j) = 2 * b_v(i, j) + a_v(i, j);
          //printf("KOKKOS: %p %e\n", b_v.data(), b_v(i, j));
        }
    });
    Kokkos::fence();
    assert(est_egal(b(0, 0), 5)); // Retour sur le host et verification
  }

  // Check basic loop on vect
  {
    DoubleVect u(nb_elem);
    u = 1.0;
    ArrOfDouble v(nb_elem);
    v = 2.0;
    const DoubleVectView a_v = u.view_ro();
    DoubleVectView b_v = v.view_rw();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
    {
      b_v(i) = 2 * b_v(i) + a_v(i);
    });
    Kokkos::fence();
    assert(est_egal(v(0), 5)); // Retour sur le host et verification
  }
  // Verification des adresses memoire:
  {
    DoubleTab tab(nb_elem,2);
    for (int i=0; i<nb_elem; i++)
      for (int j=0; j<2; j++)
        tab(i,j) = i + 0.1*j;
    const DoubleTabView tab_v = tab.view_ro();
    // La creation de la vue fait une copie de u sur le device:
    printf("Provisoire OpenMP adresse host: [%p] device: [%p]\n",(void*)tab.addrForDevice(), (void*)addrOnDevice(tab));
    assert(a.addrForDevice() != addrOnDevice(a));
    // On verifie les adresses memoires de la vue:
    printf("Provisoire Kokkos adresse            device: [%p]\n",(void*)tab_v.data());
    //assert(tab_v.h_view.view_host()==tab.addrForDevice());
    assert(tab_v.data()==addrOnDevice(tab));
    debug_device_view(tab_v, tab);
  }
  // Changement de la zone memoire sur le device avec OpenMP:
  {
    DoubleTab tab(nb_elem,2);
    for (int i=0; i<nb_elem; i++)
      for (int j=0; j<2; j++)
        tab(i,j) = i + 0.1*j;
    const DoubleTabView tab_v = tab.view_ro();
    assert(tab_v.data()==addrOnDevice(tab)); // Meme adresse
    printf("Provisoire OpenMP device before: [%p]\n", (void*)addrOnDevice(tab));
    deleteOnDevice(tab);
    allocateOnDevice(tab);
    printf("Provisoire OpenMP device after: [%p]\n", (void*)addrOnDevice(tab));
    const DoubleTabView tab_v2 = tab.view_ro();
    assert(tab_v2.data()==addrOnDevice(tab)); // Meme adresse
  }
  // Some check:
  /*
  assert(a_v.is_allocated());
  bool check = std::is_same<decltype(a_v)::memory_space, Kokkos::CudaSpace>::value;
  assert(check);
   */
#endif
#endif
}

