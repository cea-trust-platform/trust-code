#!/bin/bash
cd $TRUST_ROOT/lib/src/LIBAMGX/AmgXWrapper/example/poisson
# Test valgrind:
#[ "$TRUST_WITHOUT_HOST" = 1 ] && valgrind bin/poisson -caseName valgrind -mode AmgX_GPU -cfgFileName configs/AmgX_SolverOptions_Classical.info -Nx 2 -Ny 2 -Nz 2
N=100 && [ "`grep P400 $TRUST_ROOT/env/card.log 2>/dev/null`" != "" ] && N=85
name=$N"x"$N"x"$N
exec=bin/poisson trust -gpu dummy 4 -caseName   PETSc_GAMG_$name -mode PETSc    -cfgFileName configs/PETSc_SolverOptions_GAMG.info     -Nx $N -Ny $N -Nz $N
exec=bin/poisson trust -gpu dummy 4 -caseName AmgX_GPU_AGG_$name -mode AmgX_GPU -cfgFileName configs/AmgX_SolverOptions_Classical.info -Nx $N -Ny $N -Nz $N
exec=bin/poisson trust -gpu dummy 4 -caseName AmgX_CSR_AGG_$name -mode AmgX_CSR -cfgFileName configs/AmgX_SolverOptions_Classical.info -Nx $N -Ny $N -Nz $N

   # ---------------------------------------------------------------------------	  
   # Machine	NVidia		GFlops	Cuda	Driver	GNU	MPI		solve	GAMG	AmgX_AGG	 
   # is242981	P400 (2Go)	   	10.2	11.0   10.0.1	OpenMPI 4.0.3   	0.90s   0.78s
   # is234639	P600 		 1.1	10.0	10.2	7.4.0	OpenMPI 4.0.3		1.83s	0.67s
   # is225303 	K4000 		 1.2   	 8.0 	 9.0	5.4.0	MPICH 3.2		2.08s	crash Cuda failure: 'invalid device function' dense_lu_solver.cu:698
   # portable   GTX 1050 Ti      2.1	11.1	11.2	9.3.0   OpenMPI 4.0.3		2.07s	0.46s
   # is240831	P2000		 3.0	10.1	10.1	7.5.0	MPICH 3.2		1.72s   0.30s
   # irene	P100-PCIE-12Go	10.6	10.2	10.2	7.3.0	OpenMPI 2.0.4		1.16s	0.11s
   # cobalt	P100-PCIE-16Go	14.1	10.2	10.2	7.3.0	OpenMPI 2.0.4		1.35s	0.08s
   # cobalt     V100 ?		14.1	10.2	10.2	7.3.0	OpenMPI 2.0.4		?	?
   # orcus	V100-PCIE-16Go 	14.1	 9.2 	10.1	6.5.0				1.32s	0.07s
   # jeanzay	V100-SXM2-16Go	14.0	10.2	10.2	8.3.1	OpenMPI	4.0.2-cuda	1.15s	0.048s (7500Euros...)
   # topaze	A100-SXM-80GB 	?	11.2		8.3.0	OpenMPI 4.0.5		0.65s	0.039s

