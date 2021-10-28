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
// File:        Solv_AMGX.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/95
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_AMGX.h>
#include <Matrice_Morse.h>
#include <ctime>
#include <communications.h>

Implemente_instanciable_sans_constructeur(Solv_AMGX,"Solv_AMGX",Solv_Petsc);
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
void Solv_AMGX::initialize()
{
  if (amgx_initialized()) return;
  Nom AmgXmode = "dDDI"; // dDDI:GPU hDDI:CPU (not supported yet by AmgXWrapper)
  /* Possible de jouer avec simple precision peut etre:
  1. (lowercase) letter: whether the code will run on the host (h) or device (d).
  2. (uppercase) letter: whether the matrix precision is float (F) or double (D).
  3. (uppercase) letter: whether the vector precision is float (F) or double (D).
  4. (uppercase) letter: whether the index type is 32-bit int (I) or else (not currently supported).
  typedef enum { AMGX_mode_hDDI, AMGX_mode_hDFI, AMGX_mode_hFFI, AMGX_mode_dDDI, AMGX_mode_dDFI, AMGX_mode_dFFI } AMGX_Mode; */
  Cerr << "Initializing Amgx and reading the " << config() << " file." << finl;
  std::clock_t start = std::clock();
  SolveurAmgX_.initialize(PETSC_COMM_WORLD, AmgXmode.getString(), config().getString());
  Cout << "[AmgX] Time to initialize: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
  amgx_initialized_ = true;
  // MPI_Barrier(PETSC_COMM_WORLD); Voir dans https://github.com/barbagroup/AmgXWrapper/pull/30/commits/1554808a3689f51fa43ab81a35c47a9a1525939a
}

// Creation des objets
void Solv_AMGX::Create_objects(const Matrice_Morse& mat_morse)
{
  initialize();
  if (read_matrix_)
    {
      Cerr << "Read_matrix not supported on GPU yet." << finl;
      Process::exit();
    }
  // Creation de la matrice Petsc (CSR pointeurs dessus)
  if (MatricePetsc_ != NULL) MatDestroy(&MatricePetsc_);

  Create_MatricePetsc(MatricePetsc_, mataij_, mat_morse);
  std::clock_t start = std::clock();
  petscToCSR(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_);
  Cout << "[AmgX] Time to create CSR pointers: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  start = std::clock();
  SolveurAmgX_.setA(nRowsGlobal, nRowsLocal, nNz, rowOffsets, colIndices, values, nullptr);
  Cout << "[AmgX] Time to set matrix (copy+setup) on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl; // Attention balise lue par fiche de validation
}

// Fonction de conversion Petsc ->CSR
PetscErrorCode Solv_AMGX::petscToCSR(Mat& A, Vec& lhs_petsc, Vec& rhs_petsc)
{
  PetscFunctionBeginUser;
  PetscBool done;
  MatType type;

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
      if (localA!=NULL) MatDestroy(&localA); // Suite accroissement memoire !
      ierr = MatMPIAIJGetLocalMat(A, MAT_INITIAL_MATRIX, &localA);
      CHKERRQ(ierr);
    }
  else
    {
      SETERRQ1(PETSC_COMM_WORLD, PETSC_ERR_ARG_WRONG,"Mat type %s is not supported!\n", type);
    }

  // Get row and column indices in compressed row format
  ierr = MatGetRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE, &nRowsLocal, &rowOffsets, &colIndices, &done);
  if (done==PETSC_FALSE) Process::abort();
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

void Solv_AMGX::Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat_morse)
{
  // La matrice CSR de PETSc a ete mise a jour dans check_stencil
  std::clock_t start = std::clock();
  SolveurAmgX_.updateA(nRowsLocal, nNz, values);  // ToDo erreur valgrind au premier appel de updateA...
  Cout << "[AmgX] Time to update matrix (copy+resetup) on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl; // Attention balise lue par fiche de validation
}

// Check and return true if new stencil
bool Solv_AMGX::check_stencil(const Matrice_Morse& mat_morse)
{
  std::clock_t start = std::clock();
  // Parcours de la matrice_morse (qui peut contenir des 0 et qui n'est pas triee par colonnes croissantes)
  // si matrice sur le GPU deja construite (qui est sans 0 et qui est triee par colonnes croissantes):
  const ArrOfInt& tab1 = mat_morse.get_tab1();
  const ArrOfInt& tab2 = mat_morse.get_tab2();
  const ArrOfDouble& coeff = mat_morse.get_coeff();
  const ArrOfInt& renum_array = renum_;
  int new_stencil = 0, RowLocal = 0;
  Journal() << "Provisoire: nb_rows_=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << finl;
  for (int i = 0; i < tab1.size_array() - 1; i++)
    {
      if (items_to_keep_[i])
        {
          int nnz_row = 0;
          for (int k = tab1(i) - 1; k < tab1(i + 1) - 1; k++)
            if (coeff(k) != 0) nnz_row++;
          if (nnz_row != rowOffsets[RowLocal + 1] - rowOffsets[RowLocal])
            {
              Journal() << "Provisoire: Number of non-zero on GPU will change from " << rowOffsets[RowLocal + 1] - rowOffsets[RowLocal] << " to " << nnz_row << " on row " << RowLocal << finl;
              new_stencil = 1;
              break;
            }
          else
            {
              for (int k = tab1(i) - 1; k < tab1(i + 1) - 1; k++)
                {
                  if (coeff(k) != 0)
                    {
                      bool found = false;
                      int col = renum_array[tab2(k) - 1];
                      // Boucle pour voir si le coeff est sur le GPU:
                      int RowGlobal = decalage_local_global_+RowLocal;
                      for (int kk = rowOffsets[RowLocal]; kk < rowOffsets[RowLocal + 1]; kk++)
                        {
                          if (colIndices[kk] == col)
                            {
                              values[kk] = coeff(k); // On met a jour le coefficient
                              found = true;
                              break;
                            }
                        }
                      if (!found)
                        {
                          Journal() << "Provisoire: mat_morse(" << RowGlobal << "," << col << ")!=0 new on GPU " << finl;
                          new_stencil = 1;
                          break;
                        }
                    }
                }
            }
          RowLocal++;
        }
    }
  new_stencil = mp_max(new_stencil);
  Cout << "[AmgX] Time to check stencil: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  return new_stencil;
}

// Resolution
int Solv_AMGX::solve(ArrOfDouble& residu)
{
  if (seuil_relatif_==0)
    {
      // Crash d'AmgX (non reproduit sur les cas poisson d'AmgXWrapper), peut etre a cause d'une creation differente
      // des vecteurs PETSc (via DM) par rapport a la notre (VecCreate), si deja converge a cause d'un seuil absolu trop haut (nbiter=0)
      // Contournement en calculant le residu avant le solve et on ne resout pas si inferieur au seuil:
      Vec ResidualPetsc_;
      VecDuplicate(SolutionPetsc_, &ResidualPetsc_);
      MatResidual(MatricePetsc_, SecondMembrePetsc_, SolutionPetsc_, ResidualPetsc_);
      VecNorm(ResidualPetsc_, NORM_2, &residu(0));
      VecDestroy(&ResidualPetsc_);
      if (residu(0) < seuil_)
        {
          Cerr << "[AmgX] The residual seems to small to be solved on GPU: ||Ax-b||="<<residu(0)<<"<"<<seuil_<< finl;
          Cerr << "Please, try to use a relative tolerance, with rtol option or lower the absolute tolerance atol." << finl;
          return 0;
          //Process::exit();
        }
    }
  std::clock_t start = std::clock();
  SolveurAmgX_.solve(lhs, rhs, nRowsLocal);
  Cout << "[AmgX] Time to solve system on GPU: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
  return nbiter(residu);
}

int Solv_AMGX::nbiter(ArrOfDouble& residu)
{
  int nbiter = -1;
  SolveurAmgX_.getIters(nbiter);
  // Bug AmgX, seul le process 0 renvoie correctement nbiter...
  envoyer_broadcast(nbiter, 0);
  if (limpr() > -1)
    {
      SolveurAmgX_.getResidual(0, residu(0));
      SolveurAmgX_.getResidual(nbiter - 1, residu(nbiter));
    }
  return nbiter;
}
#endif
#endif
