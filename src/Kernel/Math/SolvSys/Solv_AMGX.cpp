/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Solv_AMGX.h>
#include <Matrice_Morse.h>
#include <ctime>
#include <communications.h>
#include <stat_counters.h>
#include <Device.h>

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
#ifdef TRUST_USE_CUDA
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
  double start = Statistiques::get_time_now();;
  SolveurAmgX_.initialize(PETSC_COMM_WORLD, AmgXmode.getString(), config().getString());
  Cout << "[AmgX] Time to initialize: " << Statistiques::get_time_now() - start << finl;
  amgx_initialized_ = true;
  // MPI_Barrier(PETSC_COMM_WORLD); Voir dans https://github.com/barbagroup/AmgXWrapper/pull/30/commits/1554808a3689f51fa43ab81a35c47a9a1525939a
}

// Creation des objets
void Solv_AMGX::Create_objects(const Matrice_Morse& mat_morse, int blocksize)
{
  initialize();
  if (read_matrix_)
    {
      Cerr << "Read_matrix not supported on GPU yet." << finl;
      Process::exit();
    }
  // Creation de la matrice Petsc (CSR pointeurs dessus)
  if (MatricePetsc_ != nullptr) MatDestroy(&MatricePetsc_);

  Create_MatricePetsc(MatricePetsc_, mataij_, mat_morse);
  double start = Statistiques::get_time_now();
  petscToCSR(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_);
  Cout << "[AmgX] Time to create CSR pointers: " << Statistiques::get_time_now() - start << finl;
  statistiques().begin_count(gpu_copytodevice_counter_);
  // Use device pointer to enable device consolidation in AmgXWrapper:
  double* values_device;
  cudaMalloc((void**)&values_device, nNz * sizeof(double));
  cudaMemcpy(values_device, values, nNz * sizeof(double), cudaMemcpyHostToDevice);
  //SolveurAmgX_.setA(nRowsGlobal, nRowsLocal, nNz, rowOffsets, colIndices, values, nullptr);
  SolveurAmgX_.setA(nRowsGlobal, nRowsLocal, nNz, rowOffsets, colIndices, values_device, nullptr);
  //cudaFree(values_device);delete[] hostArray;
  statistiques().end_count(gpu_copytodevice_counter_, (int)(sizeof(int) * (nRowsLocal + nNz) + sizeof(double) * nNz));
  Cout << "[AmgX] Time to set matrix (copy+setup) on GPU: " << statistiques().last_time(gpu_copytodevice_counter_) << finl; // Attention balise lue par fiche de validation
}

void Solv_AMGX::Create_vectors(const DoubleVect& b)
{
  if (lhs_amgx_.size_array()==0)
    {
      lhs_amgx_.resize(nb_rows_);
      rhs_amgx_.resize(nb_rows_);
    }
}

void Solv_AMGX::Update_vectors(const DoubleVect& secmem, DoubleVect& solution)
{
  // Assemblage du second membre et de la solution
  int size=solution.size_array();
  const int* index_addr = mapToDevice(index_);
  const double* solution_addr = mapToDevice(solution, "solution");
  const double* secmem_addr = mapToDevice(secmem, "secmem");
  double* lhs_amgx_addr = computeOnTheDevice(lhs_amgx_, "lhs_amgx_");
  double* rhs_amgx_addr = computeOnTheDevice(rhs_amgx_, "rhs_amgx_");
  start_timer();
  #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice)
  for (int i=0; i<size; i++)
    if (index_addr[i]!=-1)
      {
        lhs_amgx_addr[index_addr[i]] = solution_addr[i];
        rhs_amgx_addr[index_addr[i]] = secmem_addr[i];
      }
  end_timer(Objet_U::computeOnDevice, "Solv_AMGX::Update_vectors");
  if (reorder_matrix_) Process::exit("Option not supported yet for AmgX.");
}

void Solv_AMGX::Update_solution(DoubleVect& solution)
{
  int size = index_.size_array();
  const int* index_addr = mapToDevice(index_);
  const double* lhs_amgx_addr = mapToDevice(lhs_amgx_, "lhs_amgx_");
  double* solution_addr = computeOnTheDevice(solution, "solution");
  start_timer();
  #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice)
  for (int i=0; i<size; i++)
    if (index_addr[i]!=-1)
      solution_addr[i] = lhs_amgx_addr[index_addr[i]];
  end_timer(Objet_U::computeOnDevice, "Solv_AMGX::Update_solution");
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
      if (localA!=nullptr) MatDestroy(&localA); // Suite accroissement memoire !
      ierr = MatMPIAIJGetLocalMat(A, MAT_INITIAL_MATRIX, &localA);
      CHKERRQ(ierr);
    }
  else
    {
      SETERRQ(PETSC_COMM_WORLD, PETSC_ERR_ARG_WRONG,"Mat type %s is not supported!\n", type);
    }

  // Get row and column indices in compressed row format
  ierr = MatGetRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE, &nRowsLocal, &rowOffsets, &colIndices, &done);
  if (done==PETSC_FALSE) Process::abort();
  CHKERRQ(ierr);

  ierr = MatSeqAIJGetArray(localA, &values);
  CHKERRQ(ierr);

  // Get pointers to the raw data of local vectors
  /*
  ierr = VecGetArray(lhs_petsc, &lhs);
  CHKERRQ(ierr);
  ierr = VecGetArray(rhs_petsc, &rhs);
  CHKERRQ(ierr);
   */

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
  statistiques().begin_count(gpu_copytodevice_counter_);
  SolveurAmgX_.updateA(nRowsLocal, nNz, values);  // ToDo erreur valgrind au premier appel de updateA...
  statistiques().end_count(gpu_copytodevice_counter_, (int)sizeof(double)*nNz);
  Cout << "[AmgX] Time to update matrix (copy+resetup) on GPU: " << statistiques().last_time(gpu_copytodevice_counter_) << finl; // Attention balise lue par fiche de validation
}

// Check and return true if new stencil
bool Solv_AMGX::check_stencil(const Matrice_Morse& mat_morse)
{
  True_int num_devices = 0;
  cudaGetDeviceCount(&num_devices);
  if (num_devices>1)
    {
      // Exemple cas PETSC_AMGX en parallele:
      Cout << "[AmgX] In Solv_AMGX::check_stencil same_stencil=true cause bug in SolveurAmgX_::updateA on multi-GPU (ToDo: fix by switching to CSR interface)!" << finl;
      return true;
    }
  double start = Statistiques::get_time_now();
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
  Cout << "[AmgX] Time to check stencil: " << Statistiques::get_time_now() - start << finl;
  return new_stencil;
}

// Resolution
int Solv_AMGX::solve(ArrOfDouble& residu)
{
  if (computeOnDevice)
    {
      mapToDevice(rhs_amgx_);
      computeOnTheDevice(lhs_amgx_);
      statistiques().begin_count(gpu_library_counter_);
      // Offer device pointers to AmgX:
      SolveurAmgX_.solve(addrOnDevice(lhs_amgx_), addrOnDevice(rhs_amgx_), nRowsLocal, seuil_);
      statistiques().end_count(gpu_library_counter_);
    }
  else
    {
      // Offer host pointers to AmgX:
      SolveurAmgX_.solve(lhs_amgx_.addr(), rhs_amgx_.addr(), nRowsLocal, seuil_);
    }
  Cout << "[AmgX] Time to solve system on GPU: " << statistiques().last_time(gpu_library_counter_) << finl;
  return nbiter(residu);
}

int Solv_AMGX::nbiter(ArrOfDouble& residu)
{
  True_int nbiter = -1;
  SolveurAmgX_.getIters(nbiter);
  // Bug AmgX, seul le process 0 renvoie correctement nbiter...
  envoyer_broadcast(nbiter, 0);
  if (limpr() > -1)
    {
      SolveurAmgX_.getResidual(0, residu(0));
      if (nbiter>0) SolveurAmgX_.getResidual(nbiter - 1, residu(nbiter));
    }
  return nbiter;
}
#endif
#endif
