/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Solv_Petsc.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/95
//
//////////////////////////////////////////////////////////////////////////////
#include <Solv_Petsc.h>
#include <Matrice_Morse_Sym.h>
#include <stat_counters.h>
#include <Matrice_Bloc_Sym.h>
#include <Matrice_Bloc.h>
#include <Motcle.h>
#include <SChaine.h>
#include <communications.h>
#include <SFichier.h>
#include <MD_Vector_tools.h>
#include <PE_Groups.h>
#include <Comm_Group_MPI.h>
#include <DoubleTrav.h>
#include <sys/stat.h>
#ifdef PETSC_HAVE_OPENMP
#include <petscthreadcomm.h>
#endif

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_Petsc,"Solv_Petsc",SolveurSys_base);
Implemente_instanciable_sans_constructeur(Solv_Petsc_GPU,"Solv_Petsc_GPU",Solv_Petsc);

// printOn
Sortie& Solv_Petsc::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_Petsc::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

// printOn
Sortie& Solv_Petsc_GPU::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_Petsc_GPU::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

void check_not_defined(option o)
{
  if (o.defined)
    {
      Cerr << "Error! Option " << o.name << " should not be defined with the preconditionner of this solver." << finl;
      Cerr << "Change your data file." << finl;
      Process::exit();
    }
}

// Lecture et creation du solveur
void Solv_Petsc::create_solver(Entree& entree)
{
#ifdef __PETSCKSP_H
  lecture(entree);
  EChaine is(get_chaine_lue());

  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Nom pc("");
  Nom motlu;
  Nom ksp;
  is >> ksp;   // On lit le solveur en premier puis les options du solveur: PETSC ksp { ... }
  is >> motlu; // On lit l'accolade
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the parameters of the solver of PETSC " << ksp << " { ... }" << finl;
      Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
      exit();
    }
  // Verification si Petsc est bien initialise (permet d'eviter un crash en sequentiel sur les machines batch)
  PetscBool isInitialized;
  PetscInitialized(&isInitialized);
  if (!isInitialized)
    {
      Cerr << "On this queuing system cluster, you need to use mpirun even on sequential mode" << finl;
      Cerr << "(mpirun -np 1 ...) with a PETSc solver or the calculation will crash. " << finl;
      Cerr << "You can use the trust script as a workaround:" << finl;
      Cerr << "trust " << nom_du_cas() << finl;
      exit();
    }

  // Creation du solveur et association avec le preconditionneur
  solveur_cree_++;

  /************************/
  /* Set PETSC_COMM_WORLD */
  /************************/
  // Recuperation du communicateur du groupe courant
#ifdef MPI_
  if (sub_type(Comm_Group_MPI,PE_Groups::current_group()))
    PETSC_COMM_WORLD = ref_cast(Comm_Group_MPI,PE_Groups::current_group()).get_mpi_comm();
#endif

  KSPCreate(PETSC_COMM_WORLD, &SolveurPetsc_);
  KSPGetPC(SolveurPetsc_, &PreconditionneurPetsc_);

  // Add options if PETSc solver is used:
  if (PE_Groups::get_nb_groups()==1)
    {
      // _petsc.TU is only printed if one group calculation (e.g. Execute_parallel failed)
      Nom petsc_TU(nom_du_cas());
      petsc_TU+="_petsc.TU";
      add_option("log_summary",petsc_TU); 	// Monitor performances at the end of the calculation
      PetscLogAllBegin(); 			// Necessary cause if not Event logs not printed in petsc_TU file ... I don't know why...
    }
  //add_option("on_error_abort",""); // ne marche pas semble t'il

  // On doit pouvoir lire des mots cles de base (GCP, GMRES, CHOLESKY)
  // mais egalement pouvoir appeler les options Petsc avec une chaine { -ksp_type cg -pc_type sor ... }
  // Les options non reconnues doivent arreter le code
  // Reprendre le formalisme de GCP { precond ssor { omega val } seuil val }
  Motcles les_solveurs(14);
  {
    les_solveurs[0] = "CLI";
    les_solveurs[1] = "GCP";
    les_solveurs[2] = "GMRES";
    les_solveurs[3] = "CHOLESKY";
    les_solveurs[4] = "CHOLESKY_OUT_OF_CORE";
    les_solveurs[5] = "BICGSTAB";
    les_solveurs[6] = "IBICGSTAB";
    les_solveurs[7] = "CHOLESKY_SUPERLU";
    les_solveurs[8] = "PGMRES";
    les_solveurs[9] = "LU";
    les_solveurs[10] = "PIPECG";
    les_solveurs[11] = "CHOLESKY_LAPACK";
    les_solveurs[12] = "CHOLESKY_UMFPACK";
    les_solveurs[13] = "CHOLESKY_PASTIX";
  }
  int solver_supported_on_gpu_by_petsc=0;
  int rang=les_solveurs.search(ksp);
  nommer(les_solveurs[rang]);
  switch(rang)
    {
    case 0:
      {
        Cerr << "Reading of the Petsc commands:" << finl;
        Nom valeur;
        is >> motlu;
        while (motlu!=accolade_fermee)
          {
            is >> valeur;
            // "-option val" ou "-option" ?
            if (valeur.debute_par("-") || valeur==accolade_fermee)
              {
                add_option(motlu.suffix("-"), "");
                motlu = valeur;
              }
            else
              {
                add_option(motlu.suffix("-"), valeur);
                is >> motlu;
              }
          }
        // Pour faciliter le debugage:
        if (limpr()>-1)
          fixer_limpr(1); // On imprime le residu si CLI
        add_option("ksp_view","");
        add_option("options_table","");
        add_option("options_left","");
        solver_supported_on_gpu_by_petsc=1; // Not really, reserved to expert...
        break;
      }
    case 1:
      {
        KSPSetType(SolveurPetsc_, KSPCG);
        // Merge the two inner products needed in CG into a single MPI_Allreduce() call:
        // Gain interessant a partir de 4000 coeurs
        if (Process::nproc()>=4000)
          {
            //add_option("ksp_cg_single_reduction",""); Pour Petsc < 3.3, la fonction KSPCGUseSingleReduction n'etait pas disponible
            KSPCGUseSingleReduction(SolveurPetsc_,(PetscBool)1);
          }
        // But It requires two extra work vectors than the conventional implementation in PETSc.
        solver_supported_on_gpu_by_petsc=1;
        break;
      }
    case 10:
      {
        KSPSetType(SolveurPetsc_, KSPPIPECG);
        break;
      }
    case 2:
      {
        KSPSetType(SolveurPetsc_, KSPGMRES);
        solver_supported_on_gpu_by_petsc=1;
        break;
      }
    case 8:
      {
        KSPSetType(SolveurPetsc_, KSPPGMRES);
        solver_supported_on_gpu_by_petsc=1;
        break;
      }
    case 9:
    case 3:
    case 4:
      {

        // Si MUMPS est present, on le prend par defaut (solveur_direct_=1) sinon SuperLU (solveur_direct_=2):
#ifdef PETSC_HAVE_MUMPS
        solveur_direct_=1;
        // Ajout de l'option
        // Par defaut le cas PAR_Canal_incline_VEF plante sur 4 processeurs si -mat_mumps_icntl_14 inferieur a 35...
        // On revient a 75 car parfois VEF_258 plante... C'est pas clair au niveau memoire...
        // Passage a Petsc 3.3 necessite d'augmenter a plus de 75 car sinon Aero_192 crashe...
        // A 90, le cas les_Re180Pr071_T0Q_jdd2 plante sur forchat (32bits)
        // On differencie sequentiel (peu de memoire, mais estimation juste)
        // et le calcul parallele (voir peut etre une separation entre plus et moins de 16 processeurs...)
        // Peut etre equiper le script trust d'une detection des erreurs INFO(1)=-9 ...
        if (Process::nproc()==1)
          add_option("mat_mumps_icntl_14","35");
        else
          add_option("mat_mumps_icntl_14","90");

        // Option out_of_core
        if (rang==4) add_option("mat_mumps_icntl_22","1");
#else
        solveur_direct_=2;
#endif
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 5:
      {
        KSPSetType(SolveurPetsc_, KSPBCGS);
        solver_supported_on_gpu_by_petsc=1;
        break;
      }
    case 6:
      {
        KSPSetType(SolveurPetsc_, KSPIBCGS); // 1 point de synchro au lieu de 3 pour KSPBCGS
        // Pour optimiser encore les comms, voir:
        // http://www.mcs.anl.gov/petsc/petsc-as/snapshots/petsc-3.0.0/docs/manualpages/KSP/KSPIBCGS.html
        KSPSetLagNorm(SolveurPetsc_, PETSC_TRUE);
        break;
      }
    case 7:
      {
        solveur_direct_=2;
        // SuperLU_dist, parallel but not faster than MUMPS
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 11:
      {
        solveur_direct_=3;
        // Lapack, old and slow
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 12:
      {
        solveur_direct_=4;
        // Umfpack, sequential only but fast...
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        //more robustness
        add_option("mat_umfpack_pivot_tolerance","1.0");
        break;
      }
    case 13:
      {
        solveur_direct_=5;
        // Pastix supports sbaij but seems slow...
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    default:
      {
        Cerr << ksp << " : solver not officially recognized by TRUST among those possible for the moment:" << finl;
        Cerr << les_solveurs << finl;
        Cerr << "You can try to access directly to Petsc solvers with the command line:" << finl;
        Cerr << "PETSC CLI { -ksp_type solver_name -pc_type preconditioning_name -ksp_atol threshold -ksp_monitor }" << finl;
        Cerr << "See the user manual for all Petsc options." << finl;
        Process::exit();
      }
    }

  // On verifie que le solveur est supporte si l'utilisateur souhaite utiliser le calcul par GPU:
  if (cuda_)
    {
#ifdef PETSC_HAVE_CUDA
      Cerr << "GPU capabilities of PETSc will be used." << finl;
#else
      Cerr << "You can not use petsc_gpu keyword cause GPU" << finl;
      Cerr << "capabilities will not work on your workstation with PETSc." << finl;
      Cerr << "Check if you have a NVidia video card and its driver up to date." << finl;
      Cerr << "Check petsc.log file under $TRUST_ROOT/lib/src/LIBPETSC for more details." << finl;
      Process::exit();
#endif
      if (solver_supported_on_gpu_by_petsc==0)
        {
          Cerr << les_solveurs[rang] << " is not supported yet by PETSc on GPU." << finl;
          Process::exit();
        }
    }

  int convergence_with_seuil=0; // Keyword to check the convergence via seuil or nb_it_max
  // On continue a lire
  if (motlu==accolade_ouverte)
    {
      // Temporaire essayer de faire converger les noms de parametres des differentes solveurs (GCP, GMRES,...)
      Motcles les_parametres_solveur(17);
      {
        les_parametres_solveur[0] = "impr";
        les_parametres_solveur[1] = "seuil";
        les_parametres_solveur[2] = "precond";
        les_parametres_solveur[3] = "precond_nul"; // To accept the TRUST syntax
        les_parametres_solveur[4] = "nb_it_max";
        les_parametres_solveur[5] = "save_matrix_petsc_format";
        les_parametres_solveur[6] = "factored_matrix"; // Experimental
        les_parametres_solveur[7] = "read_matrix";
        les_parametres_solveur[8] = "controle_residu";
        les_parametres_solveur[9] = "cli";
        les_parametres_solveur[10] = "ordering";
        les_parametres_solveur[11] = "petsc_decide"; // Experimental
        les_parametres_solveur[12] = "aij";
        les_parametres_solveur[13] = "nb_cpus";
        les_parametres_solveur[14] = "divtol";
        les_parametres_solveur[15] = "save_matrice|save_matrix";
        les_parametres_solveur[16] = "quiet";
      }
      option_double omega("omega",1.5);
      option_int    level("level",1);
      option_double epsilon("epsilon",0.1);
      option_string ordering("ordering",(Nom)"");
      controle_residu_=0;

      is >> motlu;
      while (motlu!=accolade_fermee)
        {
          switch(les_parametres_solveur.search(motlu))
            {
            case 16:
              {
                fixer_limpr(-1);
                break;
              }
            case 0:
              {
                fixer_limpr(1);
                // Si MUMPS on ajoute des impressions sur la decomposition
                if (solveur_direct_==1)
                  add_option("mat_mumps_icntl_4","3");
                else if (solveur_direct_==2)
                  add_option("mat_superlu_dist_statprint","");
                else if (solveur_direct_==3)
                  {}
                else if (solveur_direct_==4)
                  add_option("mat_umfpack_prl","2");
                else if (solveur_direct_==5)
                  add_option("mat_pastix_verbose","2");
                else if (solveur_direct_)
                  {
                    Cerr << "impr not coded yet for this direct solver." << finl;
                    Process::exit();
                  }
                break;
              }
            case 1:
              {
                if (solveur_direct_)
                  {
                    Cerr << "Definition of " << les_parametres_solveur(les_parametres_solveur.search(motlu)) << " is useless for a direct method." << finl;
                    Cerr << "Suppress the keyword." << finl;
                    exit();
                  }
                is >> seuil_;
                convergence_with_seuil=1;
                break;
              }
            case 2:
              {
                is >> pc;
                is >> motlu;
                if (motlu != accolade_ouverte)
                  {
                    Cerr << "Error while reading the parameters of the PETSC preconditioner: precond " << pc << " { ... }" << finl;
                    Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
                    exit();
                  }
                is >> motlu;
                while (motlu!=accolade_fermee)
                  {
                    Motcles les_parametres_precond(4);
                    {
                      les_parametres_precond[0] = omega.name;
                      les_parametres_precond[1] = level.name;
                      les_parametres_precond[2] = epsilon.name;
                      les_parametres_precond[3] = ordering.name;
                    }
                    double tmp_int;
                    double tmp_double;
                    Nom tmp_string;
                    switch(les_parametres_precond.search(motlu))
                      {
                      case 0:
                        {
                          is >> tmp_double;
                          omega.value()=tmp_double;
                          omega.defined=1;
                          break;
                        }
                      case 1:
                        {
                          is >> tmp_int   ;
                          level.value()=(int)tmp_int;
                          level.defined=1;
                          break;
                        }
                      case 2:
                        {
                          is >> tmp_double;
                          epsilon.value()=tmp_double;
                          epsilon.defined=1;
                          break;
                        }
                      case 3:
                        {
                          is >> tmp_string;
                          ordering.value()=tmp_string;
                          ordering.defined=1;
                          break;
                        }
                      default:
                        {
                          Cerr << motlu << " : unrecognized option among all of those possible on Petsc preconditioner:" << finl;
                          Cerr << les_parametres_precond << finl;
                          Process::exit();
                        }
                      }
                    is >> motlu;
                  }
                break;
              }
            case 3:
              {
                pc="null";
                break;
              }
            case 4:
              {
                is >> nb_it_max_;
                convergence_with_nb_it_max_=1;
                break;
              }
            case 15:
              {
                //abort();
                save_matrice_=1;
                break;
              }
            case 5:
              {
                // on sauvegarde au format petsc
                save_matrix_=1;
                break;
              }
            case 6:
              {
                if (Process::nproc()>1)
                  {
                    Cerr << "factored_matrix option is not available for parallel calculation." << finl;
                    exit();
                  }
                if (solveur_direct_!=3)
                  {
                    // Switch to PETSc Cholesky cause MUMPS or SUPERLU don't give access to LU ?
                    Cerr << "Only cholesky_lapack keyword may be used with the option factored_matrix." << finl;
                    exit();
                  }
                is >> factored_matrix_;
                break;
              }
            case 7:
              {
                read_matrix_=1;
                break;
              }
            case 8:
              {
                is >> controle_residu_;
                Cerr << "Option controle_residu not implemented yet." << finl;
                exit();
                break;
              }
            case 9:
              {
                is >> motlu; // On lit l'accolade
                if (motlu != accolade_ouverte)
                  {
                    Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
                    exit();
                  }
                Cerr << "Reading of the Petsc commands:" << finl;
                Nom valeur;
                is >> motlu;
                while (motlu!=accolade_fermee)
                  {
                    is >> valeur;
                    // "-option val" ou "-option" ?
                    if (valeur.debute_par("-") || valeur==accolade_fermee)
                      {
                        add_option(motlu.suffix("-"), "");
                        motlu = valeur;
                      }
                    else
                      {
                        add_option(motlu.suffix("-"), valeur);
                        is >> motlu;
                      }
                  }
                // Pour faciliter le debugage:
                if (limpr()>-1)
                  fixer_limpr(1); // On imprime le residu si CLI
                add_option("ksp_view","");
                add_option("options_table","");
                add_option("options_left","");
                break;
              }
            case 10:
              {
                is >> motlu;
                // Si pas MUMPS on previent
                if (solveur_direct_!=1)
                  {
                    Cerr << "Ordering keyword for a solver is limited to Cholesky only." << finl;
                    Process::exit();
                  }
                Motcles mumps_ordering(6);
                {
                  // NB: Il y'a d'autres ordering (voir doc MUMPS)
                  mumps_ordering[0] = "amd";
                  mumps_ordering[1] = "pt-scotch";
                  mumps_ordering[2] = "parmetis";
                  mumps_ordering[3] = "scotch";
                  mumps_ordering[4] = "pord";
                  mumps_ordering[5] = "metis";
                }
                int rang_mumps=mumps_ordering.search(motlu);
                // MUMPS fait un choix automatique par defaut (selon type et taille de la matrice, et nombre de processeurs) mais a savoir que:
                // Sur le cas Cx et PAR_Cx 4 cores, Scotch en sequentiel et PT-Scotch en parallele sont les meilleurs choix
                // Sur les gros cas, parfois Metis plus rapide pour A=LU et Scotch pour x=A-1B...
                if (rang_mumps==-1)
                  {
                    Cerr << motlu << " : unrecognized ordering from those available for the MUMPS solver Cholesky:" << finl;
                    Cerr << mumps_ordering << finl;
                    Process::exit();
                  }
                else if (rang_mumps==1 || rang_mumps==2)
                  {
                    if (Process::nproc()==1)
                      {
                        Cerr << "You can't use the parallel ordering " << motlu << " during a sequential calculation." << finl;
                        Process::exit();
                      }
                    add_option("mat_mumps_icntl_28","2");  // Parallel analysis
                    add_option("mat_mumps_icntl_29",(Nom)rang_mumps);        // Parallel ordering
                  }
                else
                  {
                    add_option("mat_mumps_icntl_28","1");  // Sequential analysis
                    add_option("mat_mumps_icntl_7",(Nom)rang_mumps); // Sequential ordering
                  }
                break;
              }
            case 11:
              {
                is >> petsc_decide_;
                different_partition_ = 1; // If Petsc decides the matrix partition, the partition is often different than the TRUST partition
                break;
              }
            case 12:
              {
                mataij_=1;
                break;
              }
            case 13:
              {
                is >> motlu;
                is >> petsc_nb_cpus_;
                different_partition_ = 1; // If user decides a different number of CPUs to solve PETSc matrix, the matrix partition will be different than the TRUST partition
                if (motlu=="first")
                  petsc_cpus_selection_=1;
                else if (motlu=="every")
                  petsc_cpus_selection_=2;
                else
                  {
                    Cerr << "We should read the option first or every after the keyword nb_cpus." << finl;
                    Cerr << "Or we read: " << motlu << finl;
                    exit();
                  }
                if (petsc_nb_cpus_<1 || petsc_nb_cpus_>Process::nproc())
                  {
                    Cerr << "Incorrect number of CPUs selected for solving the PETSc matrix: " << petsc_nb_cpus_ << finl;
                    exit();
                  }
                break;
              }
            case 14:
              {
                is >> divtol_; // See http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/KSP/KSPSetTolerances.html
                break;
              }
            default:
              {
                Cerr << motlu << " : unrecognized option from those available in the Petsc solver:" << finl;
                Cerr << les_parametres_solveur << finl;
                Process::exit();
              }
            }
          is >> motlu;
        }
      // Some checks
      if (petsc_decide_ && petsc_cpus_selection_)
        {
          Cerr << "You can't use petsc_decide and nb_cpus option together." << finl;
          exit();
        }

      int pc_supported_on_gpu_by_petsc=0;
      Motcles les_precond(9);
      {
        les_precond[0] = "NULL";               // Pas de preconditionnement
        les_precond[1] = "ILU";                // Incomplete LU
        les_precond[2] = "SSOR";               // Symetric Successive Over Relaxation
        les_precond[3] = "EISENSTAT";          // Symetric Successive Over Relaxation avec Eiseinstat trick
        les_precond[4] = "SPAI";               // Sparse Approximate Inverse
        les_precond[5] = "PILUT";              // Dual-threshold incomplete LU factorisation
        les_precond[6] = "DIAG";               // Diagonal precondtioner
        les_precond[7] = "BOOMERAMG";          // Multigrid preconditioner
        les_precond[8] = "BLOCK_JACOBI_ICC";   // Block Jacobi ICC preconditioner (code dans PETSc, optimise)
        /*
          les_precond[3] = "ICC";                // Incomplete Cholesky
          les_precond[7] = "ASM";                // Additive Schwarz method (ILU as local precondioner)
          les_precond[9] = "POLY";                // Polynomial preconditioner
          les_precond[10] = "ADD_ILU0";        // Additive Schwarz method with ILU(0)
          les_precond[11] = "ADD_ILUT";        // Additive Schwarz method with dual-threshold incomplete LU factorisation
          les_precond[12] = "ADD_ILUK";        // Additive Schwarz method with ILU(k)
          les_precond[13] = "ADD_ARMS";        // Additive Schwarz method with Algebric Recursive Multilevel Solver
          les_precond[14] = "LSCH_ILU0";        // Left Schur complement with ILU(0)
          les_precond[15] = "LSCH_ILUT";        // Left Schur complement with dual-threshold incomplete LU factorisation
          les_precond[16] = "LSCH_ILUK";        // Left Schur complement with ILU(k)
          les_precond[17] = "LSCH_ARMS";        // Left Schur complement with Algebric Recursive Multilevel Solver
          les_precond[18] = "RSCH_ILU0";        // Right Schur complement with ILU(0)
          les_precond[19] = "RSCH_ILUT";        // Right Schur complement with dual-threshold incomplete LU factorisation
          les_precond[20] = "RSCH_ILUK";        // Right Schur complement with ILU(k)
          les_precond[21] = "RSCH_ARMS";        // Right Schur complement with Algebric Recursive Multilevel Solver
          les_precond[22] = "SCH_GILU0";        // Schur complement with ILU(0)
          les_precond[23] = "SCH_SGS";        // Schur complement with Gauss Seidel
        */
      }

      if (pc!="")
        {
          // On empeche le choix d'un preconditionneur avec une methode directe
          // puisque celle ci EST le preconditionneur KSPREONLY
          if (solveur_direct_)
            {
              Cerr << "Using precond keyword with a direct method like Cholesky is useless" << finl;
              Cerr << "because for PETSc the LU factorization is used as a preconditioner." << finl;
              exit();
            }
          // Option du preconditionneur
          rang = les_precond.search(pc);
          switch(rang)
            {
            case 0:
              {
                PCSetType(PreconditionneurPetsc_, PCNONE);
                pc_supported_on_gpu_by_petsc=1;
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 1:
              {
                Cout << "See http://www.ncsa.uiuc.edu/UserInfo/Resources/Software/Math/HYPRE/docs-1.6.0/HYPRE_usr_manual/node33.html" << finl;
                Cout << "to have some advices on the incomplete LU factorisation level: ILU(level)" << finl;
                // On n'attaque pas le ILU de Petsc qui n'est pas parallele
                // On prend celui de Hypre (Euclid=PILU(k)) en passant par les commandes en ligne
                // car peu de parametres peuvent etre fixes sinon
                PCSetType(PreconditionneurPetsc_, PCHYPRE);
                PCHYPRESetType(PreconditionneurPetsc_, "euclid");
                add_option("pc_hypre_euclid_levels",(Nom)level.value());
                check_not_defined(omega);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 2:
              {
                PCSetType(PreconditionneurPetsc_, PCSOR);
                if (omega.value()>=1. && omega.value()<=2.)
                  {
                    PCSORSetOmega(PreconditionneurPetsc_, omega.value());
                  }
                else
                  {
                    Cerr << "omega value for SSOR should be between 1 and 2" << finl;
                    exit();
                  }
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 3:
              {
                PCSetType(PreconditionneurPetsc_, PCEISENSTAT);
                if (omega.value()>=1. && omega.value()<=2.)
                  {
                    PCEisenstatSetOmega(PreconditionneurPetsc_, omega.value());
                  }
                else
                  {
                    Cerr << "omega value for EISENSTAT should be between 1 and 2" << finl;
                    exit();
                  }
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 4:
              {
                PCSetType(PreconditionneurPetsc_, PCHYPRE);
                PCHYPRESetType(PreconditionneurPetsc_, "parasails");
                add_option("pc_hypre_parasails_levels",(Nom)level.value());     // Higher values of level [>=0] leads to more accurate, but more expensive preconditioners (default 1)
                add_option("pc_hypre_parasails_thresh",(Nom)epsilon.value());   // Lower values of eps [0-1] leads to more accurate, but more expensive preconditioners (default 0.1)
                add_option("pc_hypre_parasails_sym","SPD"); // Matrice symetrique definie positive
                check_not_defined(omega);
                check_not_defined(ordering);
                break;
              }
            case 5:
              {
                PCSetType(PreconditionneurPetsc_, PCHYPRE);
                PCHYPRESetType(PreconditionneurPetsc_, "pilut");
                add_option("pc_hypre_pilut_factorrowsize",(Nom)level.value());        // Maximum nonzeros retained in each row of L and U (default 20)
                add_option("pc_hypre_pilut_tol",(Nom)epsilon.value());                // Values below the value are dropped in L and U (default 1.e-7)
                check_not_defined(omega);
                check_not_defined(ordering);
                break;
              }
            case 6:
              {
                PCSetType(PreconditionneurPetsc_, PCJACOBI);
                pc_supported_on_gpu_by_petsc=1;
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 7:
              {
                PCSetType(PreconditionneurPetsc_, PCHYPRE);
                PCHYPRESetType(PreconditionneurPetsc_, "boomeramg");
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 8:
              {
                PCSetType(PreconditionneurPetsc_, PCBJACOBI);
                add_option("sub_pc_type","icc");
                // On fixe le precondtionnement non symetrique pour appliquer eventuellement un ordering autre que celui par defaut (natural)
                // Un ordering rcm peut ameliorer par exemple la convergence
                // Voir le remplissage de la matrice avec -mat_view_draw -draw_pause -1
                if (ordering.value()!="")
                  {
                    add_option("sub_pc_factor_mat_ordering_type",ordering.value());
                    // Le preconditionnement natural (defaut) ne necessite pas une matrice de preconditionnement symetrique, les autres si:
                    preconditionnement_non_symetrique_=1;
                  }
                add_option("sub_pc_factor_levels",(Nom)level.value());
                check_not_defined(omega);
                check_not_defined(epsilon);
                break;
              }
            default:
              {
                Cerr << pc << " : preconditioner not officially recognized by TRUST among those possible for the moment:" << finl;
                Cerr << les_precond << finl;
                Cerr << "You can try to access directly to Petsc preconditioners with the command line." << finl;
                Cerr << "See the user manual of Petsc to do this." << finl;
                Process::exit();
              }
            }
        }
      else
        {
          if (!solveur_direct_)
            {
              Cerr << "You forgot to define a preconditionner with the keyword precond." << finl;
              Cerr << "If you don't want a preconditionner, add for the solver definition:" << finl;
              Cerr << "precond null" << finl;
              Process::exit();
            }
        }
      // On verifie que le preconditionneur est supporte si l'utilisateur souhaite utiliser le calcul par GPU:
      if (cuda_ && pc_supported_on_gpu_by_petsc==0)
        {
          Cerr << les_precond[rang] << " is not supported yet by PETSc on GPU." << finl;
          Process::exit();
        }
    }

  // On fixe des parametres du solveur et du preconditionneur selon que l'on ait un solveur direct ou iteratif
  // KSPSetInitialGuessNonzero : Resout Ax=B en supposant x nul ou non
  // KSPSetTolerances : Pour fixer les criteres de convergence du solveur iteratif
  if (solveur_direct_)
    {
      KSPSetInitialGuessNonzero(SolveurPetsc_, PETSC_FALSE);
      PCSetType(PreconditionneurPetsc_, PCLU);
    }
  else
    {
      KSPSetInitialGuessNonzero(SolveurPetsc_, PETSC_TRUE);
      if (convergence_with_nb_it_max_)
        {
          if (convergence_with_seuil)
            {
              Cerr << "You can only define solver convergence either by seuil or by nb_it_max." << finl;
              Cerr << "So suppress seuil keyword or nb_it_max keyword." << finl;
              exit();
            }
          // Convergence is defined with nb_it_max, the norm is checked after nb_it_max iterations:
          seuil_ = DMAXFLOAT;
          add_option("ksp_check_norm_iteration",(Nom)(nb_it_max_-1));
          nb_it_max_ = NB_IT_MAX_DEFINED;
        }
      KSPSetTolerances(SolveurPetsc_, 0., seuil_, (divtol_==0 ? PETSC_DEFAULT : divtol_), nb_it_max_);
    }
  // Change le calcul du test de convergence relative (||Ax-b||/||Ax(0)-b|| au lieu de ||Ax-b||/||b||)
  // Peu utilisee dans TRUST car on utilise la convergence sur la norme absolue
  // Mais cela corrige une erreur KSP_DIVERGED_DTOL quand ||Ax-b||/||b||>10000=div_tol par defaut dans PETSc (rencontree sur Etude REV_4)
  //add_option("ksp_converged_use_initial_residual_norm",1); // Before PETSc 3.5
  KSPConvergedDefaultSetUIRNorm(SolveurPetsc_); // After PETSc 3.5, a function is available

  // Surcharge eventuelle par la ligne de commande
  KSPSetFromOptions(SolveurPetsc_);
  PCSetFromOptions(PreconditionneurPetsc_);

  // Setting the names:
  KSPType type_ksp;
  KSPGetType(SolveurPetsc_, &type_ksp);
  type_ksp_=(Nom)type_ksp;

  PCType type_pc;
  PCGetType(PreconditionneurPetsc_, &type_pc);
  type_pc_=(Nom)type_pc;
#else
  Cerr << "Error, the code is not built with PETSc support." << finl;
  Cerr << "Contact TRUST support." << finl;
  Process::exit();
#endif

}

int Solv_Petsc::instance=-1;
#ifdef __PETSCKSP_H
int Solv_Petsc::KSPSolve_Stage_=0;

// Sortie Maple d'une matrice morse
void sortie_maple(Sortie& s, const Matrice_Morse& M)
{
  s.precision(30);
  s<<"B:=matrix([";
  int M_nb_lignes=M.nb_lignes();
  int M_nb_colonnes=M.nb_colonnes();
  for (int i=0; i<M_nb_lignes; i++)
    {
      s<<"[";
      for ( int j=0 ; j<M_nb_colonnes; j++ )
        {
          s<<M(i,j);
          if (j!=(M_nb_colonnes-1)) s<<",";
        }
      s<<"]";
      if (i!=(M_nb_lignes-1)) s<<",";
    }
  s<<"]):"<<finl;
}

void Solv_Petsc::MorseSymHybToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M,
                                    const DoubleVect& secmem,
                                    DoubleVect& solution)
{
  // Transposition de la matrice initiale
  Matrice_Morse MM_tot;
  MorseSymToMorse(MS,MM_tot);
  MorseHybToMorse(MM_tot, M, secmem, solution);
}

// Conversion d'une matrice morse hybride (format TRUST avec items communes) en une matrice morse (sans items communs)
void Solv_Petsc::MorseHybToMorse(const Matrice_Morse& MM_tot, Matrice_Morse& M, const DoubleVect& b, DoubleVect& solution)
{
  if (items_to_keep_.size_array()==0)
    nb_items_to_keep_ = MD_Vector_tools::get_sequential_items_flags(b.get_md_vector(), items_to_keep_, b.line_size());

  // Dimensionnement de la matrice
  int nbrows_M=nb_items_to_keep_;
  M.dimensionner(nbrows_M,0);

  const IntVect& tab1_tot = MM_tot.tab1_;
  const IntVect& tab2_tot = MM_tot.tab2_;
  const DoubleVect& coeff_tot = MM_tot.coeff_;
  IntVect& tab1 = M.tab1_;
  IntVect& tab2 = M.tab2_;
  DoubleVect& coeff = M.coeff_;

  // Construction de tab1 de la matrice morse hybride
  tab1(0)=1;
  int cpt=0;
  int size = b.size_array();
  for (int ii=0; ii<size; ii++)
    {
      if(items_to_keep_[ii])
        {
          tab1(cpt+1) = tab1_tot(ii+1)-tab1_tot(ii)+tab1(cpt);
          cpt++;
        }
    }

  M.dimensionner(nbrows_M,MM_tot.nb_colonnes(),tab1(nbrows_M)-1);

  // Construction de tab2 et coeff de la matrice morse hybride
  cpt=0;
  for (int ii=0; ii<size; ii++)
    {
      if(items_to_keep_[ii])
        {
          int dl = tab1_tot(ii);
          int fl = tab1_tot(ii+1);
          for(int jj=dl; jj<fl; jj++)
            {
              tab2(cpt)=tab2_tot(jj-1);
              coeff(cpt++)=coeff_tot(jj-1);
            }
        }
    }
}

void Solv_Petsc::MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M)
{
  M = MS;
  Matrice_Morse mattmp(MS);
  int i, ordre;
  ordre = M.ordre();
  M.transpose(mattmp);
  for (i=0; i<ordre; i++)
    M(i, i) = 0.;
  M = mattmp + M;
}

// Save Matrix and RHS in a .petsc file:
void Solv_Petsc::SaveObjectsToFile()
{
  MatInfo Info;
  MatGetInfo(MatricePetsc_,MAT_GLOBAL_SUM,&Info);
  int nnz = (int)Info.nz_allocated;

  Nom filename("Matrix_");
  filename+=(Nom)nb_rows_tot_;
  filename+="_rows_";
  filename+=(Nom)nproc();
  filename+="_cpus.petsc";

  PetscViewer viewer;
  Cerr << "Writing the global PETSc matrix (" << nb_rows_tot_<< " rows and " << nnz << " nnz) in the binary file " << filename << finl;
  // To go faster with MPI IO ?
  //PetscViewerCreate(PETSC_COMM_WORLD,&viewer);
  //PetscViewerFileSetName(viewer,filename);
  //PetscViewerBinarySetMPIIO(viewer);
  // Save the matrix:
  PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_WRITE,&viewer);
  MatView(MatricePetsc_, viewer);
  Cerr << "Writing also the RHS in the file " << filename << finl;
  // Save also the RHS:
  VecView(SecondMembrePetsc_, viewer);
  PetscViewerDestroy(&viewer);

  // ASCII output for small matrix(debugging)
  if (nb_rows_tot_<20)
    {
      Cerr << "The global PETSc matrix is small so we also print it:" << finl;
      MatView(MatricePetsc_, PETSC_VIEWER_STDOUT_WORLD);
    }
}

// Read a PETSc matrix in a file and
// returns the local number of rows
void Solv_Petsc::RestoreMatrixFromFile()
{
  Nom filename("Matrix_");
  filename+=(Nom)nb_rows_tot_;
  filename+="_rows_";
  filename+=(Nom)nproc();
  filename+="_cpus.petsc";

  PetscViewer viewer;
  Cerr << "Reading the global PETSc matrix in the binary file " << filename << finl;
  PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_READ,&viewer);
  MatCreate(PETSC_COMM_WORLD,&MatricePetsc_);
  if (petsc_decide_)
    MatSetSizes(MatricePetsc_, PETSC_DECIDE, PETSC_DECIDE, nb_rows_tot_, nb_rows_tot_);
  else if (petsc_cpus_selection_)
    {
      Cerr << "Reading a PETSc matrix with a different number of CPUs is not implemented yet." << finl;
      Cerr << "Contact TRUST support." << finl;
      exit();
    }
  else
    MatSetSizes(MatricePetsc_, nb_rows_, nb_rows_, PETSC_DECIDE, PETSC_DECIDE);
  MatLoad(MatricePetsc_, viewer);
  PetscViewerDestroy(&viewer);
  if (!matrice_symetrique_)
    {
      Cerr << "Reading a non symmetric PETSc matrix is not supported yet in TRUST." << finl;
      exit();
    }
  // Conversion AIJ to SBAIJ:
  MatSetOption(MatricePetsc_, MAT_SYMMETRIC, PETSC_TRUE);
  MatConvert(MatricePetsc_, MATSBAIJ, MAT_REUSE_MATRIX,&MatricePetsc_);
  int nb_rows_tot,nb_cols_tot;
  MatGetSize(MatricePetsc_,&nb_rows_tot,&nb_cols_tot);
  Cerr << "The matrix read has " << nb_rows_tot << " rows." << finl;
}


// Remplissage de chaine_lue_ Petsc ksp { ... }
// chaine_lue_="ksp { ... }"
void Solv_Petsc::lecture(Entree& is)
{
  // Lecture de la chaine de mot cles
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  Nom nomlu;
  is >> motlu;
  SChaine prov;
  prov<<motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading parameters of Petsc: " << finl;
      Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
      Process::exit();
    }
  prov<<" { ";
  int nb_acco=1;
  while (nb_acco!=0)
    {
      is >> nomlu;
      prov<<nomlu<<" ";
      if (nomlu==accolade_ouverte)
        nb_acco++;
      else if (nomlu==accolade_fermee)
        nb_acco--;
    }
  chaine_lue_=prov.get_str();
}


int Solv_Petsc::add_option(const Nom& string, const Nom& value)
{
  Nom option="-";
  option+=string;
  // Attention il ne retourne pas de code d'erreur si l'option est mal orthographiee!!
  // Il ne dit pas non plus qu'elle est unused avec -options_left
  // Nouveau 1.6.3 pour la ligne de commande reste prioritaire, on ne change une option
  // que si elle n'a pas deja ete specifiee...
  PetscBool flg;
  Nom vide="                                                                                                 ";
  char* tmp=strdup(vide);
  PetscOptionsGetString(PETSC_NULL,option,tmp,vide.longueur(),&flg);
  Nom actual_value(tmp);
  free(tmp);
  if (actual_value==vide)
    {
      if (value=="")
        {
          PetscOptionsSetValue(option, PETSC_NULL);
          Cerr << "Option Petsc: " << option << finl;
        }
      else
        {
          PetscOptionsSetValue(option, value);
          Cerr << "Option Petsc: " << option << " " << value << finl;
        }
      return 1;
    }
  else
    {
      Cerr << "Option Petsc: " << option << " " << value << " not taken cause " << option << " already defined to " << actual_value << finl;
      return 0;
    }
}

#ifndef PETSC_HAVE_HYPRE
// Pour que le code puisse compiler/tourner si on prend PETSc sans aucun autre package externe:
int PCHYPRESetType(PC,const char[])
{
  Cerr << "HYPRE preconditionners are not available in this TRUST version." << finl;
  Cerr << "May be, HYPRE library has been deactivated during the PETSc build process." << finl;
  Process::exit();
  return 0;
}
#endif



// Routine de monitoring appele par Petsc
PetscErrorCode MyKSPMonitor(KSP SolveurPetsc, PetscInt it, PetscReal residu, void *dummy)
{
  if (it==0)
    Cout << "Norm of the residue: " << residu << " (1)";
  else
    Cout << residu << " ";
  if ((it % 15) == 0) Cout << finl ;
  return 0;
}
#endif

// Solve system
int Solv_Petsc::resoudre_systeme(const Matrice_Base& la_matrice, const DoubleVect& secmem, DoubleVect& solution)
{
#ifdef __PETSCKSP_H
  // Si on utilise un solver petsc on le signale pour les stats finales
  statistiques().begin_count(solv_sys_petsc_counter_);
  statistiques().end_count(solv_sys_petsc_counter_,1,1);
  int new_matrix = nouvelle_matrice();
  if (new_matrix)
    {
      matrice_symetrique_=1;      // On suppose que la matrice est symetrique
      fixer_nouvelle_matrice(0);
      if (nb_matrices_creees_>0)
        {
          // On detruit la precedente matrice Petsc
          MatDestroy(&MatricePetsc_);
        }
      nb_matrices_creees_ += 1;
      if (read_matrix_)
        {
          // New in 1.6.9, it is possible to read the matrix
          Matrice_Morse mat; // Create a useless mat (cause no conversion from an existing one)
          Create_objects(mat,secmem);
        }
      else if(sub_type(Matrice_Morse_Sym,la_matrice))
        {
          // Exemple: matrice de pression en VEFPreP1B
          const Matrice_Morse_Sym& matrice = ref_cast(Matrice_Morse_Sym,la_matrice);
          assert(matrice.get_est_definie());
          Matrice_Morse mat;
          MorseSymHybToMorse(matrice,mat,secmem,solution);
          // Construction de la matrice NP : NP_mat_ et des vecteurs solutions et second_membre
          Create_objects(mat,secmem);
        }
      else if(sub_type(Matrice_Bloc_Sym,la_matrice))
        {
          // Exemple : matrice de pression en VEF P0+P1+Pa
          const Matrice_Bloc_Sym& matrice = ref_cast(Matrice_Bloc_Sym,la_matrice);
          // Conversion de la matrice Matrice_Bloc_Sym au format Matrice_Morse_Sym
          Matrice_Morse_Sym mat_sym;
          matrice.BlocSymToMatMorseSym(mat_sym);
          Matrice_Morse mat;
          MorseSymHybToMorse(mat_sym,mat,secmem,solution);
          // Destruction de la matrice morse desormais inutile
          mat_sym.dimensionner(0,0);
          Create_objects(mat,secmem);
        }
      else if(sub_type(Matrice_Morse,la_matrice))
        {
          // Exemple : matrice implicite
          matrice_symetrique_=0;
          const Matrice_Morse& mat_morse_hyb = ref_cast(Matrice_Morse,la_matrice);
          Matrice_Morse mat;
          MorseHybToMorse(mat_morse_hyb,mat,secmem,solution);
          Create_objects(mat,secmem);
        }
      else if(sub_type(Matrice_Bloc,la_matrice))
        {
          // Exemple : matrice de pression en VDF
          const Matrice_Bloc& matrice_bloc = ref_cast(Matrice_Bloc,la_matrice);
          if (!sub_type(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur()))
            matrice_symetrique_=0;
          else
            {
              // Pour un solveur direct, operation si la matrice n'est pas definie (en incompressible VDF, rien n'etait fait...)
              Matrice_Morse_Sym& mat00 = ref_cast_non_const(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur());
              if (solveur_direct_ && mat00.get_est_definie()==0 && Process::je_suis_maitre())
                mat00(0,0)*=2;
            }
          Matrice_Morse mat_morse_hyb;
          matrice_bloc.BlocToMatMorse(mat_morse_hyb);
          // Conversion de la matrice au format Matrice_Morse hybride vers le format Morse
          Matrice_Morse mat;
          MorseHybToMorse(mat_morse_hyb,mat,secmem,solution);
          Create_objects(mat,secmem);
        }
      else
        {
          Cerr<<"Solv_Petsc : Warning, we do not know yet treat a matrix of type " << la_matrice.que_suis_je() <<finl;
          exit();
        }
    }
  // Il semble que Hypre/Boomeramg plante si secmem est nul (residu nul)
  // donc on quitte avant... A generaliser pour tout le monde ?
  //  if (type_pc_==PCHYPRE && mp_max_abs_vect(secmem)==0) return 0; // Not true in 1.6.9

  // Assemblage du second membre et de la solution
  int size=secmem.size_array();
  int colonne_globale=decalage_local_global_;
  for (int i=0; i<size; i++)
    if (items_to_keep_[i])
      {
        VecSetValues(SecondMembrePetsc_, 1, &colonne_globale, &secmem(i), INSERT_VALUES);
        VecSetValues(SolutionPetsc_, 1, &colonne_globale, &solution(i), INSERT_VALUES);
        colonne_globale++;
      }
  VecAssemblyBegin(SecondMembrePetsc_);
  VecAssemblyEnd(SecondMembrePetsc_);
  VecAssemblyBegin(SolutionPetsc_);
  VecAssemblyEnd(SolutionPetsc_);
//  VecView(SecondMembrePetsc_,PETSC_VIEWER_STDOUT_WORLD);
//  VecView(SolutionPetsc_,PETSC_VIEWER_STDOUT_WORLD);

  // Save the matrix and the RHS
  // if the matrix has changed...
  if (save_matrix_ && new_matrix)
    SaveObjectsToFile();

  // Affichage par MyKSPMonitor
  if (!solveur_direct_)
    {
      if (limpr()==1)
        KSPMonitorSet(SolveurPetsc_, MyKSPMonitor, PETSC_NULL, PETSC_NULL);
      else
        KSPMonitorCancel(SolveurPetsc_);
    }
  // Historique du residu
  int size_residu = nb_it_max_ + 1;
  //DoubleTrav residu(size_residu); // bad_alloc sur gros cas, curie pourquoi ?
  ArrOfDouble residu(size_residu);
  KSPSetResidualHistory(SolveurPetsc_, residu.addr(), size_residu, PETSC_TRUE);

  //////////////////////////
  // Solve the linear system
  //////////////////////////
  PetscLogStagePush(KSPSolve_Stage_);
  KSPSolve(SolveurPetsc_, SecondMembrePetsc_, SolutionPetsc_);
  PetscLogStagePop();

  if (different_partition_)
    {
      // TRUST and PETSc has different partition, a local vector LocalSolutionPetsc_ is gathered from the global vector SolutionPetsc_ :
      VecScatterBegin(VecScatter_, SolutionPetsc_, LocalSolutionPetsc_, INSERT_VALUES, SCATTER_FORWARD);
      VecScatterEnd  (VecScatter_, SolutionPetsc_, LocalSolutionPetsc_, INSERT_VALUES, SCATTER_FORWARD);
      // Use the local vector to get the solution:
      int colonne_locale=0;
      for (int i=0; i<size; i++)
        if (items_to_keep_[i])
          {
            VecGetValues(LocalSolutionPetsc_, 1, &colonne_locale, &solution(i));
            colonne_locale++;
          }
      assert(nb_rows_==colonne_locale);
    }
  else
    {
      // TRUST and PETSc has same partition, local solution can be accessed from the global vector:
      colonne_globale=decalage_local_global_;
      for (int i=0; i<size; i++)
        if (items_to_keep_[i])
          {
            VecGetValues(SolutionPetsc_, 1, &colonne_globale, &solution(i));
            colonne_globale++;
          }
    }
  solution.echange_espace_virtuel();

  // Analyse de la convergence par Petsc
  KSPConvergedReason Reason;
  KSPGetConvergedReason(SolveurPetsc_, &Reason);
  if (Reason<0)
    {
      Cerr << "No convergence on the resolution with the Petsc solver." << finl;
      Cerr << "Reason given by Petsc: ";
      if      (Reason==KSP_DIVERGED_NULL)                Cerr << "KSP_DIVERGED_NULL" << finl;
      else if (Reason==KSP_DIVERGED_ITS)                 Cerr << "KSP_DIVERGED_ITS" << finl;
      else if (Reason==KSP_DIVERGED_DTOL)                Cerr << "KSP_DIVERGED_DTOL" << finl;
      else if (Reason==KSP_DIVERGED_BREAKDOWN)           Cerr << "KSP_DIVERGED_BREAKDOWN" << finl;
      else if (Reason==KSP_DIVERGED_BREAKDOWN_BICG)      Cerr << "KSP_DIVERGED_BREAKDOWN_BICG" << finl;
      else if (Reason==KSP_DIVERGED_NONSYMMETRIC)        Cerr << "KSP_DIVERGED_NONSYMMETRIC" << finl;
      else if (Reason==KSP_DIVERGED_INDEFINITE_PC)       Cerr << "KSP_DIVERGED_INDEFINITE_PC" << finl;
      else if (Reason==KSP_DIVERGED_NANORINF)            Cerr << "KSP_DIVERGED_NANORINF" << finl;
      else if (Reason==KSP_DIVERGED_INDEFINITE_MAT)      Cerr << "KSP_DIVERGED_INDEFINITE_MAT" << finl;
      else Cerr << Reason << finl;
      exit();
    }
  // Recuperation du nombre d'iterations
  int nbiter;
  KSPGetIterationNumber(SolveurPetsc_, &nbiter);

  if (limpr()>-1)
    {
      // MyKSPMonitor ne marche pas pour certains solveurs (residu(0) n'est pas calcule):
      if (solveur_direct_ || type_ksp_==KSPIBCGS)
        {
          // Calcul de residu(0)=||B||
          VecNorm(SecondMembrePetsc_, NORM_2, &residu(0));
          // On l'affiche pour les solveurs directs (pour les autres TRUST s'en occupe):
          if (solveur_direct_) MyKSPMonitor(SolveurPetsc_, 0, residu(0), 0);
        }
      // Idem: l'historique du residu est mal evalue pour certains solveurs:
      // donc on le calcul a la derniere iteration:
      if (residu(0)>0 && (solveur_direct_ || type_ksp_==KSPIBCGS))
        {
          // Calcul de residu(nbiter)=||Ax-B||
          VecScale(SecondMembrePetsc_, -1);
          MatMultAdd(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_, SecondMembrePetsc_);
          VecNorm(SecondMembrePetsc_, NORM_2, &residu(nbiter));
        }
      // Affichage residu final absolu et (relatif)
      double residu_relatif=(residu(0)>0?residu(nbiter)/residu(0):residu(nbiter));
      Cout << finl << "Final residue: " << residu(nbiter) << " ( " << residu_relatif << " )"<<finl;
    }
  return nbiter;
#else
  return -1;
#endif
}

#ifdef __PETSCKSP_H
inline int ligne_inutile(const Matrice_Morse& mat, const DoubleVect& secmem, int& i)
{
  if (secmem(i)==0.                         // Le second membre est nul
      && mat.tab1_[i+1]-mat.tab1_[i]==1        // Et il n'y a qu'un terme non nul sur la ligne
      && mat.tab2_[mat.tab1_[i]-1]-1==i)        // Et c'est la diagonale
    {
      Cerr << "[" << Process::me() << "] Line " << i << " useless..." << finl;
      return 1;                        // Alors cette ligne est inutile (item periodique, arete superflue,...)
    }
  else
    return 0;
}

// Creation d'une MatricePetsc et d'un SecondMembrePetsc a partir de mat et b
int Solv_Petsc::Create_objects(Matrice_Morse& mat, const DoubleVect& b)
{
  // Initialisation du tableau items_to_keep_ si ce n'est pas deja fait
  if (items_to_keep_.size_array()==0)
    nb_items_to_keep_ = MD_Vector_tools::get_sequential_items_flags(b.get_md_vector(), items_to_keep_, b.line_size());

  // Compute important value:
  nb_rows_ = nb_items_to_keep_;
  nb_rows_tot_ = mp_sum(nb_rows_);
  decalage_local_global_ = mppartial_sum(nb_rows_);
  //Journal()<<"nb_rows_=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << " decalage_local_global_=" << decalage_local_global_ << finl;
  if (read_matrix_)
    {
      // Read the PETSc matrix
      RestoreMatrixFromFile();
      int nb_local_rows,nb_local_cols;
      MatGetLocalSize(MatricePetsc_,&nb_local_rows,&nb_local_cols);
      if (nb_local_rows!=nb_items_to_keep_)
        {
          Cerr << "The matrix read has " << nb_local_rows << " local columns whereas" << finl;
          Cerr << "the RHS/Solution vectors have a size of " << nb_items_to_keep_ << "." << finl;
          Cerr << "Check your data file or the file containing the PETSc matrix." << finl;
          exit();
        }
    }


  /**********************/
  /* Build renum_ array */
  /**********************/
  if (nb_matrices_creees_==1)
    {
      const MD_Vector& md = b.get_md_vector();
      renum_.reset();
      renum_.resize(0, b.line_size());
      MD_Vector_tools::creer_tableau_distribue(md, renum_, Array_base::NOCOPY_NOINIT);
    }
  int cpt=0;
  int size=items_to_keep_.size_array();
  ArrOfInt& renum_array = renum_;  // tableau vu comme lineaire
  for(int i=0; i<size; i++)
    {
      if(items_to_keep_[i])
        {
          renum_array[i]=cpt+decalage_local_global_;
          cpt++;
        }
    }
  renum_.echange_espace_virtuel();

  /*******************/
  /* Setting mataij_ */
  /*******************/
  // Matrice non symetrique, on utilise le format aij et non sbaij:
  if (!matrice_symetrique_) mataij_=1;

  // Je n'arrive pas a faire marcher le stockage symetrique avec le preconditionneur PCEISENSTAT
  // qui est interessant car necessite 2 fois moins d'operations que le SSOR
  if (type_pc_==PCEISENSTAT) mataij_=1;

  // Dans le cas de SUPERLU_DIST pour Cholesky, je n'arrive pas a faire marcher le stockage
  // symetrique donc l'utilisation de SUPERLU_DIST n'est pas encore optimale en RAM...
  if (solveur_direct_==2) mataij_=1;
  // IDEM pour UMFPACK qui ne supporte que le format AIJ:
  if (solveur_direct_==4) mataij_=1;

  // Dans le cas de CUDA, seul le format AIJ est supporte pour le moment:
  if (cuda_==1) mataij_=1;

#ifdef PETSC_HAVE_OPENMP
  // Dans le cas d'OpenMP, seul le format aij est multithreade:
  int nthreads;
  PetscThreadCommGetNThreads(PETSC_COMM_WORLD,&nthreads);
  if (nthreads>1) mataij_=1;
#endif

  // Dans le cas de save_matrix_ en parallele
  // Sinon, cela bloque avec sbaij:
  if (save_matrix_ && Process::nproc()>1) mataij_=1;

  // Error in PETSc when read/save the factored matrix if matrix is sbaij
  // so aij is selected instead:
  if (factored_matrix_!="") mataij_=1;

  // Ajout d'un test de verification de la symetrie supposee de la matrice PETSc
  // Ce test a permis de trouver un defaut de parallelisme sur le remplissage
  // de la matrice en pression lors de l'introduction de l'option volume etendu
  int check_matrice_symetrique_=1;
  // Check cancelled for:
#ifdef PETSC_HAVE_CUDA
  check_matrice_symetrique_=0; // Bug with CUDA ?
#endif
#ifdef NDEBUG
  check_matrice_symetrique_=0; // Not done in production
#endif
  check_matrice_symetrique_*=(matrice_symetrique_?1:0);
  Mat MatricePetscComplete;
  if (check_matrice_symetrique_)
    {
      // On construit une matrice PETSc complete sans hypothese sur la symetrie
      Create_MatricePetsc(MatricePetscComplete, 1, mat);
    }

  /*************************/
  /* Preconditioner matrix */
  /*************************/
  // Remplissage d'une matrice de preconditionnement non symetrique
  // pour certains precondionneurs (ILU pour HYPRE et SPAI)
  // Sinon message MatGetRow non supporte par MATMPISBAIJ
  Mat MatricePrecondionnementPetsc;
  if (matrice_symetrique_ && (type_pc_=="hypre" || type_pc_=="spai"))
    preconditionnement_non_symetrique_=1;
  if (preconditionnement_non_symetrique_)
    Create_MatricePetsc(MatricePrecondionnementPetsc, 1, mat);

  /**********************************************************************/
  /* Creation de la matrice Petsc avec hypothese ou non sur la symetrie */
  /**********************************************************************/
  if (!read_matrix_)
    {
      // Dans le cas d'une matrice symetrique, on elimine les coefficients
      // qui seront dans la partie inferieure a la diagonale. Pour cela on
      // les mets a 0 et on appelle compacte qui les supprime
      if (mataij_==0)
        {
          IntVect& tab1=mat.tab1_;
          IntVect& tab2=mat.tab2_;
          DoubleVect& coeff=mat.coeff_;
          for (int i=0; i<nb_rows_; i++)
            for (int k=tab1(i)-1; k<tab1(i+1)-1; k++)
              if (renum_array[tab2(k)-1]<i+decalage_local_global_)
                coeff(k)=0;
          mat.compacte(1);
        }
      Create_MatricePetsc(MatricePetsc_, mataij_, mat);

      /***************************************/
      /* Test de verification de la symetrie */
      /***************************************/
      if (check_matrice_symetrique_)
        {
          PetscBool matrices_identiques;
          // On teste l'egalite des 2 matrices en faisant n produits matrice-vecteur
          int n=10;
          MatMultAddEqual(MatricePetsc_,MatricePetscComplete,n,&matrices_identiques);
          if (!matrices_identiques)
            {
              Cerr << "Error: matrix PETSc are different according to the symmetric storage or not." << finl;
              if (Process::nproc()>1) Cerr << "Check if the matrix is correct in parallel." << finl;
              Cerr << "Contact TRUST support team." << finl;
              exit();
            }
          MatDestroy(&MatricePetscComplete);
        }
    }
  if (limpr()==1)
    Cerr << "Order of the PETSc matrix : " << nb_rows_tot_ << " (~ " << (petsc_cpus_selection_?int(nb_rows_tot_/petsc_nb_cpus_):nb_rows_) << " unknowns per PETSc process )" << finl;
  /* Seems petsc_decide=1 have no interest. On PETSC_GCP with n=2 (20000cell/n), the ratio is 99%-101% and petsc_decide is slower
  Even with n=9, ratio is 97%-103%, and petsc_decide is slower by 10%. Better load balance but increased MPI cost and lower convergence...
  Hope it will be better with GPU
  if (!petsc_decide_)
  {
     // Try to detect the possible gain with petsc_decide_
     int min_nb_rows = mp_min(nb_rows_);
     int max_nb_rows = mp_max(nb_rows_);
     int new_nb_rows = PETSC_DECIDE;
     PetscSplitOwnership(PETSC_COMM_WORLD, &new_nb_rows, &nb_rows_tot_);
     int min_new_nb_rows = mp_min(new_nb_rows);
     int max_new_nb_rows = mp_max(new_nb_rows);
     Cerr << min_nb_rows << " " << max_nb_rows << " " << min_new_nb_rows << " " << max_new_nb_rows << finl;
  }
  */

  /*****************************************************************************/
  /* Changement du preconditionneur pour profiter de la symetrie de la matrice */
  /*****************************************************************************/
  if (matrice_symetrique_)
    {
      MatSetOption(MatricePetsc_,MAT_SYMMETRIC,PETSC_TRUE); // Utile ?
      if (type_pc_==PCLU)
        {
          // PCCHOLESKY is only supported for sbaij matrix format else error message for example with SuperLu:
          // "Mat type seqaij symbolic factor Cholesky using solver package superlu_dist"
          if (mataij_==0)
            PCSetType(PreconditionneurPetsc_, PCCHOLESKY); // Precond PCLU -> PCCHOLESKY
        }
      else if (type_pc_==PCSOR)
        PCSORSetSymmetric(PreconditionneurPetsc_, SOR_LOCAL_SYMMETRIC_SWEEP); // Precond SOR -> SSOR
    }

  /*******************************************/
  /* Choix du package pour le solveur direct */
  /*******************************************/
  static int message_affi=1;
  if (solveur_direct_==1)
    {
      // Message pour prevenir
      if (message_affi)
        {
          Cout << "The LU decomposition of a matrix with ";
          Cout << "Cholesky from MUMPS may take several minutes, please wait..." << finl;
          Cout << "If the decomposition fails/crashes cause a lack of memory, then increase the number of CPUs for your calculation" << finl;
          Cout << "or use Cholesky_out_of_core keyword to write the decomposition on the disk, thus saving memory but with an extra CPU cost during solve." << finl;
          Cout << "To see the RAM required by the decomposition in the .out file, add impr option to the solver: petsc cholesky { impr }" << finl;
          Cout << "If an error INFOG(1)=-8|-9|-17|-20 is returned, you can try to increase the ICNTL(14) parameter of MUMPS by using the -mat_mumps_icntl_14 command line option." << finl;
          message_affi=0;
        }
      PCFactorSetMatSolverPackage(PreconditionneurPetsc_, MATSOLVERMUMPS);
    }
  else if (solveur_direct_==2)
    {
      Cout << "Cholesky from SUPERLU_DIST may take several minutes, please wait..." << finl;
      PCFactorSetMatSolverPackage(PreconditionneurPetsc_, MATSOLVERSUPERLU_DIST);
    }
  else if (solveur_direct_==3)
    {
      Cout << "Cholesky from PETSc may take several minutes, please wait..." << finl;
      PCFactorSetMatSolverPackage(PreconditionneurPetsc_, MATSOLVERPETSC);
    }
  else if (solveur_direct_==4)
    {
      Cout << "Cholesky from UMFPACK may take several minutes, please wait..." << finl;
      PCFactorSetMatSolverPackage(PreconditionneurPetsc_, MATSOLVERUMFPACK);
    }
  else if (solveur_direct_==5)
    {
      Cout << "Cholesky from Pastix may take several minutes, please wait..." << finl;
      PCFactorSetMatSolverPackage(PreconditionneurPetsc_, MATSOLVERPASTIX);
    }
  else if (solveur_direct_)
    {
      Cerr << "PCFactorSetMatSolverPackage not called for direct solver, solveur_direct_=" << solveur_direct_ << finl;
      Cerr << "Contact TRUST support." << finl;
      exit();
    }

  /****************************************/
  /* Association de la matrice au solveur */
  /****************************************/
  if (preconditionnement_non_symetrique_)
    {
      KSPSetOperators(SolveurPetsc_, MatricePetsc_, MatricePrecondionnementPetsc);
      MatDestroy(&MatricePrecondionnementPetsc);
    }
  else
    {
      KSPSetOperators(SolveurPetsc_, MatricePetsc_, MatricePetsc_);
    }

  /************************************/
  /* Factored matrix if direct solver */
  /************************************/
  if (solveur_direct_)
    {
      // Syntax: factored_matrix save|read|disk
      // disk means read the factored_matrix or compute it if not found then save it to disk
      Nom filename(Objet_U::nom_du_cas());
      filename+="_Factored_Matrix.petsc";
      if (factored_matrix_=="read" || factored_matrix_=="disk")
        {
          int filename_found=0;
          // Advice: stat file from master and broadcast
          if (Process::je_suis_maitre())
            {
              struct stat f;
              if (!stat(filename,&f)) filename_found=1;
            }
          envoyer_broadcast(filename_found, 0);
          if (filename_found)
            {
              // File found, we read it:
              factored_matrix_="read";
            }
          else
            {
              // File not found, two cases:
              Cerr << "File " << filename << " not found";
              if (factored_matrix_=="read")
                {
                  Cerr << "!" << finl;
                  exit();
                }
              else
                {
                  Cerr << "." << finl;
                  Cerr << "So we will compute the factored matrix and we will save it to disk." << finl;
                  factored_matrix_="save";
                }
            }
        }
      if (factored_matrix_=="read")
        {
          PetscViewer viewer;
          PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_READ,&viewer);
          PCFactorSetUpMatSolverPackage(PreconditionneurPetsc_);
          Mat FactoredMatrix;
          PCFactorGetMatrix(PreconditionneurPetsc_,&FactoredMatrix);
//	MatCreate(PETSC_COMM_WORLD,&FactoredMatrix);
//	MatSetSizes(FactoredMatrix, nb_rows_, nb_rows_, PETSC_DECIDE, PETSC_DECIDE);
//	MatSetType(FactoredMatrix,MATSEQAIJ);
          Cerr << "Reading the factored matrix in the file " << filename << finl;
          MatLoad(FactoredMatrix, viewer);
          PetscViewerDestroy(&viewer);
        }
      else if (factored_matrix_=="save")
        {
          Mat FactoredMatrix;
          // Compute the factored matrix:
          PCFactorSetUpMatSolverPackage(PreconditionneurPetsc_);
          PCSetUp(PreconditionneurPetsc_);
          // Get the factored matrix:
          PCFactorGetMatrix(PreconditionneurPetsc_,&FactoredMatrix);
//NO	MatConvert(FactoredMatrix,MATSEQDENSE,MAT_INITIAL_MATRIX,&FactoredMatrix);
//	MatScalar *a;
//	MatDenseGetArray(FactoredMatrix,&a);
          if (nb_rows_<20)
            {
              Cerr << "A=" << finl;
              MatView(MatricePetsc_, PETSC_VIEWER_STDOUT_WORLD);
              Cerr << "LU=" << finl;
              MatView(FactoredMatrix, PETSC_VIEWER_STDOUT_WORLD);
            }
          // Save to disk:
          Cerr << "Writing the factored matrix in the file " << filename << finl;
          Cerr << "Not implemented yet." << finl;
          exit();
          PetscViewer viewer;
          PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_WRITE,&viewer);
          MatView(FactoredMatrix, viewer);
          PetscViewerDestroy(&viewer);

//          PetscViewer viewer2;
//          PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_READ,&viewer2);
//          MatLoad(FactoredMatrix, viewer2);
//          PetscViewerDestroy(&viewer2);
        }
      else if (factored_matrix_!="")
        {
          Cerr << "Unknown option for factored_matrix option: " << factored_matrix_ << finl;
          Cerr << "Options are: read|save|disk" << finl;
          exit();
        }
    }
  /*************************************/
  /* Mise en place du preconditionneur */
  /*************************************/
  KSPSetUp(SolveurPetsc_);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Calcul du residu de la meme maniere que le GCP de TRUST : ||Ax-B|| pour pouvoir comparer les performances//
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (matrice_symetrique_)
    {
      if (type_ksp_==KSPCG || type_ksp_==KSPPIPECG || type_ksp_==KSPGROPPCG || type_ksp_==KSPRICHARDSON)
        {
          // Residu=||Ax-b|| comme dans TRUST pour GCP sinon on ne peut comparer les convergences
          KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
        }
      else if (type_ksp_==KSPPGMRES)
        {
          // PGMRES ne peut etre que preconditionne a gauche (CAx=Cb)
          // et on ne peut avoir que le residu preconditionne (||CAx-Cb||)
          // -> on ne peut comparer la convergence avec le GMRES...
          KSPSetPCSide(SolveurPetsc_, PC_LEFT);
          // KSPSetNormType(SolveurPetsc, KSP_NORM_UNPRECONDITIONED);
        }
      else
        {
          // Le preconditionnement a droite permet que le residu utilise pour la convergence
          // soit le residu reel ||Ax-b|| et non le residu preconditionne pour certains solveurs
          // avec un preconditionnement a gauche (ex: GMRES). Ainsi, on peut comparer strictement
          // les performances des solveurs (TRUST ou PETSC) entre eux
          KSPSetPCSide(SolveurPetsc_, PC_RIGHT);
          // Pour un certain nombre de solveurs, il faut preciser la facon dont la norme ||Ax-b||
          // sera calculee:
          if (type_ksp_==KSPBCGS || type_ksp_==KSPIBCGS || type_ksp_==KSPGMRES)
            {
              KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
            }
        }
    }
  /**************************************************/
  /* Build x and b during the first matrix creation */
  /**************************************************/
  // We suppose after that, if a new matrix is recreated onto the solver,
  // the number of rows is the same...
  if (nb_matrices_creees_==1)
    {
      // Build x
      VecCreate(PETSC_COMM_WORLD,&SecondMembrePetsc_);
      // Set sizes:
      if (petsc_decide_)
        VecSetSizes(SecondMembrePetsc_, PETSC_DECIDE, nb_rows_tot_);
      else if (petsc_cpus_selection_)
        {
          int nb_rows_petsc = compute_nb_rows_petsc(nb_rows_tot_);
          VecSetSizes(SecondMembrePetsc_, nb_rows_petsc, PETSC_DECIDE);
        }
      else
        VecSetSizes(SecondMembrePetsc_, nb_rows_, PETSC_DECIDE);

      // Set type:
      if (Process::nproc()>1)
        VecSetType(SecondMembrePetsc_, cuda_==1 ? VECMPICUSP : VECMPI);
      else
        VecSetType(SecondMembrePetsc_, cuda_==1 ? VECSEQCUSP : VECSEQ);
      VecSetFromOptions(SecondMembrePetsc_);
      // Build b
      VecDuplicate(SecondMembrePetsc_,&SolutionPetsc_);
      // Initialize x to avoid a crash on GPU later with VecSetValues... (bug PETSc?)
      if (cuda_==1) VecSet(SolutionPetsc_,0.0);

      // Only in the case where TRUST and PETSc partitions are not the same
      // VecGetValues can only get values on the same processor, so need to gather values from
      // global vector SolutionPetsc_ to a local vector LocalSolutionPetsc_ before using VecGetValues
      // It will add an extra MPI cost with this operation.
      if (different_partition_)
        {
          // Create the local vector of length nb_rows_:
          VecCreateSeq(PETSC_COMM_SELF, nb_rows_, &LocalSolutionPetsc_);
          // Create the Scatter context to gather from the global solution to the local solution
          ArrOfInt from(nb_rows_);
          for (int i=0; i<nb_rows_; i++)
            from(i)=decalage_local_global_+i; // Global indices in SolutionPetsc_
          IS fromis;
          ISCreateGeneral(PETSC_COMM_WORLD, from.size_array(), from.addr(), PETSC_COPY_VALUES, &fromis);
          VecScatterCreate(SolutionPetsc_, fromis, LocalSolutionPetsc_, NULL, &VecScatter_);
          ISDestroy(&fromis);
          // Will permit later with VecScatterBegin/VecScatterEnd something like:
          // LocalSolutionPetsc_[tois[i]]=SolutionPetsc_[fromis[i]]
        }
    }
  return 1;
}

int Solv_Petsc::compute_nb_rows_petsc(int nb_rows_tot)
{
  // Case the user specifies a number of CPUs:
  int nb_rows_petsc = nb_rows_tot / petsc_nb_cpus_;
  // Process 0 takes the possible rows in excess:
  if (je_suis_maitre()) nb_rows_petsc = nb_rows_tot - (petsc_nb_cpus_ - 1) * nb_rows_petsc;
  //
  if (petsc_cpus_selection_==1)
    {
      // First nb_cpus_ CPUs only so:
      if (Process::me() >= petsc_nb_cpus_) nb_rows_petsc = 0;
    }
  else if (petsc_cpus_selection_==2)
    {
      // Every nb_cpus CPUs only so:
      if (Process::me() % petsc_nb_cpus_ != 0) nb_rows_petsc = 0;
    }
  else
    {
      Cerr << "Error: petsc_cpus_selection_=" << petsc_cpus_selection_ << finl;
      exit();
    }
  return nb_rows_petsc;
}

// Creation d'une matrice Petsc en fonction d'une matrice stockee au format CSR
void Solv_Petsc::Create_MatricePetsc(Mat& MatricePetsc, int mataij, Matrice_Morse& mat)
{
  // Recuperation des donnees
  IntVect& tab1_=mat.tab1_;
  IntVect& tab2_=mat.tab2_;
  DoubleVect& coeff_=mat.coeff_;
  int nb_rows = mat.nb_lignes();
  int nb_rows_tot = mp_sum(nb_rows);

  /////////////////////////////////////
  // On cree et dimensionne la matrice
  /////////////////////////////////////
  // Based on src/ksp/ksp/examples/tutorials/ex2.c
  MatCreate(PETSC_COMM_WORLD, &MatricePetsc);
  if (petsc_decide_)
    MatSetSizes(MatricePetsc, PETSC_DECIDE, PETSC_DECIDE, nb_rows_tot, nb_rows_tot);
  else if (petsc_cpus_selection_)
    {
      int nb_rows_petsc = compute_nb_rows_petsc(nb_rows_tot);
      Journal() << "Process " << Process::me() << " has " << nb_rows_petsc << " rows of the matrix PETSc." << finl;
      MatSetSizes(MatricePetsc, nb_rows_petsc, nb_rows_petsc, PETSC_DECIDE, PETSC_DECIDE);
    }
  else // Normal use: partition of PETSc matrix is dicted by TRUST matrix:
    MatSetSizes(MatricePetsc, nb_rows, nb_rows, PETSC_DECIDE, PETSC_DECIDE);

  //////////////////////////////////////////////
  // Determination du nombre d'elements non nuls
  //////////////////////////////////////////////
  ArrOfInt nnz(nb_rows);
  ArrOfInt d_nnz(nb_rows);
  ArrOfInt o_nnz(nb_rows);
  d_nnz=0;
  o_nnz=0;
  ArrOfInt& renum_array = renum_;  // tableau vu comme lineaire
  int premiere_colonne_globale = decalage_local_global_;
  int derniere_colonne_globale = nb_rows+decalage_local_global_;
  for(int i=0; i<nb_rows; i++)
    {
      nnz(i)=tab1_(i+1)-tab1_(i); // Nombre d'elements non nuls sur la ligne i
      for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        {
          int colonne_locale = tab2_(k) - 1;
          int colonne_globale = renum_array[colonne_locale];
          if (colonne_globale>=premiere_colonne_globale && colonne_globale<derniere_colonne_globale)
            d_nnz(i)++;
          else
            o_nnz(i)++;
        }
      //Journal() << i+decalage_local_global_ << " " << d_nnz(i) << " " <<o_nnz(i) << finl;
    }
  /************************/
  /* Typage de la matrice */
  /************************/
  if (mataij==0)
    {
      // On utilise SBAIJ pour une matrice symetrique (plus rapide que AIJ)
      MatSetType(MatricePetsc, (Process::nproc()==1?MATSEQSBAIJ:MATMPISBAIJ));
    }
  else
    {
      // On utilise AIJ car je n'arrive pas a faire marcher avec BAIJ
#ifdef PETSC_HAVE_CUDA
      if (cuda_==1)
        MatSetType(MatricePetsc, (Process::nproc()==1?MATSEQAIJCUSP:MATMPIAIJCUSP));
      else
#endif
        MatSetType(MatricePetsc, (Process::nproc()==1?MATSEQAIJ:MATMPIAIJ));
    }
  // Surcharge eventuelle par ligne de commande avec -mat_type:
  // Example: now possible to change aijcusp to aijcusparse via CLI
  MatSetFromOptions(MatricePetsc);

  /********************************************/
  /* Preallocation de la taille de la matrice */
  /********************************************/
  // TRES important pour la vitesse de construction de la matrice
  if (mataij==0)
    {
      if (different_partition_)
        {
          // If partition of TRUST and PETSc differs, difficult to preallocate the matrix finely so:
          // ToDo, try to optimize:
          int nz=(int)mp_max((nnz.size_array()==0?0:max_array(nnz)));
          MatSeqSBAIJSetPreallocation(MatricePetsc, block_size_, nz, PETSC_NULL);
          MatMPISBAIJSetPreallocation(MatricePetsc, block_size_, nz, PETSC_NULL, nz, PETSC_NULL);
        }
      else
        {
          MatSeqSBAIJSetPreallocation(MatricePetsc, block_size_, PETSC_DEFAULT, nnz.addr());
          // Test on nb_rows==0 is to avoid PAR_docond_anisoproc hangs
          MatMPISBAIJSetPreallocation(MatricePetsc, block_size_, (nb_rows==0?0:PETSC_DEFAULT), d_nnz.addr(), (nb_rows==0?0:PETSC_DEFAULT), o_nnz.addr());
        }
    }
  else
    {
      if (different_partition_)
        {
          // If partition of TRUST and PETSc differs, difficult to preallocate the matrix finely so:
          // ToDo, try to optimize:
          int nz=(int)mp_max((nnz.size_array()==0?0:max_array(nnz)));
          MatSeqAIJSetPreallocation(MatricePetsc, nz, PETSC_NULL);
          MatMPIAIJSetPreallocation(MatricePetsc, nz, PETSC_NULL, nz, PETSC_NULL);
        }
      else
        {
          MatSeqAIJSetPreallocation(MatricePetsc, PETSC_DEFAULT, nnz.addr());
          // Test on nb_rows==0 is to avoid PAR_docond_anisoproc hangs
          MatMPIAIJSetPreallocation(MatricePetsc, (nb_rows==0?0:PETSC_DEFAULT), d_nnz.addr(), (nb_rows==0?0:PETSC_DEFAULT), o_nnz.addr());
        }
    }

  /*****************************/
  /* Remplissage de la matrice */
  /*****************************/
  // ligne par ligne avec un tableau coeff et tab2 qui contiennent
  // les coefficients et les colonnes globales pour chaque ligne
  // On dimensionne ces tableaux a la taille la plus grande possible
  int size = (nb_rows==0?0:max_array(nnz)); // Test sur nb_rows si nul (cas proc vide) car sinon max_array plante
  ArrOfDouble coeff(size);
  ArrOfInt tab2(size);
  for(int i=0; i<nb_rows; i++)
    {
      int ligne_globale = i + decalage_local_global_;
      int ncol=0;
      for (int k=tab1_(i)-1; k<tab1_(i+1)-1; k++)
        {
          coeff[ncol]=coeff_(k);
          tab2[ncol]=renum_array[tab2_(k)-1];
          //Journal() << "Inserting a(" << ligne_globale <<","<<tab2[ncol]<<")="<<coeff[ncol]<<finl;
          ncol++;
        }
      assert(ncol==nnz(i));
      MatSetValues(MatricePetsc, 1, &ligne_globale, ncol, tab2.addr(), coeff.addr(), INSERT_VALUES);
    }
  /****************************/
  /* Assemblage de la matrice */
  /****************************/
  MatAssemblyBegin(MatricePetsc, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(MatricePetsc, MAT_FINAL_ASSEMBLY);
}

#endif
