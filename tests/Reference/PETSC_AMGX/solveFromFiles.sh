#!/bin/bash
# Resout Ax=B en lisant une matrice PETSc, un second membre et la solution exacte (optionel)
echo "Usage: $0 file." 
#[ ! -f "$1" ] && exit 1
$TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper/example/bin/solveFromFiles -caseName amgx -mode AmgX_GPU -cfgFileName $1 -matrixFileName $2 -rhsFileName $3 -exactFileName $4
