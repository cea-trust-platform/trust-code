#!/bin/bash
# Read a PETSc matrix and solve (compute condition number):
echo "Usage: $0 [-n NP] [-cn] matrix.petsc [petsc_options]"
# See https://www.mcs.anl.gov/petsc/petsc-current/src/ksp/ksp/tutorials/ex10.c.html
exec=`pwd`/ex10
if [ ! -f $exec ]
then
   dir=$PETSC_ROOT/linux_opt/share/petsc/examples/src/ksp/ksp/tutorials
   cp $dir/ex10.c $dir/makefile .
   make ex10 1>/dev/null 2>&1 || exit -1
fi
if [ "$1" != "" ]
then
   [ "$1" = -n ] && shift && NP=$1 && shift
   # See https://www.mcs.anl.gov/petsc/documentation/faq.html#conditionnumber: -pc_type jacobi pour voir l'effet du preconditionnement
   [ "$1" = -cn ] && shift && cn="-pc_type none -ksp_type gmres -ksp_monitor_singular_value -ksp_gmres_restart 1000"
   filename=$1 && shift && [ "$NP" = "" ] && NP=`echo $filename | awk -F_ '{print $4}'`
   petsc_options=$*
   echo "./ex10 -f $filename -ksp_view -ksp_monitor $cn $petsc_options 1>${filename%.petsc}.log 2>&1"
   trust dummy $NP -f $filename -ksp_view -ksp_monitor $cn $petsc_options 1>${filename%.petsc}.log 2>&1
fi
