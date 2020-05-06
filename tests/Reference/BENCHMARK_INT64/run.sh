#!bin/bash
# Run a 1,600,000,000 cells benchmark on 25600 cores on rome partition (needs a int64 binary):
[ "$TRUST_ROOT" = "" ] && echo "Initialize Trust environment." && exit -1

# Create the paritioned mesh:
mkdir -p Zones Zones_refined Zones_refined_refined
gunzip -c cylindre.geom.gz > cylindre.geom
make_PAR.data Cx 25600

# Run:
srun -p rome -n 25600 $exec PAR_Cx 25600 -journal=0 2>&1 | tee PAR_Cx.log

