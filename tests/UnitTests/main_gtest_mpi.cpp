/*
 * Main for the parallel unit tests of TRUST.
 *
 * We use the same initialisation sequence as in TRUST itself to load
 *  - Kokkos
 *  - and MPI
 */

#include <gtest/gtest.h>
#include <MAIN.h>
#include <mon_main.h>

//class mon_main;

int main(int argc, char **argv)
{

  mon_main * p = nullptr;
  main_TRUST(argc,argv,p, /* force_mpi */ true);

  ::testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();

  //This includes Kokkos::finalize and MPI::finalize
  if(p) delete p;

  return result;
}
