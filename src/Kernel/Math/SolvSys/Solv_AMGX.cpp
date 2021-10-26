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
void Solv_AMGX::Create_objects(const Matrice_Morse& mat)
{
  initialize();
  if (read_matrix_)
    {
      Cerr << "Read matrix not supported on GPU yet." << finl;
      Process::exit();
    }
  // Creation de la matrice Petsc si necessaire
  std::clock_t start = std::clock();
  // ToDo coder sans passer par une Matrice Petsc
  if (MatricePetsc_!=NULL) MatDestroy(&MatricePetsc_);
  Create_MatricePetsc(MatricePetsc_, mataij_, mat);
  petscToCSR(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_);
  Cout << "[AmgX] Time to build matrix: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  /*
   // ToDo essayer de s'affranchir avec la nouvelle API de la matrice/vec PETSc, encore un pb en parallele en VEF (items communs?)
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
  Cout << "[AmgX] Time to set matrix (copy+setup) on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
}

void Solv_AMGX::Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat)
{
  Solv_Petsc::Update_matrix(MatricePetsc, mat);
  std::clock_t start = std::clock();
  petscToCSR(MatricePetsc, SolutionPetsc_, SecondMembrePetsc_); // Semble utile en //. Pourquoi ?
  Cout << "[AmgX] Time to petscToCSR: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
  start = std::clock();
  SolveurAmgX_.updateA(nRowsLocal, nNz, values);
  //SolveurAmgX_.updateA(nRowsLocal, nNz, mat.get_coeff().addr());
  Cout << "[AmgX] Time to update matrix (copy+resetup) on GPU: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << finl;
}

// Check and return true if new stencil
bool Solv_AMGX::check_stencil(const Matrice_Morse& mat_morse)
{
  // Parcours de la matrice_morse (qui peut contenir des 0 et qui n'est pas triee par colonnes croissantes)
  // si matrice sur le GPU deja construite (qui est sans 0 et qui est triee par colonnes croissantes):
  if (nRowsGlobal==0)
    return true;
  const ArrOfInt& tab1 = mat_morse.get_tab1();
  const ArrOfInt& tab2 = mat_morse.get_tab2();
  const ArrOfDouble& coeff = mat_morse.get_coeff();
  const ArrOfInt& renum_array = renum_;
  int new_stencil = 0, RowLocal = 0, nnz_local = 0;
  for (int i = 0; i < tab1.size_array() - 1; i++)
    {
      if (items_to_keep_[i])
        {
          int nnz_row = 0;
          for (int k = tab1(i) - 1; k < tab1(i + 1) - 1; k++)
            if (coeff(k) != 0) nnz_row++;
          if (nnz_row != rowOffsets[i + 1] - rowOffsets[i])
            {
              Journal() << "Provisoire: Number of non-zero on GPU will change from " << rowOffsets[i + 1] - rowOffsets[i] << " to " << nnz_row << " on row " << RowLocal << finl;
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
                      //Journal() << "Provisoire: mat_morse(" << RowLocal << "," << col << ")!=0";
                      // Boucle pour voir si le coeff est sur le GPU:
                      for (int kk = rowOffsets[i]; kk < rowOffsets[i + 1]; kk++)
                        if (colIndices[kk] == col)
                          {
                            found = true;
                            break;
                          }
                      if (!found)
                        {
                          Journal() << "Provisoire: mat_morse(" << RowLocal << "," << col << ")!=0 new on GPU " << finl;
                          new_stencil = 1;
                          break;
                        }
                      nnz_local++;
                    }
                }
            }
          RowLocal++;
        }
    }
  new_stencil = mp_max(new_stencil);
  if (new_stencil)
    {
      rebuild_matrix_ = true;
      clean_matrix_   = true;
    }
  int nnz_global = mp_sum(nnz_local);
  Journal() << "Provisoire: Solv_AMGX::check_stencil nnz_global(mat_morse)=" << nnz_global << " Same stencil on GPU ? " << (new_stencil ? "Yes" : "No") << finl;
  if (limpr() == 1)
    {
      bool verbose = true;
      ArrOfDouble nnz(2); // Pas ArrOfInt car nnz peut depasser 2^32 facilement
      nnz(0)=nnz_global;
      nnz(1)=mat_morse.nb_coeff();
      Cout << "Order of the PETSc matrix : " << nb_rows_tot_ << " (~ "
           << (petsc_cpus_selection_ ? (int) (nb_rows_tot_ / petsc_nb_cpus_) : nb_rows_)
           << " unknowns per PETSc process ) " << (new_stencil ? "New stencil." : "Same stencil.");
      if (verbose)
        {
          if (nnz[1] > 0)
            {
              Cout << " (nonzeros: " << nnz[0] << "/" << nnz[1] << ")" << finl;
              double ratio = 1 - (double) nnz(0) / (double) nnz(1);
              if (ratio > 0.2)
                Cout << "Warning! Trust matrix contains a lot of useless stored zeros: " << (int) (ratio * 100) << "%" << finl;
            }
          if (clean_matrix_ && nnz[1]-nnz[0]>0) Cout << "[AmgX] Discarding " << nnz[1]-nnz[0] << " zeros from TRUST matrix into the PETSc matrix ..." << finl;
        }
      else
        Cout << finl;
    }
  return new_stencil;
  //return Solv_Petsc::check_stencil(mat_morse);
}

// Resolution
int Solv_AMGX::solve(ArrOfDouble& residu)
{
  {
    // Crash d'AmgX (non reproduit sur les cas poisson d'AmgXWrapper), peut etre a cause d'une creation differente
    // des vecteurs PETSc (via DM) par rapport a la notre (VecCreate), si deja converge (nbiter=0)
    // Contournement en calculant le residu avant le solve et on ne resout pas si inferieur au seuil:
    Vec ResidualPetsc_;
    VecDuplicate(SolutionPetsc_, &ResidualPetsc_);
    MatResidual(MatricePetsc_, SecondMembrePetsc_, SolutionPetsc_, ResidualPetsc_);
    VecNorm(ResidualPetsc_, NORM_2, &residu(0));
    VecDestroy(&ResidualPetsc_);
    if (residu(0) < seuil_)
      {
        Cout << "[AmgX] Not solved on GPU to avoid a (ToDo: fix) crash as it's already converged: ||Ax-b||="<<residu(0)<<"<"<<seuil_<< finl;
        return 0;
      }
  }
  std::clock_t start = std::clock();
  SolveurAmgX_.solve(lhs, rhs, nRowsLocal);
  Cout << "[AmgX] Time to solve on GPU: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << finl;
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
