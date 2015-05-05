#!/bin/bash
NB_PROCS=$1
shift
modules=$*
for mpi in $modules
do
   module unload $modules
   module load $mpi
   exec=osu-$mpi/libexec/osu-micro-benchmarks/mpi/collective/osu_allreduce
   if [ -f $exec ]
   then
      mpirun -np $NB_PROCS $exec 1>`basename $exec`-$mpi.out 2>&1
   fi
done   
