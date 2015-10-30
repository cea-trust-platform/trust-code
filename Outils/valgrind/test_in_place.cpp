#include <mpi.h>
#include <iostream>

int main_without_finalize(int argc,char **argv)
{
    int i, rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    double dt=1.*rank;
    MPI_Allreduce(MPI_IN_PLACE, &dt,   1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    std::cerr<<" dt "<<dt<<std::endl;
    MPI_Barrier(MPI_COMM_WORLD);            
    
    return 0;
}

int main(int argc, char **argv)
{
  MPI_Init(&argc,&argv);
  
  int res = main_without_finalize(argc, argv);
  MPI_Finalize();
  return res;
}
