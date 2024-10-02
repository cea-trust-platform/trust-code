#include <gtest/gtest.h>
#include <TRUSTArray.h>

int main(int argc, char **argv) {
    Kokkos::initialize();

    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    Kokkos::finalize();

    return result;
}
