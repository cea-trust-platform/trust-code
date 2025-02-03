#include <gtest/gtest.h>
#include <TRUSTArray.h>
#include <Comm_Group_Noparallel.h>
#include <PE_Groups.h>


int main(int argc, char **argv) {
    Kokkos::initialize();

    Comm_Group_Noparallel groupe_trio;
    PE_Groups::initialize(groupe_trio);

    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    Kokkos::finalize();

    return result;
}
