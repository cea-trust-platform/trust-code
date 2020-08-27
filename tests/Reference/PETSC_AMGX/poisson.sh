#!/bin/bash
# Teste la validate du fichier .amgx avec le cas poisson de AmgXWrapper
echo "Usage: $0 file.amgx" 
#[ ! -f "$1" ] && exit 1
valgrind="" && [ "$VALGRIND" != "" ] && valgrind=valgrind
$valgrind $TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper/example/poisson/bin/poisson -caseName amgx -mode AmgX_GPU -cfgFileName $1 -Nx 10 -Ny 10 -Nz 10
