# When using MPI GPU Aware, PETSc solvers on GPU in TRUST doesn't converge well
# Vu avec OpenMPI 4.0.5, 4.1.0, 4.1.5 de OpenMPI (officiel ou NVHPC)
# Semble OK avec OpenMPI 5.0.5 !
make_PAR.data mpi_gpu_aware_issue_petsc
NP=$?
TRUST_USE_MPI_GPU_AWARE=1 trust PAR_mpi_gpu_aware_issue_petsc $NP

