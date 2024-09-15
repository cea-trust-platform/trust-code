#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cuda_runtime.h>

int main(int argc, char** argv) {
    int world_size, world_rank;
    
    if (getenv("SLURM_LOCALID")!=NULL)
    {
       // Cluster multi-gpu
       char* local_rank_env = getenv("SLURM_LOCALID");
       int rank = atoi(local_rank_env);
       cudaError_t cudaRet = cudaSetDevice(rank);
       if(cudaRet != cudaSuccess)
         {
          printf("Error: cudaSetDevice failed\n");
          abort();
         }
       else
          std::cout << "Set MPI rank " << rank << " to device " << rank << std::endl;
	  
       MPI_Init(NULL, NULL);
       MPI_Comm_size(MPI_COMM_WORLD, &world_size);
       MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);  
    }
    else
    {
       // PC mono-gpu

       MPI_Init(NULL, NULL);
       MPI_Comm_size(MPI_COMM_WORLD, &world_size);
       MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

       char* local_rank_env = getenv("OMPI_COMM_WORLD_RANK");
       int rank = atoi(local_rank_env);
       int device = 0;
       cudaError_t cudaRet = cudaSetDevice(device);
       if(cudaRet != cudaSuccess)
         {
          printf("Error: cudaSetDevice failed\n");
          abort();
         }
       else
          std::cout << "Set MPI rank " << rank << " to device " << device << std::endl;    
    }
    
    const int N = 1000000;  // Adjust this based on the size of data you're sending
    std::vector<double> array(N, 1.0f);  // Initialize with some data
    double* host_data = array.data();
    // Initialize data on device:
    double* device_data;
    cudaMalloc((void**)&device_data, N * sizeof(double));
    cudaMemcpy(device_data, host_data, N * sizeof(double), cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();


    MPI_Barrier(MPI_COMM_WORLD); // Make sure all processes are ready before starting the clock
    auto start = std::chrono::high_resolution_clock::now();
    if (world_rank == 0) {
        cudaMemcpy(host_data, device_data, N, cudaMemcpyDeviceToHost);
        MPI_Send(host_data, N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    } else if (world_rank == 1) {
        MPI_Recv(host_data, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	cudaMemcpy(device_data, host_data, N, cudaMemcpyHostToDevice);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    if (world_rank == 1) {
        std::cout << "Time taken for MPI Send/Receive on the HOST: " << diff.count() << " s " << 8.0*N/(1024*1024*1024)/diff.count() << " GB/s\n";
        std::cout << "If it crash now, that means your MPI is not GPU-Aware:\n";
    }


    MPI_Barrier(MPI_COMM_WORLD); // Make sure all processes are ready before starting the clock
    
    start = std::chrono::high_resolution_clock::now();
    if (world_rank == 0) {
        MPI_Send(device_data, N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    } else if (world_rank == 1) {
        MPI_Recv(device_data, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
        
    MPI_Barrier(MPI_COMM_WORLD);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;

    if (world_rank == 1) {
        std::cout << "Time taken for MPI Send/Receive with GPU-Direct: " << diff.count() << " s " << 8.0*N/(1024*1024*1024)/diff.count() << " GB/s\n";
    }

    cudaFree(device_data);
    MPI_Finalize();
}

