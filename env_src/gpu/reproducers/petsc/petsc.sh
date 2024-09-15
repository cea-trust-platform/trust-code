#!/bin/bash
echo "Usage: $0 [-mx NX -my NY -Nx NPx -Ny NPy] [petsc_options]"
exec=`pwd`/ex19
rm -f $exec
if [ ! -f $exec ]
then
   [ "$TRUST_CC_BASE_EXTP" != "" ] && export MPICH_CC=$TRUST_cc_BASE_EXTP && export OMPI_CC=$TRUST_cc_BASE_EXTP
   make `basename $exec` || exit -1
fi

NP=4 && [ "$1" = -n ] && shift && NP=$1 && shift
# Semble marcher sur JZ avec GPU-Direct;
petsc_options="-ksp_type cg -pc_type hypre -pc_hypre_boomeramg_strong_threshold 0.7 -mx 100 -my 100 -ksp_view -dm_vec_type cuda"
[ "$*" != "" ] && petsc_options=$*
trust dummy $NP $petsc_options
