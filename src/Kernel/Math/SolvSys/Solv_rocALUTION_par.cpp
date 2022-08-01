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

#include <Solv_rocALUTION_par.h>
#include <Matrice_Morse_Sym.h>
#include <TRUSTVect.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>
#include <Comm_Group_MPI.h>
#include <MD_Vector_std.h>
#include <MD_Vector_composite.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION_par,"Solv_rocALUTION_par",SolveurSys_base);

// printOn
Sortie& Solv_rocALUTION_par::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_rocALUTION_par::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

Solv_rocALUTION_par::Solv_rocALUTION_par()
{
  initialize();
}

Solv_rocALUTION_par::Solv_rocALUTION_par(const Solv_rocALUTION_par& org)
{
  initialize();
  // on relance la lecture ....
  EChaine recup(org.get_chaine_lue());
  readOn(recup);
}

Solv_rocALUTION_par::~Solv_rocALUTION_par()
{
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
  if (p!=nullptr)
    {
      p->Clear();
      delete p;
    }
  if (sp_p!=nullptr)
    {
      sp_p->Clear();
      delete sp_p;
    }
}

void Solv_rocALUTION_par::initialize()
{
  ls = nullptr;
  sp_ls = nullptr;
  p = nullptr;
  sp_p = nullptr;
  write_system_ = false;
}

double precond_option_par(Entree& is, const Motcle& motcle)
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

// Fonction template pour la creation des precond simple ou double precision
template <typename T>
Solver<GlobalMatrix<T>, GlobalVector<T>, T>* create_rocALUTION_precond(EChaine& is)
{
  Solver<GlobalMatrix<T>, GlobalVector<T>, T>* p;
  Motcle precond;
  is >> precond;
  Cerr << precond << " ";
  // Preconditioner
  if (precond==(Motcle)"Jacobi" || precond==(Motcle)"diag") // OK en ~335 its
    {
      p = new Jacobi<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  /* ToDo: precond sequentiels !
  else if (precond==(Motcle)"ILUT") // OK en 78 its
    {
      p = new ILUT<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"SPAI") // Converge pas
    {
      p = new SPAI<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"ILU") // OK en ~123 its (level 0), 76 its (level 1)
    {
      p = new ILU<GlobalMatrix<T>, GlobalVector<T>, T>();
      int level = (int)precond_option_par(is, "level");
      if (level>=0) dynamic_cast<ILU<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).Set(level, true);
    }
  else if (precond==(Motcle)"MultiColoredSGS") // OK en ~150 its (mais 183 omega 1.6!) (semble equivalent a GCP/SSOR mais 3 fois plus lent)
    {
      p = new MultiColoredSGS<GlobalMatrix<T>, GlobalVector<T>, T>();
      double omega = precond_option_par(is, "omega");
      if (omega>=0) dynamic_cast<MultiColoredSGS<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).SetRelaxation(omega);
    }
  else if (precond==(Motcle)"GS") // Converge pas
    {
      p = new GS<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"MultiColoredGS") // Converge pas
    {
      p = new MultiColoredGS<GlobalMatrix<T>, GlobalVector<T>, T>();
      double omega = ::precond_option_par(is, "omega");
      if (omega>=0) dynamic_cast<MultiColoredGS<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).SetRelaxation(omega);
    }
  else if (precond==(Motcle)"SGS") // Converge pas
    {
      p = new SGS<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"BlockPreconditioner")   // ToDo: See page 67
    {
      p = new BlockPreconditioner<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"SAAMG" || precond==(Motcle)"SA-AMG")  // Converge en 100 its
    {
      p = new SAAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    }
  else if (precond==(Motcle)"UAAMG" || precond==(Motcle)"UA-AMG")  // Converge en 120 its
    {
      p = new UAAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
      dynamic_cast<UAAMG<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).Verbose(0);
    }
  else if (precond==(Motcle)"PairwiseAMG")  // Converge pas
    {
      p = new PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
      dynamic_cast<PairwiseAMG<GlobalMatrix<T>, GlobalVector<T>, T> &>(*p).Verbose(0);
    }
  else if (precond==(Motcle)"RugeStuebenAMG" || precond==(Motcle)"C-AMG")  // Converge en 57 its (equivalent a C-AMG ?)
    {
      p = new RugeStuebenAMG<GlobalMatrix<T>, GlobalVector<T>, T>();
      precond_option_par(is, "");
    } */
  else
    {
      Cerr << "Error! Unknown rocALUTION preconditionner: " << precond << finl;
      Process::exit();
      return nullptr;
    }
  return p;
}

// Fonction template pour la creation des solveurs simple ou double precision
template <typename T>
IterativeLinearSolver<GlobalMatrix<T>, GlobalVector<T>, T>* create_rocALUTION_solver(const Motcle& solver)
{
  if (solver==(Motcle)"GCP")
    {
      return new CG<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else if (solver==(Motcle)"GMRES")
    {
      return new GMRES<GlobalMatrix<T>, GlobalVector<T>, T>();
    }
  else
    {
      Cerr << solver << " solver not recognized for rocALUTION." << finl;
      Process::exit();
      return nullptr;
    }
}

// Lecture et creation du solveur
void Solv_rocALUTION_par::create_solver(Entree& entree)
{
#ifdef ROCALUTION_ROCALUTION_HPP_
  // Valeurs par defaut:
  atol_ = 1.e-12;
  rtol_ = 1.e-12;
  double div_tol = 1e3;
  bool mixed_precision = false;

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
      p = create_rocALUTION_precond<double>(precond);
      ls = create_rocALUTION_solver<double>(solver);
      ls->SetPreconditioner(*p);
    }
  ls->InitTol(atol_, rtol_, div_tol);
  //p->FlagPrecond();
  //ls->Clear();
  //ls->InitMaxIter(500);
  //ls->InitMinIter(20);
#endif
}

static int save=0;
void write_matrix_par(const Matrice_Base& a)
{
  Nom name("trust_matrix");
  name += (Nom)save;
  Matrice_Morse csr = ref_cast(Matrice_Morse, a);
  csr.WriteFileMTX(name);
}
void write_vectors_par(const GlobalVector<double>& rhs, const GlobalVector<double>& sol)
{
  if (Process::nproc()>1) Process::exit("Not available in parallel yet.");
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
void write_matrix_par(const GlobalMatrix<double>& mat)
{
  if (Process::nproc()>1) Process::exit("Not available in parallel yet.");
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
      Cerr << nom << " Trust " << norm_t << " rocALUTION " << norm_t << " difference " << difference << finl;
      Process::exit();
    }
}
double residual_par(const Matrice_Base& a, const DoubleVect& b, const DoubleVect& x)
{
  DoubleVect e(b);
  e *= -1;
  a.ajouter_multvect(x, e);
  return e.mp_norme_vect();
}
double residual_device_par(const GlobalMatrix<double>& a, const GlobalVector<double>& b, const GlobalVector<double>& x, GlobalVector<double>& e)
{
  a.Apply(x, &e);
  e.ScaleAdd(-1.0, b);
  return e.Norm();
}

int Solv_rocALUTION_par::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
//#ifdef ROCALUTION_ROCALUTION_HPP_
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
      if (nb_rows_<20) Journal() << "Provisoire matrice a=" << a << finl;
      construit_matrice_morse_intermediaire(a, csr);
      if (nb_rows_<20) Journal() << "Provisoire matrice csr=" << csr << finl;
      Cout << "[rocALUTION] Time to convert TRUST matrix: " << (rocalution_time() - tick) / 1e6 << finl;

      // Save TRUST matrix to check:
      if (write_system_) write_matrix_par(csr);

      //if (nouveau_stencil_)
      Create_objects(csr);
      //else
      //    Update_matrix(mat, csr);

      // Save rocALUTION matrix to check
      if (write_system_) write_matrix_par(mat);
    }
  assert(pm.GetLocalSize()==b.size_reelle());
  assert(pm.GetLocalSize()==x.size_reelle());

  // Build rhs and initial solution:
  tick = rocalution_time();
  GlobalVector<double> sol(pm);
  GlobalVector<double> rhs(pm);
  GlobalVector<double> e(pm);
  long N = pm.GetGlobalSize();
  sol.Allocate("a", N);
  //sol.CopyFromData(x.addr());
  sol.GetInterior().CopyFromData(x.addr()); // ToDo ghost aussi ?
  rhs.Allocate("rhs", N);
  //rhs.CopyFromData(b.addr());
  rhs.GetInterior().CopyFromData(b.addr()); // ToDo ghost aussi ?
  Cout << "[rocALUTION] Time to build vectors: " << (rocalution_time() - tick) / 1e6 << finl;
  tick = rocalution_time();
  sol.MoveToAccelerator();
  rhs.MoveToAccelerator();
  e.MoveToAccelerator();
  e.Allocate("e", N);
  Cout << "[rocALUTION] Time to move vectors on device: " << (rocalution_time() - tick) / 1e6 << finl;
  if (write_system_) write_vectors_par(rhs, sol);
  sol.Info();

#ifndef NDEBUG
  // Check before solves:
  pm.Status();
  rhs.Check();
  sol.Check();
  mat.Check();
  check(x, sol, "Before ||x||");
  check(b, rhs, "Before ||b||");
#endif
  // Petit bug rocALUTION (division par 0 si rhs nul, on contourne en mettant la verbosity a 0)
  ls->Verbose(limpr() && rhs.Norm()>0 ? 2 : 0);
  if (sp_ls!=nullptr) sp_ls->Verbose(limpr() && rhs.Norm()>0 ? 2 : 0);

  // Solve A x = rhs
  tick = rocalution_time();
  // Calcul des residus sur le host la premiere fois (plus sur) puis sur device ensuite (plus rapide)
  double res_initial = first_solve_ ? residual_par(a, b, x) : residual_device_par(mat, rhs, sol, e);
  ls->Solve(rhs, &sol);
  if (ls->GetSolverStatus()==3) Process::exit("Divergence for solver.");
  if (ls->GetSolverStatus()==4) Cout << "Maximum number of iterations reached." << finl;
  Cout << "[rocALUTION] Time to solve: " << (rocalution_time() - tick) / 1e6 << finl;

  int nb_iter = ls->GetIterationCount();
  double res_final = ls->GetCurrentResidual();

  // Recupere la solution
  sol.MoveToHost();
  //sol.CopyToData(x.addr());
  sol.GetInterior().CopyToData(x.addr());
  x.echange_espace_virtuel();
  if (first_solve_) res_final = residual_par(a, b, x); // Securite a la premiere resolution
#ifndef NDEBUG
  check(x, sol, "After ||x||");
  check(b, rhs, "After ||b||");
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
  sol.Clear();
  rhs.Clear();
  if (nb_iter>1) first_solve_ = false;
  return nb_iter;
  /* #else
      return -1;
  #endif */
}

void Solv_rocALUTION_par::Create_objects(const Matrice_Morse& csr)
{
  double tick = rocalution_time();
  const ArrOfInt& tab1 = csr.get_tab1();
  const ArrOfInt& tab2 = csr.get_tab2();
  const ArrOfDouble& coeff = csr.get_coeff();
  const ArrOfInt& renum_array = renum_;  // tableau vu comme lineaire
  std::vector<int> tab1_c, ghost_tab1_c;
  std::vector<int> tab2_c, ghost_tab2_c;
  std::vector<double> coeff_c, ghost_coeff_c;
  int cpt = 0, ghost_cpt = 0;
  tab1_c.push_back(cpt);
  ghost_tab1_c.push_back(ghost_cpt);
  for(int i=0; i<tab1.size_array() - 1; i++)
    {
      if (items_to_keep_[i]) // Item sequentiel
        {
          //int ligne_globale = cpt + decalage_local_global_;
          for (int k = tab1[i] - 1; k < tab1[i + 1] - 1; k++)
            {
              double c = coeff[k];
              int j = tab2[k] - 1;
              //  int j = renum_array[tab2[k] - 1];
              if (j<nb_rows_)
                {
                  tab2_c.push_back(j);
                  coeff_c.push_back(c);
                  cpt++;
                }
              else
                {
                  ghost_tab2_c.push_back(j-nb_rows_);
                  ghost_coeff_c.push_back(c);
                  ghost_cpt++;
                }
            }
          tab1_c.push_back(cpt);
          ghost_tab1_c.push_back(ghost_cpt);
        }
    }

  // Copie de la matrice TRUST dans une matrice rocALUTION
  int N = (int)tab1_c.size();
  bool debug = N <= 100;
  int nnz = (int)coeff_c.size();
  Cout << "[rocALUTION] Build a matrix with local N= " << N << " and local nnz=" << nnz << finl;

  pm.SetGlobalSize(nb_rows_tot_);
  pm.SetLocalSize(nb_rows_);

  const MD_Vector_base& mdv = renum_.get_md_vector().valeur();
  const MD_Vector_std& md = sub_type(MD_Vector_composite, mdv) ? ref_cast(MD_Vector_composite, mdv).global_md_ : ref_cast(MD_Vector_std, mdv);

  int boundary_nnz = md.items_to_send_.get_data().size_array();
  const int* bnd = md.items_to_send_.get_data().addr();
  pm.SetBoundaryIndex(boundary_nnz, bnd);
  int neighbors = md.pe_voisins_.size_array();
  const int* recv = md.pe_voisins_.addr();

  if (debug)
    {
      Journal() << "Provisoire items_to_recv_:" << finl;
      md.items_to_recv_.ecrire(Journal());
      Journal() << "Provisoire blocs_to_recv_:" << finl;
      md.blocs_to_recv_.ecrire(Journal());
    }
  std::vector<int> recv_offset;
  int offset = 0;
  recv_offset.push_back(offset);
  for (int pe=0; pe<md.blocs_to_recv_.get_nb_lists(); pe++)
    {
      assert(md.blocs_to_recv_.get_list_size(pe)==2);
      int index = md.blocs_to_recv_.get_index()[pe];
      offset += md.blocs_to_recv_.get_data()[index+1] - md.blocs_to_recv_.get_data()[index];
      recv_offset.push_back(offset);
    }
  pm.SetReceivers(neighbors, recv, &recv_offset[0]);
  const int* sender = md.pe_voisins_.addr();
  const int* send_offset = md.items_to_send_.get_index().addr();
  pm.SetSenders(neighbors, sender, send_offset);

  if (debug) pm.WriteFileASCII("pm");

  // Fill the GlobalMatrix:
  mat.SetParallelManager(pm);
  mat.SetLocalDataPtrCSR(reinterpret_cast<int **>(&tab1_c), reinterpret_cast<int **>(&tab2_c),
                         reinterpret_cast<double **>(&coeff_c), "mat", (int)coeff_c.size());   // LocalMatrix
  mat.SetGhostDataPtrCSR(reinterpret_cast<int **>(&ghost_tab1_c), reinterpret_cast<int **>(&ghost_tab2_c),
                         reinterpret_cast<double **>(&ghost_coeff_c), "ghost", (int)ghost_coeff_c.size());    // LocalMatrix ghost

  if (debug)
    {
      csr.imprimer_formatte(Journal());
      mat.WriteFileMTX("rocALUTION_matrix.mtx");
    }
  Cout << "[rocALUTION] Time to build matrix: " << (rocalution_time() - tick) / 1e6 << finl;

  mat.Info();
#ifndef NDEBUG
  mat.Check();
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
// C-AMG:
  /* Process::exit("Not parallelized yet");
    try
      {
        // ToDo: tuning C-AMG (pas efficace encore). On doit le faire apres le SetOperator(mat) pour avoir les differentes grilles:
        auto& mg = dynamic_cast<RugeStuebenAMG<GlobalMatrix<double>, GlobalVector<double>, double> &>(*p);
        //mg.Verbose(2);
        // Specify coarsest grid solver:
        bool ChangeDefaultCoarsestSolver = false; // Default ?
        if (ChangeDefaultCoarsestSolver)
          {
            mg.SetManualSolver(true);
            Solver<GlobalMatrix<double>, GlobalVector<double>, double> *cgs;
            cgs = new CG<GlobalMatrix<double>, GlobalVector<double>, double>();
        //cgs = new LU<GlobalMatrix<T>, GlobalVector<T>, T>();
            mg.SetSolver(*cgs);
          }
        bool ChangeDefaultSmoothers = false; // Default: FixedPoint/Jacobi ?
        if (ChangeDefaultSmoothers)
          {
        // Specify smoothers:
            mg.SetManualSmoothers(true);
            mg.SetOperator(mat);
            mg.BuildHierarchy();
        // Smoother for each level
            mg.SetSmootherPreIter(1);
            mg.SetSmootherPostIter(1);
            int levels = mg.GetNumLevels();
            auto **sm = new IterativeLinearSolver<GlobalMatrix<double>, GlobalVector<double>, double> *[levels - 1];
            auto **gs = new Preconditioner<GlobalMatrix<double>, GlobalVector<double>, double> *[levels - 1];
            for (int i = 0; i < levels - 1; ++i)
              {
        // Smooth with (ToDo ameliorer car pas efficace...)
                FixedPoint<GlobalMatrix<double>, GlobalVector<double>, double> *fp;
                fp = new FixedPoint<GlobalMatrix<double>, GlobalVector<double>, double>;
                sm[i] = fp;
                gs[i] = new MultiColoredSGS<GlobalMatrix<double>, GlobalVector<double>, double>;
                sm[i]->SetPreconditioner(*gs[i]);
                sm[i]->Verbose(0);
              }
            mg.SetSmoother(sm);
          }
      }
    catch(const std::bad_cast& e)
      { };
  */
  ls->Build();
  ls->Print();
  /*
  if (sp_ls!=nullptr) sp_ls->Print();
  if (p!=nullptr) p->Print();
  if (sp_p!=nullptr) sp_p->Print(); */
  Cout << "[rocALUTION] Time to build solver on device: " << (rocalution_time() - tick) / 1e6 << finl;
}
