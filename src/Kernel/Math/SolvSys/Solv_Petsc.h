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
// File:        Solv_Petsc.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_Petsc_included
#define Solv_Petsc_included

#include <petsc_for_kernel.h>
#include <EChaine.h>
#undef setbit // Sinon conflit de ArrOfBit.h avec Petsc
#include <ArrOfBit.h>
#include <IntTab.h>
#include <SolveurSys_base.h>
#include <ArrOfDouble.h>

#ifdef PETSCKSP_H
#include <petscdm.h>
#endif

class IntVect;
class DoubleTab;
class Matrice_Morse_Sym;
class Matrice_Morse;



class Solv_Petsc : public SolveurSys_base
{

  Declare_instanciable_sans_constructeur_ni_destructeur(Solv_Petsc);

public :

  inline Solv_Petsc();
  inline ~Solv_Petsc();
  inline int solveur_direct() const
  {
    return (solveur_direct_>0);
  };
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect& );
  inline int resoudre_systeme(const Matrice_Base& M, const DoubleVect& A, DoubleVect& B, int niter_max)
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
  // Timers:
  static PetscLogStage KSPSolve_Stage_;
#endif
  static int instance;               // Nombre d'instances en cours de la classe
  static int numero_solveur;         // Compte les solveurs crees et utilises pour le prefix des options

protected :
#ifdef PETSCKSP_H
  void construit_renum(const DoubleVect&);
  void check_aij(const Matrice_Morse&);
  virtual void Create_objects(const Matrice_Morse&, const DoubleVect&); // Construit les objets Petsc
  void Create_vectors(const DoubleVect&); // Construit les vecteurs Petsc x et b
  void Create_DM(const DoubleVect& ); // Construit un DM (Distributed Mesh)
  void Create_MatricePetsc(Mat&, int, const Matrice_Morse&);
  virtual int solve(ArrOfDouble& residual); // Solve Ax=b and return residual
  virtual void finalize() {};
  virtual void set_config(const Nom&) {};
  bool enable_ksp_view( void );
  int add_option(const Nom& option, const Nom& value, int cli = 0);
  void MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M);
  void SaveObjectsToFile();
  void RestoreMatrixFromFile();
  int compute_nb_rows_petsc(int);

  // Attributes
  int nb_matrices_creees_;
  int solveur_cree_;
  double seuil_;
  double seuil_relatif_;
  double divtol_;

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
  bool amgx_initialized_;	// Amgx initialise
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
  if (solveur_cree_)
    {
      assert(solveur_cree_==1);
      KSPDestroy(&SolveurPetsc_);
      finalize();
    }
  if (nb_matrices_creees_)
    {
      // Destruction des vecteurs
      VecDestroy(&SecondMembrePetsc_);
      VecDestroy(&SolutionPetsc_);
      if (petsc_decide_)
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
  seuil_ = 1e-12;
  seuil_relatif_ = 0;
  divtol_ = 0;
  nb_matrices_creees_ = 0;
  solveur_cree_ = 0;
  petsc_cpus_selection_ = 0;	     // By default, 0 (no selection). 1 means: first petsc_nb_cpus_ CPUs is used, 2 means: every petsc_nb_cpus_ CPUs is used
  petsc_nb_cpus_ = Process::nproc(); // By default the number of processes
  different_partition_ = 0;          // By default, same matrix partition
  petsc_decide_ = 0;                 // By default, 0. 1 is OK but does NOT improve performance (rather decrease)
  convergence_with_nb_it_max_ = 0;
  nb_it_max_ = NB_IT_MAX_DEFINED;
  save_matrix_=0;
  mataij_=0;
  factored_matrix_="";
  solveur_direct_=0;
  controle_residu_=0;
  gpu_=false;
  amgx_=false;
  amgx_initialized_=false;
  block_size_=1;
  option_prefix_="??";
  dm_=NULL;
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


