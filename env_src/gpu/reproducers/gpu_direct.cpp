#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cuda_runtime.h>

int main(int argc, char** argv) {
    if (getenv("SLURM_LOCALID")!=NULL)
    {
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
    }
    else
    {
       std::cout << "No binding between MPI ranks and device(s)" << std::endl;
    }
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const int N = 1000000;  // Adjust this based on the size of data you're sending
    std::vector<double> data(N, 1.0f);  // Initialize with some data

    MPI_Barrier(MPI_COMM_WORLD); // Make sure all processes are ready before starting the clock
    auto start = std::chrono::high_resolution_clock::now();
    if (world_rank == 0) {
        MPI_Send(data.data(), N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    } else if (world_rank == 1) {
        MPI_Recv(data.data(), N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    if (world_rank == 1) {
        std::cout << "Time taken for MPI Send/Receive on the HOST: " << diff.count() << " s " << 8.0*N/(1024*1024*1024)/diff.count() << " GB/s\n";
        std::cout << "If it crash now, that means your MPI is not GPU-Aware:\n";
    }


    double* device_data;
    cudaMalloc((void**)&device_data, N * sizeof(double));

    if (world_rank == 0) {
        // Initialize data on device in rank 0 process
        // This could be done with a CUDA kernel
        double* host_data = new double[N];
        for (int i = 0; i < N; i++) {
            host_data[i] = 1.0f;
        }
        cudaMemcpy(device_data, host_data, N * sizeof(double), cudaMemcpyHostToDevice);
        delete[] host_data;
    }
    
    MPI_Barrier(MPI_COMM_WORLD); // Make sure all processes are ready before starting the clock
    start = std::chrono::high_resolution_clock::now();
    if (world_rank == 0) {
        MPI_Send(device_data, N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    } else if (world_rank == 1) {
        MPI_Recv(device_data, N, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;

    if (world_rank == 1) {
        std::cout << "Time taken for MPI Send/Receive with GPU-Direct: " << diff.count() << " s " << 8.0*N/(1024*1024*1024)/diff.count() << " GB/s\n";
    }

    cudaFree(device_data);
    MPI_Finalize();
}

