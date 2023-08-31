#include <iostream>
#include <omp.h>
#include <chrono>
#include <cuda_runtime.h>

int main() {
    #pragma omp target
    {;}
    // Exemple: 1 millions mailles 100^3, CL 6xn^2, donc 60000 
    const int N = 60000; // Gain interessant sur les petits envoi de donnees:
//[Pageable memory] Mean time taken to copy data H->D: 5.14008e-05 s 8.69704 GB/s
//[Pageable memory] Mean time taken to copy data D->H: 5.34574e-05 s 8.36245 GB/s
//[Pinned memory]   Mean time taken to copy data H->D: 2.40212e-05 s 18.61 GB/s
//[Pinned memory]   Mean time taken to copy data D->H: 2.24864e-05 s 19.8802 GB/s
    const int samples = 10;
    // Initialize data on the host
    double *host_data = new double[N]();

    for (int i = 0; i < N; i++)
        host_data[i] = 0;
    #pragma omp target enter data map(alloc:host_data[0:N])
    
    double HtoD=0;
    double DtoH=0; 
    for (int sample=0;sample<samples;sample++)
    {
       auto start = std::chrono::high_resolution_clock::now();
       #pragma omp target update to(host_data[0:N])
       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double> diff = end - start;
       HtoD += diff.count();       

       #pragma omp target teams distribute parallel for
       for (int i = 0; i < N; i++)
          host_data[i] = 1;

       // Copy data from device to host
       start = std::chrono::high_resolution_clock::now();
       #pragma omp target update from(host_data[0:N])
       end = std::chrono::high_resolution_clock::now();
       diff = end - start;
       DtoH += diff.count();
       if (host_data[10]!=1) abort();
    }
    #pragma omp target exit data map(release:host_data[0:N])
    HtoD/=samples;
    DtoH/=samples;
    std::cout << "[Pageable memory] Mean time taken to copy data H->D: " << HtoD << " s " << 8.0*N/(1024*1024*1024)/HtoD << " GB/s\n"; 
    std::cout << "[Pageable memory] Mean time taken to copy data D->H: " << DtoH << " s " << 8.0*N/(1024*1024*1024)/DtoH << " GB/s\n"; 

    // Pin memory on the host
    cudaError_t err = cudaHostRegister(host_data, N * sizeof(double), cudaHostRegisterDefault);
    double* pinned_mem = host_data;
    
    for (int i = 0; i < N; i++)
        pinned_mem[i] = 0;
    #pragma omp target enter data map(alloc:pinned_mem[0:N])

    HtoD=0;
    DtoH=0;
    for (int sample=0;sample<samples;sample++)
    {
       // Copy data from host to device using OpenMP target directives
       auto start = std::chrono::high_resolution_clock::now();
       #pragma omp target update to(pinned_mem[0:N])
       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double> diff = end - start;
       HtoD += diff.count();
        
       // Compute data on device:
       #pragma omp target teams distribute parallel for
       for (int i = 0; i < N; i++)
          pinned_mem[i] = 1;
    
       // Copy data from device to host
       start = std::chrono::high_resolution_clock::now();
       #pragma omp target update from(pinned_mem[0:N])
       end = std::chrono::high_resolution_clock::now();
       diff = end - start;
       DtoH += diff.count();
       if (pinned_mem[10]!=1) abort();
    }
    #pragma omp target exit data map(release:pinned_mem[0:N])
    HtoD/=samples;
    DtoH/=samples;
    std::cout << "[Pinned memory]   Mean time taken to copy data H->D: " << HtoD << " s " << 8.0*N/(1024*1024*1024)/HtoD << " GB/s\n"; 
    std::cout << "[Pinned memory]   Mean time taken to copy data D->H: " << DtoH << " s " << 8.0*N/(1024*1024*1024)/DtoH << " GB/s\n"; 

    cudaHostUnregister(pinned_mem);
    return 0;
}

