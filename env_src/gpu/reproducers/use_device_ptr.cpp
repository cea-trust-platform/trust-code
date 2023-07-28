#include <iostream>
#include <omp.h>
#include <chrono>
#include <cuda_runtime.h>
#include <mpi.h>

int main() {
    // Binding sommaire entre processes et devices:
    MPI_Init(NULL, NULL);  
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int nDevs = omp_get_num_devices();
    int devID = rank >= nDevs ? nDevs-1 : rank;
    omp_set_default_device(devID);
    std::cerr << "[OpenMP] Assigning local rank " << rank << " to device " << devID << "/" << nDevs-1 << std::endl;

    #pragma omp target
    {;}
    int N=100;
    char* host_data = new char[N]();
    #pragma omp target enter data map(alloc:host_data[0:N])
    std::cerr << "[" << rank << "] Host pointer  :" << (void*)host_data << std::endl;
    // Example on how to get device pointer:
    #pragma omp target data use_device_ptr(host_data)
    {
       std::cerr << "[" << rank << "] Device pointer:" << (void*)host_data << std::endl;
    }
    std::cerr << "[" << rank << "] Host pointer  :" << (void*)host_data << std::endl;
    #pragma omp target exit data map(release:host_data[0:N])

    MPI_Finalize();
    return 0;
}

