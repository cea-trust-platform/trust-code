/**
 * @file solve.cpp
 * @brief Definition of member functions regarding solving in AmgXSolver.
 * @author Pi-Yueh Chuang (pychuang@gwu.edu)
 * @date 2016-01-08
 * \copyright Copyright (c) 2015-2019 Pi-Yueh Chuang, Lorena A. Barba.
 *            This project is released under MIT License.
 */


// AmgXWrapper
# include "AmgXSolver.hpp"
#include <iostream>

/* \implements AmgXSolver::solve */
PetscErrorCode AmgXSolver::solve(Vec& p, Vec& b)
{
  PetscFunctionBeginUser;

  PetscErrorCode      ierr;

  if (globalSize != gpuWorldSize)
    {
      ierr = VecScatterBegin(scatterRhs,
                             b, redistRhs, INSERT_VALUES, SCATTER_FORWARD);
      CHK;
      ierr = VecScatterBegin(scatterLhs,
                             p, redistLhs, INSERT_VALUES, SCATTER_FORWARD);
      CHK;

      ierr = VecScatterEnd(scatterRhs,
                           b, redistRhs, INSERT_VALUES, SCATTER_FORWARD);
      CHK;
      ierr = VecScatterEnd(scatterLhs,
                           p, redistLhs, INSERT_VALUES, SCATTER_FORWARD);
      CHK;

      if (gpuWorld != MPI_COMM_NULL)
        {
          ierr = solve_real(redistLhs, redistRhs);
          CHK;
        }
      ierr = MPI_Barrier(globalCpuWorld);
      CHK;

      ierr = VecScatterBegin(scatterLhs,
                             redistLhs, p, INSERT_VALUES, SCATTER_REVERSE);
      CHK;
      ierr = VecScatterEnd(scatterLhs,
                           redistLhs, p, INSERT_VALUES, SCATTER_REVERSE);
      CHK;
    }
  else
    {
      if (gpuWorld != MPI_COMM_NULL)
        {
          ierr = solve_real(p, b);
          CHK;
        }
      ierr = MPI_Barrier(globalCpuWorld);
      CHK;
    }

  PetscFunctionReturn(0);
}


/* \implements AmgXSolver::solve_real */
PetscErrorCode AmgXSolver::solve_real(Vec& p, Vec& b)
{
  PetscFunctionBeginUser;

  PetscErrorCode      ierr;

  double              *unks,
                      *rhs;

  int                 size;

  AMGX_SOLVE_STATUS   status;

  // get size of local vector (p and b should have the same local size)
  ierr = VecGetLocalSize(p, &size);
  CHK;

  // get pointers to the raw data of local vectors
  ierr = VecGetArray(p, &unks);
  CHK;
  ierr = VecGetArray(b, &rhs);
  CHK;

  // upload vectors to AmgX
  AMGX_vector_upload(AmgXP, size, 1, unks);
  AMGX_vector_upload(AmgXRHS, size, 1, rhs);

  // solve
  ierr = MPI_Barrier(gpuWorld);
  CHK;
  AMGX_solver_solve(solver, AmgXRHS, AmgXP);

  // get the status of the solver
  AMGX_solver_get_status(solver, &status);

  // check whether the solver successfully solve the problem
  if (status != AMGX_SOLVE_SUCCESS) SETERRQ1(globalCpuWorld,
                                               PETSC_ERR_CONV_FAILED, "AmgX solver failed to solve the system! "
                                               "The error code is %d.\n", status);

  // download data from device
  AMGX_vector_download(AmgXP, unks);

  // restore PETSc vectors
  ierr = VecRestoreArray(p, &unks);
  CHK;
  ierr = VecRestoreArray(b, &rhs);
  CHK;

  PetscFunctionReturn(0);
}


/* \implements AmgXSolver::solve */
PetscErrorCode AmgXSolver::solve(PetscScalar* p, const PetscScalar* b, const int nRows, const double seuil)
{
  PetscFunctionBeginUser;

  int ierr;

  if (consolidationStatus == ConsolidationStatus::Device)
    {
      CHECK(cudaMemcpy((void**)&pCons[rowDispls[myDevWorldRank]], p, sizeof(PetscScalar) * nRows, cudaMemcpyDefault));
      CHECK(cudaMemcpy((void**)&rhsCons[rowDispls[myDevWorldRank]], b, sizeof(PetscScalar) * nRows, cudaMemcpyDefault));

      // Must synchronize here as device to device copies are non-blocking w.r.t host
      CHECK(cudaDeviceSynchronize());
      ierr = MPI_Barrier(devWorld);
      CHK;
    }
  else if (consolidationStatus == ConsolidationStatus::Host)
    {
      MPI_Request req[2];
      ierr = MPI_Igatherv(p, nRows, MPI_DOUBLE, &pCons[rowDispls[myDevWorldRank]], nRowsInDevWorld.data(), rowDispls.data(), MPI_DOUBLE, 0, devWorld, &req[0]);
      CHK;
      ierr = MPI_Igatherv(b, nRows, MPI_DOUBLE, &rhsCons[rowDispls[myDevWorldRank]], nRowsInDevWorld.data(), rowDispls.data(), MPI_DOUBLE, 0, devWorld, &req[1]);
      CHK;
      MPI_Waitall(2, req, MPI_STATUSES_IGNORE);
    }

  if (gpuWorld != MPI_COMM_NULL)
    {
      // Upload potentially consolidated vectors to AmgX
      if (consolidationStatus == ConsolidationStatus::None)
        {
          AMGX_vector_upload(AmgXP, nRows, 1, p);
          AMGX_vector_upload(AmgXRHS, nRows, 1, b);
        }
      else
        {
          AMGX_vector_upload(AmgXP, nConsRows, 1, pCons);
          AMGX_vector_upload(AmgXRHS, nConsRows, 1, rhsCons);
        }

      ierr = MPI_Barrier(gpuWorld);
      CHK;

      // On calcule le residu avant le solve pour eviter un crash si deja converge
      double t_norm = 0;
      AMGX_solver_calculate_residual_norm(solver, AmgXA, AmgXRHS, AmgXP, &t_norm);
      if (t_norm > seuil)
        {
          // Solve
          AMGX_solver_solve(solver, AmgXRHS, AmgXP);

          // Get the status of the solver
          AMGX_SOLVE_STATUS status;
          AMGX_solver_get_status(solver, &status);

          // Check whether the solver successfully solved the problem
          if (status != AMGX_SOLVE_SUCCESS)
            SETERRQ1(globalCpuWorld,
                     PETSC_ERR_CONV_FAILED, "AmgX solver failed to solve the system! "
                     "The error code is %d.\n",
                     status);
        }
      // Download data from device
      if (consolidationStatus == ConsolidationStatus::None)
        {
          AMGX_vector_download(AmgXP, p);
        }
      else
        {
          AMGX_vector_download(AmgXP, pCons);

          // AMGX_vector_download invokes a device to device copy here, so it is essential that
          // the root rank blocks the host before other ranks copy from the consolidated solution
          CHECK(cudaDeviceSynchronize());
        }
    }

  // If the matrix is consolidated, scatter the
  if (consolidationStatus == ConsolidationStatus::Device)
    {
      // Must synchronise before each rank attempts to read from the consolidated solution
      ierr = MPI_Barrier(devWorld);
      CHK;

      CHECK(cudaMemcpy((void **) p, &pCons[rowDispls[myDevWorldRank]], sizeof(PetscScalar) * nRows,
                       cudaMemcpyDefault));
      CHECK(cudaDeviceSynchronize());
    }
  else if (consolidationStatus == ConsolidationStatus::Host)
    {
      // Must synchronise before each rank attempts to read from the consolidated solution
      ierr = MPI_Barrier(devWorld);
      CHK;

      ierr = MPI_Scatterv(&pCons[rowDispls[myDevWorldRank]], nRowsInDevWorld.data(), rowDispls.data(), MPI_DOUBLE,
                          p, nRows, MPI_DOUBLE, 0, devWorld);
      CHK;
    }
  ierr = MPI_Barrier(globalCpuWorld);
  CHK;

  PetscFunctionReturn(0);
}