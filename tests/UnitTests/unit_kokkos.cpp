#include <gtest/gtest.h>
#include <View_Types.h>
#include <TRUSTTab.h>
#include <iostream>
#include <Device.h>
#include <ParserView.h>
#include <Kokkos_UnorderedMap.hpp>
#include <Kokkos_UniqueToken.hpp>
#include <TRUSTTrav.h>

TEST(KokkosTest, DoubleTabView)
{
  const int nb_elem = 3;
  const int nb_compo = 2;
  
  // Create and initialize a DoubleTab on the host.
  DoubleTab b(nb_elem, nb_compo);
  b = 2.0;

  {
    // Get a read-write view.
    DoubleTabView b_v = b.view_rw();
    // Launch a Kokkos kernel to check and update the values.
    Kokkos::parallel_for("DoubleTabViewKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
      // Note: Using assert() in device code is acceptable if it is enabled.
      assert(b_v(i, 0) == 2);
      for (int j = 0; j < nb_compo; j++)
        b_v(i, j) = 2 * b_v(i, j);
    });
    Kokkos::fence();
    // Check on the host that the value was doubled.
    EXPECT_EQ(b(0, 0), 4);
  }
}

//---------------------------------------------------------------------
// Test 2: Check CDoubleTabView
//---------------------------------------------------------------------
TEST(KokkosTest, CDoubleTabView)
{
  const int nb_elem = 3;
  const int nb_compo = 2;
  
  DoubleTab a(nb_elem, nb_compo);
  a = 1.0;
  
  {
    CDoubleTabView a_v = a.view_ro();
    Kokkos::parallel_for("CDoubleTabViewKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
      assert(a_v(i, 0) == 1);
    });
    Kokkos::fence();
    EXPECT_EQ(a(0, 0), 1);
  }
}

TEST(KokkosTest, DoubleTabViewBasicLoop)
{
  const int nb_elem = 3;
  const int nb_compo = 2;
  
  DoubleTab a(nb_elem, nb_compo);
  DoubleTab b(nb_elem, nb_compo);
  a = 1;
  b = 2;
  
  CDoubleTabView a_v = a.view_ro();
  DoubleTabView b_v = b.view_rw();
  
  Kokkos::parallel_for("BasicLoopKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
    for (int j = 0; j < nb_compo; j++)
    {
      b_v(i, j) = 2 * b_v(i, j) + a_v(i, j);
    }
  });
  Kokkos::fence();
  EXPECT_TRUE(est_egal(b(0, 0), 5));
}

TEST(KokkosTest, DoubleVectBasicLoop)
{
  const int nb_elem = 3;
  
  DoubleVect u(nb_elem);
  u = 1.0;
  ArrOfDouble v(nb_elem);
  v = 2.0;
  
  CDoubleArrView a_v = u.view_ro();
  DoubleArrView b_v = v.view_rw();
  
  Kokkos::parallel_for("BasicLoopVectKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
    b_v(i) = 2 * b_v(i) + a_v(i);
  });
  Kokkos::fence();
  EXPECT_TRUE(est_egal(v(0), 5));
}

TEST(KokkosTest, DoubleVectAsDoubleTabBasicLoop)
{
  const int nb_elem = 3;
  
  // Construct a DoubleTab that is to be treated as a DoubleVect.
  DoubleTab tab(nb_elem);
  // Cast to DoubleVect and get a writable view.
  DoubleArrView vect_v = static_cast<DoubleVect&>(tab).view_rw();
  
  Kokkos::parallel_for("DoubleVectAsDoubleTabKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
    vect_v(i) = 2;
  });
  Kokkos::fence();
  
  // Verify on host that every element was set to 2.
  for (int i = 0; i < nb_elem; ++i)
    EXPECT_EQ(tab(i), 2);
}

TEST(KokkosTest, DoubleTabAliasedByDoubleVect)
{
  const int nb_elem = 3;
  const int dim = 3;
  
  DoubleTab tab(nb_elem, dim);
  // Alias the tab as a DoubleVect.
  DoubleVect& vect = tab;
  DoubleArrView vect_v = vect.view_rw();
  int size = vect.size_array();
  
  Kokkos::parallel_for("DoubleVectAliasingKernel", size, KOKKOS_LAMBDA(const int i) {
    vect_v(i) = 2;
  });
  Kokkos::fence();
  
  // Verify that every entry is now 2.
  for (int i = 0; i < nb_elem; ++i)
    for (int j = 0; j < dim; ++j)
      EXPECT_EQ(tab(i, j), 2);
}


TEST(KokkosTest, AddressVerification)
{
  const int nb_elem = 3;
  
  DoubleTab tab(nb_elem, 2);
  for (int i = 0; i < nb_elem; ++i)
    for (int j = 0; j < 2; ++j)
      tab(i, j) = i + 0.1 * j;
  
  CDoubleTabView tab_v = tab.view_ro();
  
  // Verify that the pointer on the host (data()) is different from the device pointer.
#ifdef TRUST_USE_GPU
  EXPECT_NE(tab.data(), addrOnDevice(tab));
#else 
  EXPECT_EQ(tab.data(), addrOnDevice(tab));
#endif
  
  // Check that the view's data pointer matches the device pointer.
  EXPECT_EQ(tab_v.data(), addrOnDevice(tab));
}


TEST(KokkosTest, DeviceMemoryReallocation)
{
  const int nb_elem = 3;
  
  DoubleTab tab(nb_elem, 2);
  for (int i = 0; i < nb_elem; ++i)
    for (int j = 0; j < 2; ++j)
      tab(i, j) = i + 0.1 * j;
  
  CDoubleTabView tab_v = tab.view_ro();
  EXPECT_EQ(tab_v.data(), addrOnDevice(tab)); // same address as expected
  
  // Delete then reallocate the device memory.
  deleteOnDevice(tab);
  allocateOnDevice(tab);
  
  CDoubleTabView tab_v2 = tab.view_ro();
  EXPECT_EQ(tab_v2.data(), addrOnDevice(tab));
}


TEST(KokkosTest, CppObjectInKokkosRegion)
{
  const int nb_elem = 3;
  
  ArrOfDouble f(nb_elem);
  f = 0;
  std::string expr("2*x+2");
  ParserView parser(expr, 1);
  parser.addVar("x");
  parser.parseString();
  
  DoubleArrView f_v = f.view_rw();
  Kokkos::parallel_for("ParserKernel", nb_elem, KOKKOS_LAMBDA(const int i) {
    int threadId = parser.acquire();
    double x = static_cast<double>(i);
    parser.setVar(0, x, threadId);
    f_v(i) = parser.eval(threadId);
    parser.release(threadId);
  });
  Kokkos::fence();
  
  EXPECT_EQ(f(0), 2);
  EXPECT_EQ(f(nb_elem - 1), 2 * nb_elem);
}

TEST(KokkosTest, VectOfDoubleTrav)
{
  const int nb_elem = 3;
  const int nb = 5;
  
  using DoubleTravs = TRUST_Vector<DoubleTrav>;
  DoubleTravs tab(nb);
  Kokkos::Array<CDoubleTabView, nb> tabs;
  
  for (int n = 0; n < nb; ++n)
  {
    tab[n].resize(nb_elem, 1);
    tab[n] = static_cast<double>(n); // Initialize on host
    auto tab_view = tab[n].view_ro(); // Copy on the device
    tabs[n] = tab_view;
  }
  
  Kokkos::parallel_for(nb_elem, KOKKOS_LAMBDA(const int i) {
    for (int n = 0; n < nb; ++n)
    {
      auto tab_v = tabs[n];
      assert(tab_v(i, 0) == n);
    }
  });
}