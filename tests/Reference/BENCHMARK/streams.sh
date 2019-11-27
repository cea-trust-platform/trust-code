#!/bin/bash
export PETSC_DIR=$TRUST_ROOT/lib/src/LIBPETSC/petsc/linux_opt
cp -f $PETSC_DIR/src/benchmarks/streams/* .

MPI_BINDING=""
MPIEXEC="mpiexec $MPI_BINDING"
if [ $TRUST_WITHOUT_HOST = 1 ]
then
   # Simple PC:
   let NPMAX=$TRUST_NB_PROCS
else
   # Cluster, on fait tourner sur au moins 2 nodes:
   let NPMAX=$TRUST_NB_PROCS+$TRUST_NB_PROCS
fi
make streams NPMAX=$NPMAX MPIEXEC=$MPIEXEC
for suffix in png log
do
   echo "Creating scaling_$$.$suffix"
   mv -f scaling.$suffix scaling_$$.$suffix
done
