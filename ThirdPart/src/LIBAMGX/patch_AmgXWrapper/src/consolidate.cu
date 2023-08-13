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
 * \file consolidate.cu
 * \brief Definition of member functions related to matrix consolidation.
 * \author Matt Martineau (mmartineau@nvidia.com)
 * \date 2020-07-31
 */

#include <AmgXSolver.hpp>

#include <numeric>

/*
    Changes local row offsets to describe the consolidated row space on
    the root rank.
*/
__global__ void fixConsolidatedRowOffsets(int nLocalRows, int offset, int* rowOffsets)
{
    for(int i = threadIdx.x + blockIdx.x*blockDim.x; i < nLocalRows; i += blockDim.x*gridDim.x)
    {
        rowOffsets[i] += offset;
    }
}

// A set of handles to the device data storing a consolidated CSR matrix
struct ConsolidationHandles
{
    cudaIpcMemHandle_t rhsConsHandle;
    cudaIpcMemHandle_t solConsHandle;
    cudaIpcMemHandle_t rowOffsetsConsHandle;
    cudaIpcMemHandle_t colIndicesConsHandle;
    cudaIpcMemHandle_t valuesConsHandle;
};

/* \implements AmgXSolver::initializeConsolidation */
PetscErrorCode AmgXSolver::initializeConsolidation(
    const PetscInt nLocalRows,
    const PetscInt nLocalNz,
    const PetscScalar* values)
{
    PetscFunctionBeginUser;

    // Check if multiple ranks are associated with a device
    if (devWorldSize == 1)
    {
        consolidationStatus = ConsolidationStatus::None;
        PetscFunctionReturn(0);
    }

    nRowsInDevWorld.resize(devWorldSize);
    nnzInDevWorld.resize(devWorldSize);
    rowDispls.resize(devWorldSize+1, 0);
    nzDispls.resize(devWorldSize+1, 0);

    // Fetch to all the number of local rows on each rank
    MPI_Request req[2];
    int ierr = MPI_Iallgather(&nLocalRows, 1, MPI_INT, nRowsInDevWorld.data(), 1, MPI_INT, devWorld, &req[0]); CHK;

    // Fetch to all the number of non zeros on each rank
    ierr = MPI_Iallgather(&nLocalNz, 1, MPI_INT, nnzInDevWorld.data(), 1, MPI_INT, devWorld, &req[1]); CHK;
    MPI_Waitall(2, req, MPI_STATUSES_IGNORE);

    // Calculate consolidate number of rows, non-zeros, and calculate row, non-zero displacements
    nConsNz = std::accumulate(nnzInDevWorld.begin(), nnzInDevWorld.end(), 0);
    nConsRows = std::accumulate(nRowsInDevWorld.begin(), nRowsInDevWorld.end(), 0);
    std::partial_sum(nRowsInDevWorld.begin(), nRowsInDevWorld.end(), rowDispls.begin()+1);
    std::partial_sum(nnzInDevWorld.begin(), nnzInDevWorld.end(), nzDispls.begin()+1);

    // Consolidate the CSR matrix data from multiple ranks sharing a single GPU to a
    // root rank, allowing multiple ranks per GPU. This allows overdecomposing the problem
    // when there are more CPU cores than GPUs, without the inefficiences of performing
    // the linear solve on multiple separate domains.
    // If the data is a device pointer then use IPC handles to perform the intra-GPU
    // copies from the allocations of different processes operating the same GPU.
    // Opening the handles as an initialization step means it is not necessary to
    // repeatedly call cudaIpcOpenMemHandle, which can be expensive.
    cudaPointerAttributes att;
    cudaError_t err = cudaPointerGetAttributes(&att, values);
    if (err != cudaErrorInvalidValue && att.type == cudaMemoryTypeDevice)
    {
        ConsolidationHandles handles;
        // The data is already on the GPU so consolidate there
        if (gpuProc == 0)
        {
            // We are consolidating data that already exists on the GPU
            CHECK(cudaMalloc((void**)&rhsCons, sizeof(PetscScalar) * nConsRows));
            CHECK(cudaMalloc((void**)&pCons, sizeof(PetscScalar) * nConsRows));
            CHECK(cudaMalloc((void**)&rowOffsetsCons, sizeof(PetscInt) * (nConsRows+1)));
            CHECK(cudaMalloc((void**)&colIndicesGlobalCons, sizeof(PetscInt) * nConsNz));
            CHECK(cudaMalloc((void**)&valuesCons, sizeof(PetscScalar) * nConsNz));

            CHECK(cudaIpcGetMemHandle(&handles.rhsConsHandle, rhsCons));
            CHECK(cudaIpcGetMemHandle(&handles.solConsHandle, pCons));
            CHECK(cudaIpcGetMemHandle(&handles.rowOffsetsConsHandle, rowOffsetsCons));
            CHECK(cudaIpcGetMemHandle(&handles.colIndicesConsHandle, colIndicesGlobalCons));
            CHECK(cudaIpcGetMemHandle(&handles.valuesConsHandle, valuesCons));
        }

        MPI_Bcast(&handles, sizeof(ConsolidationHandles), MPI_BYTE, 0, devWorld);

        if(gpuProc == MPI_UNDEFINED)
        {
            CHECK(cudaIpcOpenMemHandle((void**)&rhsCons, handles.rhsConsHandle, cudaIpcMemLazyEnablePeerAccess));
            CHECK(cudaIpcOpenMemHandle((void**)&pCons, handles.solConsHandle, cudaIpcMemLazyEnablePeerAccess));
            CHECK(cudaIpcOpenMemHandle((void**)&rowOffsetsCons, handles.rowOffsetsConsHandle, cudaIpcMemLazyEnablePeerAccess));
            CHECK(cudaIpcOpenMemHandle((void**)&colIndicesGlobalCons, handles.colIndicesConsHandle, cudaIpcMemLazyEnablePeerAccess));
            CHECK(cudaIpcOpenMemHandle((void**)&valuesCons, handles.valuesConsHandle, cudaIpcMemLazyEnablePeerAccess));
        }

        consolidationStatus = ConsolidationStatus::Device;
    }
    else
    {
        if (gpuProc == 0)
        {
            // The data is already on the CPU so consolidate there
            rowOffsetsCons = new int[nConsRows+1];
            colIndicesGlobalCons = new PetscInt[nConsNz];
            valuesCons = new PetscScalar[nConsNz];
            rhsCons = new PetscScalar[nConsRows];
            pCons = new PetscScalar[nConsRows];
        }

        consolidationStatus = ConsolidationStatus::Host;
    }

    PetscFunctionReturn(0);
}

/* \implements AmgXSolver::consolidateMatrix */
PetscErrorCode AmgXSolver::consolidateMatrix(
    const PetscInt nLocalRows,
    const PetscInt nLocalNz,
    const int* rowOffsets,
    const PetscInt* colIndicesGlobal,
    const PetscScalar* values)
{
    PetscFunctionBeginUser;

    // Consolidation has been previously used, must deallocate the structures
    if (consolidationStatus != ConsolidationStatus::Uninitialized)
    {
        // XXX Would the maintainers be happy to include a warning message here,
        // that makes it clear updateA should be preferentially adopted by developers
        // if the sparsity pattern does not change? This would avoid many costs,
        // including re-consolidation costs.

        finalizeConsolidation();
    }

    // Allocate space for the structures required to consolidate
    initializeConsolidation(nLocalRows, nLocalNz, values);

    switch(consolidationStatus)
    {

    case ConsolidationStatus::None:
    {
        // Consolidation is not required
        PetscFunctionReturn(0);
    }
    case ConsolidationStatus::Uninitialized:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Attempting to consolidate before consolidation is initialized.\n");
        break;
    }
    case ConsolidationStatus::Device:
    {
        // Copy the data to the consolidation buffer
        CHECK(cudaMemcpy(&rowOffsetsCons[rowDispls[myDevWorldRank]], rowOffsets, sizeof(PetscInt) * nLocalRows, cudaMemcpyDefault));
        CHECK(cudaMemcpy(&colIndicesGlobalCons[nzDispls[myDevWorldRank]], colIndicesGlobal, sizeof(PetscInt) * nLocalNz, cudaMemcpyDefault));
        CHECK(cudaMemcpy(&valuesCons[nzDispls[myDevWorldRank]], values, sizeof(PetscScalar) * nLocalNz, cudaMemcpyDefault));

        // cudaMemcpy does not block the host in the cases above, device to device copies,
        // so sychronize with device to ensure operation is complete. Barrier on all devWorld
        // ranks to ensure full arrays are populated before the root process uses the data.
        CHECK(cudaDeviceSynchronize());
        int ierr = MPI_Barrier(devWorld); CHK;

        if (gpuProc == 0)
        {
            // Adjust merged row offsets so that they are correct for the consolidated matrix
            for (int i = 1; i < devWorldSize; ++i)
            {
                int nthreads = 128;
                int nblocks = nRowsInDevWorld[i] / nthreads + 1;
                fixConsolidatedRowOffsets<<<nblocks, nthreads>>>(nRowsInDevWorld[i], nzDispls[i], &rowOffsetsCons[rowDispls[i]]);
            }

            // Manually add the last entry of the rowOffsets list, which is the
            // number of non-zeros in the CSR matrix
            CHECK(cudaMemcpy(&rowOffsetsCons[nConsRows], &nConsNz, sizeof(int), cudaMemcpyDefault));
        }
        else
        {
            // Close IPC handles and deallocate for consolidation
            CHECK(cudaIpcCloseMemHandle(rowOffsetsCons));
            CHECK(cudaIpcCloseMemHandle(colIndicesGlobalCons));
        }

        CHECK(cudaDeviceSynchronize());
        break;
    }
    case ConsolidationStatus::Host:
    {
        // Gather the matrix data to the root rank for consolidation
        MPI_Request req[3];
        int ierr = MPI_Igatherv(rowOffsets, nLocalRows, MPI_INT, rowOffsetsCons, nRowsInDevWorld.data(), rowDispls.data(), MPI_INT, 0, devWorld, &req[0]); CHK;
        ierr = MPI_Igatherv(colIndicesGlobal, nLocalNz, MPI_INT, colIndicesGlobalCons, nnzInDevWorld.data(), nzDispls.data(), MPI_INT, 0, devWorld, &req[1]); CHK;
        ierr = MPI_Igatherv(values, nLocalNz, MPI_DOUBLE, valuesCons, nnzInDevWorld.data(), nzDispls.data(), MPI_DOUBLE, 0, devWorld, &req[2]); CHK;
        MPI_Waitall(3, req, MPI_STATUSES_IGNORE);

        if (gpuProc == 0)
        {
            // Adjust merged row offsets so that they are correct for the consolidated matrix
            for (int j = 1; j < devWorldSize; ++j)
            {
                for(int i = 0; i < nRowsInDevWorld[j]; ++i)
                {
                    rowOffsetsCons[rowDispls[j] + i] += nzDispls[j];
                }
            }

            // Manually add the last entry of the rowOffsets list, which is the
            // number of non-zeros in the CSR matrix
            rowOffsetsCons[nConsRows] = nConsNz;
        }

        break;
    }
    default:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Incorrect consolidation status set.\n");
        break;
    }

    }

    PetscFunctionReturn(0);
}

/* \implements AmgXSolver::reconsolidateValues */
PetscErrorCode AmgXSolver::reconsolidateValues(
    const PetscInt nLocalNz,
    const PetscScalar* values)
{
    PetscFunctionBeginUser;

    switch (consolidationStatus)
    {

    case ConsolidationStatus::None:
    {
        // Consolidation is not required
        PetscFunctionReturn(0);
    }
    case ConsolidationStatus::Uninitialized:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Attempting to re-consolidate before consolidation is initialized.\n");
        break;
    }
    case ConsolidationStatus::Device:
    {
        CHECK(cudaDeviceSynchronize());
        int ierr = MPI_Barrier(devWorld); CHK;

        // The data is already on the GPU so consolidate there
        CHECK(cudaMemcpy(&valuesCons[nzDispls[myDevWorldRank]], values, sizeof(PetscScalar) * nLocalNz, cudaMemcpyDefault));

        CHECK(cudaDeviceSynchronize());
        ierr = MPI_Barrier(devWorld); CHK;

        break;
    }
    case ConsolidationStatus::Host:
    {
        // Gather the matrix values to the root rank for consolidation
        int ierr = MPI_Gatherv(values, nLocalNz, MPI_DOUBLE, valuesCons, nnzInDevWorld.data(), nzDispls.data(), MPI_DOUBLE, 0, devWorld); CHK;
        break;
    }
    default:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Incorrect consolidation status set.\n");
        break;
    }

    }

    PetscFunctionReturn(0);
}

/* \implements AmgXSolver::freeConsStructure */
PetscErrorCode AmgXSolver::freeConsStructure()
{
    PetscFunctionBeginUser;

    // Only the root rank maintains a consolidated structure
    if(gpuProc == MPI_UNDEFINED)
    {
        PetscFunctionReturn(0);
    }

    switch(consolidationStatus)
    {

    case ConsolidationStatus::None:
    {
        // Consolidation is not required
        PetscFunctionReturn(0);
    }
    case ConsolidationStatus::Uninitialized:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                 "Attempting to free consolidation structures before consolidation is initialized.\n");
        break;
    }
    case ConsolidationStatus::Device:
    {
        // Free the device allocated consolidated CSR matrix structure
        CHECK(cudaFree(rowOffsetsCons));
        CHECK(cudaFree(colIndicesGlobalCons));
        break;
    }
    case ConsolidationStatus::Host:
    {
        // Free the host allocated consolidated CSR matrix structure
        delete[] rowOffsetsCons;
        delete[] colIndicesGlobalCons;
        break;
    }
    default:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Incorrect consolidation status set.\n");
        break;
    }

    }

    PetscFunctionReturn(0);
}

/* \implements AmgXSolver::finalizeConsolidation */
PetscErrorCode AmgXSolver::finalizeConsolidation()
{
    PetscFunctionBeginUser;

    switch(consolidationStatus)
    {

    case ConsolidationStatus::None:
    case ConsolidationStatus::Uninitialized:
    {
        // Consolidation is not required or uninitialized
        PetscFunctionReturn(0);
    }
    case ConsolidationStatus::Device:
    {
        if (gpuProc == 0)
        {
            // Deallocate the CSR matrix values, solution and RHS
            CHECK(cudaFree(valuesCons));
            CHECK(cudaFree(pCons));
            CHECK(cudaFree(rhsCons));
        }
        else
        {
            // Close the remaining IPC memory handles
            CHECK(cudaIpcCloseMemHandle(valuesCons));
            CHECK(cudaIpcCloseMemHandle(pCons));
            CHECK(cudaIpcCloseMemHandle(rhsCons));
        }
        break;
    }
    case ConsolidationStatus::Host:
    {
        if(gpuProc == 0)
        {
            delete[] valuesCons;
            delete[] pCons;
            delete[] rhsCons;
        }
        break;
    }
    default:
    {
        SETERRQ(MPI_COMM_WORLD, PETSC_ERR_SUP_SYS,
                "Incorrect consolidation status set.\n");
        break;
    }

    }

    // Free the local GPU partitioning structures
    if(consolidationStatus == ConsolidationStatus::Device || consolidationStatus == ConsolidationStatus::Host)
    {
        nRowsInDevWorld.clear();
        nnzInDevWorld.clear();
        rowDispls.clear();
        nzDispls.clear();
    }

    consolidationStatus = ConsolidationStatus::Uninitialized;

    PetscFunctionReturn(0);
}
