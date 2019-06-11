#!/bin/bash

# This script has to be removed and the gmsh_exec renamed into gmsh in $TRUST_ROOT/Outils/Gmsh/install_gmsh once MED will be compiled with cmake+hdf5 from conda

direc=$(dirname $0)

LD_LIBRARY_PATH=$TRUST_ROOT/exec/python/lib:$LD_LIBRARY_PATH 
${direc}/gmsh_exec $@ 

