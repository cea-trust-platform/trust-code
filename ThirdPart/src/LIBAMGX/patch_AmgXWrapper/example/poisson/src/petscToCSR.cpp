/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * \file petscToCSR.cpp
 * \brief Converts between a PETSc matrix and a raw CSR format
 * \author Matt Martineau (NVIDIA mmartineau@nvidia.com)
 * \date 2020-08-05
 */

// headers
#include "petscToCSR.hpp"
#include <iostream>
#include <cstring>

PetscErrorCode petscToCSR(
    Mat &A,
    PetscInt& nRowsLocal,
    PetscInt& nRowsGlobal,
    PetscInt& nNz,
    const PetscInt*& rowOffsets,
    const PetscInt*& colIndices,
    PetscScalar*& values,
    double*& lhs,
    double*& rhs,
    Vec& lhs_petsc,
    Vec& rhs_petsc)
{
    PetscFunctionBeginUser;

    PetscBool done;
    MatType type;
    Mat localA;

    // Get the Mat type
    PetscErrorCode ierr = MatGetType(A, &type); CHKERRQ(ierr);

    // Check whether the Mat type is supported
    if (std::strcmp(type, MATSEQAIJ) == 0) // sequential AIJ
    {
        // Make localA point to the same memory space as A does
        localA = A;
    }
    else if (std::strcmp(type, MATMPIAIJ) == 0)
    {
        // Get local matrix from redistributed matrix
        ierr = MatMPIAIJGetLocalMat(A, MAT_INITIAL_MATRIX, &localA); CHKERRQ(ierr);
    }
    else
    {
        SETERRQ1(PETSC_COMM_WORLD, PETSC_ERR_ARG_WRONG,
                 "Mat type %s is not supported!\n", type);
    }

    // Get row and column indices in compressed row format
    ierr = MatGetRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE,
                       &nRowsLocal, &rowOffsets, &colIndices, &done); CHKERRQ(ierr);

    ierr = MatSeqAIJGetArray(localA, &values); CHKERRQ(ierr);

    // Get pointers to the raw data of local vectors
    ierr = VecGetArray(lhs_petsc, &lhs); CHKERRQ(ierr);
    ierr = VecGetArray(rhs_petsc, &rhs); CHKERRQ(ierr);

    // Calculate the number of rows in nRowsGlobal
    ierr = MPI_Allreduce(&nRowsLocal, &nRowsGlobal, 1, MPI_INT, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
    // Store the number of non-zeros
    nNz = rowOffsets[nRowsLocal];
    std::cout << "nRowsLocal=" << nRowsLocal << std::endl;
    std::cout << "nRowsGlobal=" << nRowsGlobal << std::endl;
    std::cout << "nNz=" << nNz << std::endl;
    PetscFunctionReturn(0);
}
