#include <iostream>
#include <omp.h>
#include <chrono>
#include <cuda_runtime.h>

int main() {
    #pragma omp target
    {;}
    int N=100;
    char* host_data = new char[N]();
    #pragma omp target enter data map(alloc:host_data[0:N])
    std::cerr << "Host pointer  :" << (void*)host_data << std::endl;
    // Example on how to get device pointer:
    #pragma omp target data use_device_ptr(host_data)
    {
       std::cerr << "Device pointer:" << (void*)host_data << std::endl;
    }
    std::cerr << "Host pointer  :" << (void*)host_data << std::endl;
    #pragma omp target exit data map(release:host_data[0:N])
    return 0;
}

