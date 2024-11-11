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
#include <iostream>
#include <Device.h>
#include <ParserView.h>

/*! Teste les methodes de l'interface Kokkos utilisees dans TRUST
 */
class Object
{
public:
  virtual ~Object() {}
  Object() {}
  virtual void setSize(int i) { les_var.resize(i); }
  void setVar(int i, double val) { les_var[i] = val; }
  double eval() { return 2*les_var[0]+2; }
private:

protected:
  ArrOfDouble les_var;
};
class ObjectView : public Object
{
public:
  ObjectView()
  {
    Object();
  }
  virtual void setSize(int i) override
  {
    Object::setSize(i);
    les_var_view = les_var.view_rw();
  }
  // Les methodes sur le device sont const
  KOKKOS_FUNCTION
  void setVar(int i, double val) const { les_var_view[i] = val; }
  KOKKOS_FUNCTION
  double eval() const { return 2*les_var_view[0]+2; }
private:
  DoubleArrView les_var_view;
};
template<typename ExecSpace>
void kernel(ArrOfDouble& f, ObjectView& MyObjectDevice)
{
  int nb_elem = f.size_array();
  DoubleArrView f_v = f.view_rw();
  Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
  {
    MyObjectDevice.setVar(0, 0.);
    f_v(i) = MyObjectDevice.eval();
  });
}
void kokkos_self_test()
{
#ifndef NDEBUG
#ifdef _OPENMP_TARGET
  static bool kokkos_self_tested_ = false;

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
  // Check CDoubleTabView:
  {
    CDoubleTabView a_v = a.view_ro();
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
    CDoubleTabView a_v = a.view_ro();
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
    CDoubleArrView a_v = u.view_ro();
    DoubleArrView b_v = v.view_rw();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
    {
      b_v(i) = 2 * b_v(i) + a_v(i);
    });
    Kokkos::fence();
    assert(est_egal(v(0), 5)); // Retour sur le host et verification
  }
  // Check DoubleVect built as a DoubleTab
  {
    DoubleTab tab(nb_elem);
    DoubleArrView vect_v = static_cast<DoubleVect&>(tab).view_rw(); // It works but ToDo Kokkos, try to do better ? DoubleArrView vect_v = tab.view_rw(); ?
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i)
    {
      vect_v(i) = 2;
    });
  }
  // Check DoubleTab aliased by a DoubleVect
  {
    int dim = 3;
    DoubleTab tab(nb_elem, dim);
    DoubleVect& vect = tab;
    DoubleArrView vect_v = vect.view_rw();
    int size = vect.size_array();
    Kokkos::parallel_for(size, KOKKOS_LAMBDA(const int i)
    {
      vect_v(i) = 2;
    });
  }
  // Verification des adresses memoire:
  {
    DoubleTab tab(nb_elem,2);
    for (int i=0; i<nb_elem; i++)
      for (int j=0; j<2; j++)
        tab(i,j) = i + 0.1*j;
    CDoubleTabView tab_v = tab.view_ro();
    // La creation de la vue fait une copie de u sur le device:
    //printf("Provisoire OpenMP adresse host: [%p] device: [%p]\n",(void*)tab.data(), (void*)addrOnDevice(tab));
    assert(a.data() != addrOnDevice(a));
    // On verifie les adresses memoires de la vue:
    //printf("Provisoire Kokkos adresse            device: [%p]\n",(void*)tab_v.data());
    //assert(tab_v.h_view.view_host()==tab.data());
    assert(tab_v.data()==addrOnDevice(tab));
    //debug_device_view(tab_v, tab);
  }
  // Changement de la zone memoire sur le device avec OpenMP:
  {
    DoubleTab tab(nb_elem,2);
    for (int i=0; i<nb_elem; i++)
      for (int j=0; j<2; j++)
        tab(i,j) = i + 0.1*j;
    CDoubleTabView tab_v = tab.view_ro();
    assert(tab_v.data()==addrOnDevice(tab)); // Meme adresse
    //printf("Provisoire OpenMP device before: [%p]\n", (void*)addrOnDevice(tab));
    deleteOnDevice(tab);
    allocateOnDevice(tab);
    //printf("Provisoire OpenMP device after: [%p]\n", (void*)addrOnDevice(tab));
    CDoubleTabView tab_v2 = tab.view_ro();
    assert(tab_v2.data()==addrOnDevice(tab)); // Meme adresse
  }
  // C++ object in Kokkos region
  {
    ArrOfDouble f(nb_elem);
    f=0;
    // Kernel host
    Object MyObjectHost;
    MyObjectHost.setSize(nb_elem);
    MyObjectHost.setVar(0, 0.);
    assert(MyObjectHost.eval()==2.);

    // Kernel host or host
    ObjectView MyObjectDevice;
    MyObjectDevice.setSize(nb_elem);
    assert(!f.isDataOnDevice());
    kernel<Kokkos::DefaultHostExecutionSpace>(f, MyObjectDevice);
    assert(f(0)==2);
    mapToDevice(f); // envoi de f sur le device
    assert(f.isDataOnDevice());
    kernel<Kokkos::DefaultExecutionSpace>(f, MyObjectDevice);
    assert(f(0)==2);
  }
  // Parser sur le host:
  {
    ArrOfDouble f(nb_elem);
    f = 0;
    std::string expr("2*x+2");
    Parser parser(expr, 1);
    parser.addVar("x");
    parser.parseString();
    for (int i = 0; i < nb_elem; i++)
      {
        double x = (double) i;
        parser.setVar(0, x);
        f(i) = parser.eval();
      }
    assert(f(0) == 2);
    assert(f(nb_elem - 1) == 2 * nb_elem);
  }
  {
    /*
    class toto
    {
    public:
    void setVar(int i, double val) const { x[i] = val; }
    double getVar(int i) const { return x[i]; }
    private:
    mutable double x[3];
    };
    toto t;
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(
                         const int i)
    {
    t.setVar(0, (double)i);
    Kokkos::printf("Provisoire %d %f \n",i,t.getVar(0));
    }); */
    ArrOfDouble f(nb_elem);
    f = 0;
    std::string expr("2*x+2");
    // Parser sur le device;
    ParserView parser(expr, 1, nb_elem);
    parser.addVar("x");
    parser.parseString();
    DoubleArrView f_v = f.view_rw();
    Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(
                           const int i)
    {
      double x = (double) i;
      parser.setVar(0, x, i);
      f_v(i) = parser.eval(i);
    });
    assert(f(0) == 2);
    assert(f(nb_elem - 1) == 2 * nb_elem);
  }
// Some check:
  /*
  assert(a_v.is_allocated());
  bool check = std::is_same<decltype(a_v)::memory_space, Kokkos::CudaSpace>::value;
  assert(check);
   */

// ToDo implement reference counter for a view on tab
  /*
  DoubleTab tab(N);
  DoubleArrView view = tab.ro(); 	// Data on device
  copyFromeDevice(tab);				// Data on host !
  // How to detect that now view is invalid and can't be used in a Kokkos region now ?
   Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int elem) { view(elem) = 1.0; });
   */
#endif
#endif
}

