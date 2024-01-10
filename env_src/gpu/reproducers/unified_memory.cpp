#include <iostream>
#include <omp.h>
#include <chrono>
#include <cuda_runtime.h>

int main() {
    #pragma omp target
    {;}
    // Define the size of the array
    const int N = 1000000;

    {
       double *data = new double[N];
       // Initialize data to 1 on the host
       for (int i = 0; i < N; i++) 
          data[i] = 1;
       if (data[0]!=1) std::cerr << "Error: data[0]=" << data[0] << " should be 1." << std::endl;
       #pragma omp target enter data map(alloc:data[0:N])       
       // Perform the computation on the device with explicite copy:
       auto start = std::chrono::high_resolution_clock::now();
       #pragma omp target update to(data[0:N])
       #pragma omp target teams distribute parallel for
       for (int i = 0; i < N; i++)
           data[i] = data[i] * 2;       
       
       std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
       std::cout << "[Non Unified Memory] H2D explicit copy then Device Kernel: " << 1000*diff.count() << " ms " << std::endl;   

       // Loop on host with explicity copy: 
       start = std::chrono::high_resolution_clock::now();
       int OK=1;
       #pragma omp target update from(data[0:N])
       for (int i = 0; i < N; i++)
          if (data[i]!=2) OK=0;
       diff = std::chrono::high_resolution_clock::now() - start;
       std::cout << "[Non Unified Memory] D2H explicit copy then Host Kernel: " << 1000*diff.count() << " ms " << std::endl;   
       std::cerr << (!OK ? "Error!" : "OK!") << std::endl;       
       // Free memory
       free(data);
    }

    {
       // Allocate Unified Memory
       double *data;
       cudaMallocManaged(&data, N*sizeof(double));
       
       // Initialize data to 1 on the host
       for (int i = 0; i < N; i++)
          data[i] = 1;
       if (data[0]!=1) std::cerr << "Error: data[0]=" << data[0] << " should be 1." << std::endl;

       // Perform the computation on the device : data is automatically copied to device
       auto start = std::chrono::high_resolution_clock::now();
       #pragma omp target teams distribute parallel for
       for (int i = 0; i < N; i++)
           data[i] = data[i] * 2;

       // Wait for GPU to finish before accessing on host
       cudaDeviceSynchronize();
       std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
       std::cout << "[Non Unified Memory] H2D automatic copy during Device Kernel: " << 1000*diff.count() << " ms " << std::endl;   
 
       // Normally thanks to UM, data is updated on the host:
       start = std::chrono::high_resolution_clock::now();
       int OK=1;
       for (int i = 0; i < N; i++)
          if (data[i]!=2) OK=0;
       diff = std::chrono::high_resolution_clock::now() - start;
       std::cout << "[Non Unified Memory] D2H automatic copy during Host Kernel: " << 1000*diff.count() << " ms " << std::endl;   
       std::cerr << (!OK ? "Error!" : "OK!") << std::endl;       
       // Free memory
       cudaFree(data);
    }
    return 0;
}

