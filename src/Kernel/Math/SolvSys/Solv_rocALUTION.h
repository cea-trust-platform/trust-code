/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Solv_rocALUTION_included
#define Solv_rocALUTION_included

#include <Solv_Externe.h>
#include <rocalution_for_kernel.h>
#include <Motcle.h>
#include <Device.h>
class EChaine;

// Initializer de rocALUTION inclus dans la classe pour ne pas l'appeler depuis main.cpp de TRUST
#ifdef ROCALUTION_ROCALUTION_HPP_
static int rocalution_initialized = 0;
struct rocalution_initializer
{
  rocalution_initializer()
  {
    if (rocalution_initialized==0)
      {
        disable_accelerator_rocalution(!Objet_U::computeOnDevice); // TRUST_DISABLE_DEVICE=1 $exec pour desactiver la version GPU de rocALUTION
        set_omp_affinity_rocalution(false); // Disable OpenMP thread affinity
        //char* dev_per_node = getenv("TRUST_DEVICES_PER_NODE");
        //init_rocalution(Process::me(), dev_per_node==NULL ? 1 : atoi(dev_per_node));
        //set_device_rocalution(Process::me());
        init_rocalution(Process::me(),
                        1); // Lancement sur lumi01 (8 GPUS pour 64 cores, 1 rank MPI/GPU): srun --cpus-per-task=8 --gpus-per-task=1 --threads-per-core=1 -n ...
        set_omp_threads_rocalution(1); // Disable OpenMP
        info_rocalution();
      }
    rocalution_initialized++;
  }
};
#endif

class Solv_rocALUTION : public Solv_Externe
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION);

public :
  Solv_rocALUTION();
  Solv_rocALUTION(const Solv_rocALUTION&);
  ~Solv_rocALUTION() override;

  inline int solveur_direct() const override { return 0; };
  inline int resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x, int niter_max) override { return resoudre_systeme(a,b,x); };

  int resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x) override;
  void create_solver(Entree& entree);

private :
  void initialize();
  void Create_objects(const Matrice_Morse&);
#ifdef ROCALUTION_ROCALUTION_HPP_
  rocalution_initializer rocalution_initializer_;
  template <typename T>
  Solver<GlobalMatrix<T>, GlobalVector<T>, T>* create_rocALUTION_precond(EChaine& is);
  template <typename T>
  IterativeLinearSolver<GlobalMatrix<T>, GlobalVector<T>, T>* create_rocALUTION_solver(const Motcle& solver);

  ParallelManager pm;
  GlobalMatrix<double> mat;
  // Solver:
  IterativeLinearSolver<GlobalMatrix<double>, GlobalVector<double>, double>* ls;
  // Preconditioners:
  Solver<GlobalMatrix<double>, GlobalVector<double>, double>* gp; // global parallel precond
  Solver<LocalMatrix<double>, LocalVector<double>, double>* lp;  // local precond
  Solver<LocalMatrix<double>, LocalVector<double>, double>* local_solver; // local solver for multigrid
  // Mixed-precision solver:
  IterativeLinearSolver<GlobalMatrix<float>, GlobalVector<float>, float>* sp_ls;
  Solver<GlobalMatrix<float>, GlobalVector<float>, float>* sp_p;
  double atol_, rtol_;
  bool write_system_;
  bool first_solve_ = true;
  int precond_verbosity_;
  Motcle smoother_= "JACOBI";
  Motcle coarse_grid_solver_ = "LU";
  ArrOfDouble x_,b_;
  GlobalVector<double> sol, rhs, e;
#endif
};

#endif


