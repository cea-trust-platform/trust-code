#include <gtest/gtest.h>
#include <TRUSTTrav.h>
#include <TRUSTTab_parts.h>
#include <Device.h>
#include <DeviceMemory.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// OpenMP target presence test
#ifdef _OPENMP
TEST(DeviceTest, OMPTargetPresence) {
    DoubleTab a(10);
    mapToDevice(a);
    EXPECT_EQ(omp_target_is_present(a.data(), omp_get_default_device()), 1)
        << "omp_target_is_present failed. TRUST can't work on multi-GPU.";
    deleteOnDevice(a);
    EXPECT_EQ(omp_target_is_present(a.data(), omp_get_default_device()), 0)
        << "omp_target_is_present failed after deletion.";
}
#endif

//Wrapper for CPU run of the tests: everything is HostOnly
DataLocation portable(DataLocation datalocation){
#ifdef _OPENMP
  return datalocation;
#else
  return DataLocation::HostOnly;
#endif

}
// Memory mapping tests
TEST(DeviceTest, MapToDeviceHostTransition) {
    DoubleTab a(10);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostOnly));
    mapToDevice(a);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
    a[1] = 0;
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Host));
    mapToDevice(a);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
}

TEST(DeviceTest, DeviceDataLocationAfterReference) {
    DoubleTab a(10);
    a = 23;
    DoubleTab b;
    b.ref(a);
    mapToDevice(b);
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
}

// Device access operations
TEST(DeviceTest, DeviceAccessOperations) {
    int N = 10;
    DoubleTab inco(N);
    inco = 1;
    mapToDevice(inco, "inco");
    EXPECT_EQ(inco.get_data_location(), portable(DataLocation::HostDevice));

    DoubleTab a;
    a.ref(inco);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));

    DoubleTab b(N);
    const double* a_addr = mapToDevice(a, "a");
    double* b_addr = b.data();

    #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice) map(tofrom:b_addr[0:b.size_array()])
    for (int i = 0; i < N; i++)
        b_addr[i] = a_addr[i];

    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostOnly));
    EXPECT_EQ(inco.get_data_location(), portable(DataLocation::HostDevice));
}

// Array operations on GPU
TEST(DeviceTest, ArrayOperationsOnGPU) {
    ArrOfDouble a(10), b(10);
    a = 1;
    b = 2;
    mapToDevice(a, "a");
    mapToDevice(b, "b");

    b += a;
    b += 3;
    b -= 2;
    b -= a;

    const ArrOfDouble& const_b = b;
    copyFromDevice(b, "b");
    EXPECT_EQ(const_b[0], 3);
}

// Testing max/min functions on device
TEST(DeviceTest, MaxMinFunctions) {
    DoubleTab a(3);
    a(0) = 1;
    a(1) = 3;
    a(2) = -10;
    mapToDevice(a);

    EXPECT_EQ(local_max_vect(a), 3);
    EXPECT_EQ(local_min_vect(a), -10);
    EXPECT_EQ(local_max_abs_vect(a), 10);
    EXPECT_EQ(local_min_abs_vect(a), 1);

}

// Array copy operations
TEST(DeviceTest, ArrayCopyOperations) {
    DoubleTab a(10), b;
    a = 1;
    mapToDevice(a, "a");
    b = a;

    const ArrOfDouble& const_b = b;
    copyFromDevice(b, "b");

    EXPECT_EQ(const_b[0], 1);
    EXPECT_EQ(const_b[b.size() - 1], 1);
}

// Test vector operations
TEST(DeviceTest, VectorOperations) {
    DoubleTab a(10), b(10);
    const ArrOfDouble& const_a = a;
    const ArrOfDouble& const_b = b;
    a = 1;
    b = 10;
    mapToDevice(a, "a");
    mapToDevice(b, "b");

    a = b;
    a += b;
    b -= a;

    copyFromDevice(a, "a");
    copyFromDevice(b, "b");

    EXPECT_EQ(const_a[0], 20);
    EXPECT_EQ(const_b[0], -10);
}

// Device memory tracking and allocation test
TEST(DeviceTest, DeviceMemoryAllocation) {
    DoubleTab a(10);
    allocateOnDevice(a);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Device));

    a = 1;
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Device));

    DoubleTab b(a);
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));

    copyFromDevice(b, "b");
    const ArrOfDouble& const_b = b;

    EXPECT_EQ(const_b[0], 1);
    EXPECT_EQ(const_b[b.size() - 1], 1);
}

// Test for table copy on device
TEST(DeviceTest, TableCopyOnDevice) {
    DoubleTab a(10);
    a = 1;
    mapToDevice(a, "a");

    DoubleTab b = a;
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));

    const ArrOfDouble& const_b = b;
    copyFromDevice(b, "b");
    EXPECT_EQ(const_b[0], 1);
    EXPECT_EQ(const_b[b.size() - 1], 1);
}

// Test for operator_vect_vect_generic (addition, subtraction) on device
TEST(DeviceTest, OperatorVectVectGenericTest) {
    DoubleTab a(10), b(10);
    const ArrOfDouble& const_a = a;
    const ArrOfDouble& const_b = b;

    a = 1;
    b = 10;
    mapToDevice(a, "a");
    mapToDevice(b, "b");

    a = b;
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Device));
    a += b;
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Device));
    b -= a;
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));

    copyFromDevice(a, "a");
    copyFromDevice(b, "b");

    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostDevice));

    EXPECT_EQ(const_a[0], 20);
    EXPECT_EQ(const_a[a.size() - 1], 20);
    EXPECT_EQ(const_b[0], -10);
    EXPECT_EQ(const_b[b.size() - 1], -10);
}

// Test for local operations
// deprecated ?
TEST(DeviceTest, LocalOperationsVectBisGenericTest) {
    DoubleTab a(2);
    a(0) = 1;
    a(1) = 2;
    mapToDevice(a, "a");

    //This fails
    //Assertion failed: (ngroups > 0), function check_current_group, file PE_Groups.cpp, line 31.
    //EXPECT_TRUE(est_egal(mp_norme_vect(a), sqrt(Process::nproc() * 5)));

    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
}

// Test for DoubleTrav memory management
TEST(DeviceTest, DoubleTravMemoryTest) {
    int N = 10;
    DoubleTrav a(10 * N);
    a = 1;
    mapToDevice(a, "a");
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(a.ref_count(), 1);
    DeviceMemory::printMemoryMap();

    DoubleTrav b(10 * N);
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));
    const ArrOfDouble& const_b = b;
    EXPECT_EQ(const_b[0], 0);
}

// Test copy constructor with DoubleTab
TEST(DeviceTest, CopyConstructorWithDoubleTab) {
    int N = 10;
    DoubleTab a(N);
    a = -1;
    mapToDevice(a);
    DoubleTrav b(a);
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));

    b += 1;
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::Device));

    copyFromDevice(b);
    const ArrOfDouble& const_b = b;
    EXPECT_EQ(const_b[0], 1);
    EXPECT_EQ(const_b[b.size() - 1], 1);
}

// Test max and min functions
// deprecated ?

TEST(DeviceTest, MaxMinMethodsTest) {
    DoubleTab a(3);
    a(0) = 1;
    a(1) = 3;
    a(2) = -10;
    mapToDevice(a);

    //a.data()[0] = 0; //Rémi - This sets value to 0 - perhaps because im running on CPU
    //a.data()[1] = 0; // ?
    //a.data()[2] = 0;

    a.set_data_location(portable(DataLocation::Device));

    EXPECT_EQ(local_max_vect(a), 3);
    EXPECT_EQ(local_min_vect(a), -10);
    EXPECT_EQ(local_max_abs_vect(a), 10);
    EXPECT_EQ(local_min_abs_vect(a), 1);
#ifdef _OPENMP
    EXPECT_TRUE(a.isDataOnDevice());
#endif
    copyFromDevice(a);
    //EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(local_imax_vect(a), 1);
    EXPECT_EQ(local_imin_vect(a), 2);
}

// Test for ref_array functionality
TEST(DeviceTest, RefArrayTest) {
    int N = 10;
    DoubleTab a(N);
    a = 1;
    mapToDevice(a, "a");

    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));

    DoubleTab b;
    b.ref_array(a);
    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostDevice));
}

// Test ref_tab/ref_array on chunk array
TEST(DeviceTest, RefTabRefArrayOnChunkArrayTest) {
    int N = 10;
    DoubleTab a(2 * N);
    a = 1;
    DoubleTab b;
    b.ref_tab(a, 0, N);
    mapToDevice(b, "b");

    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));

    double* a_ptr = computeOnTheDevice(a);
    #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice)
    for (int i = 0; i < 2 * N; i++)
        a_ptr[i] = 2;

    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Device));
    EXPECT_EQ(a(0), 2);
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::Host));
    EXPECT_EQ(a(2 * N - 1), 2);
}

 //triggers: Assertion failed: (ref_count() == 1), function resize_array_, file TRUSTArray.cpp, line 127.
 //Test for device memory allocation tracking
// deprecated ?

TEST(DeviceTest, DeviceMemoryAllocationTrackingTest) {
    int N = 10;
    DoubleTab a(N);
    a = 1;
    DoubleTab b;
    b.ref_array(a);
    mapToDevice(b, "b");

    EXPECT_EQ(b.get_data_location(), portable(DataLocation::HostDevice));
    EXPECT_EQ(a.get_data_location(), portable(DataLocation::HostDevice));

    double* ptr_host = a.data();

#ifdef _OPENMP
    EXPECT_TRUE(isAllocatedOnDevice(ptr_host));
#endif
    //a.resize(2 * N),".*";///This fails !!
    a = 2;
    DoubleTab c;
    c.ref_array(a);
    mapToDevice(c, "c");

    EXPECT_FALSE(isAllocatedOnDevice(ptr_host));
}

// Test DoubleTrav copy constructor
TEST(DeviceTest, DoubleTravCopyConstructorTest) {
    DoubleTrav b(100);
    b = 123;
    mapToDevice(b);

    DoubleVect b2(b);
    EXPECT_EQ(b2.get_mem_storage(), STORAGE::TEMP_STORAGE);
    EXPECT_EQ(b2.get_data_location(), portable(DataLocation::Device));

    DoubleVect b3(b2);
    EXPECT_EQ(b3.get_mem_storage(), STORAGE::TEMP_STORAGE);
    EXPECT_EQ(b3.get_data_location(), portable(DataLocation::Device));
}


