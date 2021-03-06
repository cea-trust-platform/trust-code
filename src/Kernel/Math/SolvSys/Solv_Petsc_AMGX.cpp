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
// File:        Solv_Petsc_AMGX.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/95
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_Petsc_AMGX.h>
#include <Matrice_Morse.h>
#include <ctime>

Implemente_instanciable_sans_constructeur(Solv_Petsc_AMGX,"Solv_Petsc_AMGX",Solv_Petsc);
// printOn
Sortie& Solv_Petsc_AMGX::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}
// readOn
Entree& Solv_Petsc_AMGX::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

#ifdef PETSCKSP_H
#ifdef PETSC_HAVE_CUDA
void Solv_Petsc_AMGX::initialize()
{
  if (amgx_initialized()) return;
  Nom AmgXmode = "dDDI"; // dDDI:GPU hDDI:CPU (not supported yet by AmgXWrapper)
  /* Possible de jouer avec simple precision peut etre:
  1. (lowercase) letter: whether the code will run on the host (h) or device (d).
  2. (uppercase) letter: whether the matrix precision is float (F) or double (D).
  3. (uppercase) letter: whether the vector precision is float (F) or double (D).
  4. (uppercase) letter: whether the index type is 32-bit int (I) or else (not currently supported).
  typedef enum { AMGX_mode_hDDI, AMGX_mode_hDFI, AMGX_mode_hFFI, AMGX_mode_dDDI, AMGX_mode_dDFI, AMGX_mode_dFFI } AMGX_Mode; */
  Cerr << "Initializing Amgx and reading the " << config << " file." << finl;
  std::clock_t start = std::clock();
  SolveurAmgX_.initialize(PETSC_COMM_WORLD, AmgXmode.getString(), config.getString());
  Cout << "[AmgX] Time to initialize: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
  amgx_initialized_ = true;
  // MPI_Barrier(PETSC_COMM_WORLD); Voir dans https://github.com/barbagroup/AmgXWrapper/pull/30/commits/1554808a3689f51fa43ab81a35c47a9a1525939a
}

// Creation des objets
void Solv_Petsc_AMGX::Create_objects(const Matrice_Morse& mat)
{
  initialize();
  // Creation de la matrice Petsc si necessaire
  if (!read_matrix_)
    {
      std::clock_t start = std::clock();
      Create_MatricePetsc(MatricePetsc_, mataij_, mat);
      Cout << "[Petsc] Time to build matrix: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
    }
  std::clock_t start = std::clock();
  SolveurAmgX_.setA(MatricePetsc_);
  Cout << "[AmgX] Time to copy matrix on GPU: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
}

void Solv_Petsc_AMGX::Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat)
{
  std::clock_t start = std::clock();
  Solv_Petsc::Update_matrix(MatricePetsc, mat);
  Cout << "[Petsc] Time to update matrix: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  start = std::clock();
  SolveurAmgX_.setA(MatricePetsc); // Pas optimal recopie a chaque fois CPU->GPU (Mieux: Solv_AMGX)
  Cout << "[AmgX] Time to copy matrix on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
}

// Resolution
int Solv_Petsc_AMGX::solve(ArrOfDouble& residu)
{
  std::clock_t start = std::clock();
  SolveurAmgX_.solve(SolutionPetsc_, SecondMembrePetsc_);
  Cout << "[AmgX] Time to solve on GPU: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
  return nbiter(residu);
}

int Solv_Petsc_AMGX::nbiter(ArrOfDouble& residu)
{
  int nbiter = -1;
  SolveurAmgX_.getIters(nbiter);
  if (limpr() > -1)
    {
      SolveurAmgX_.getResidual(0, residu(0));
      SolveurAmgX_.getResidual(nbiter - 1, residu(nbiter));
    }
  return nbiter;
}
#endif
#endif

Implemente_instanciable_sans_constructeur(Solv_AMGX,"Solv_AMGX",Solv_Petsc_AMGX);
// printOn
Sortie& Solv_AMGX::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}
// readOn
Entree& Solv_AMGX::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

#ifdef PETSCKSP_H
#ifdef PETSC_HAVE_CUDA
void Solv_AMGX::Create_objects(const Matrice_Morse& mat)
{
  initialize();
  // Creation de la matrice Petsc si necessaire
  std::clock_t start = std::clock();
  clean_matrix_ = false; // ToDo : on ne peut pas enlever les 0 de la matrice PETSc car le stencil de la matrice PETSc (CPU) et le stencil de la matrice GPU doivent etre identiques pour updateA...
  Create_MatricePetsc(MatricePetsc_, mataij_, mat);
  petscToCSR(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_);
  Cout << "[Petsc] Time to build matrix: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  /*
      nRowsGlobal = nb_rows_tot_;
      nRowsLocal = nb_rows_;
      // Numerotation archaique (Fortran) de Matrice_morse qui fait suer:
      ArrOfInt rowOffsets(mat.get_tab1());
      rowOffsets-=1;
      ArrOfInt colIndices(mat.get_tab2());
      colIndices-=1;
      nNz = rowOffsets[nRowsLocal];
      VecGetArray(SolutionPetsc_, &lhs);
      VecGetArray(SecondMembrePetsc_, &rhs);
  SolveurAmgX_.setA(nRowsGlobal, nRowsLocal, nNz, rowOffsets.addr(), colIndices.addr(), mat.get_coeff().addr(), nullptr);
  */
  SolveurAmgX_.setA(nRowsGlobal, nRowsLocal, nNz, rowOffsets, colIndices, values, nullptr);
  Cout << "[AmgX] Time to copy matrix on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
}

void Solv_AMGX::Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat)
{
  std::clock_t start = std::clock();
  Solv_Petsc::Update_matrix(MatricePetsc, mat);
  Cout << "[Petsc] Time to update matrix: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  start = std::clock();
  petscToCSR(MatricePetsc, SolutionPetsc_, SecondMembrePetsc_); // Semble utile en //. Pourquoi ?
  Cout << "[Petsc] Time to petscToCSR: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  start = std::clock();
  SolveurAmgX_.updateA(nRowsLocal, nNz, values);
  //SolveurAmgX_.updateA(nRowsLocal, nNz, mat.get_coeff().addr());
  Cout << "[AmgX] Time to update matrix on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
}

// Resolution
int Solv_AMGX::solve(ArrOfDouble& residu)
{
  std::clock_t start = std::clock();
  SolveurAmgX_.solve(lhs, rhs, nRowsLocal);
  Cout << "[AmgX] Time to solve on GPU: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
  return nbiter(residu);
}
#endif
#endif
