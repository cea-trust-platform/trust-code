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

#include <Solv_rocALUTION.h>
#include <Matrice_Morse_Sym.h>
#include <TRUSTVect.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>
#include <Comm_Group_MPI.h>
#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>
#include <Device.h>
#include <stat_counters.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION, "Solv_rocALUTION", Solv_Externe);

// printOn
Sortie& Solv_rocALUTION::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_rocALUTION::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

Solv_rocALUTION::Solv_rocALUTION()
{
  initialize();
}

Solv_rocALUTION::Solv_rocALUTION(const Solv_rocALUTION& org)
{
  initialize();
  // on relance la lecture ....
  EChaine recup(org.get_chaine_lue());
  readOn(recup);
}

Solv_rocALUTION::~Solv_rocALUTION()
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  if (ls!=nullptr)
    {
      ls->Clear();
      delete ls;
    }
  if (gp!=nullptr)
    {
      gp->Clear();
      delete gp;
    }
  if (lp!=nullptr)
    {
      lp->Clear();
      delete lp;
    }
  if (local_solver!=nullptr)
    {
      local_solver->Clear();
      delete local_solver;
    }
  if (sp_ls!=nullptr)
    {
      sp_ls->Clear();
      delete sp_ls;
    }
  if (sp_p!=nullptr)
    {
      sp_p->Clear();
      delete sp_p;
    }
  rocalution_initialized--;
  if (rocalution_initialized==0)
    {
      info_rocalution();
      stop_rocalution();
    }
#endif
}

void Solv_rocALUTION::initialize()
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  ls = nullptr;
  gp = nullptr;
  lp = nullptr;
  local_solver = nullptr;
  sp_ls = nullptr;
  sp_p = nullptr;
  write_system_ = false;
#endif
}

double precond_option(Entree& is, const Motcle& motcle)
{
  Motcle motlu;
  is >> motlu;
  Cerr << motlu << " ";
  if (motlu!="{") Process::exit("\nWe are waiting {");
  is >> motlu;
  Cerr << motlu << " ";
  if (motlu==motcle)
    {
      double val;
      is >> val;
      Cerr << val << " ";
      is >> motlu;
      Cerr << motlu << " ";
      return val;
    }
  if (motlu!="}") Process::exit("\nWe are waiting }");
  return -1;
}

enum Verbosity { No, Limited, High};
// Fonction template pour la creation des precond simple ou double precision
#ifdef ROCALUTION_ROCALUTION_HPP_
template <typename T>
Solver<GlobalMatrix<T>, GlobalVector<T>, T>* Solv_rocALUTION::create_rocALUTION_precond(EChaine& is)
{
  Solver<GlobalMatrix<T>, GlobalVector<T>, T>* p;
  Motcle precond;
  is >> precond;
  Cerr << precond << " ";
  // Preconditioner globaux paralleles:
  if (precond==(Motcle)"Jacobi" || precond==(Motcle)"diag") // OK en ~335 its
    {
      p = new Jacobi<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option(is, "");
    }
  else if (precond==(Motcle)"PairwiseAMG|Pairwise-AMG|PW-AMG") // Converge pas sur P0P1...
    {
      // Crashe parfois en parallele:
      // https://github.com/ROCmSoftwarePlatform/rocALUTION/issues/195
      p = new PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      auto& pairwiseamg = dynamic_cast<PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p);
      //pairwiseamg.SetBeta(0.25); // [0-1] default 0.25
      //pairwiseamg.SetCoarsestLevel(10000); // Sinon boucle indefiniment...
      pairwiseamg.SetOrdering(2); // default
      //pairwiseamg.SetCoarseningFactor(19); // [0-20] default 4
      //pairwiseamg.InitMaxIter(1);

      //pairwiseamg.SetKcycleFull(true); // default true
      precond_option(is, "");
    }
  /* Crash bizarre en sequentiel et parallele.
   * ToDo OpenMP travail sur les samples cg-rsamg_mpi.cpp et cg-rsamg.cpp
   Le support annonce que cg-rsamg_mpi pas supporte encore...: https://github.com/ROCmSoftwarePlatform/rocALUTION/issues/196
  else if (precond==(Motcle)"RugeStuebenAMG|RSAMG|RS-AMG|C-AMG")  // Setup rapide sur CPU. Converge vite.
    {
      p = new RugeStuebenAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      auto& rsamg = dynamic_cast<RugeStuebenAMG<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p);
      // Base sur le sample rocALUTION/clients/samples/cg-rsamg_mpi.cpp
      rsamg.InitMaxIter(1);
      rsamg.SetCoarseningStrategy(CoarseningStrategy::PMIS);
      rsamg.SetInterpolationType(InterpolationType::ExtPI); // Tres important pour une bonne convergence
      rsamg.SetCoarsestLevel(20);
      rsamg.SetInterpolationFF1Limit(false); // Limit operator complexity
      //rsamg.SetStrengthThreshold(0.1); // Pas d'effet flagrant
      precond_option(is, "");
    } */
  else if (precond==(Motcle)"null")
    {
      p = nullptr;
      precond_option(is, ""); // Pas de preconditionnement
    }
  else
    {
      // See https://github.com/ROCmSoftwarePlatform/rocALUTION/wiki/functionality-table
      // Local preconditionners with BlockJacobi preconditionner:
      p = new BlockJacobi<GlobalMatrix<T>, GlobalVector<T>, T>();
      if (precond == (Motcle) "MCSGS|MultiColoredSGS|SSOR")  // GPU build/solver
        {
          lp = new MultiColoredSGS<LocalMatrix<T>, LocalVector<T>, T>();
          double omega = ::precond_option(is, "omega");
          if (omega >= 0) dynamic_cast<MultiColoredSGS<LocalMatrix<T>, LocalVector<T>, T> &>(*lp).SetRelaxation(omega);
        }
      else if (precond == (Motcle) "MCGS|MultiColoredGS|SOR")  // GPU build/solve
        {
          lp = new MultiColoredGS<LocalMatrix<T>, LocalVector<T>, T>(); // Attention converge avec BICGSTAB pas CG...
          double omega = ::precond_option(is, "omega");
          if (omega >= 0) dynamic_cast<MultiColoredGS<LocalMatrix<T>, LocalVector<T>, T> &>(*lp).SetRelaxation(omega);

          if(dynamic_cast<CG<GlobalMatrix<double>, GlobalVector<double>, double> *>(ls) != nullptr)
            Process::exit("Error, non symmetric Gauss Seidel preconditioners should be used with BiCGSTab solver, not CG.");
        }
      else if (precond == (Motcle) "ILU")  // ILU(0) sur GPU build/solver
        {
          lp = new ILU<LocalMatrix<T>, LocalVector<T>, T>(); // Converge pas non plus donc probleme...
          int level = (int) precond_option(is, "level");
          if (level >= 0)
            dynamic_cast<ILU<LocalMatrix<T>, LocalVector<T>, T> &>(*lp).Set(level, true);
        }
      // Aucun AMG n'a un build sur GPU donc lenteur possible a changement de matrice...
      else if (precond == (Motcle) "SAAMG|SA-AMG")  // Converge avec BICGStab (setup SAAMG desormais sur GPU mais bloque ou TRES lent sur GPU4...)
        {
          lp = new SAAMG<LocalMatrix<T>, LocalVector<T>, T>();
          auto& saamg = dynamic_cast<SAAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          saamg.InitMaxIter(1);
          saamg.SetCoarseningStrategy(CoarseningStrategy::PMIS); // Atttention peut etre couteux en P0P1...
          saamg.SetLumpingStrategy(LumpingStrategy::AddWeakConnections); // Ameliore convergence d'apres sample ?
          saamg.SetCoarsestLevel(200);
          saamg.SetCouplingStrength(0.001);
          precond_option(is, "");
        }
      else if (precond==(Motcle)"UAAMG|UA-AMG")  // Setup rapide mais sur CPU
        {
          lp = new UAAMG<LocalMatrix<T>, LocalVector<T>, T>();
          auto& uuamg = dynamic_cast<UAAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          uuamg.InitMaxIter(1);
          uuamg.SetCoarseningStrategy(CoarseningStrategy::PMIS);
          uuamg.SetCoarsestLevel(200);
          uuamg.SetCouplingStrength(0.001);
          precond_option(is, "");
        }
      else if (precond==(Motcle)"RugeStuebenAMG|RSAMG|RS-AMG|C-AMG")
        {
          lp = new RugeStuebenAMG<LocalMatrix<T>, LocalVector<T>, T>();
          auto& rsamg = dynamic_cast<RugeStuebenAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          // Base sur le sample rocALUTION/clients/samples/cg-rsamg.cpp
          rsamg.InitMaxIter(1);
          rsamg.SetCoarseningStrategy(CoarseningStrategy::PMIS);
          rsamg.SetInterpolationType(InterpolationType::ExtPI); // Tres important pour une bonne convergence
          rsamg.SetCoarsestLevel(20);
          rsamg.SetInterpolationFF1Limit(false); // Limit operator complexity
          //rsamg.SetStrengthThreshold(0.1); // Pas d'effet flagrant
          precond_option(is, "");
        }
      else if (precond == (Motcle) "SPAI")  // ??
        {
          lp = new SPAI<LocalMatrix<T>, LocalVector<T>, T>();
          precond_option(is, "");
        }
      else if (precond == (Motcle) "FSAI")  // ??
        {
          lp = new FSAI<LocalMatrix<T>, LocalVector<T>, T>();
          precond_option(is, "");
        }
      else if (precond == (Motcle) "GS")  // Non, CPU seulement
        {
          lp = new GS<LocalMatrix<T>, LocalVector<T>, T>();
          precond_option(is, "");
        }
      else if (precond == (Motcle) "SGS")  // Non, CPU seulement
        {
          lp = new SGS<LocalMatrix<T>, LocalVector<T>, T>();
          precond_option(is, "");
        }
      else
        {
          Cerr << "Error! Unknown rocALUTION preconditionner: " << precond << finl;
          Process::exit();
          return nullptr;
        }
      // Coarse solver for local AMG
      try
        {
          auto& base_amg = dynamic_cast<BaseAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          if (coarse_grid_solver_ == "LU") // Solveur direct construit et inverse sur CPU
            local_solver = new LU<LocalMatrix<T>, LocalVector<T>, T>();
          else if (coarse_grid_solver_ == "INVERSION") // Solveur direct construit et inverse sur GPU
            local_solver = new Inversion<LocalMatrix<T>, LocalVector<T>, T>();
          else if (coarse_grid_solver_ == "JACOBI") // Default
            local_solver = new Jacobi<LocalMatrix<T>, LocalVector<T>, T>();
          else
            Process::exit("Unknown coarse grid solver.");
          local_solver->Verbose(precond_verbosity_);
          base_amg.SetManualSolver(true);
          base_amg.SetSolver(*local_solver);
          Cout << "[rocALUTION] Setting " << coarse_grid_solver_ << " for coarse grid solver." << finl;
          // Plante:
          //base_amg.SetHostLevels(coarse_grids_host_); // Par defaut 0 (donc tous les levels sur GPU)
          //Cout << "[rocALUTION] Solving " << coarse_grids_host_ << " coarse grids on the host." << finl;
        }
      catch (const std::bad_cast&)
        {
          Cout << "Error, you can't use smoother " << coarse_grid_solver_ << " with this solver." << finl;
          //Process::exit();
        }

      // Set local preconditionner to BlockJacobi global one
      dynamic_cast<BlockJacobi<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).Set(*lp);
    }
  if (local_solver != nullptr)
    {
      local_solver->Verbose(precond_verbosity_);
      local_solver->Print();
    }
  if (lp != nullptr) lp->Verbose(precond_verbosity_);
  if (p != nullptr) p->Verbose(precond_verbosity_);
  return p;
}

// Fonction template pour la creation des solveurs simple ou double precision
template <typename T>
IterativeLinearSolver<GlobalMatrix<T>, GlobalVector<T>, T>* Solv_rocALUTION::create_rocALUTION_solver(const Motcle& solver)
{
  if (solver==(Motcle)"GCP")
    {
      return new CG<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"FGCP")
    {
      return new FCG<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"GMRES")
    {
      return new GMRES<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"FGMRES")
    {
      return new FGMRES<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"FP|FIXEDPOINT")
    {
      return new FixedPoint<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"BICGSTAB")
    {
      return new BiCGStab<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else
    {
      Cerr << solver << " solver not recognized for rocALUTION." << finl;
      Process::exit();
      return nullptr;
    }
}

static double dtol_ = 1.e-12; // default tolerance
#endif

// Lecture et creation du solveur
void Solv_rocALUTION::create_solver(Entree& entree)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  // Valeurs par defaut:
  atol_ = dtol_;
  rtol_ = dtol_;
  double div_tol = 1e3;
  bool mixed_precision = false;
  precond_verbosity_ = Verbosity::No;

  lecture(entree);
  EChaine is(get_chaine_lue());
  Motcle accolade_ouverte("{"), accolade_fermee("}");
  Motcle solver, motlu;
  EChaine precond;
  is >> solver;   // On lit le solveur en premier puis les options du solveur
  is >> motlu; // On lit l'accolade
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the parameters of the solver " << solver << " { ... }" << finl;
      Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
      exit();
    }
  // Lecture des parametres du solver:
  // precond name { [ omega double } [ level int ] } [impr] [seuil|atol double] [rtol double] }
  is >> motlu;
  while (motlu!=accolade_fermee)
    {
      if (motlu==(Motcle)"impr")
        {
          fixer_limpr(1);
        }
      else if (motlu==(Motcle)"seuil" || motlu==(Motcle)"atol")
        {
          is >> atol_;
        }
      else if (motlu==(Motcle)"rtol")
        {
          is >> rtol_;
        }
      else if (motlu==(Motcle)"precond")
        {
          Nom str("");
          is >> motlu;
          while (motlu!=accolade_fermee)
            {
              if (motlu==(Motcle)"impr")
                precond_verbosity_ = Verbosity::High;
              else if (motlu==(Motcle)"smoother")
                is >> smoother_;
              else if (motlu==(Motcle)"coarse_grid_solver")
                is >> coarse_grid_solver_;
              else if (motlu==(Motcle)"omega")
                is >> omega_;
              else if (motlu==(Motcle)"coarse_grids_host")
                is >> coarse_grids_host_;
              else
                str+=motlu+" ";
              is >> motlu;
            }
          str+=motlu;
          precond.init(str);
        }
      else if (motlu==(Motcle)"save_matrix_mtx_format")
        {
          write_system_ = true;
        }
      else if (motlu==(Motcle)"mixed_precision")
        {
          // Mixed precision
          mixed_precision = true;
        }
      else
        {
          Cerr << motlu << " keyword not recognized for rocALUTION solver " << solver << finl;
          Process::exit();
        }
      is >> motlu;
    }

  // Creation des solveurs et precond rocALUTION
  if (mixed_precision)
    {
      Process::exit("Not implemented in parallel");
      /*
      ls = new MixedPrecisionDC<GlobalMatrix<double>, GlobalVector<double>, double, GlobalMatrix<float>, GlobalVector<float>, float>();
      sp_p = create_rocALUTION_precond<float>(precond);
      sp_ls = create_rocALUTION_solver<float>(solver);
      sp_ls->SetPreconditioner(*sp_p);
      sp_ls->InitTol(atol_, rtol_, div_tol); // ToDo "The stopping criteria for the inner solver has to be tuned well for good performance."
      */
    }
  else
    {
      ls = create_rocALUTION_solver<double>(solver);
      gp = create_rocALUTION_precond<double>(precond);
      if (gp!= nullptr) ls->SetPreconditioner(*gp);
    }
  ls->InitTol(atol_, rtol_, div_tol);
  //p->FlagPrecond();
  //ls->Clear();
  //ls->InitMaxIter(500);
  //ls->InitMinIter(20);
#endif
}

static int save=0;
void write_matrix(const Matrice_Base& a)
{
  Nom name("trust_matrix");
  name += (Nom)save;
  Matrice_Morse csr = ref_cast(Matrice_Morse, a);
  csr.WriteFileMTX(name);
}
#ifdef ROCALUTION_ROCALUTION_HPP_
void write_vectors(const GlobalVector<double>& rhs, const GlobalVector<double>& sol)
{
  Nom filename(Objet_U::nom_du_cas());
  filename = Objet_U::nom_du_cas();
  filename += "_rocalution_rhs";
  filename += (Nom)save;
  filename += ".vec";
  Cout << "Write rhs into " << filename << finl;
  rhs.WriteFileASCII(filename.getString());
  filename = Objet_U::nom_du_cas();
  filename += "_rocalution_sol";
  filename += (Nom)save;
  filename += ".vec";
  Cout << "Write initial solution into " << filename << finl;
  sol.WriteFileASCII(filename.getString());
}
void write_matrix(const GlobalMatrix<double>& mat)
{
  Nom filename(Objet_U::nom_du_cas());
  filename += "_rocalution_matrix";
  filename += (Nom)save;
  filename += ".mtx";
  Cout << "Writing rocALUTION matrix into " << filename << finl;
  mat.WriteFileMTX(filename.getString());
}

void check(const DoubleVect& t, GlobalVector<double>& r, const Nom& nom)
{
  double norm_t = t.mp_norme_vect();
  double norm_r = r.Norm();
  double difference = std::abs((norm_t - norm_r)/(norm_t+norm_r+DMINFLOAT));
  if (difference>1.e-8)
    {
      Cerr << nom << " : Trust value=" << norm_t << " is different than rocALUTION value=" << norm_r << finl;
      Process::exit();
    }
}
double residual(const Matrice_Base& a, const DoubleVect& b, const DoubleVect& x)
{
  DoubleVect e(b);
  e *= -1;
  a.ajouter_multvect(x, e);
  return e.mp_norme_vect();
}
double residual_device(const GlobalMatrix<double>& a, const GlobalVector<double>& b, const GlobalVector<double>& x, GlobalVector<double>& e)
{
  if (Objet_U::computeOnDevice) statistiques().begin_count(gpu_library_counter_);
  a.Apply(x, &e);
  e.ScaleAdd(-1.0, b);
  double norm = e.Norm();
  if (Objet_U::computeOnDevice) statistiques().end_count(gpu_library_counter_);
  return norm;
}
#endif

int Solv_rocALUTION::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  if (write_system_) save++;
  double tick;
  bool gpu = computeOnDevice && _rocalution_available_accelerator();
  bool solutionOnDevice = x.isDataOnDevice();
  bool keepDataOnDevice = gpu && solutionOnDevice;
  if (!keepDataOnDevice)
    {
      b.checkDataOnHost();
      x.checkDataOnHost();
    }
#ifdef MPI_
  MPI_Comm comm;
  if (sub_type(Comm_Group_MPI,PE_Groups::current_group()))
    comm = ref_cast(Comm_Group_MPI,PE_Groups::current_group()).get_mpi_comm();
  else
    comm = MPI_COMM_WORLD;
  pm.SetMPICommunicator(&comm);
#endif
  if (nouvelle_matrice())
    {
      // Conversion matrice stockage symetrique vers matrice stockage general:
      Matrice_Morse csr;
      tick = rocalution_time();
      if (renum_.size_array()==0) construit_renum(b);
      construit_matrice_morse_intermediaire(a, csr);
      //assert(csr.check_sorted_morse_matrix_structure()); // Provisoire
      if (nb_rows_<20)
        {
          Journal() << "Provisoire sm b=" << finl;
          b.ecrit(Journal());
          Journal() << "Provisoire matrice csr=" << finl;
          csr.imprimer_formatte(Journal());
        }
      Cout << "[rocALUTION] Time to convert TRUST matrix: " << (rocalution_time() - tick) / 1e6 << finl;

      // Save TRUST matrix to check:
      if (write_system_) write_matrix(csr);

      //if (nouveau_stencil_)
      Create_objects(csr);
      //else
      //    Update_matrix(mat, csr);

      // Save rocALUTION matrix to check
      if (write_system_) write_matrix(mat);

      // Fixe le nombre d'iterations max:
      ls->InitMaxIter(nb_rows_tot_);
    }
  else
    {
      // Provisoire pourquoi rebuild pour pairwise amg ?
      if(dynamic_cast<PairwiseAMG<GlobalMatrix<double>, GlobalVector<double>, double> *>(gp) != nullptr)
        {
          tick = rocalution_time();
          ls->ReBuildNumeric();
          Cout << "[rocALUTION] Time to rebuild solver: " << (rocalution_time() - tick) / 1e6 << finl;
        }
    }
  bool debug = b.size_array() < 100;
  if (debug)
    {
      // PL: (int) necessaire pour int64
      Journal() << "pm.GetLocalNrow()=" << (int)pm.GetLocalNrow() << " <> b.size_array()=" << b.size_array() << finl;
      Journal() << "mat.GetN()=" << (int)mat.GetN() << " mat.GetLocalN()=" << (int)mat.GetLocalN() << " mat.GetGhostN()=" << (int)mat.GetGhostN() << finl;
    }

  // Build rhs and initial solution:
  tick = rocalution_time();
  auto N = pm.GetGlobalNrow();
  int size=b.size_array();
  if (rhs_host.size_array()==0)
    {
      // Allocation initiale
      sol_host.resize(size);
      rhs_host.resize(size);
      sol.SetParallelManager(pm);
      rhs.SetParallelManager(pm);
      e.SetParallelManager(pm);
      sol.Allocate("a", N);
      rhs.Allocate("rhs", N);
      e.Allocate("e", N);
    }
  const double * x_addr        = keepDataOnDevice ? mapToDevice(x)               : x.addr();
  const double * b_addr        = keepDataOnDevice ? mapToDevice(b)               : b.addr();
  const int * index_addr       = keepDataOnDevice ? mapToDevice(index_)          : index_.addr();
  double * sol_host_addr       = keepDataOnDevice ? computeOnTheDevice(sol_host) : sol_host.addr();
  double * rhs_host_addr       = keepDataOnDevice ? computeOnTheDevice(rhs_host) : rhs_host.addr();
  start_timer();
  #pragma omp target teams distribute parallel for if (keepDataOnDevice)
  for (int i=0; i<size; i++)
    if (index_addr[i]!=-1)
      {
        sol_host_addr[index_addr[i]] = x_addr[i];
        rhs_host_addr[index_addr[i]] = b_addr[i];
      }
  end_timer(keepDataOnDevice, "Solv_rocALUTION::Update_vectors");

  if (keepDataOnDevice)
    {
      // Les vecteurs rocALUTION sont deplaces sur le device pour une mise a jour sur le device (optimal)
      if (gpu) statistiques().begin_count(gpu_copytodevice_counter_);
      sol.MoveToAccelerator();
      rhs.MoveToAccelerator();
      e.MoveToAccelerator();
      if (gpu) statistiques().end_count(gpu_copytodevice_counter_, 3 * (int)sizeof(double) * nb_rows_);
      sol.GetInterior().CopyFromData(addrOnDevice(sol_host));
      rhs.GetInterior().CopyFromData(addrOnDevice(rhs_host));
    }
  else
    {
      // Les vecteurs sont remplis sur le host puis deplaces vers le device
      sol.GetInterior().CopyFromData(sol_host_addr);
      rhs.GetInterior().CopyFromData(rhs_host_addr);
      if (gpu) statistiques().begin_count(gpu_copytodevice_counter_);
      sol.MoveToAccelerator();
      rhs.MoveToAccelerator();
      e.MoveToAccelerator();
      if (gpu) statistiques().end_count(gpu_copytodevice_counter_, 3 * (int)sizeof(double) * nb_rows_);
    }
  Cout << "[rocALUTION] Time to build and move vectors on device: " << (rocalution_time() - tick) / 1e6 << finl;

  if (write_system_) write_vectors(rhs, sol);
  sol.Info();
#ifndef NDEBUG
  // Check before solves:
  pm.Status();
  assert(rhs.Check()); // ToDo signaler au support rocALUTION que Check() ne renvoie pas un boolean
  assert(sol.Check());
  assert(e.Check());
  check(x, sol, "Before solve ||x||");
  check(b, rhs, "Before solve ||b||");
#endif
  // Petit bug rocALUTION (division par 0 si rhs nul, on contourne en mettant la verbosity a 0)
  ls->Verbose(limpr() && rhs.Norm()>0 ? Verbosity::High : Verbosity::No);
  if (sp_ls!=nullptr) sp_ls->Verbose(limpr() && rhs.Norm()>0 ? Verbosity::High : Verbosity::No);

  // Solve A x = rhs
  tick = rocalution_time();
  bool provisoire = false;
  if (provisoire)
    {
      DoubleVect err(x);
      a.multvect(x, err);
      mat.Apply(sol, &e); // e=Ax
      for (int i=0; i<nb_rows_; i++)
        Cerr << "Ax[" << i << "]=" << e.GetInterior()[i] << finl;
      //for (int i=nb_rows_; i<nb_rows_tot_; i++)
      //  Cerr << "Ax[" << i << "]=" << e.GetGhost()[i-nb_rows_] << finl;
      Cerr << "||Ax||reel=" <<e.GetInterior().Norm() << finl;
      //Cerr << "||Ax||ghost=" << e.GetGhost().Norm() << finl;
      Journal() << "Ax=" << finl;
      err.ecrit(Journal());
      sol.WriteFileASCII("x");
      e.WriteFileASCII("Ax");

      double t = err.mp_norme_vect();
      double r = e.Norm();
      Cerr << "Provisoire ||Ax||host=" << t << " ||Ab||device=" << r << finl;
      if (std::abs(t - r) / (t + r + DMINFLOAT) > 1.e-7)
        {
          Cerr << "Error, difference=" << t - r << finl;
          Process::exit();
        }
    }
  // Calcul des residus sur le host la premiere fois (plus sur) puis sur device ensuite (plus rapide)
  double res_initial = first_solve_ ? residual(a, b, x) : residual_device(mat, rhs, sol, e);
  if (gpu) statistiques().begin_count(gpu_library_counter_);
  ls->Solve(rhs, &sol);
  if (gpu) statistiques().end_count(gpu_library_counter_);
  if (ls->GetSolverStatus()==3) Process::exit("Divergence for solver.");
  if (ls->GetSolverStatus()==4)
    {
      Cout << "Error, maximum number of iterations reached." << finl;
      Cout << "If you used GCP, try BiCGSTAB solver." << finl;
      Process::exit();
    }
  Cout << "[rocALUTION] Time to solve: " << (rocalution_time() - tick) / 1e6 << finl;

  // Recupere la solution
  tick = rocalution_time();
  int nb_iter = ls->GetIterationCount();
  double res_final = ls->GetCurrentResidual();

  if (keepDataOnDevice)
    {
      // Les vecteurs sont mis a jour entre eux sur le device (optimal)
      sol.GetInterior().CopyToData(addrOnDevice(sol_host));
    }
  else
    {
      // Le vecteur est deplace sur le host pour mettre a jour la solution
      if (gpu) statistiques().begin_count(gpu_copyfromdevice_counter_);
      sol.MoveToHost();
      if (gpu) statistiques().end_count(gpu_copyfromdevice_counter_, (int)sizeof(double) * nb_rows_);
      sol.GetInterior().CopyToData(sol_host_addr);
    }
  double * xx_addr = keepDataOnDevice ? computeOnTheDevice(x) : x.addr();
  start_timer();
#ifndef TRUST_USE_CUDA
  #pragma omp target teams distribute parallel for if (keepDataOnDevice)
#endif
  for (int i=0; i<size; i++)
    if (index_addr[i]!=-1)
      xx_addr[i] = sol_host_addr[index_addr[i]];
  end_timer(keepDataOnDevice, "Solv_rocALUTION::Update_solution");
  x.echange_espace_virtuel();
  if (first_solve_) res_final = residual(a, b, x); // Securite a la premiere resolution
#ifndef NDEBUG
  check(x, sol, "After solve ||x||");
  check(b, rhs, "After solve ||b||");
#endif

  // Check residual e=||Ax-rhs||:
  if (atol_>dtol_ && res_final>atol_)
    {
      Cerr << "Solution not correct ! ||Ax-b|| = " << res_final << " > atol = " << atol_ << finl;
      Process::exit();
    }
  else if (rtol_>dtol_ && res_final>rtol_*res_initial)
    {
      Cerr << "Solution not correct ! ||Ax-b||/||Ax0-b|| = " << res_final << " > rtol = " << rtol_ << finl;
      Process::exit();

    }
  if (limpr()>-1)
    {
      double residu_relatif=(res_initial>0?res_final/res_initial:res_final);
      Cout << finl << "Final residue: " << res_final << " ( " << residu_relatif << " )"<<finl;
    }
  fixer_nouvelle_matrice(0);
  //sol.Clear();
  //rhs.Clear();
  //e.Clear();
  if (nb_iter>1) first_solve_ = false;
  Cout << "[rocALUTION] Time to get solution: " << (rocalution_time() - tick) / 1e6 << finl;
  if (solutionOnDevice) mapToDevice(x,"Send x back to device after rocALUTION");
  return nb_iter;
#else
  Process::exit("Sorry, rocALUTION solvers not available with this build.");
  return -1;
#endif
}

void Solv_rocALUTION::Create_objects(const Matrice_Morse& csr)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  double tick = rocalution_time();
  const ArrOfInt& tab1 = csr.get_tab1();
  const ArrOfInt& tab2 = csr.get_tab2();
  const ArrOfDouble& coeff = csr.get_coeff();
  const MD_Vector_base& mdv = renum_.get_md_vector().valeur();
  const MD_Vector_std& md = sub_type(MD_Vector_composite, mdv) ? ref_cast(MD_Vector_composite, mdv).global_md_ : ref_cast(MD_Vector_std, mdv);
  if (local_renum_.size_array()==0) // ToDo OpenMP: bug a trouver sur ce tableau et son utilisation dans rocALUTION pour P1/P1Bulle
    {
      int size = items_to_keep_.size_array();
      local_renum_.resize(size);
      int row=0, row_ghost=0;
      for (int i=0; i<size; i++)
        {
          if (items_to_keep_[i])
            {
              local_renum_[i] = row;
              row++;
            }
          else
            {
              local_renum_[i] = row_ghost;
              row_ghost++;
            }
        }
    }
  bool debug = renum_.size_array() <= 100;
  if (debug)
    {
      Journal() << "nb_rows=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << finl;
      Journal() << "Provisoire decalage_local_global_=" << decalage_local_global_ << finl;
      Journal() << "Provisoire items_to_keep_:" << finl;
      Journal() << items_to_keep_ << finl;
      Journal() << "Provisoire renum_:" << finl;
      renum_.ecrit(Journal());
      Journal() << "Provisoire local_renum_=" << finl;
      local_renum_.ecrit(Journal());
      Journal() << "Provisoire items_to_send_:" << finl;
      md.items_to_send_.ecrire(Journal());
      Journal() << "Provisoire items_to_recv_:" << finl;
      md.items_to_recv_.ecrire(Journal());
      Journal() << "Provisoire blocs_to_recv_:" << finl;
      md.blocs_to_recv_.ecrire(Journal());
    }
  std::vector<int> tab1_c, ghost_tab1_c;
  std::vector<int> tab2_c, ghost_tab2_c;
  std::vector<double> coeff_c, ghost_coeff_c;
  int cpt = 0, ghost_cpt = 0;
  tab1_c.push_back(cpt);
  ghost_tab1_c.push_back(ghost_cpt);
  Journal() << "nb_rows=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << finl;

  int row=0;
  for(int i=0; i<tab1.size_array() - 1; i++)
    {
      if (items_to_keep_[i]) // Item sequentiel
        {
          for (int k = tab1[i] - 1; k < tab1[i + 1] - 1; k++)
            {
              int j = tab2[k] - 1;
              int col = local_renum_[j];
              double c = coeff[k];
              if (items_to_keep_[j])
                {
                  tab2_c.push_back(col);
                  coeff_c.push_back(c);
                  if (debug) Journal() << "Provisoire interior: " << row+1 << " " << col+1 << " " << c << finl;
                  cpt++;
                }
              else
                {
                  ghost_tab2_c.push_back(col);
                  ghost_coeff_c.push_back(c);
                  if (debug) Journal() << "\t\t\t\t\t\tProvisoire ghost: " << row+1 << " " << col+1 << " " << c << finl;
                  ghost_cpt++;
                }
            }
          tab1_c.push_back(cpt);
          ghost_tab1_c.push_back(ghost_cpt);
          row++;
        }
    }

  // Copie de la matrice TRUST dans une matrice rocALUTION
  auto N = tab1_c.size();
  auto nnz = coeff_c.size();
  Cout << "[rocALUTION] Build a matrix with local N= " << N << " and local nnz=" << nnz << finl;

  pm.SetGlobalNrow(nb_rows_tot_);
  pm.SetGlobalNcol(nb_rows_tot_);
  pm.SetLocalNrow(nb_rows_);
  pm.SetLocalNcol(nb_rows_);

  if (Process::is_parallel())
    {
      int boundary_nnz = md.items_to_send_.get_data().size_array();

      // Renum items_to_send et la tri:
      std::vector<True_int> renum_items_to_send_;
      for (int pe = 0; pe < md.items_to_send_.get_nb_lists(); pe++)
        {
          ArrOfInt items_pe;
          md.items_to_send_.copy_list_to_array(pe, items_pe);
          int size = items_pe.size_array();
          for (int i = 0; i < size; i++)
            items_pe[i] = local_renum_[md.items_to_send_(pe, i)];
          items_pe.array_trier_retirer_doublons();
          // Argh, pas specifie dans la doc mais il fallait que BoundaryIndex soit triee (ToDo le dire au support rocALUTIION pour la doc...)
          for (True_int i=0; i<size; i++)
            renum_items_to_send_.push_back(items_pe[i]);
        }
      pm.SetBoundaryIndex(boundary_nnz, &renum_items_to_send_[0]);
      int neighbors = md.pe_voisins_.size_array();
      std::vector<True_int> recv;
      for (int i=0; i<neighbors; i++)
        recv.push_back(md.pe_voisins_[i]);
      std::vector<True_int> recv_offset;
      True_int offset = 0;
      recv_offset.push_back(offset);
      // items virtuels communs:
      assert(md.items_to_recv_.get_nb_lists() == neighbors);
      assert(md.blocs_to_recv_.get_nb_lists() == neighbors);
      // Nb items recus:
      for (int pe = 0; pe < md.blocs_to_recv_.get_nb_lists(); pe++)
        {
          offset += md.items_to_recv_.get_list_size(pe); // nb items virtuels communs
          const int nblocs = md.blocs_to_recv_.get_list_size(pe) / 2;
          for (int ibloc = 0; ibloc < nblocs; ibloc++)
            {
              const int jdeb = md.blocs_to_recv_(pe, ibloc * 2);
              const int jfin = md.blocs_to_recv_(pe, ibloc * 2 + 1);
              offset += jfin - jdeb; // nb items virtuels non communs
            }
          recv_offset.push_back(offset);
        }
      pm.SetReceivers(neighbors, &recv[0], &recv_offset[0]);
      std::vector<True_int> sender;
      std::vector<True_int> send_offset;
      for (int i=0; i<md.pe_voisins_.size_array(); i++)
        sender.push_back(md.pe_voisins_[i]);
      for (int i=0; i<md.items_to_send_.get_index().size_array(); i++)
        send_offset.push_back(md.items_to_send_.get_index()[i]);
      pm.SetSenders(neighbors, &sender[0], &send_offset[0]);
      if (debug) pm.WriteFileASCII("pm");
    }

  // Fill the GlobalMatrix:
  mat.SetParallelManager(pm);

  True_int* row_offset = new True_int[tab1_c.size()];
  True_int* col = new True_int[tab2_c.size()];
  double* val = new double[coeff_c.size()];
  std::copy(tab1_c.begin(), tab1_c.end(), row_offset);
  std::copy(tab2_c.begin(), tab2_c.end(), col);
  std::copy(coeff_c.begin(), coeff_c.end(), val);
  mat.SetLocalDataPtrCSR(&row_offset, &col, &val, "mat", coeff_c.size());
  if (Process::is_parallel())
    {
      row_offset = new True_int[ghost_tab1_c.size()];
      col = new True_int[ghost_tab2_c.size()];
      val = new double[ghost_coeff_c.size()];
      std::copy(ghost_tab1_c.begin(), ghost_tab1_c.end(), row_offset);
      std::copy(ghost_tab2_c.begin(), ghost_tab2_c.end(), col);
      std::copy(ghost_coeff_c.begin(), ghost_coeff_c.end(), val);
      mat.SetGhostDataPtrCSR(&row_offset, &col, &val, "ghost", ghost_coeff_c.size());    // LocalMatrix ghost
    }
  /* Cela serait plus simple en passant les std::vector mais valgrind rale lors d'un free_host a la fin lors de ~GlobalMatrix :-(
  mat.SetLocalDataPtrCSR(reinterpret_cast<int **>(&tab1_c), reinterpret_cast<int **>(&tab2_c), reinterpret_cast<double **>(&coeff_c), "mat", coeff_c.size());   // LocalMatrix
  if (Process::is_parallel())
    mat.SetGhostDataPtrCSR(reinterpret_cast<int **>(&ghost_tab1_c), reinterpret_cast<int **>(&ghost_tab2_c),
                           reinterpret_cast<double **>(&ghost_coeff_c), "ghost", ghost_coeff_c.size());    // LocalMatrix ghost
  */
  mat.Sort();

  if (debug)
    {
      Nom filename(Objet_U::nom_du_cas());
      filename += "_rocalution_matrix";
      filename += (Nom)save;
      filename += ".mtx";
      mat.WriteFileMTX(filename.getString());
    }
  Cout << "[rocALUTION] Time to build matrix: " << (rocalution_time() - tick) / 1e6 << finl;

  mat.Info();
#ifndef NDEBUG
  assert(mat.Check());
#endif
  tick = rocalution_time();
  if (Objet_U::computeOnDevice) statistiques().begin_count(gpu_copytodevice_counter_);
  mat.MoveToAccelerator(); // Important: move mat to device so after ls is built on device (best for performance)
  if (Objet_U::computeOnDevice) statistiques().end_count(gpu_copytodevice_counter_, (int)(sizeof(int)*(N+nnz)+sizeof(double)*nnz));
  Cout << "[rocALUTION] Time to copy matrix on device: " << (rocalution_time() - tick) / 1e6 << finl;

  tick = rocalution_time();
  ls->SetOperator(mat);
  if (sp_ls!=nullptr)    // Important: Inner solver for mixed-precision solver set after SetOperator and before Build
    {
      Process::exit("Not implemented in //");
      /*
      auto& mp_ls = dynamic_cast<MixedPrecisionDC<GlobalMatrix<double>, GlobalVector<double>, double, GlobalMatrix<float>, GlobalVector<float>, float> &>(*ls);
      mp_ls.Set(*sp_ls);
       */
    }
  // AMG smoothers On doit le faire apres le SetOperator(mat) pour avoir les differentes grilles:
  // Default: FixedPoint/Jacobi ?
  if (lp!=nullptr)
    {
      try
        {
          auto& mg = dynamic_cast<BaseAMG<LocalMatrix<double>, LocalVector<double>, double> &>(*lp);
          mg.SetOperator(mat.GetInterior());
          mg.BuildHierarchy();
          mg.SetManualSmoothers(true);
          // Smoother for each level
          mg.SetSmootherPreIter(1);
          mg.SetSmootherPostIter(1);
          int levels = mg.GetNumLevels();
          auto **sm = new IterativeLinearSolver<LocalMatrix<double>, LocalVector<double>, double> *[levels - 1];
          auto **gs = new Preconditioner<LocalMatrix<double>, LocalVector<double>, double> *[levels - 1];
          for (int i = 0; i < levels - 1; ++i)
            {
              if (smoother_ == "CHEBYSHEV")
                {
                  Chebyshev<LocalMatrix<double>, LocalVector<double>, double> *krylov;    // GPU build/solve
                  krylov = new Chebyshev<LocalMatrix<double>, LocalVector<double>, double>;
                  sm[i] = krylov;
                  //sm[i]->SetPreconditioner(*gs[i]);
                  sm[i]->Verbose(precond_verbosity_);
                }
              else
                {
                  FixedPoint<LocalMatrix<double>, LocalVector<double>, double> *fp;
                  fp = new FixedPoint<LocalMatrix<double>, LocalVector<double>, double>;
                  sm[i] = fp;
                  if (smoother_ == "GS")
                    gs[i] = new GS<LocalMatrix<double>, LocalVector<double>, double>;    // Non, CPU only
                  else if (smoother_ == "MCGS|SOR")
                    gs[i] = new MultiColoredGS<LocalMatrix<double>, LocalVector<double>, double>;    // GPU build/solve
                  else if (smoother_ == "MCSGS|SSOR")
                    gs[i] = new MultiColoredSGS<LocalMatrix<double>, LocalVector<double>, double>;    // GPU build/solve
                  else if (smoother_ == "JACOBI") // Defaut
                    gs[i] = new Jacobi<LocalMatrix<double>, LocalVector<double>, double>;
                  else
                    Process::exit("Unknown smoother.");
                  fp->SetRelaxation(omega_); // defaut 1.0: Important pour la vitesse de convergence 0.8 semble bien
                  Cout << "[rocALUTION] Relaxation factor set to " << omega_ << finl;
                  sm[i]->SetPreconditioner(*gs[i]);
                  sm[i]->Verbose(precond_verbosity_);
                }
            }
          Cout << "[rocALUTION] Setting smoother on the " << levels << " levels: ";
          gs[0]->Print();
          mg.SetSmoother(sm);
          mg.Verbose(precond_verbosity_);
        }
      catch (const std::bad_cast&)
        {
          Cout << "[rocALUTION] You can't use smoother " << smoother_ << " with this solver." << finl;
          //Process::exit();
        };
    }

  ls->Build();
  ls->Print();
  /*
  if (sp_ls!=nullptr) sp_ls->Print();
  if (p!=nullptr) p->Print();
  if (sp_p!=nullptr) sp_p->Print(); */
  Cout << "[rocALUTION] Time to build solver: " << (rocalution_time() - tick) / 1e6 << finl;
#endif
}
