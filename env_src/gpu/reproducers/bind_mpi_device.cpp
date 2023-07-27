#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <chrono>
//#include <cuda_runtime.h>

int AmgXWrapperScheduling(int rank, int nRanks, int nDevs)
{
  int devID;
  if (nRanks <= nDevs) // Less process than devices
    devID = rank;
  else // More processes than devices
    {
      int nBasic = nRanks / nDevs,
          nRemain = nRanks % nDevs;
      if (rank < (nBasic+1)*nRemain)
        devID = rank / (nBasic + 1);
      else
        devID = (rank - (nBasic+1)*nRemain) / nBasic + nRemain;
    }
  return devID;
}

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);
  MPI_Comm localWorld;
  MPI_Comm globalWorld = MPI_COMM_WORLD;
  MPI_Comm_split_type(globalWorld, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &localWorld);
  int rank; // Local rank
  MPI_Comm_rank(localWorld, &rank);
  int nRanks; // Local number of ranks
  MPI_Comm_size(localWorld, &nRanks);
  // Node name:
  int     len;
  char    name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(name, &len);
  std::string nodeName = name;
  int nDevs = omp_get_num_devices(); // Local number of devices
  int devID = AmgXWrapperScheduling(rank, nRanks, nDevs);
  std::cerr << "Initializing OpenMP offload on devices..."  << std::endl;
  std::cerr << "[OpenMP] Assigning local rank " << rank << " (global rank " << rank << ") of node " << nodeName.c_str() << " to its device " << devID << "/" << nDevs-1 << std::endl;
  omp_set_default_device(devID);
  // Dummy target region, so as not to measure startup time later:
  #pragma omp target
  { ; }
  MPI_Finalize();
}

