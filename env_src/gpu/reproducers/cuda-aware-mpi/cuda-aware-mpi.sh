#!/bin/bash
# Importance de faire le mapping avant le MPI_Init
make || exit -1
N=2 && dec="2 1"
N=4 && dec="2 2"
N=8 && dec="4 2"
# Fonctionne sur JeanZay (module sw openmpi/4.1.5-cuda) avec un gain de -50% sur les communications entre 2 noeuds GPU:
#Total Jacobi run time: 1.0161 sec.			      |	Total Jacobi run time: 0.9470 sec.
#Average per-process communication time: 0.1564 sec.	      |	Average per-process communication time: 0.0873 sec.
#Measured lattice updates: 132.04 GLU/s (total), 16.50 GLU/s ( |	Measured lattice updates: 141.68 GLU/s (total), 17.71 GLU/s (
#Measured FLOPS: 660.19 GFLOPS (total), 82.52 GFLOPS (per proc |	Measured FLOPS: 708.40 GFLOPS (total), 88.55 GFLOPS (per proc
for exec in jacobi_cuda_normal_mpi jacobi_cuda_aware_mpi
do
   echo -e "\n\n==== $exec ===="
   touch $exec.data
   exec=`pwd`/$exec trust $exec.data $N -t $dec
done
