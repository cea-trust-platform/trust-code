/****************************************************************************
* Copyright (c) 2021, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Solv_Petsc_AMGX.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_Petsc_AMGX_included
#define Solv_Petsc_AMGX_included

#include <Solv_Petsc.h>
#include <cstring>
#ifdef PETSC_HAVE_CUDA
#include <AmgXSolver.hpp>
#include <amgx_c.h>
#endif

class Solv_Petsc_AMGX : public Solv_Petsc
{
  Declare_instanciable_sans_constructeur(Solv_Petsc_AMGX);
#ifdef PETSCKSP_H
#ifdef PETSC_HAVE_CUDA
public :
  virtual void set_config(const Nom& filename)
  {
    this->config = filename;
  }
  inline Solv_Petsc_AMGX()
  {
    amgx_=true;
    config = Objet_U::nom_du_cas();
    config += ".amgx";
  };
protected :
  virtual void Create_objects(const Matrice_Morse&);
  virtual void Update_matrix(Mat&, const Matrice_Morse&);
  virtual int solve(ArrOfDouble& residual);
  int nbiter(ArrOfDouble& residual);
  void initialize();
  virtual void finalize()
  {
    if (amgx_initialized_)
      {
        SolveurAmgX_.finalize();
        amgx_initialized_ = false;
      }
  }
protected:
  Nom config; // Fichier de configuration
  AmgXSolver SolveurAmgX_; // Instance de AmgXWrapper
#endif
#endif
};

class Solv_AMGX : public Solv_Petsc_AMGX
{
  Declare_instanciable_sans_constructeur(Solv_AMGX);
#ifdef PETSCKSP_H
#ifdef PETSC_HAVE_CUDA
public :
protected :
  virtual void Create_objects(const Matrice_Morse&);
  virtual void Update_matrix(Mat&, const Matrice_Morse&);
  virtual int solve(ArrOfDouble& residual);
private:
  /*
  int nRowsLocal, nRowsGlobal, nNz;
  //const int *colIndices = nullptr, *rowOffsets = nullptr;
  double *lhs, *rhs;
  */
  PetscInt nRowsLocal, nRowsGlobal, nNz;
  const PetscInt *colIndices = nullptr, *rowOffsets = nullptr;
  double *lhs, *rhs;
  PetscScalar *values;

  // Fonction provisoire de conversion Petsc ->CSR
  PetscErrorCode petscToCSR(
    Mat& A,
    Vec& lhs_petsc,
    Vec& rhs_petsc)
  {
    PetscFunctionBeginUser;

    PetscBool done;
    MatType type;
    Mat localA;

    // Get the Mat type
    PetscErrorCode ierr = MatGetType(A, &type);
    CHKERRQ(ierr);

    // Check whether the Mat type is supported
    if (std::strcmp(type, MATSEQAIJ) == 0) // sequential AIJ
      {
        // Make localA point to the same memory space as A does
        localA = A;
      }
    else if (std::strcmp(type, MATMPIAIJ) == 0)
      {
        // Get local matrix from redistributed matrix
        ierr = MatMPIAIJGetLocalMat(A, MAT_INITIAL_MATRIX, &localA);
        CHKERRQ(ierr);
      }
    else
      {
        SETERRQ1(PETSC_COMM_WORLD, PETSC_ERR_ARG_WRONG,
                 "Mat type %s is not supported!\n", type);
      }

    // Get row and column indices in compressed row format
    ierr = MatGetRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE,
                       &nRowsLocal, &rowOffsets, &colIndices, &done);
    CHKERRQ(ierr);

    ierr = MatSeqAIJGetArray(localA, &values);
    CHKERRQ(ierr);

    // Get pointers to the raw data of local vectors
    ierr = VecGetArray(lhs_petsc, &lhs);
    CHKERRQ(ierr);
    ierr = VecGetArray(rhs_petsc, &rhs);
    CHKERRQ(ierr);

    // Calculate the number of rows in nRowsGlobal
    ierr = MPI_Allreduce(&nRowsLocal, &nRowsGlobal, 1, MPI_INT, MPI_SUM, PETSC_COMM_WORLD);
    CHKERRQ(ierr);

    // Store the number of non-zeros
    nNz = rowOffsets[nRowsLocal];

    PetscFunctionReturn(0);
  }
#endif
#endif
};
#endif


