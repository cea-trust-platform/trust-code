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

#include <Solv_rocALUTION.h>
#include <Matrice_Morse_Sym.h>
#include <TRUSTVect.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>
#include <Comm_Group_MPI.h>
#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>

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
  if (sp_ls!=nullptr)
    {
      sp_ls->Clear();
      delete sp_ls;
    }
  if (gp!=nullptr)
    {
      gp->Clear();
      delete gp;
    }
  if (sp_p!=nullptr)
    {
      sp_p->Clear();
      delete sp_p;
    }
#endif
}

void Solv_rocALUTION::initialize()
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  ls = nullptr;
  sp_ls = nullptr;
  gp = nullptr;
  sp_p = nullptr;
  lp = nullptr;
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
static Solver<LocalMatrix<double>, LocalVector<double>, double>* local_solver; // ToDo attribut?
// Fonction template pour la creation des precond simple ou double precision
#ifdef ROCALUTION_ROCALUTION_HPP_
template <typename T>
Solver<GlobalMatrix<T>, GlobalVector<T>, T>* Solv_rocALUTION::create_rocALUTION_precond(EChaine& is)
{
  Solver<GlobalMatrix<T>, GlobalVector<T>, T>* p;
  Motcle precond;
  is >> precond;
  Cerr << precond << " ";
  // Preconditioner
  if (precond==(Motcle)"Jacobi" || precond==(Motcle)"diag") // OK en ~335 its
    {
      p = new Jacobi<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option(is, "");
    }
  else if (precond==(Motcle)"PairwiseAMG" || precond == (Motcle) "Pairwise-AMG") //
    {
      p = new PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      auto& pairwiseamg = dynamic_cast<PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p);
      pairwiseamg.SetBeta(0.25); // [0-1] default 0.25
      pairwiseamg.SetCoarsestLevel(10000); // Sinon boucle indefiniment...
      pairwiseamg.SetOrdering(MIS); // default
      pairwiseamg.SetCoarseningFactor(10); // [0-20] default 4
      pairwiseamg.SetKcycleFull(true); // default true
      pairwiseamg.InitMaxIter(1);
      precond_option(is, "");
    }
  else if (precond==(Motcle)"null")
    {
      p = nullptr;
      precond_option(is, ""); // Pas de preconditionnement
    }
  else
    {
      // Local preconditionners with BlockJacobi preconditionner:
      p = new BlockJacobi<GlobalMatrix<T>, GlobalVector<T>, T>();
      if (precond == (Motcle) "MultiColoredGS")  //
        {
          lp = new MultiColoredGS<LocalMatrix<T>, LocalVector<T>, T>(); // Attention converge avec BICGSTAB pas CG...
          double omega = ::precond_option(is, "omega");
          if (omega >= 0) dynamic_cast<MultiColoredGS<LocalMatrix<T>, LocalVector<T>, T> &>(*lp).SetRelaxation(omega);
        }
      else if (precond == (Motcle) "ILU")  //
        {
          lp = new ILU<LocalMatrix<T>, LocalVector<T>, T>(); // Converge pas non plus donc probleme...
          int level = (int) precond_option(is, "level");
          if (level >= 0)
            dynamic_cast<ILU<LocalMatrix<T>, LocalVector<T>, T> &>(*lp).Set(level, true);
        }
      else if (precond == (Motcle) "SAAMG" || precond == (Motcle) "SA-AMG")  // Converge avec BICGStab (setup SAAMG desormais sur GPU)
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
      else if (precond==(Motcle)"UAAMG" || precond==(Motcle)"UA-AMG")  // Converge en ? its
        {
          lp = new UAAMG<LocalMatrix<T>, LocalVector<T>, T>();
          auto& uuamg = dynamic_cast<UAAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          uuamg.InitMaxIter(1);
          uuamg.SetCoarseningStrategy(CoarseningStrategy::PMIS);
          uuamg.SetCoarsestLevel(200);
          uuamg.SetCouplingStrength(0.001);
          precond_option(is, "");
        }
      else if (precond==(Motcle)"RugeStuebenAMG" || precond==(Motcle)"C-AMG")  // Equivalent a C-AMG ?
        {
          lp = new RugeStuebenAMG<LocalMatrix<T>, LocalVector<T>, T>();
          auto& rsamg = dynamic_cast<RugeStuebenAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
          rsamg.InitMaxIter(1);
          rsamg.SetCoarseningStrategy(CoarseningStrategy::PMIS);
          rsamg.SetCoarsestLevel(200);
          precond_option(is, "");
        }
      else if (precond == (Motcle) "GS")  //
        {
          lp = new GS<LocalMatrix<T>, LocalVector<T>, T>(); // ???
          precond_option(is, "");
        }
      else if (precond == (Motcle) "SGS")  //
        {
          lp = new SGS<LocalMatrix<T>, LocalVector<T>, T>(); // ???
          precond_option(is, "");
        }
      else
        {
          Cerr << "Error! Unknown rocALUTION preconditionner: " << precond << finl;
          Process::exit();
          return nullptr;
        }
      // Coarse solver for local AMG
      if (coarse_grid_solver_=="LU")
        {
          try
            {
              // *** warning: LocalMatrix::LUFactorize() is performed in DENSE format
              // *** warning: LocalMatrix::LUFactorize() is performed on the host
              auto& base_amg = dynamic_cast<BaseAMG<LocalMatrix<T>, LocalVector<T>, T> &>(*lp);
              local_solver = new LU<LocalMatrix<T>, LocalVector<T>, T>();
              local_solver->Verbose(precond_verbosity_);
              base_amg.SetManualSolver(true);
              base_amg.SetSolver(*local_solver);
            }
          catch (const std::bad_cast& error) {}
        }
      else if (coarse_grid_solver_!="JACOBI")
        {
          Process::exit("LU or JACOBI solver can be used on coarse grid.");
        }
      // Set local preconditionner to BlockJacobi global one
      dynamic_cast<BlockJacobi<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).Set(*lp);
    }
  p->Verbose(precond_verbosity_);
  if (lp!= nullptr)  lp->Verbose(precond_verbosity_);
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
#endif

// Lecture et creation du solveur
void Solv_rocALUTION::create_solver(Entree& entree)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  // Valeurs par defaut:
  atol_ = 1.e-12;
  rtol_ = 1.e-12;
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
      gp = create_rocALUTION_precond<double>(precond);
      ls = create_rocALUTION_solver<double>(solver);
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
  a.Apply(x, &e);
  e.ScaleAdd(-1.0, b);
  return e.Norm();
}
#endif

int Solv_rocALUTION::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  if (write_system_) save++;
  double tick;

  MPI_Comm comm = MPI_COMM_WORLD;
  pm.SetMPICommunicator(&comm);
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
      try
        {
          // Provisoire pourquoi rebuild pour pairwise amg ?
          auto& mg = dynamic_cast<PairwiseAMG<GlobalMatrix<double>, GlobalVector<double>, double> &>(*gp);
          tick = rocalution_time();
          ls->ReBuildNumeric();
          Cout << "[rocALUTION] Time to rebuild solver: " << (rocalution_time() - tick) / 1e6 << finl;
        }
      catch (const std::bad_cast& error) {}
    }
  bool debug = b.size_array() < 100;
  if (debug)
    {
      Journal() << "pm.GetLocalNrow()=" << pm.GetLocalNrow() << " <> b.size_array()=" << b.size_array() << finl;
      Journal() << "mat.GetN()=" << mat.GetM() << " mat.GetLocalN()=" << mat.GetLocalN() << " mat.GetGhostN()=" << mat.GetGhostN() << finl;
    }
  // Build rhs and initial solution:
  tick = rocalution_time();


  long N = pm.GetGlobalNrow();
  int size=b.size_array();
  if (b_.size_array()==0)
    {
      // Allocation initiale
      x_.resize(size);
      b_.resize(size);
      sol.SetParallelManager(pm);
      rhs.SetParallelManager(pm);
      e.SetParallelManager(pm);
      sol.Allocate("a", N);
      rhs.Allocate("rhs", N);
      e.Allocate("e", N);
    }
  int row=0;
  int row_ghost=nb_rows_;
  for (int i=0; i<size; i++)
    {
      if (items_to_keep_[i])
        {
          x_(row) = x(i);
          b_(row) = b(i);
          row++;
        }
      else
        {
          x_(row_ghost) = x(i);
          b_(row_ghost) = b(i);
          row_ghost++;
        }
    }
  sol.GetInterior().CopyFromData(&x_[0]);
  rhs.GetInterior().CopyFromData(&b_[0]);
  // ToDo pour eviter une copie (mais bizarre ne marche pas):
  //sol.SetDataPtr(reinterpret_cast<double **>(&x_), "x", size);
  //rhs.SetDataPtr(reinterpret_cast<double **>(&b_), "b", size);

  bool provisoire = false;
  if (provisoire)
    {
#ifdef NDEBUG
      Process::exit("Supprimer!");
#endif
      x = 0;
      sol.Zeros();
      e.Zeros();
      if (Process::me() == 0)
        {
          x[0] = 1;
          sol[0] = 1;
        }
      x.echange_espace_virtuel();
    }

  Cout << "[rocALUTION] Time to build vectors: " << (rocalution_time() - tick) / 1e6 << finl;
  tick = rocalution_time();
  sol.MoveToAccelerator();
  rhs.MoveToAccelerator();
  e.MoveToAccelerator();
  Cout << "[rocALUTION] Time to move vectors on device: " << (rocalution_time() - tick) / 1e6 << finl;
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
  ls->Solve(rhs, &sol);
  if (ls->GetSolverStatus()==3) Process::exit("Divergence for solver.");
  if (ls->GetSolverStatus()==4)
    {
      Cout << "Error, maximum number of iterations reached." << finl;
      Cout << "If you used GCP, try BiCGSTAB solver." << finl;
      Process::exit();
    }
  Cout << "[rocALUTION] Time to solve: " << (rocalution_time() - tick) / 1e6 << finl;
  tick = rocalution_time();

  int nb_iter = ls->GetIterationCount();
  double res_final = ls->GetCurrentResidual();

  // Recupere la solution
  sol.MoveToHost();
  sol.GetInterior().CopyToData(x_.addr());
  row = 0;
  for (int i=0; i<size; i++)
    if (items_to_keep_[i])
      {
        x(i) = x_(row);
        row++;
      }
  x.echange_espace_virtuel();
  if (first_solve_) res_final = residual(a, b, x); // Securite a la premiere resolution
#ifndef NDEBUG
  check(x, sol, "After solve ||x||");
  check(b, rhs, "After solve ||b||");
#endif

  // Check residual e=||Ax-rhs||:
  if (res_final>atol_)
    {
      Cerr << "Solution not correct ! ||Ax-b|| = " << res_final << finl;
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
  const ArrOfInt& renum_array = renum_;  // tableau vu comme lineaire
  const MD_Vector_base& mdv = renum_.get_md_vector().valeur();
  const MD_Vector_std& md = sub_type(MD_Vector_composite, mdv) ? ref_cast(MD_Vector_composite, mdv).global_md_ : ref_cast(MD_Vector_std, mdv);
  IntVect local_renum(items_to_keep_.size_array()); // ToDo remonter dans Solv_externe
  int col_virt=0,col_reel=0;
  for(int i=0; i<items_to_keep_.size_array(); i++)
    if (items_to_keep_[i])
      {
        local_renum[i] = col_reel;
        col_reel++;
      }
    else
      {
        local_renum[i] = col_virt;
        col_virt++;
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
      Journal() << "Provisoire local_renum=" << finl;
      local_renum.ecrit(Journal());
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
              int col = local_renum[j];
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
  int N = (int)tab1_c.size();
  int nnz = (int)coeff_c.size();
  Cout << "[rocALUTION] Build a matrix with local N= " << N << " and local nnz=" << nnz << finl;

  pm.SetGlobalNrow(nb_rows_tot_);
  pm.SetGlobalNcol(nb_rows_tot_);
  pm.SetLocalNrow(nb_rows_);
  pm.SetLocalNcol(nb_rows_);

  if (Process::nproc()>1)
    {
      int boundary_nnz = md.items_to_send_.get_data().size_array();

      // Renum items_to_send et la tri:
      ArrOfInt renum_items_to_send_(md.items_to_send_.get_data().size_array());
      int first_element_dest = 0;
      for (int pe = 0; pe < md.items_to_send_.get_nb_lists(); pe++)
        {
          ArrOfInt items_pe;
          md.items_to_send_.copy_list_to_array(pe, items_pe);
          int size = items_pe.size_array();
          for (int i = 0; i < size; i++)
            items_pe[i] = local_renum[md.items_to_send_(pe, i)];
          items_pe.array_trier_retirer_doublons();
          // Argh, pas specifie dans la doc mais il fallait que BoundaryIndex soit triee (ToDo le dire au support rocALUTIION pour la doc...)
          renum_items_to_send_.inject_array(items_pe, size, first_element_dest, 0);
          first_element_dest += size;
        }
      const int *bnd = renum_items_to_send_.addr();
      pm.SetBoundaryIndex(boundary_nnz, bnd);
      int neighbors = md.pe_voisins_.size_array();
      const int *recv = md.pe_voisins_.addr();

      std::vector<int> recv_offset;
      int offset = 0;
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
      pm.SetReceivers(neighbors, recv, &recv_offset[0]);
      const int *sender = md.pe_voisins_.addr();
      const int *send_offset = md.items_to_send_.get_index().addr();
      pm.SetSenders(neighbors, sender, send_offset);
      if (debug) pm.WriteFileASCII("pm");
    }

  // Fill the GlobalMatrix:
  mat.SetParallelManager(pm);
  mat.SetLocalDataPtrCSR(reinterpret_cast<int **>(&tab1_c), reinterpret_cast<int **>(&tab2_c),
                         reinterpret_cast<double **>(&coeff_c), "mat", (int)coeff_c.size());   // LocalMatrix
  if (Process::nproc()>1)
    mat.SetGhostDataPtrCSR(reinterpret_cast<int **>(&ghost_tab1_c), reinterpret_cast<int **>(&ghost_tab2_c),
                           reinterpret_cast<double **>(&ghost_coeff_c), "ghost", (int)ghost_coeff_c.size());    // LocalMatrix ghost
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
  mat.MoveToAccelerator(); // Important: move mat to device so after ls is built on device (best for performance)
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
  try
    {
      auto& mg = dynamic_cast<SAAMG<LocalMatrix<double>, LocalVector<double>, double> &>(*lp);
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
          FixedPoint<LocalMatrix<double>, LocalVector<double>, double> *fp;
          fp = new FixedPoint<LocalMatrix<double>, LocalVector<double>, double>;
          sm[i] = fp;
          if (smoother_=="GS")
            gs[i] = new GS<LocalMatrix<double>, LocalVector<double>, double>;    // Converge mieux avec BICGstab mais plus lent que Jacobi
          else
            gs[i] = new Jacobi<LocalMatrix<double>, LocalVector<double>, double>;
          //fp->SetRelaxation(0.7);
          sm[i]->SetPreconditioner(*gs[i]);
          sm[i]->Verbose(precond_verbosity_);
        }
      Cout << "Setting smoother on the " << levels << " levels: ";
      gs[0]->Print();
      mg.SetSmoother(sm);
      mg.Verbose(precond_verbosity_);
    }
  catch (const std::bad_cast& error)
    {
      //Cout << "precond is:" << finl;
      //lp->Print();
    };

  ls->Build();
  ls->Print();
  /*
  if (sp_ls!=nullptr) sp_ls->Print();
  if (p!=nullptr) p->Print();
  if (sp_p!=nullptr) sp_p->Print(); */
  Cout << "[rocALUTION] Time to build solver: " << (rocalution_time() - tick) / 1e6 << finl;
#endif
}
