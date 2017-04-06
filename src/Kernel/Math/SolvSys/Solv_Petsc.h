/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
#ifdef __PETSCKSP_H
  inline Solv_Petsc& operator=(const Solv_Petsc&);
  inline Solv_Petsc(const Solv_Petsc&);

  inline void initialize();
  inline const Nom& get_chaine_lue() const
  {
    return chaine_lue_ ;
  };
  inline int cuda() const
  {
    return cuda_;
  };

  void lecture(Entree&);
  // Timers:
  static PetscLogStage KSPSolve_Stage_;
#endif
  static int instance;          // Nombre d'instance de la classe

protected :
#ifdef __PETSCKSP_H
  int Create_objects(Matrice_Morse&, const DoubleVect&); // Construit les objets Petsc Matrice et SecondMembre
  void Create_MatricePetsc(Mat&, int, Matrice_Morse&);
  int add_option(const Nom& option, const Nom& value);
  void MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M); // Temporaire
  void MorseSymHybToMorse(const Matrice_Morse_Sym& , Matrice_Morse& ,const DoubleVect&,DoubleVect&); // Temporaire
  void MorseHybToMorse(const Matrice_Morse& , Matrice_Morse& ,const DoubleVect&,DoubleVect&);
  void SaveObjectsToFile();
  void RestoreMatrixFromFile();
  int compute_nb_rows_petsc(int);

  // Attributes
  int nb_matrices_creees_;
  int solveur_cree_;
  double seuil_;
  double divtol_;

  // Objets Petsc
  Mat MatricePetsc_;
  Vec SecondMembrePetsc_;
  Vec SolutionPetsc_;		// Globale solution
  KSP SolveurPetsc_;
  PC PreconditionneurPetsc_;
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
  int read_matrix_;		// Read constant matrix in a file
  Nom factored_matrix_;		// Deal with the A=LU factorization on disk
  int mataij_;			// Force the use of a Mataij matrix
  Nom type_pc_;			// Preconditioner type
  Nom type_ksp_;		// KSP type

  int petsc_nb_cpus_;		// Number of CPUs used to solve the PETSc matrix
  int petsc_cpus_selection_;	// Selection of CPUs used to solve the PETSc matrix
  int different_partition_;	// Different partition for the TRUST matrix and the PETSc matrix
  int petsc_decide_;		// PETSc decide of the matrix partition
  Vec LocalSolutionPetsc_;	// Local solution in case of petsc_decide_=1
  VecScatter VecScatter_;	// Scatter context needed when petsc_decide_=1 to gather values of global to local solution
#endif
  int solveur_direct_;          // Pour savoir si l'on manipule un solveur direct et non iteratif
  Nom chaine_lue_; 		// Chaine des mots cles lus
  int cuda_;                    // Utilisation des GPU avec CUDA
};

class Solv_Petsc_GPU : public Solv_Petsc
{
  Declare_instanciable_sans_constructeur(Solv_Petsc_GPU);
public :
  inline Solv_Petsc_GPU()
  {
    cuda_=1;
  };
};

#define NB_IT_MAX_DEFINED 10000

inline Solv_Petsc::Solv_Petsc()
{
#ifdef __PETSCKSP_H
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
#ifdef __PETSCKSP_H
  if (solveur_cree_)
    {
      assert(solveur_cree_==1);
      KSPDestroy(&SolveurPetsc_);
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
    }
  initialize();
#endif
}

#ifdef __PETSCKSP_H
inline void Solv_Petsc::initialize()
{
  matrice_symetrique_=-1;
  preconditionnement_non_symetrique_=0;
  seuil_ = 1e-12;
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
  read_matrix_=0;
  solveur_direct_=0;
  controle_residu_=0;
  cuda_=0;
  block_size_=1;
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
      Cerr << "-log_view : to have at the end of the calculation, informations about performances and memory." << finl;
      Cerr << "-log_all : trace all PETSc calls." << finl;
      Cerr << "-malloc_dump : to have at the end of the calculation the memory not deallocated by PETSc." << finl;
      Cerr << "-help : to know all the commands lines of PETSc including those related to the solver/preconditioner selected." << finl;
      Cerr << "******************************************************************************************" << finl;
      PetscLogStageRegister("KSPSolve",&KSPSolve_Stage_);
    }
}

inline Solv_Petsc::Solv_Petsc(const Solv_Petsc& org):SolveurSys_base::SolveurSys_base()
{
  initialize();
  instance++;
  // Journal()<<"copie solv_petsc "<<instance<<finl;
  cuda_=org.cuda();
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


