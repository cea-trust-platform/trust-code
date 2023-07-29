/**
 * \file AmgXSolver.hpp
 * \brief Definition of class AmgXSolver.
 * \author Pi-Yueh Chuang (pychuang@gwu.edu)
 * \date 2015-09-01
 * \copyright Copyright (c) 2015-2019 Pi-Yueh Chuang, Lorena A. Barba.
 * \copyright Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *            This project is released under MIT License.
 */

#ifdef TRUST_USE_CUDA
# pragma once

// CUDA
#include <cuda_runtime.h>

// STL
# include <string>
# include <vector>

// AmgX
# include <amgx_c.h>

// PETSc
# include <petscmat.h>
# include <petscvec.h>


/** \brief A macro to check the returned CUDA error code.
 *
 * \param call [in] Function call to CUDA API.
 */
# define CHECK(call)                                                        \
{                                                                           \
    const cudaError_t       error = call;                                   \
    if (error != cudaSuccess)                                               \
    {                                                                       \
        SETERRQ4(PETSC_COMM_WORLD, PETSC_ERR_SIG,                           \
            "Error: %s:%d, code:%d, reason: %s\n",                          \
            __FILE__, __LINE__, error, cudaGetErrorString(error));          \
    }                                                                       \
}


/** \brief A shorter macro for checking PETSc returned error code. */
# define CHK CHKERRQ(ierr)


/** \brief A wrapper class for coupling PETSc and AmgX.
 *
 * This class is a wrapper of AmgX library for PETSc. PETSc users only need to
 * pass a PETSc matrix and vectors into an AmgXSolver instance to solve their
 * linear systems. The class is designed specifically for the situation where
 * the number of MPI processes is more than the number of GPU devices.
 *
 * Eaxmple usage:
 * \code
 * int main(int argc, char **argv)
 * {
 *     // initialize matrix A, RHS, etc using PETSc
 *     ...
 *
 *     // create an instance of the solver wrapper
 *     AmgXSolver    solver;
 *     // initialize the instance with communicator, executation mode, and config file
 *     solver.initialize(comm, mode, file);
 *     // set matrix A. Currently it only accept PETSc AIJ matrix
 *     solver.setA(A);
 *     // solve. x and rhs are PETSc vectors. unkns will be the final result in the end
 *     solver.solve(unks, rhs);
 *     // get number of iterations
 *     int         iters;
 *     solver.getIters(iters);
 *     // get residual at the last iteration
 *     double      res;
 *     solver.getResidual(iters, res);
 *     // finalization
 *     solver.finalize();
 *
 *     // other codes
 *     ....
 *
 *     return 0;
 * }
 * \endcode
 */
class AmgXSolver
{
    public:

        /** \brief Default constructor. */
        AmgXSolver() = default;


        /** \brief Construct a AmgXSolver instance.
         *
         * \param comm [in] MPI communicator.
         * \param modeStr [in] A string; target mode of AmgX (e.g., dDDI).
         * \param cfgFile [in] A string; the path to AmgX configuration file.
         */
        AmgXSolver(const MPI_Comm &comm,
                const std::string &modeStr, const std::string &cfgFile);


        /** \brief Destructor. */
        ~AmgXSolver();


        /** \brief Initialize a AmgXSolver instance.
         *
         * \param comm [in] MPI communicator.
         * \param modeStr [in] A string; target mode of AmgX (e.g., dDDI).
         * \param cfgFile [in] A string; the path to AmgX configuration file.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode initialize(const MPI_Comm &comm,
                const std::string &modeStr, const std::string &cfgFile);


        /** \brief Finalize this instance.
         *
         * This function destroys AmgX data. When there are more than one
         * AmgXSolver instances, the last one destroyed is also in charge of
         * destroying the shared resource object and finalizing AmgX.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode finalize();


        /** \brief Set up the matrix used by AmgX.
         *
         * This function will automatically convert PETSc matrix to AmgX matrix.
         * If the number of MPI processes is higher than the number of available
         * GPU devices, we also redistribute the matrix in this function and
         * upload redistributed one to GPUs.
         *
         * Note: currently we can only handle AIJ/MPIAIJ format.
         *
         * \param A [in] A PETSc Mat.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode setA(const Mat &A);


        /** \brief Set up the matrix used by AmgX.
         *
         * This function sets up the AmgX matrix from the provided CSR data
         * structures and partition data.
         *
         * \param nGlobalRows [in] The number of global rows.
         * \param nLocalRows [in] The number of local rows on this rank.
         * \param nLocalNz [in] The total number of non zero entries locally.
         * \param rowOffsets [in] The local CSR matrix row offsets.
         * \param colIndicesGlobal [in] The global CSR matrix column indices.
         * \param values [in] The local CSR matrix values.
         * \param partData [in] Array of length nGlobalRows containing the rank
         * id of the owning rank for each row.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode setA(
            const PetscInt nGlobalRows,
            const PetscInt nLocalRows,
            const PetscInt nLocalNz,
            const PetscInt* rowOffsets,
            const PetscInt* colIndicesGlobal,
            const PetscScalar* values,
            const PetscInt* partData);


        /** \brief Re-sets up an existing AmgX matrix.
         *
         * Replaces the matrix coefficients with the provided values and performs
         * a resetup for the AmgX matrix.
         *
         * \param nLocalRows [in] The number of local rows on this rank.
         * \param nLocalNz [in] The total number of non zero entries locally.
         * \param values [in] The local CSR matrix values.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode updateA(
            const PetscInt nLocalRows,
            const PetscInt nLocalNz,
            const PetscScalar* values);


        /** \brief Solve the linear system.
         *
         * \p p vector will be used as an initial guess and will be updated to the
         * solution by the end of solving.
         *
         * For cases that use more MPI processes than the number of GPUs, this
         * function will do data gathering before solving and data scattering
         * after the solving.
         *
         * \param p [in, out] A PETSc Vec object representing unknowns.
         * \param b [in] A PETSc Vec representing right-hand-side.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode solve(Vec &p, Vec &b);


        /** \brief Solve the linear system.
         *
         * \p p vector will be used as an initial guess and will be updated to the
         * solution by the end of solving.
         *
         * For cases that use more MPI processes than the number of GPUs, this
         * function will do data gathering before solving and data scattering
         * after the solving.
         *
         * \param p [in, out] The unknown array.
         * \param b [in] The RHS array.
         * \param nRows [in] The number of rows in this rank.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode solve(PetscScalar *p, const PetscScalar *b, const int nRows, const double seuil);


        /** \brief Get the number of iterations of the last solving.
         *
         * \param iter [out] Number of iterations.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode getIters(int &iter);


        /** \brief Get the residual at a specific iteration during the last solving.
         *
         * \param iter [in] Target iteration.
         * \param res [out] Returned residual.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode getResidual(const int &iter, double &res);


    private:

        /** \brief Current count of AmgXSolver instances.
         *
         * This static variable is used to count the number of instances. The
         * fisrt instance is responsable for initializing AmgX library and the
         * resource instance.
         */
        static int              count;

        /** \brief A flag indicating if this instance has been initialized. */
        bool                    isInitialized = false;

        /** \brief The name of the node that this MPI process belongs to. */
        std::string             nodeName;




        /** \brief Number of local GPU devices used by AmgX.*/
        PetscMPIInt             nDevs;

        /** \brief The ID of corresponding GPU device used by this MPI process. */
        PetscMPIInt             devID;

        /** \brief A flag indicating if this process will talk to GPU. */
        PetscMPIInt             gpuProc = MPI_UNDEFINED;

        /** \brief A communicator for global world. */
        MPI_Comm                globalCpuWorld;

        /** \brief A communicator for local world (i.e., in-node). */
        MPI_Comm                localCpuWorld;

        /** \brief A communicator for MPI processes that can talk to GPUs. */
        MPI_Comm                gpuWorld;

        /** \brief A communicator for processes sharing the same devices. */
        MPI_Comm                devWorld;

        /** \brief Size of \ref AmgXSolver::globalCpuWorld "globalCpuWorld". */
        PetscMPIInt             globalSize;

        /** \brief Size of \ref AmgXSolver::localCpuWorld "localCpuWorld". */
        PetscMPIInt             localSize;

        /** \brief Size of \ref AmgXSolver::gpuWorld "gpuWorld". */
        PetscMPIInt             gpuWorldSize;

        /** \brief Size of \ref AmgXSolver::devWorld "devWorld". */
        PetscMPIInt             devWorldSize;

        /** \brief Rank in \ref AmgXSolver::globalCpuWorld "globalCpuWorld". */
        PetscMPIInt             myGlobalRank;

        /** \brief Rank in \ref AmgXSolver::localCpuWorld "localCpuWorld". */
        PetscMPIInt             myLocalRank;

        /** \brief Rank in \ref AmgXSolver::gpuWorld "gpuWorld". */
        PetscMPIInt             myGpuWorldRank;

        /** \brief Rank in \ref AmgXSolver::devWorld "devWorld". */
        PetscMPIInt             myDevWorldRank;




        /** \brief A parameter used by AmgX. */
        int                     ring;

        /** \brief AmgX solver mode. */
        AMGX_Mode               mode;

        /** \brief AmgX config object. */
        AMGX_config_handle      cfg = nullptr;

        /** \brief AmgX matrix object. */
        AMGX_matrix_handle      AmgXA = nullptr;

        /** \brief AmgX vector object representing unknowns. */
        AMGX_vector_handle      AmgXP = nullptr;

        /** \brief AmgX vector object representing RHS. */
        AMGX_vector_handle      AmgXRHS = nullptr;

        /** \brief AmgX solver object. */
        AMGX_solver_handle      solver = nullptr;

        /** \brief AmgX resource object.
         *
         * Due to the design of AmgX library, using more than one resource
         * instance may cause some problems. So we make the resource instance
         * as a static member to keep only one instance.
         */
        static AMGX_resources_handle   rsrc;

        /** \brief Initialize consolidation, if required. Allocates space to hold
         * consolidated CSR matrix and solution/RHS vectors on the root rank and,
         * if device pointer consolidation, allocates and opens IPC memory handles.
         * Calculates and stores consolidated sizes and displacements.
         *
         * \param nLocalRows [in] The number of rows owned by this rank.
         * \param nLocalNz [in] The number of non-zeros owned by this rank.
         * \param values [in] The values of the CSR matrix A.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode initializeConsolidation(
            const PetscInt nLocalRows,
            const PetscInt nLocalNz,
            const PetscScalar *values);

        /** \brief Realise consolidation, if required. This copies data from multiple ranks
         * that share a single GPU, so a single consolidated CSR matrix can be passed to
         * AmgX. As such, users can efficiently execute with more MPI ranks than GPUs,
         * potentially improving performance where un-accelerated components of an application
         * require CPU resources. CUDA IPC is leveraged to avoid buffering data onto the host,
         * rather enabling fast intra-GPU copies if the local CSR matrices are already on the GPU.
         *
         * \param nLocalRows [in] The number of rows owned by this rank.
         * \param nLocalNz [in] The number of non-zeros owned by this rank.
         * \param rowOffsets [in] The row offsets of the CSR matrix A.
         * \param colIndicesGlobal [in] The global column indices of the CSR matrix A.
         * \param values [in] The values of the CSR matrix A.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode consolidateMatrix(
            const PetscInt nLocalRows,
            const PetscInt nLocalNz,
            const PetscInt *rowOffsets,
            const PetscInt *colIndicesGlobal,
            const PetscScalar *values);

        /** \brief Re-consolidates the values of the CSR matrix from multiple ranks.
         *
         * \param nLocalNz [in] The number of non-zeros owned by this rank.
         * \param values [in] The values of the CSR matrix A.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode reconsolidateValues(
            const PetscInt nLocalNz,
            const PetscScalar *values);

        /** \brief De-allocates consolidated data structures, if any, after the AmgX matrix
         * has been constructed and the duplicate data becomes redundant.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode freeConsStructure();

        /** \brief De-allocates all consolidated matrix structures.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode finalizeConsolidation();

        /** \brief Enumeration for the status of matrix consolidation for the solver.*/
        enum class ConsolidationStatus {
            Uninitialized,
            None,
            Host,
            Device
        } consolidationStatus;

        /** \brief The number of non-zeros consolidated from multiple ranks to a device.*/
        int nConsNz = 0;

        /** \brief The number of rows consolidated from multiple ranks to a device.*/
        int nConsRows = 0;

        /** \brief The row offsets consolidated onto a single device.*/
        PetscInt *rowOffsetsCons = nullptr;

        /** \brief The global column indices consolidated onto a single device.*/
        PetscInt *colIndicesGlobalCons = nullptr;

        /** \brief The values consolidated onto a single device.*/
        PetscScalar* valuesCons = nullptr;

        /** \brief The solution vector consolidated onto a single device.*/
        PetscScalar* pCons = nullptr;

        /** \brief The RHS vector consolidated onto a single device.*/
        PetscScalar* rhsCons = nullptr;

        /** \brief The number of rows per rank associated with a single device.*/
        std::vector<int> nRowsInDevWorld;

        /** \brief The number of non-zeros per rank associated with a single device.*/
        std::vector<int> nnzInDevWorld;

        /** \brief The row displacements per rank associated with a single device.*/
        std::vector<int> rowDispls;

        /** \brief The non-zero displacements per rank associated with a single device.*/
        std::vector<int> nzDispls;

        /** \brief A VecScatter for gathering/scattering between original PETSc
         *         Vec and temporary redistributed PETSc Vec.*/
        VecScatter              scatterLhs = nullptr;

        /** \brief A VecScatter for gathering/scattering between original PETSc
         *         Vec and temporary redistributed PETSc Vec.*/
        VecScatter              scatterRhs = nullptr;

        /** \brief A temporary PETSc Vec holding redistributed unknowns. */
        Vec                     redistLhs = nullptr;

        /** \brief A temporary PETSc Vec holding redistributed RHS. */
        Vec                     redistRhs = nullptr;




        /** \brief Set AmgX solver mode based on the user-provided string.
         *
         * Available modes are: dDDI, dDFI, dFFI, hDDI, hDFI, hFFI.
         *
         * \param modeStr [in] a std::string.
         * \return PetscErrorCode.
         */
        PetscErrorCode setMode(const std::string &modeStr);


        /** \brief Get the number of GPU devices on this computing node.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode setDeviceCount();


        /** \brief Set the ID of the corresponding GPU used by this process.
         *
         * \return PetscErrorCode.
         */
        PetscErrorCode setDeviceIDs();


        /** \brief Initialize all MPI communicators.
         *
         * The \p comm provided will be duplicated and saved to the
         * \ref AmgXSolver::globalCpuWorld "globalCpuWorld".
         *
         * \param comm [in] Global communicator.
         * \return PetscErrorCode.
         */
        PetscErrorCode initMPIcomms(const MPI_Comm &comm);


        /** \brief Perform necessary initialization of AmgX.
         *
         * This function initializes AmgX for current instance. Based on
         * \ref AmgXSolver::count "count", only the instance initialized first
         * is in charge of initializing AmgX and the resource instance.
         *
         * \param cfgFile [in] Path to AmgX solver configuration file.
         * \return PetscErrorCode.
         */
        PetscErrorCode initAmgX(const std::string &cfgFile);


        /** \brief Get IS for the row indices that processes in
         *      \ref AmgXSolver::gpuWorld "gpuWorld" will held.
         *
         * \param A [in] PETSc matrix.
         * \param devIS [out] PETSc IS.
         * \return PetscErrorCode.
         */
        PetscErrorCode getDevIS(const Mat &A, IS &devIS);


        /** \brief Get local sequential PETSc Mat of the redistributed matrix.
         *
         * \param A [in] Original PETSc Mat.
         * \param devIS [in] PETSc IS representing redistributed row indices.
         * \param localA [out] Local sequential redistributed matrix.
         * \return PetscErrorCode.
         */
        PetscErrorCode getLocalA(const Mat &A, const IS &devIS, Mat &localA);


        /** \brief Redistribute matrix.
         *
         * \param A [in] Original PETSc Mat object.
         * \param devIS [in] PETSc IS representing redistributed rows.
         * \param newA [out] Redistributed matrix.
         * \return PetscErrorCode.
         */
        PetscErrorCode redistMat(const Mat &A, const IS &devIS, Mat &newA);


        /** \brief Get \ref AmgXSolver::scatterLhs "scatterLhs" and
         *      \ref AmgXSolver::scatterRhs "scatterRhs".
         *
         * \param A1 [in] Original PETSc Mat object.
         * \param A2 [in] Redistributed PETSc Mat object.
         * \param devIS [in] PETSc IS representing redistributed row indices.
         * \return PetscErrorCode.
         */
        PetscErrorCode getVecScatter(const Mat &A1, const Mat &A2, const IS &devIS);


        /** \brief Get data of compressed row layout of local sparse matrix.
         *
         * \param localA [in] Sequential local redistributed PETSc Mat.
         * \param localN [out] Number of local rows.
         * \param row [out] Row vector in compressed row layout.
         * \param col [out] Column vector in compressed row layout.
         * \param data [out] Data vector in compressed row layout.
         * \return PetscErrorCode.
         */
        PetscErrorCode getLocalMatRawData(const Mat &localA,
                PetscInt &localN, std::vector<PetscInt> &row,
                std::vector<PetscInt64> &col, std::vector<PetscScalar> &data);


        /** \brief Destroy the sequential local redistributed matrix.
         *
         * \param A [in] Original PETSc Mat.
         * \param localA [in, out] Local matrix, returning null pointer.
         * \return PetscErrorCode.
         */
        PetscErrorCode destroyLocalA(const Mat &A, Mat &localA);

        /** \brief Check whether the global matrix distribution is contiguous
         *
         * If the global matrix is distributed such that contiguous chunks of rows are
         * distributed over the individual ranks in ascending rank order, the partition vector
         * has trivial structure (i.e. [0, ..., 0, 1, ..., 1, ..., R-1, ..., R-1] for R ranks) and
         * its calculation can be skipped since all information is available to AmgX through
         * the number of ranks and the partition *offsets* (i.e. how many rows are on each rank).
         *
         * \param devIS [in] PETSc IS representing redistributed row indices.
         * \param isContiguous [out] Whether the global matrix is contiguously distributed.
         * \param partOffsets [out] If contiguous, holds the partition offsets for all R ranks
         * \return PetscErrorCode.
         */
        PetscErrorCode checkForContiguousPartitioning(
            const IS &devIS, PetscBool &isContiguous, std::vector<PetscInt> &partOffsets);

        /** \brief Get partition data required by AmgX.
         *
         * \param devIS [in] PETSc IS representing redistributed row indices.
         * \param N [in] Total number of rows in global matrix.
         * \param partData [out] Partition data, either explicit vector or offsets.
         * \param usesOffsets [out] If PETSC_TRUE, partitioning is contiguous and partData contains
         *      partition offsets, see checkForContiguousPartitioning(). Otherwise, contains explicit
         *      partition vector.
         * \return PetscErrorCode.
         */
        PetscErrorCode getPartData(const IS &devIS,
                const PetscInt &N, std::vector<PetscInt> &partData, PetscBool &usesOffsets);


        /** \brief Function that actually solves the system.
         *
         * This function won't check if the process is involved in AmgX solver.
         * So if calling this function from processes not in the
         * \ref AmgXSolver::gpuWorld "gpuWorld", something bad will happen.
         * This function hence won't do data gathering/scattering, either.
         *
         * \param p [in, out] PETSc Vec for unknowns.
         * \param b [in] PETSc Vec for RHS.
         * \return PetscErrorCode.
         */
        PetscErrorCode solve_real(Vec &p, Vec &b);
};
#endif