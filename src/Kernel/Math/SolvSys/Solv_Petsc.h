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

#ifndef Solv_Petsc_included
#define Solv_Petsc_included

#include <petsc_for_kernel.h>
#include <EChaine.h>
#include <Nom.h>
#undef setbit // Sinon conflit de ArrOfBit.h avec Petsc
#include <SolveurSys_base.h>
#include <ArrOfBit.h>
#ifdef PETSCKSP_H
#include <petscdm.h>
#include <TRUSTTab.h>
#endif

class Matrice_Morse_Sym;
class Matrice_Morse;


enum solveur_direct_ { no, mumps, superlu_dist, petsc, umfpack, pastix, cholmod, cli };
extern bool gmres_right_unpreconditionned;

class Solv_Petsc : public SolveurSys_base
{

  Declare_instanciable_sans_constructeur_ni_destructeur(Solv_Petsc);

public :

  inline Solv_Petsc();
  inline ~Solv_Petsc() override;
  inline int solveur_direct() const override
  {
    return (solveur_direct_>0);
  };
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& ) override;
  inline int resoudre_systeme(const Matrice_Base& M, const DoubleVect& A, DoubleVect& B, int niter_max) override
  {
    return resoudre_systeme(M,A,B);
  };
  void create_solver(Entree&);
  inline void reset();
  inline bool read_matrix() const
  {
    return read_matrix_ == 1;
  }
#ifdef PETSCKSP_H
  inline Solv_Petsc& operator=(const Solv_Petsc&);
  inline Solv_Petsc(const Solv_Petsc&);

  inline void initialize();
  inline const Nom& get_chaine_lue() const
  {
    return chaine_lue_ ;
  };
  inline bool gpu() const
  {
    return gpu_;
  };
  inline bool amgx() const
  {
    return amgx_;
  };
  inline bool amgx_initialized()
  {
    return amgx_initialized_;
  };
  void lecture(Entree&);
  PetscErrorCode set_convergence_test(PetscErrorCode (*converge)(KSP,PetscInt,PetscReal,KSPConvergedReason*,void*),void *cctx,PetscErrorCode (*destroy)(void*))
  {
    return KSPSetConvergenceTest(SolveurPetsc_, converge, cctx, destroy);
  }
  // Timers:
  static PetscLogStage KSPSolve_Stage_;
  void set_rtol(const double& rtol)
  {
    seuil_relatif_ = rtol;
    KSPSetTolerances(SolveurPetsc_, seuil_relatif_, seuil_, (divtol_==0 ? PETSC_DEFAULT : divtol_), nb_it_max_);
  }
#endif
  static int instance;               // Nombre d'instances en cours de la classe
  static int numero_solveur;         // Compte les solveurs crees et utilises pour le prefix des options
protected :
#ifdef PETSCKSP_H
  void construit_renum(const DoubleVect&);
  void check_aij(const Matrice_Morse&);
  void Create_vectors(const DoubleVect&); // Construit les vecteurs Petsc x et b
  void Create_DM(const DoubleVect& ); // Construit un DM (Distributed Mesh)
  void Create_MatricePetsc(Mat&, int, const Matrice_Morse&); // Construit et remplit une matrice Petsc depuis la matrice_morse
  virtual void Create_objects(const Matrice_Morse&, int); // Construit differents objets PETSC dont matrice
  virtual void Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat_morse); // Fill the (previously allocated) PETSc matrix with mat_morse coefficients
  virtual int solve(ArrOfDouble& residual); // Solve Ax=b and return residual
  virtual void finalize() {};
  virtual bool check_stencil(const Matrice_Morse&);
  bool nouveau_stencil()
  {
    return nouveau_stencil_;
  }; // ToDo: Remonter dans Solveur_Sys avec nouvelle_matrice
  bool enable_ksp_view( void );
  int add_option(const Nom& option, const double& value, int cli = 0);
  int add_option(const Nom& option, const Nom& value, int cli = 0);
  void MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M);
  void SaveObjectsToFile();
  void RestoreMatrixFromFile();
  int compute_nb_rows_petsc(int);

  // Attributes
  double seuil_;
  double seuil_relatif_;
  double divtol_;
  bool nouveau_stencil_;

  // Objets Petsc
  Mat MatricePetsc_;
  Vec SecondMembrePetsc_;
  Vec SolutionPetsc_;		// Globale solution
  KSP SolveurPetsc_;
  PC PreconditionneurPetsc_;
  DM dm_;                       //description de champs PETSC
  int decalage_local_global_;   // Decalage numerotation local/global pour matrice et vecteur PETSc
  int nb_rows_;                 // Nombre de lignes locales de la matrice TRUST
  int nb_rows_tot_;             // Nombre de lignes globales de la matrice TRUST
  IntTab renum_;                // Tableau de renumerotation lignes matrice TRUST -> matrice Petsc
  ArrOfBit items_to_keep_;      // Faut t'il conserver dans la matrice Petsc la ligne item de la matrice TRUST ?
  int nb_items_to_keep_;        // Nombre local d'items a conserver
  int matrice_symetrique_;      // Drapeau sur la symetrie de la matrice
  int preconditionnement_non_symetrique_; // Drapeau sur la symetrie de la matrice de preconditionnement
  int nb_it_max_;		// Maximal number of iterations
  int convergence_with_nb_it_max_; 	// Convergence decided with nb_it_max_ specified and not by seuil threshold
  int ignore_nb_it_max_;
  int controle_residu_;         // Verification si le residu ||Ax-B||<seuil
  int block_size_;              // Block size for SBAIJ matrix
  int save_matrix_;             // Save constant matrix in a file
  Nom factored_matrix_;		// Deal with the A=LU factorization on disk
  int mataij_;			// Force the use of a Mataij matrix
  Nom type_pc_;			// Preconditioner type
  Nom type_ksp_;		// KSP type
  Nom option_prefix_;		// Prefix des options CLI (permet de faire plusieurs solveurs en CLI)

  int petsc_nb_cpus_;		// Number of CPUs used to solve the PETSc matrix
  int petsc_cpus_selection_;	// Selection of CPUs used to solve the PETSc matrix
  int different_partition_;	// Different partition for the TRUST matrix and the PETSc matrix
  int petsc_decide_;		// PETSc decide of the matrix partition
  Vec LocalSolutionPetsc_;	// Local solution in case of petsc_decide_=1
  VecScatter VecScatter_;	// Scatter context needed when petsc_decide_=1 to gather values of global to local solution
#endif
  int solveur_direct_;          // Pour savoir si l'on manipule un solveur direct et non iteratif
  Nom chaine_lue_; 		// Chaine des mots cles lus
  int read_matrix_;		// Read constant matrix in a file
  bool gpu_;                    // Utilisation des solveurs GPU de PETSc
  bool amgx_;			// Utilisation des solveurs GPU de AMGX
  const Nom config();    // Nom du fichier de config eventuel
  bool amgx_initialized_;	// Amgx initialise
  // Options dev:
  bool ignore_new_nonzero_;
  bool rebuild_matrix_;
  bool allow_realloc_;
  bool clean_matrix_;
  bool reduce_ram_;
  bool verbose = false; // Timing
};

#define NB_IT_MAX_DEFINED 10000

inline Solv_Petsc::Solv_Petsc()
{
  read_matrix_=0;
#ifdef PETSCKSP_H
  initialize();
  instance++;
  //  Journal()<<"creation solv_petsc "<<instance<<finl;
#endif
}

inline Solv_Petsc::~Solv_Petsc()
{
  reset();
  instance--;
  // Journal()<<" destr solv_petsc "<<instance<<finl;
}
inline void Solv_Petsc::reset()
{
#ifdef PETSCKSP_H
  if (SolveurPetsc_!=NULL)
    {
      KSPDestroy(&SolveurPetsc_);
      finalize();
    }
  if (MatricePetsc_!=NULL)
    {
      // Destruction des vecteurs
      VecDestroy(&SecondMembrePetsc_);
      VecDestroy(&SolutionPetsc_);
      if (LocalSolutionPetsc_!=NULL)
        {
          VecDestroy(&LocalSolutionPetsc_);
          VecScatterDestroy(&VecScatter_);
        }
      // Destruction matrice
      MatDestroy(&MatricePetsc_);
      // Destruction DM
      if (dm_!=NULL)
        DMDestroy(&dm_);
    }
  initialize();
#endif
}

#ifdef PETSCKSP_H
inline void Solv_Petsc::initialize()
{
  matrice_symetrique_=-1;
  preconditionnement_non_symetrique_=0;
  seuil_ = 0;
  seuil_relatif_ = 0;
  divtol_ = 0;
  nouveau_stencil_ = true;
  petsc_cpus_selection_ = 0;	     // By default, 0 (no selection). 1 means: first petsc_nb_cpus_ CPUs is used, 2 means: every petsc_nb_cpus_ CPUs is used
  petsc_nb_cpus_ = Process::nproc(); // By default the number of processes
  different_partition_ = 0;          // By default, same matrix partition
  petsc_decide_ = 0;                 // By default, 0. 1 is OK but does NOT improve performance (rather decrease)
  convergence_with_nb_it_max_ = 0;
  ignore_nb_it_max_ = 0;
  nb_it_max_ = NB_IT_MAX_DEFINED;
  save_matrix_=0;
  mataij_=0;
  factored_matrix_="";
  solveur_direct_=no;
  controle_residu_=0;
  gpu_=false;
  amgx_=false;
  amgx_initialized_=false;
  block_size_=1;
  option_prefix_="??";
  dm_=NULL;
  MatricePetsc_ = NULL;
  SecondMembrePetsc_ = NULL;
  SolutionPetsc_ = NULL;
  SolveurPetsc_ = NULL;
  LocalSolutionPetsc_ = NULL;
  VecScatter_ = NULL;
  // Dev:
  ignore_new_nonzero_ = false;
  rebuild_matrix_ = false;
  allow_realloc_ = true;
  clean_matrix_ = true;
  reduce_ram_ = false;
  if (instance==-1)
    {
      // First initialization:
      instance=0;
      char version[256];
      PetscGetVersion(version,256);
      Cerr << "******************************************************************************************" << finl;
      Cerr << "Commands lines possible for " << version << ":" << finl;
      Cerr << "-ksp_view : to have some informations on the solver/preconditioner used by PETSc." << finl;
      Cerr << "-info : to have among other informations on storage of matrices of PETSc." << finl;
      Cerr << "-log_summary : to have at the end of the calculation, informations about performances and memory." << finl;
      Cerr << "-log_all : trace all PETSc calls." << finl;
      Cerr << "-malloc_dump : to have at the end of the calculation the memory not deallocated by PETSc." << finl;
      Cerr << "-help : to know all the commands lines of PETSc including those related to the solver/preconditioner selected." << finl;
      Cerr << "******************************************************************************************" << finl;
      if (!disable_TU)
        Cerr << "NB: if you want to disable the wrinting of the *_petsc.TU file then specify the disable_TU flag in your datafile before reading the block of schema in time." << finl;
      else
        Cerr << "Reading of disable_TU flag => Disable the writing of the *_petsc.TU file."<< finl;
      PetscLogStageRegister("KSPSolve",&KSPSolve_Stage_);
    }
}

inline Solv_Petsc::Solv_Petsc(const Solv_Petsc& org):SolveurSys_base::SolveurSys_base()
{
  initialize();
  instance++;
  // Journal()<<"copie solv_petsc "<<instance<<finl;
  read_matrix_=org.read_matrix();
  gpu_=org.gpu();
  amgx_=org.amgx();
  option_prefix_=org.option_prefix_;
  // on relance la lecture ....
  EChaine recup(org.get_chaine_lue());
  readOn(recup);
}

inline Solv_Petsc& Solv_Petsc::operator=(const Solv_Petsc&)
{
  Cerr<<"Solv_Petsc::operator=(const Solv_Petsc&) is not coded."<<finl;
  exit();
  return (*this);
}

#endif

class option
{
public:
  int defined;
  Nom name;
};

class option_string : public option
{
public:
  option_string(Nom n, Nom v)
  {
    defined=0;
    name=n;
    value_=v;
  }
  Nom value_;
  inline Nom& value()
  {
    return value_;
  };
};

class option_int : public option
{
public:
  option_int(Nom n, int v)
  {
    defined=0;
    name=n;
    value_=v;
  }
  int value_;
  inline int& value()
  {
    return value_;
  };
};

class option_double : public option
{
public:
  option_double(Nom n, double v)
  {
    defined=0;
    name=n;
    value_=v;
  }
  double value_;
  inline double& value()
  {
    return value_;
  };
};

#endif


