/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Solv_Petsc.h>
#ifdef PETSCKSP_H
#include <petscis.h>
#include <petscdmshell.h>
#include <petscsection.h>
#ifdef PETSC_HAVE_HYPRE
#include <HYPRE_config.h>
#endif
#include <cfenv>
#include <tuple>
#include <Matrice_Morse_Sym.h>
#include <stat_counters.h>
#include <Matrice_Bloc_Sym.h>
#include <Matrice_Bloc.h>
#include <communications.h>
#include <MD_Vector_tools.h>
#include <PE_Groups.h>
#include <Comm_Group_MPI.h>
#include <map>
#include <ctime>
#include <EFichier.h>
#include <sys/stat.h>
#endif
#include <Matrice_Petsc.h>
#include <Motcle.h>
#include <SChaine.h>
#include <SFichier.h>
#include <TRUSTTrav.h>
#include <MD_Vector_composite.h>
#include <vector>
#include <functional>

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_Petsc,"Solv_Petsc",Solv_Externe);

// XD petsc solveur_sys_base petsc 0 Solver via Petsc API
// XD   attr solveur solveur_petsc_deriv solveur 0 solver type and options

// XD solveur_petsc_deriv objet_u solveur_petsc_deriv -1 Additional information is available in the PETSC documentation: https://petsc.org/release/manual/
// XD   attr seuil floattant seuil 1 corresponds to the iterative solver convergence value. The iterative solver converges when the Euclidean residue standard ||Ax-B|| is less than seuil.
// XD   attr quiet rien quiet 1 is a keyword which is used to not displaying any outputs of the solver.
// XD   attr impr rien impr 1 used to request display of the Euclidean residue standard each time this iterates through the conjugated gradient (display to the standard outlet).
// XD   attr rtol floattant rtol 1 not_set
// XD   attr atol floattant atol 1 not_set
// XD   attr save_matrix_mtx_format rien save_matrix_mtx_format 1 not_set

// XD solveur_petsc_lu solveur_petsc_deriv lu -1 Several solvers through PETSc API are available. NL2 TIPS: NL2 NL2 NL2 A) Solver for symmetric linear systems (e.g: Pressure system from Navier-Stokes equations): NL2 -The CHOLESKY parallel solver is from MUMPS library. It offers better performance than all others solvers if you have enough RAM for your calculation. A parallel calculation on a cluster with 4GBytes on each processor, 40000 cells/processor seems the upper limit. Seems to be very slow to initialize above 500 cpus/cores. NL2 -When running a parallel calculation with a high number of cpus/cores (typically more than 500) where preconditioner scalabilty is the key for CPU performance, consider BICGSTAB with BLOCK_JACOBI_ICC(1) as preconditioner or if not converges, GCP with BLOCK_JACOBI_ICC(1) as preconditioner. NL2 -For other situations, the first choice should be GCP/SSOR. In order to fine tune the solver choice, each one of the previous list should be considered. Indeed, the CPU speed of a solver depends of a lot of parameters. You may give a try to the OPTIMAL solver to help you to find the fastest solver on your study. NL2 NL2 B) Solver for non symmetric linear systems (e.g.: Implicit schemes): NL2 The BICGSTAB/DIAG solver seems to offer the best performances.

// XD solveur_petsc_Cholesky_superlu solveur_petsc_deriv Cholesky_superlu -1 Parallelized Cholesky from SUPERLU_DIST library (less CPU and RAM, efficient than the previous one)

// XD solveur_petsc_Cholesky_pastix solveur_petsc_deriv Cholesky_pastix -1  Parallelized Cholesky from PASTIX library.

// XD solveur_petsc_Cholesky_umfpack solveur_petsc_deriv Cholesky_umfpack -1 Sequential Cholesky from UMFPACK library (seems fast).

// XD solveur_petsc_Cholesky_out_of_core solveur_petsc_deriv Cholesky_out_of_core -1 Same as the previous one but with a written LU decomposition of disk (save RAM memory but add an extra CPU cost during Ax=B solve).

// XD solveur_petsc_cholesky solveur_petsc_deriv cholesky -1 Parallelized version of Cholesky from MUMPS library. This solver accepts an option to select a different ordering than the automatic selected one by MUMPS (and printed by using the impr option). The possible choices are Metis, Scotch, PT-Scotch or Parmetis. The two last options can only be used during a parallel calculation, whereas the two first are available for sequential or parallel calculations. It seems that the CPU cost of A=LU factorization but also of the backward/forward elimination steps may sometimes be reduced by selecting a different ordering (Scotch seems often the best for b/f elimination) than the default one. NL2 Notice that this solver requires a huge amont of memory compared to iterative methods. To know how much RAM you will need by core, then use the impr option to have detailled informations during the analysis phase and before the factorisation phase (in the following output, you will learn that the largest memory is taken by the zeroth CPU with 108MB): NL2 Rank of proc needing largest memory in IC facto :  0  NL2 Estimated corresponding MBYTES for IC facto : 108 NL2 Thanks to the following graph, you read that in order to solve for instance a flow on a mesh with 2.6e6 cells, you will need to run a parallel calculation on 32 CPUs if you have cluster nodes with only 4GB/core (6.2GB*0.42~2.6GB) : NL2 \includepng{{petscgraph.jpeg}}{{10}}
// XD   attr save_matrice|save_matrix rien save_matrix 1 not_set
// XD   attr save_matrix_petsc_format rien save_matrix_petsc_format 1 not_set
// XD   attr reduce_ram rien reduce_ram 1 not_set
// XD   attr cli_quiet solveur_petsc_option_cli cli_quiet 1 not_set
// XD   attr cli solveur_petsc_option_cli cli 1 not_set

// XD solveur_petsc_cholesky_mumps_blr solveur_petsc_deriv cholesky_mumps_blr -1 BLR for (Block Low-Rank)
// XD   attr reduce_ram rien reduce_ram 1 not_set
// XD   attr dropping_parameter floattant dropping_parameter 1 not_set
// XD   attr cli solveur_petsc_option_cli cli 1 not_set

// XD solveur_petsc_option_cli bloc_lecture nul -1 solver

// XD solveur_petsc_cli solveur_petsc_deriv cli 0  Command Line Interface. Should be used only by advanced users, to access the whole solver/preconditioners from the PETSC API. To find all the available options, run your calculation with the -ksp_view -help options: NL2 trust datafile [N] --ksp_view --help NL2 -pc_type Preconditioner:(one of) none jacobi pbjacobi bjacobi sor lu shell mg eisenstat ilu icc cholesky asm ksp composite redundant nn mat fieldsplit galerkin openmp spai hypre tfs (PCSetType) NL2 HYPRE preconditioner options: NL2 -pc_hypre_type pilut (choose one of) pilut parasails boomeramg NL2 HYPRE ParaSails Options NL2 -pc_hypre_parasails_nlevels 1: Number of number of levels (None) NL2 -pc_hypre_parasails_thresh 0.1: Threshold (None) NL2 -pc_hypre_parasails_filter 0.1: filter (None) NL2 -pc_hypre_parasails_loadbal 0: Load balance (None) NL2 -pc_hypre_parasails_logging: FALSE Print info to screen (None) NL2 -pc_hypre_parasails_reuse: FALSE Reuse nonzero pattern in preconditioner (None) NL2 -pc_hypre_parasails_sym nonsymmetric (choose one of) nonsymmetric SPD nonsymmetric,SPD NL2 NL2 Krylov Method (KSP) Options NL2 -ksp_type Krylov method:(one of) cg cgne stcg gltr richardson chebychev gmres tcqmr bcgs bcgsl cgs tfqmr cr lsqr preonly qcg bicg fgmres minres symmlq lgmres lcd (KSPSetType) NL2 -ksp_max_it 10000: Maximum number of iterations (KSPSetTolerances) NL2 -ksp_rtol 0: Relative decrease in residual norm (KSPSetTolerances) NL2 -ksp_atol 1e-12: Absolute value of residual norm (KSPSetTolerances) NL2 -ksp_divtol 10000: Residual norm increase cause divergence (KSPSetTolerances) NL2 -ksp_converged_use_initial_residual_norm: Use initial residual residual norm for computing relative convergence NL2 -ksp_monitor_singular_value stdout: Monitor singular values (KSPMonitorSet) NL2 -ksp_monitor_short stdout: Monitor preconditioned residual norm with fewer digits (KSPMonitorSet) NL2 -ksp_monitor_draw: Monitor graphically preconditioned residual norm (KSPMonitorSet) NL2 -ksp_monitor_draw_true_residual: Monitor graphically true residual norm (KSPMonitorSet) NL2 NL2 Example to use the multigrid method as a solver, not only as a preconditioner: NL2 Solveur_pression Petsc CLI {-ksp_type richardson -pc_type hypre -pc_hypre_type boomeramg -ksp_atol 1.e-7 }
// XD   attr seuil suppress_param seuil 1 corresponds to the iterative solver convergence value. The iterative solver converges when the Euclidean residue standard  is less than seuil.
// XD   attr quiet suppress_param quiet 1 is a keyword which is used to not displaying any outputs of the solver.
// XD   attr impr suppress_param impr 1 impress or not
// XD   attr rtol suppress_param rtol 1 not_set
// XD   attr atol suppress_param atol 1 not_set
// XD   attr save_matrix_mtx_format suppress_param save_matrix_mtx_format 1 not_set
// XD   attr cli_bloc bloc_lecture cli_bloc 0 bloc

// XD solveur_petsc_cli_quiet solveur_petsc_deriv cli_quiet 0 solver
// XD   attr seuil suppress_param seuil 1 corresponds to the iterative solver convergence value. The iterative solver converges when the Euclidean residue standard is less than seuil.
// XD   attr quiet suppress_param quiet 1 is a keyword which is used to not displaying any outputs of the solver.
// XD   attr impr suppress_param impr 1 impress or not
// XD   attr rtol suppress_param rtol 1 not_set
// XD   attr atol suppress_param atol 1 not_set
// XD   attr save_matrix_mtx_format suppress_param save_matrix_mtx_format 1 not_set
// XD   attr cli_quiet_bloc bloc_lecture cli_quiet_bloc 0 bloc

// XD solveur_petsc_IBICGSTAB solveur_petsc_deriv IBICGSTAB -1 Improved version of previous one for massive parallel computations (only a single global reduction operation instead of the usual 3 or 4).
// XD   attr precond preconditionneur_petsc_deriv precond 1 not_set

// XD solveur_petsc_BICGSTAB solveur_petsc_deriv BICGSTAB -1 Stabilized Bi-Conjugate Gradient
// XD   attr precond preconditionneur_petsc_deriv precond 1 not_set

// XD solveur_petsc_gmres solveur_petsc_deriv gmres -1 Generalized Minimal Residual
// XD   attr precond preconditionneur_petsc_deriv precond 1 not_set
// XD   attr reuse_preconditioner_nb_it_max entier reuse_preconditioner_nb_it_max 1 not_set
// XD   attr save_matrix_petsc_format rien save_matrix_petsc_format 1 not_set
// XD   attr nb_it_max entier nb_it_max 1 In order to specify a given number of iterations instead of a condition on the residue with the keyword seuil. May be useful when defining a PETSc solver for the implicit time scheme where convergence is very fast: 5 or less iterations seems enough.

// XD solveur_petsc_gcp solveur_petsc_deriv gcp -1 Preconditioned Conjugate Gradient
// XD   attr precond preconditionneur_petsc_deriv precond 1 preconditioner
// XD   attr precond_nul rien precond_nul 1 No preconditioner used, equivalent to precond null { }
// XD   attr rtol floattant rtol 1 not_set
// XD   attr reuse_preconditioner_nb_it_max entier reuse_preconditioner_nb_it_max 1 not_set
// XD   attr cli solveur_petsc_option_cli cli 1 not_set
// XD   attr reorder_matrix entier reorder_matrix 1 not_set
// XD   attr read_matrix rien read_matrix 1 save_matrix|read_matrix are the keywords to save|read into a file the constant matrix A of the linear system Ax=B solved (eg: matrix from the pressure linear system for an incompressible flow). It is useful when you want to minimize the MPI communications on massive parallel calculation. Indeed, in VEF discretization, the overlapping width (generaly 2, specified with the largeur_joint option in the partition keyword partition) can be reduced to 1, once the matrix has been properly assembled and saved. The cost of the MPI communications in TRUST itself (not in PETSc) will be reduced with length messages divided by 2. So the strategy is: NL2 I) Partition your VEF mesh with a largeur_joint value of 2 NL2 II) Run your parallel calculation on 0 time step, to build and save the matrix with the save_matrix option. A file named Matrix_NBROWS_rows_NCPUS_cpus.petsc will be saved to the disk (where NBROWS is the number of rows of the matrix and NCPUS the number of CPUs used). NL2 III) Partition your VEF mesh with a largeur_joint value of 1 NL2 IV) Run your parallel calculation completly now and substitute the save_matrix option by the read_matrix option. Some interesting gains have been noticed when the cost of linear system solve with PETSc is small compared to all the other operations.
// XD   attr save_matrice|save_matrix rien save_matrix 1 see read_matrix
// XD   attr petsc_decide entier petsc_decide 1 not_set
// XD   attr pcshell chaine pcshell 1 not_set
// XD   attr aij rien aij 1 not_set

// XD solveur_petsc_PIPECG solveur_petsc_deriv PIPECG -1 Pipelined Conjugate Gradient (possible reduced CPU cost during massive parallel calculation due to a single non-blocking reduction per iteration, if TRUST is built with a MPI-3 implementation)... no example in TRUST


// XD preconditionneur_petsc_deriv objet_u preconditionneur_petsc_deriv -1 Preconditioners available with petsc solvers

// XD preconditionneur_petsc_diag preconditionneur_petsc_deriv diag -1 Diagonal (Jacobi) preconditioner.

// XD preconditionneur_petsc_c_amg preconditionneur_petsc_deriv c-amg -1 preconditionner

// XD preconditionneur_petsc_sa_amg preconditionneur_petsc_deriv sa-amg -1 preconditionner

// XD preconditionneur_petsc_BLOCK_JACOBI_ICC preconditionneur_petsc_deriv BLOCK_JACOBI_ICC -1 Incomplete Cholesky factorization for symmetric matrix with the PETSc implementation.
// XD   attr level entier level 1 factorization level (default value, 1). In parallel, the factorization is done by block (one per processor by default).
// XD   attr ordering chaine(into=["natural","rcm"]) ordering 1  The ordering of the local matrix is natural by default, but rcm ordering, which reduces the bandwith of the local matrix, may interestingly improves the quality of the decomposition and reduces the number of iterations.

// XD preconditionneur_petsc_boomeramg preconditionneur_petsc_deriv boomeramg -1 Multigrid preconditioner (no option is available yet, look at CLI command and Petsc documentation to try other options).

// XD preconditionneur_petsc_null preconditionneur_petsc_deriv null -1 No preconditioner used

// XD preconditionneur_petsc_lu preconditionneur_petsc_deriv lu -1 preconditionner

// XD preconditionneur_petsc_jacobi preconditionneur_petsc_deriv jacobi -1 preconditionner

// XD preconditionneur_petsc_EISENTAT preconditionneur_petsc_deriv EISENTAT -1 SSOR version with Eisenstat trick which reduces the number of computations and thus CPU cost...
// XD   attr omega floattant omega 1 relaxation factor

// XD preconditionneur_petsc_ssor preconditionneur_petsc_deriv ssor -1 Symmetric Successive Over Relaxation algorithm.
// XD   attr omega floattant omega 1 relaxation factor (default value, 1.5)

// XD preconditionneur_petsc_block_jacobi_ilu preconditionneur_petsc_deriv block_jacobi_ilu -1 preconditionner
// XD   attr level entier level 1 not_set

// XD preconditionneur_petsc_spai preconditionneur_petsc_deriv spai -1  Spai Approximate Inverse algorithm from Parasails Hypre library.
// XD   attr level entier level 1 first parameter
// XD   attr epsilon floattant epsilon 1 second parameter

// XD preconditionneur_petsc_pilut preconditionneur_petsc_deriv pilut -1 Dual Threashold Incomplete LU factorization.
// XD   attr level entier level 1 factorization level
// XD   attr epsilon floattant epsilon 1 drop tolerance



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

void check_not_defined(option o)
{
  if (o.defined)
    {
      Cerr << "Error! Option " << o.name << " should not be defined with the preconditioner of this solver." << finl;
      Cerr << "Change your data file." << finl;
      Process::exit();
    }
}

//check to see if a string is a number
#ifdef PETSCKSP_H
static bool is_number(const std::string& s)
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}
#endif

bool gmres_right_unpreconditionned=true;
// Lecture et creation du solveur
void Solv_Petsc::create_solver(Entree& entree)
{
  if (amgx_ || std::getenv("TRUST_PETSC_VERBOSE")) verbose = true;
#ifdef PETSCKSP_H

  if(!std::is_same<PetscInt, trustIdType>::value)
    Process::exit("Type mismatch!!! PetscInt and trustIdType should be equal!!! PETSc not compiled in 64b??");

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
  if (option_prefix_=="??") // Prefix non fixe
    {
      numero_solveur++;
      option_prefix_="";
      if (numero_solveur > 1)
        {
          // On cree un prefix pour les options si plus d'un solveur pour les differencier. Exemple:
          // premier solveur -ksp_type ... -pc_type ...
          // deuxieme solveur -solver2_ksp_type ... -solver2_pc-type ...
          // troisieme solveur -solveur3_ksp_type ...
          option_prefix_ += "solver";
          option_prefix_ += (Nom) numero_solveur;
          option_prefix_ += "_";
        }
    }

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
  if (PE_Groups::get_nb_groups()==1 && !disable_TU)
    {
      // _petsc.TU is only printed if one group calculation (e.g. Execute_parallel failed)
      Nom petsc_TU(":");
      petsc_TU+=nom_du_cas();
      petsc_TU+="_petsc.TU";
#ifdef TRUST_USE_GPU
      if (instance==1) PetscLogGpuTime(); // May slow down calculation
      //add_option("log_view_gpu_time",""); 	// Pas pris en compte...
#endif
      add_option("log_view",petsc_TU); 	// Monitor performances at the end of the calculation
      PetscLogDefaultBegin(); 		// Necessary cause if not Event logs not printed in petsc_TU file ... I don't know why...
    }
#ifdef NDEBUG
  // PETSc 3.14 active par defaut les exceptions, on desactive en production ?
  // PetscSetFPTrap(PETSC_FP_TRAP_OFF);
  // Utiliser -fp_trap 0 a l'execution plutot: Segfault vu sur petsc gmres { precond diag ... }
#endif
  //add_option("on_error_abort",""); // ne marche pas semble t'il
  // On doit pouvoir lire des mots cles de base (GCP, GMRES, CHOLESKY)
  // mais egalement pouvoir appeler les options Petsc avec une chaine { -ksp_type cg -pc_type sor ... }
  // Les options non reconnues doivent arreter le code
  // Reprendre le formalisme de GCP { precond ssor { omega val } seuil val }
  Motcles les_solveurs(21);
  {
    les_solveurs[0] = "CLI";
    les_solveurs[1] = "GCP";
    les_solveurs[2] = "GMRES";
    les_solveurs[3] = "CHOLESKY|MUMPS";
    les_solveurs[4] = "CHOLESKY_OUT_OF_CORE|MUMPS_OUT_OF_CORE";
    les_solveurs[5] = "BICGSTAB";
    les_solveurs[6] = "IBICGSTAB";
    les_solveurs[7] = "CHOLESKY_SUPERLU|LU_SUPERLU";
    les_solveurs[8] = "PGMRES";
    les_solveurs[9] = "LU";
    les_solveurs[10] = "PIPECG";
    les_solveurs[11] = "CHOLESKY_LAPACK";
    les_solveurs[12] = "CHOLESKY_UMFPACK";
    les_solveurs[13] = "CHOLESKY_PASTIX";
    les_solveurs[14] = "CLI_VERBOSE";
    les_solveurs[15] = "CLI_QUIET";
    les_solveurs[16] = "CHOLESKY_MUMPS_BLR|MUMPS_BLR";
    les_solveurs[17] = "CHOLESKY_CHOLMOD";
    les_solveurs[18] = "PIPECG2";
    les_solveurs[19] = "FGMRES";
    les_solveurs[20] = "LU_STRUMPACK";
  }
  int solver_supported_on_gpu_by_petsc=0;
  int solver_supported_on_gpu_by_amgx=0;
  amgx_options_="";
  int rang=les_solveurs.search(ksp);
  nommer(les_solveurs[rang]);
  switch(rang)
    {
    case 0:
    case 14:
    case 15:
      {
        if (rang == 15) fixer_limpr(-1);  // Quiet
        else fixer_limpr(1); // On imprime le residu
        solver_supported_on_gpu_by_petsc=1; // Not really, reserved to expert...
        solver_supported_on_gpu_by_amgx=1;  // Not really, reserved to expert...
        if (limpr() >= 0) Cerr << "Reading of the " << (amgx_ ? "AmgX" : "Petsc") << " commands:" << finl;
        Nom valeur;
        is >> motlu;
        if (amgx_)
          {
            while (motlu!=accolade_fermee)
              {
                // -config file.json
                if (motlu == "-file")
                  {
                    is >> motlu;
                    if (Process::je_suis_maitre())
                      {
                        Cerr << "Reading AmgX config file " << motlu << " :" << finl;
                        EFichier config_amgx(motlu);
                        std::string line;
                        while (!config_amgx.eof())
                          {
                            std::getline(config_amgx.get_ifstream(), line);
                            if (line.find("#") && line.find("config_version"))
                              {
                                Cerr << line << finl;
                                amgx_options_+=line;
                                amgx_options_+="\n";
                              }
                          }
                      }
                  }
                else
                  {
                    amgx_options_ += motlu;
                    amgx_options_ += "\n";
                  }
                is >> motlu;
              }
          }
        else
          while (motlu!=accolade_fermee)
            {
              is >> valeur;
              // "-option val" ou "-option" ?
              if (valeur.debute_par("-") || valeur==accolade_fermee)
                {
                  add_option(motlu.suffix("-"), "", 1);
                  motlu = valeur;
                }
              else
                {
                  if (motlu == "-ksp_type" && valeur=="preonly") solveur_direct_=cli; // Activate direct solveur if using -ksp_preonly ...
                  if (motlu == "-ksp_max_it") ignore_nb_it_max_ = 1; //pour un comportement similaire a l'option nb_it_max
                  add_option(motlu.suffix("-"), valeur, 1);
                  is >> motlu;
                }
            }
        // Pour faciliter le debugage:
        if (rang == 14) // Verbose
          {
            add_option("ksp_view", "");
            add_option("options_view", "");
            add_option("options_left", "");
          }
        if (!amgx_)
          {
            // Changement dans PETSc 3.21: plus de preconditioneur par defaut
            // On met ILU(0) comme auparavant pour ne pas changer tous les jeux de donnees qui ont: "petsc cli { }"
            Nom current_pc;
            Nom option="-";
            option+=option_prefix_;
            option+="pc_type";
            if (!has_option(option, current_pc))
              {
                if (Process::nproc()>1)
                  {
                    add_option("pc_type", "bjacobi");
                    add_option("sub_pc_type", "ilu");
                  }
                else
                  {
                    add_option("pc_type", "ilu");
                  }
              }
          }
        break;
      }
    case 1:
      {
        KSPSetType(SolveurPetsc_, KSPCG);
        // Residu=||Ax-b|| comme dans TRUST pour GCP sinon on ne peut comparer les convergences
        KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
        // Merge the two inner products needed in CG into a single MPI_Allreduce() call:
        // Gain interessant a partir de 4000 coeurs
        if (Process::nproc()>=4000)
          {
            //add_option("ksp_cg_single_reduction",""); Pour Petsc < 3.3, la fonction KSPCGUseSingleReduction n'etait pas disponible
            KSPCGUseSingleReduction(SolveurPetsc_,(PetscBool)1);
          }
        // But It requires two extra work vectors than the conventional implementation in PETSc.
        solver_supported_on_gpu_by_petsc=1;
        solver_supported_on_gpu_by_amgx=1;
        add_amgx_option("solver(s)","PCG"); // CG avec preconditionnement
        // PCGF : Flexible CG avec preconditionnement
        break;
      }
    case 10:
      {
        KSPSetType(SolveurPetsc_, KSPPIPECG);
        // Residu=||Ax-b|| comme dans TRUST pour GCP sinon on ne peut comparer les convergences
        KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
        break;
      }
    case 18:
      {
        KSPSetType(SolveurPetsc_, KSPPIPECG2);
        // Residu=||Ax-b|| comme dans TRUST pour GCP sinon on ne peut comparer les convergences
        KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
        break;
      }
    case 2:
      {
        KSPSetType(SolveurPetsc_, KSPGMRES);
        // Le preconditionnement a droite permet que le residu utilise pour la convergence
        // soit le residu reel ||Ax-b|| et non le residu preconditionne pour certains solveurs
        // avec un preconditionnement a gauche (ex: GMRES). Ainsi, on peut comparer strictement
        // les performances des solveurs (TRUST ou PETSC) entre eux
        if (gmres_right_unpreconditionned)
          {
            KSPSetPCSide(SolveurPetsc_, PC_RIGHT);
            KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
          }
        solver_supported_on_gpu_by_petsc=1;
        solver_supported_on_gpu_by_amgx=1;
        if (amgx_)
          {
            add_amgx_option("solver(s)","GMRES"); // GMRES
            Process::exit("Gmres solver on GPU with AmgX fails to return a valid solution. Try GCP, BiCGSTAB or FGMRES solvers.");
          }
        break;
      }
    case 19:
      {
        KSPSetType(SolveurPetsc_, KSPFGMRES);
        KSPSetPCSide(SolveurPetsc_, PC_RIGHT);
        KSPSetNormType(SolveurPetsc_, KSP_NORM_UNPRECONDITIONED);
        solver_supported_on_gpu_by_petsc=1;
        solver_supported_on_gpu_by_amgx=1;
        add_amgx_option("solver(s)","FGMRES"); // FGMRES
        break;
      }
    case 8:
      {
        KSPSetType(SolveurPetsc_, KSPPGMRES);
        // PGMRES ne peut etre que preconditionne a gauche (CAx=Cb)
        // et on ne peut avoir que le residu preconditionne (||CAx-Cb||)
        // -> on ne peut comparer la convergence avec le GMRES...
        KSPSetPCSide(SolveurPetsc_, PC_LEFT);
        // KSPSetNormType(SolveurPetsc, KSP_NORM_UNPRECONDITIONED);
        solver_supported_on_gpu_by_petsc=1;
        break;
      }
    case 3:
    case 4:
    case 9:
    case 16:
      {
        // Si MUMPS est present, on le prend par defaut (solveur_direct_=1) sinon SuperLU (solveur_direct_=2):
#ifdef PETSC_HAVE_MUMPS
        solveur_direct_ = mumps;
        // Option out_of_core
        if (rang == 4) add_option("mat_mumps_icntl_22", "1");

        // Option BLR
        if (rang == 16)
          {
            Cerr
                << "Activating BLR factorization. For more info, see http://mumps.enseeiht.fr/doc/userguide_5.1.2.pdf (page 18, 51, 52)."
                << finl;
            add_option("mat_mumps_icntl_35", "1");
          }
#else
        solveur_direct_=superlu_dist;
#endif
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 20:
      {
        // Strumpack faster than MUMPS on CPU sometimes (LU and solve, ex: JEL_bous)
        solveur_direct_ = strumpack;
        // ToDo add BLR option
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        solver_supported_on_gpu_by_petsc=1;
        if (gpu_)
          {
            // Triangular solve by default on GPU but limited to 1 MPI rank
            add_option("mat_strumpack_gpu", "1");
            add_option("mat_strumpack_metis_nodeNDP", "1"); // See https://github.com/pghysels/STRUMPACK/issues/127
          }
        break;
      }
    case 5:
      {
        KSPSetType(SolveurPetsc_, KSPBCGS);
        solver_supported_on_gpu_by_petsc=1;
        solver_supported_on_gpu_by_amgx=1;
        // BICGSTAB // BICGSTAB sans preconditionnement
        add_amgx_option("solver(s)","PBICGSTAB"); // BICGSTAB avec precondtionnement
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
        solveur_direct_=superlu_dist;
        // SuperLU_dist, parallel but not faster than MUMPS
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        // ToDo: Update again SuperLU_dist cause GPU Triangular solver available for L (soon U)
        // But slower on GPU than Strumpack during factorization...
        solver_supported_on_gpu_by_petsc=1;
        //if (gpu_) add_option("XXX", "1");
        break;
      }
    case 11:
      {
        if (Process::is_parallel()) Process::exit("Cholesky_lapack can't be used for parallel calculation.");
        solveur_direct_=petsc;
        // Lapack, old and slow (non pas vrai sur petites matrices d'ordre 100 - 10000 !)
        add_option("pc_factor_nonzeros_along_diagonal", ""); // Moins robuste que MUMPS pour un pivot nul donc on reordonne pour eviter
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 12:
      {
        if (Process::is_parallel()) Process::exit("Cholesky_umfpack can't be used for parallel calculation.");
        solveur_direct_=umfpack;
        // Umfpack, sequential only but fast...
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        //more robustness
        add_option("mat_umfpack_pivot_tolerance","1.0");
        break;
      }
    case 13:
      {
        solveur_direct_=pastix;
        // Pastix supports sbaij but seems slow...
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    case 17:
      {
        if (Process::is_parallel()) Process::exit("Cholesky_cholmod can't be used for parallel calculation.");
        solveur_direct_=cholmod;
        // Cholmod Cholesky (pas LU), sequentiel, supporte multi-GPU
        if (!matrice_symetrique_)
          {
            Cerr << ksp << " is only supported for symmetric linear system." << finl;
            Process::exit();
          }
        solver_supported_on_gpu_by_petsc=1;
        KSPSetType(SolveurPetsc_, KSPPREONLY);
        break;
      }
    default:
      {
        Cerr << ksp << " : solver not officially recognized by TRUST among those possible for the moment:" << finl;
        Cerr << les_solveurs << finl;
        Cerr << "You can try to access directly to Petsc solvers with the command line:" << finl;
        Cerr << "PETSC CLI { -ksp_type solver_name -pc_type preconditioning_name -ksp_atol threshold -ksp_monitor }" << finl;
        Cerr << "See the reference manual for all Petsc options." << finl;
        Process::exit();
      }
    }

  // On verifie que le solveur est supporte sur GPU:
  if (gpu_)
    {
#if defined(PETSC_HAVE_CUDA) || defined(PETSC_HAVE_HIP)
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
  if (amgx_)
    {
#ifdef TRUST_USE_CUDA
      Cerr << "GPU capabilities of AmgX will be used." << finl;
#else
      Cerr << "You can not use amgx keyword cause TRUST version is not build with CUDA support." << finl;
      Process::exit();
#endif
      if (solver_supported_on_gpu_by_amgx==0)
        {
          Cerr << les_solveurs[rang] << " is not supported yet by AmgX on GPU." << finl;
          Process::exit();
        }
    }

  int convergence_with_seuil=0; // Keyword to check the convergence via seuil or nb_it_max
  // On continue a lire
  if (motlu==accolade_ouverte)
    {
      // Temporaire essayer de faire converger les noms de parametres des differentes solveurs (GCP, GMRES,...)
      Motcles les_parametres_solveur(31);
      {
        les_parametres_solveur[0] = "impr";
        les_parametres_solveur[1] = "seuil"; // Seuil absolu (atol)
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
        les_parametres_solveur[17] = "restart";
        les_parametres_solveur[18] = "cli_verbose";
        les_parametres_solveur[19] = "dropping_parameter";
        les_parametres_solveur[20] = "rtol"; // Seuil relatif
        les_parametres_solveur[21] = "atol"; // Seuil absolu <=> seuil
        les_parametres_solveur[22] = "ignore_new_nonzero";
        les_parametres_solveur[23] = "rebuild_matrix";
        les_parametres_solveur[24] = "allow_realloc";
        les_parametres_solveur[25] = "clean_matrix";
        les_parametres_solveur[26] = "save_matrix_mtx_format";
        les_parametres_solveur[27] = "reuse_preconditioner_nb_it_max";
        les_parametres_solveur[28] = "reduce_ram";
        les_parametres_solveur[29] = "reorder_matrix";
        les_parametres_solveur[30] = "pcshell"; // user defined preconditionner
      }
      option_double omega("omega",amgx_ ? 0.9 : 1.5);
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
                if (solveur_direct_==mumps)
                  add_option("mat_mumps_icntl_4","3");
                else if (solveur_direct_==superlu_dist)
                  add_option("mat_superlu_dist_printstat","");
                else if (solveur_direct_==petsc)
                  {}
                else if (solveur_direct_==umfpack)
                  add_option("mat_umfpack_prl","2");
                else if (solveur_direct_==pastix)
                  add_option("mat_pastix_verbose","2");
                else if (solveur_direct_==cholmod)
                  add_option("mat_cholmod_print","3");
                else if (solveur_direct_==strumpack)
                  add_option("mat_strumpack_verbose", "1");
                else if (solveur_direct_)
                  {
                    Cerr << "impr not coded yet for this direct solver." << finl;
                    Process::exit();
                  }
                break;
              }
            case 1:
            case 21:
              {
                if (solveur_direct_)
                  {
                    Cerr << "Definition of " << les_parametres_solveur[les_parametres_solveur.search(motlu)] << " is useless for a direct method." << finl;
                    Cerr << "Suppress the keyword." << finl;
                    exit();
                  }
                is >> seuil_;
                convergence_with_seuil=1;
                add_amgx_option("s:convergence","ABSOLUTE");
                add_amgx_option("s:tolerance",Nom(seuil_,"%e"));
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
                          add_amgx_option("p:ilu_sparsity_level",(Nom)level.value());
                          // Coloring level:  1 par defaut  Doit valoir ilu_sparsity_level+1 pour MULTICOLOT_INU (voir AmgX reference guide)
                          add_amgx_option("p:coloring_level",(Nom)Nom(level.value()+1));
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
                          if (amgx_)
                            {
                              Cerr << "Reading option: " << motlu << finl;
                              add_amgx_option(motlu);
                              break;
                            }
                          else
                            {
                              Cerr << motlu
                                   << " : unrecognized option among all of those possible on Petsc preconditioner:"
                                   << finl;
                              Cerr << les_parametres_precond << finl;
                              Process::exit();
                            }
                        }
                      }
                    is >> motlu;
                  }
                break;
              }
            case 30:
              {
                pc="pcshell";
                OWN_PTR(PCShell_base)& pcs = pc_user_.pc_shell;
                is >> motlu;
                pcs.typer(motlu);
                is >> pcs.valeur();
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
                add_amgx_option("s:max_iters",Nom(nb_it_max_));
                break;
              }
            case 15:
              {
                save_matrice_=1;
                break;
              }
            case 5:
              {
                // on sauvegarde au format petsc
                save_matrix_=1;
                break;
              }
            case 26:
              {
                // on sauvegarde au format matrix market
                save_matrix_=2;
                break;
              }
            case 6:
              {
                if (Process::is_parallel())
                  {
                    Cerr << "factored_matrix option is not available for parallel calculation." << finl;
                    exit();
                  }
                if (solveur_direct_!=petsc)
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
                    // adding a test to support negative value
                    bool negative_value = is_number(valeur.getSuffix("-").getString());
                    if ((valeur.debute_par("-") && !negative_value) || valeur==accolade_fermee)
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
                if (limpr()>-1)
                  fixer_limpr(1); // On imprime le residu si CLI
                // Pour faciliter le debugage:
                if (rang == 18) // Verbose
                  {

                    add_option("ksp_view", "");
                    add_option("options_view", "");
                    add_option("options_left", "");
                  }
                break;
              }
            case 10:
              {
                is >> motlu;
                // Si pas MUMPS on previent
                if (solveur_direct_!=mumps)
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
                    if (Process::is_sequential())
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
                different_partition_ = petsc_decide_; // If Petsc decides the matrix partition, the partition is often different than the TRUST partition
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
            case 17:
              {
                KSPType type_ksp_method;
                KSPGetType(SolveurPetsc_, &type_ksp_method);
                if ((Nom)type_ksp_method != Nom("gmres") && ((Nom)type_ksp_method != Nom("fgmres")))
                  {
                    Cerr << "restart option is available only with [f]gmres methods" << finl;
                    exit();
                  }
                int restart_gmres;
                is >> restart_gmres;
                KSPGMRESSetRestart(SolveurPetsc_,restart_gmres);
                break;
              }
            case 19:
              {
                // Si pas MUMPS on previent
                if (solveur_direct_!=mumps)
                  {
                    Cerr << les_parametres_solveur[rang] << " keyword for a solver is limited to " << les_solveurs[14] << " only." << finl;
                    Process::exit();
                  }
                double dropping_parameter;
                is >> dropping_parameter;
                add_option("mat_mumps_cntl_7",dropping_parameter);    // Dropping parameter
                break;
              }
            case 20:
              {
                if (solveur_direct_)
                  {
                    Cerr << "Definition of " << les_parametres_solveur[les_parametres_solveur.search(motlu)] << " is useless for a direct method." << finl;
                    Cerr << "Suppress the keyword." << finl;
                    exit();
                  }
                is >> seuil_relatif_;
                convergence_with_seuil=1;
                add_amgx_option("s:convergence","RELATIVE_INI_CORE");
                add_amgx_option("s:tolerance",Nom(seuil_relatif_,"%e"));
                break;
              }
            case 22:
              int flag;
              is >> flag;
              ignore_new_nonzero_ = (bool)flag;
              break;
            case 23:
              is >> flag;
              rebuild_matrix_ = (bool)flag;
              break;
            case 24:
              is >> flag;
              allow_realloc_ = (bool)flag;
              break;
            case 25:
              is >> flag;
              clean_matrix_ = (bool)flag;
              break;
            case 27:
              set_reuse_preconditioner(true);
              is >> reuse_preconditioner_nb_it_max_;
              break;
            case 28:
              reduce_ram_ = true;
              break;
            case 29:
              is >> flag;
              reorder_matrix_ = (bool)flag;
              break;
            default:
              {
                if (amgx_)
                  {
                    Cerr << "Reading option: " << motlu << finl;
                    add_amgx_option(motlu);
                  }
                else
                  {
                    Cerr << motlu << " : unrecognized option from those available in the Petsc solver:" << finl;
                    Cerr << les_parametres_solveur << finl;
                    Process::exit();
                  }
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
      int pc_supported_on_gpu_by_amgx=0;
      Motcles les_precond(18);
      {
        les_precond[0] = "NULL";               // Pas de preconditionnement
        les_precond[1] = "ILU";                // Incomplete LU
        les_precond[2] = "SSOR";               // Symetric Successive Over Relaxation
        les_precond[3] = "EISENSTAT";          // Symetric Successive Over Relaxation avec Eiseinstat trick
        les_precond[4] = "SPAI";               // Sparse Approximate Inverse
        les_precond[5] = "PILUT";              // Dual-threshold incomplete LU factorisation
        les_precond[6] = "DIAG|JACOBI";        // Diagonal (Jacobi) precondtioner
        les_precond[7] = "BOOMERAMG";          // Multigrid preconditioner
        les_precond[8] = "BLOCK_JACOBI_ICC";   // Block Jacobi ICC preconditioner (code dans PETSc, optimise)
        les_precond[9] = "BLOCK_JACOBI_ILU";   // Block Jacobi ILU preconditioner (code dans PETSc, optimise)
        les_precond[10] = "C-AMG";    // Classical AMG
        les_precond[11] = "SA-AMG";   // Smooth Aggregated AMG
        les_precond[12] = "GS";   // Gauss-Seidel
        les_precond[13] = "PCSHELL"; // user defined preconditionner
        les_precond[14] = "LU|MUMPS";   // MUMPS LU
        les_precond[15] = "UA-AMG";   // Unsmoothed Aggregated AMG
        les_precond[16] = "ILU_MUMPS";      // Incomplete ILU with a Block Low Ranking from MUMPS
        les_precond[17] = "ILU_STRUMPACK";  // Incomplete ILU with a Block Low Ranking from STRUMPACK
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
                pc_supported_on_gpu_by_amgx=1;
                add_amgx_option("s:preconditioner(p)","NOSOLVER");
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 1:
              {
                //Cout << "See http://www.ncsa.uiuc.edu/UserInfo/Resources/Software/Math/HYPRE/docs-1.6.0/HYPRE_usr_manual/node33.html" << finl;
                //Cout << "to have some advices on the incomplete LU factorisation level: ILU(level)" << finl;
                // On n'attaque pas le ILU de Petsc qui n'est pas parallele
                // On prend celui de Hypre (Euclid=PILU(k)) en passant par les commandes en ligne
                // car peu de parametres peuvent etre fixes sinon
                //PCSetType(PreconditionneurPetsc_, PCHYPRE);
                //PCHYPRESetType(PreconditionneurPetsc_, "euclid");
                //add_option("pc_hypre_euclid_levels",(Nom)level.value());
                //check_not_defined(omega);
                //check_not_defined(epsilon);
                //check_not_defined(ordering);
                //
                // CHANGES in the PETSc 3.6 version: Removed -pc_hypre_type euclid due to bit-rot
                pc_supported_on_gpu_by_amgx=1;
                if (amgx_)
                  add_amgx_option("s:preconditioner(p)","MULTICOLOR_DILU");
                else if (gpu_)
                  {
                    add_option("pc_type","ilu");
                    add_option("pc_factor_mat_solver_type","cusparse");
                    add_option("pc_factor_levels",(Nom)level.value());
                  }
                else
                  {
                    Cerr << "Error: CHANGES in the PETSc 3.6 version: Removed -pc_hypre_type euclid due to bit-rot."
                         << finl;
                    Cerr << "So the ILU { level k } preconditioner no longer available. " << finl;
                    Cerr << "Change your data file." << finl;
                    Process::exit();
                  }
                break;
              }
            case 2:
              {
                PCSetType(PreconditionneurPetsc_, PCSOR);
                if (amgx_) Process::exit("SSOR is not available on GPU, try GS (Gauss Seidel)");
                if (omega.value()>=1. && omega.value()<=2.)
                  {
                    PCSORSetOmega(PreconditionneurPetsc_, omega.value());
                  }
                else
                  {
                    Cerr << "omega value for SSOR should be between 1 and 2" << finl;
                    exit();
                  }
                pc_supported_on_gpu_by_amgx=0;
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
                add_option("pc_hypre_parasails_nlevels",(Nom)level.value());     // Higher values of level [>=0] leads to more accurate, but more expensive preconditioners (default 1)
                add_option("pc_hypre_parasails_thresh",(Nom)epsilon.value());   // Lower values of eps [0-1] leads to more accurate, but more expensive preconditioners (default 0.1)
                //add_option("pc_hypre_parasails_sym","SPD"); // Matrice symetrique definie positive. PL: comment cela a pu marcher avant ? La matrice n'est pas toujours symetrique.
                check_not_defined(omega);
                check_not_defined(ordering);
#ifdef INT_is_64_
                KSPType type_ksp;
                KSPGetType(SolveurPetsc_, &type_ksp);
                if ((Nom)type_ksp==KSPCG) Process::exit("SPAI preconditioner is not supported anymore. If you want to keep this precond, try using BICGSTAB solver insted of GCP.");
#endif
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
                //
                // ERROR VALGRIND
                //Cerr << "Error VALGRIND with PETSc Dual Threashold Incomplete LU factorization." << finl;
                //Cerr << "So the PILUT { level k epsilon thresh } preconditioner no longer available. " << finl;
                //Cerr << "Change your data file." << finl;
                //Process::exit();
                break;
              }
            case 6:
              {
                PCSetType(PreconditionneurPetsc_, PCJACOBI);
                pc_supported_on_gpu_by_petsc=1;
                pc_supported_on_gpu_by_amgx=1;
                if (amgx_)
                  {
                    add_amgx_option("s:preconditioner(p)","BLOCK_JACOBI");
                    Process::exit("Diagonal preconditioner on GPU with AmgX is slow to converge. Try GS (Gauss-Seidel) preconditioner.");
                  }
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 9: // ilu
            case 8: // icc
              {
                if (rang==9) preconditionnement_non_symetrique_ = 1;
                pc_supported_on_gpu_by_amgx=1;
                pc_supported_on_gpu_by_petsc=1;
                if (amgx_)
                  add_amgx_option("s:preconditioner(p)","MULTICOLOR_DILU"); // MULTICOLOR_ILU plante...
                else
                  {
                    add_option("sub_pc_type",rang==8 ? "icc" : "ilu");
                    add_option("sub_pc_factor_levels",(Nom)level.value());
                    // On fixe le precondtionnement non symetrique pour appliquer eventuellement un ordering autre que celui par defaut (natural)
                    // Un ordering rcm peut ameliorer par exemple la convergence
                    // Voir le remplissage de la matrice avec -mat_view_draw -draw_pause -1
                    if (ordering.value()!="")
                      {
                        add_option("sub_pc_factor_mat_ordering_type",ordering.value());
                        // Le preconditionnement natural (defaut) ne necessite pas une matrice de preconditionnement symetrique, les autres si:
                        preconditionnement_non_symetrique_=1;
                      }

                  }
                PCSetType(PreconditionneurPetsc_, PCBJACOBI);
                check_not_defined(omega);
                check_not_defined(epsilon);
                break;
              }
            case 14:
              {
                // LU|MUMPS { ordering XXX }
                //preconditionnement_non_symetrique_ = 1;
                add_option("sub_pc_type", "lu");
                add_option("sub_pc_factor_mat_solver_type","mumps");
                if(limpr()) add_option("mat_mumps_icntl_4","3");
                if (ordering.value()!="")
                  add_option("sub_pc_factor_mat_ordering_type",ordering.value());
                PCSetType(PreconditionneurPetsc_, PCBJACOBI);
                check_not_defined(omega);
                check_not_defined(epsilon);
                break;
              }
            case 7:
              {
#ifdef HYPRE_USING_GPU
                // GPU build of Hypre provides only gpu preconditionner now. No runtime switch to CPU or GPU versions yet...
                gpu_ = 1;
#endif
                PCSetType(PreconditionneurPetsc_, PCHYPRE);
                PCHYPRESetType(PreconditionneurPetsc_, "boomeramg"); // Classical C-AMG
                pc_supported_on_gpu_by_petsc=1;
                // Changement pc_hypre_boomeramg_relax_type_all pour PETSc 3.10, la matrice de
                // preconditionnement etant seqaij, symetric-SOR/jacobi (defaut) provoque KSP_DIVERGED_INDEFINITE_PC
                // Voir: https://lists.mcs.anl.gov/mailman/htdig/petsc-users/2012-December/015922.html
                if (!gpu_) add_option("pc_hypre_boomeramg_relax_type_all", "Jacobi");
                // Voir https://mooseframework.inl.gov/releases/moose/2021-05-18/application_development/hypre.html
                //if (dimension==3) Cerr << "Warning, on massive parallel calculation for best performance, consider playing with -pc_hypre_boomeramg_strong_threshold 0.7 or 0.8 or 0.9" << finl;
                if (dimension==3) add_option("pc_hypre_boomeramg_strong_threshold", "0.7");
                if (limpr()) add_option("pc_hypre_boomeramg_print_statistics","1");
                check_not_defined(omega);
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                // Page 16: https://prace-ri.eu/wp-content/uploads/WP294-PETSc4FOAM-A-Library-to-plug-in-PETSc-into-the-OpenFOAM-Framework.pdf
                /*
                add_option("pc_hypre_boomeramg_max_iter","1");
                add_option("pc_hypre_boomeramg_strong_threshold","0.7");
                add_option("pc_hypre_boomeramg_grid_sweeps_up","1");
                add_option("pc_hypre_boomeramg_grid_sweeps_down","1");
                add_option("pc_hypre_boomeramg_agg_nl","2");
                add_option("pc_hypre_boomeramg_agg_num_paths","1");
                add_option("pc_hypre_boomeramg_max_levels","25");
                add_option("pc_hypre_boomeramg_coarsen_type","HMIS");
                add_option("pc_hypre_boomeramg_interp_type","ext+i");
                add_option("pc_hypre_boomeramg_P_max","2");
                add_option("pc_hypre_boomeramg_truncfactor","0.2");*/
                break;
              }
            case 10: // Classical AMG
            case 11: // Smooth Aggregated AMG
            case 15: // Unsmoothed Aggregated AMG
              {
                pc_supported_on_gpu_by_amgx=1;
                pc_supported_on_gpu_by_petsc=1;
                preconditionnement_non_symetrique_ = 1;
                if (amgx_)
                  {
                    add_amgx_option("s:preconditioner(p)","AMG");
                    add_amgx_option("s:use_scalar_norm","1");
                    add_amgx_option("p:error_scaling","0");
                    add_amgx_option("p:print_grid_stats","1");
                    add_amgx_option("p:max_iters","1");
                    add_amgx_option("p:cycle","V");
                    add_amgx_option("p:min_coarse_rows","2");
                    add_amgx_option("p:max_levels","100");
                    add_amgx_option("p:smoother(smoother)","BLOCK_JACOBI");
                    add_amgx_option("p:presweeps","1");
                    add_amgx_option("p:postsweeps","1");
                    add_amgx_option("p:coarsest_sweeps","1");
                    add_amgx_option("p:coarse_solver","DENSE_LU_SOLVER");
                    add_amgx_option("p:dense_lu_num_rows","2");
                    if (rang==10) // C-AMG
                      {
                        add_amgx_option("p:algorithm","CLASSICAL","Best choice if you have enough memory and fine tune carefully p:selector and p:strength parameters.");
                        //add_amgx_option("p:selector","PMIS","PMIS selector seems to take less memory, and much faster setup than HMIS."); // Ne permet pas autre chose que p:strength_threshold=0.25 sur le maillage 220e6 de DomainFlowLES !
                        add_amgx_option("p:selector","HMIS","PMIS may offer faster setup but weaker (some issues for a high number of GPUs) than serial implementation!");
                        add_amgx_option("p:interpolator","D2","Also available D1. D2 is considerably more expensive during both setup and solve phases, but convergence on difficult problems");
                        Nom strength("AHAT");
                        add_amgx_option("p:strength",strength,"Choose the strength of connection metric to use. Allowable options are AHAT and ALL");
                        //if (strength=="AHAT") add_amgx_option("p:strength_threshold","0.25","All edges with strength below this threshold will be discarded. Higher: faster setup, lower memory but lower convergence");
                        // Change 0.25 to 0.8 as Boomeramg: in all cases, less memory and faster times globally
                        if (strength=="AHAT") add_amgx_option("p:strength_threshold","0.8","All edges with strength below this threshold will be discarded. Higher: faster setup, lower memory but lower convergence. You may try 0.25 for better convergence if enough memory.");
                      }
                    else if (rang==11) // SA-AMG
                      {
                        add_amgx_option("p:algorithm","AGGREGATION");
                        add_amgx_option("p:selector","SIZE_2");
                        add_amgx_option("p:max_matching_iterations","100000");
                        add_amgx_option("p:max_unassigned_percentage","0.0");
                      }
                    else
                      {
                        Process::exit("Not supported for AmgX");
                      }
                    add_amgx_option("smoother:relaxation_factor","0.8");
                  }
                else
                  {
                    // ToDo : trouver des parametres pour PETSc afin d'avoir une comparaison possible CPU vs GPU (meme its par exemple):
                    add_option("pc_type","gamg");
                    // Ajout pour retrouver la convergence de PETSc 3.14:
                    //add_option("mg_levels_pc_type","sor");
                    //add_option("pc_gamg_threshold","0.");
                    if (rang==10) // C-AMG
                      {
                        // Convergence fortement degradee 3.14 -> 3.20 malgre les options precedentes...
                        add_option("pc_gamg_type","classical");
                      }
                    else if (rang==11) // SA-AMG
                      {
                        add_option("pc_gamg_type","agg");
                        add_option("pc_gamg_agg_nsmooths","1");
                        // Non performances catastrophiques:
                        //add_option("pc_gamg_threshold","0.7"); // Fix in parallel:  Computed maximum singular value as zero
                        //add_option("pc_gamg_aggressive_square_graph","1");
                      }
                    else if (rang==15) // UA-AMG
                      {
                        add_option("pc_gamg_type","agg");
                        add_option("pc_gamg_agg_nsmooths","0");
                        //add_option("pc_gamg_aggressive_square_graph","1");
                      }
                    else
                      {
                        Process::exit("Usupported precond for PETSc.");
                      }
                  }
                break;
              }
            case 12:
              {
                if (!amgx_) Process::exit("GS (Gauss-Seidel) not available yet.");
                if (omega.value()<0 || omega.value()>2)
                  {
                    Cerr << "Relaxation value omega for GS should be between 0 and 2" << finl;
                    exit();
                  }
                pc_supported_on_gpu_by_amgx=1;
                if (amgx_)
                  {
                    add_amgx_option("s:preconditioner(p)","GS"); // Non documente...
                    //add_amgx_option("s:preconditioner(p)","MULTICOLOR_GS"); // MULTICOLOR_GS lent dans AmgX ?
                    add_amgx_option("p:relaxation_factor", Nom(omega.value())); // Defaut 0.9
                    if (matrice_symetrique_) add_amgx_option("p:symmetric_GS","1");
                  }
                check_not_defined(level);
                check_not_defined(epsilon);
                check_not_defined(ordering);
                break;
              }
            case 13:
              {

                PCSetType(PreconditionneurPetsc_, PCSHELL);
                //PetscNew(&pc_user_);

                auto PCShellUserApply =  [](PC pc_apply, Vec x, Vec y)
                {
                  PCstruct *pcstruct;

                  PCShellGetContext(pc_apply,(void**) &pcstruct);
                  OWN_PTR(PCShell_base)& pcs=pcstruct->pc_shell;
                  return pcs->computePC(pc_apply,x,y);
                };

                auto PCShellUserDestroy =  [](PC pc_apply)
                {
                  PCstruct *pcstruct;

                  PCShellGetContext(pc_apply,(void**) &pcstruct);
                  OWN_PTR(PCShell_base)& pcs=pcstruct->pc_shell;
                  return pcs->destroyPC(pc_apply);
                };

                PCShellSetApply(PreconditionneurPetsc_, PCShellUserApply);
                PCShellSetContext(PreconditionneurPetsc_, &pc_user_);
                PCShellSetDestroy(PreconditionneurPetsc_, PCShellUserDestroy);

                break;
              }
            case 16:
              {
                // ILU_MUMPS:
                add_option("pc_type", "cholesky"); // Attention, si on met LU en sequentiel il n'utilise pas MUMPS...
                add_option("pc_factor_mat_solver_type", "mumps");
                add_option("mat_mumps_icntl_35", "1"); // BLR enabled
                //add_option("mat_mumps_icntl_36", "??"); controls the choice of BLR factorization variant
                //add_option("mat_mumps_icntl_38", "??"); sets the estimated compression rate of LU factors with BLR
                add_option("mat_mumps_cntl_7", (double)epsilon.value()); // Droping parameter
                //if (limpr()) add_option("mat_mumps_icntl_4","3"); // verobose
                check_not_defined(level);
                break;
              }
            case 17:
              {
                // ILU_STRUMPACK:
                add_option("pc_type", "ilu");
                add_option("mat_type", "aij");
                add_option("pc_factor_mat_solver_type", "strumpack");
                add_option("mat_strumpack_compression","BLR"); // Type of rank-structured compression in sparse LU factors (choose one of) NONE HSS BLR HODLR BLR_HODLR ZFP_BLR_HODLR LOSSLESS LOSSY
                add_option("mat_strumpack_compression_rel_tol", (double)epsilon.value()); // Compression parameter
                //add_option("mat_strumpack_compression_abs_tol", (Nom)epsilon.value()); // Compression parameter
                //add_option("mat_strumpack_compression_lossy_precision,"1-64"); // Precision when using lossy compression [1-64],
                //if (limpr()) add_option("mat_strumpack_verbose", "1"); // Provisoire
                check_not_defined(level);
                break;
              }
            default:
              {
                Cerr << pc << " : preconditioner not officially recognized by TRUST among those possible for the moment:" << finl;
                Cerr << les_precond << finl;
                Cerr << "You can try to access directly to Petsc preconditioners with the command line." << finl;
                Cerr << "See the reference manual of Petsc to do this." << finl;
                Process::exit();
              }
            }
        }
      else
        {
          if (!solveur_direct_)
            {
              Cerr << "You forgot to define a preconditioner with the keyword precond." << finl;
              Cerr << "If you don't want a preconditioner, add for the solver definition:" << finl;
              Cerr << "precond null" << finl;
              Process::exit();
            }
          else
            {
              // Pour un solveur direct le preconditionner EST le solveur:
              pc_supported_on_gpu_by_petsc = solver_supported_on_gpu_by_petsc;
              pc_supported_on_gpu_by_amgx = solver_supported_on_gpu_by_amgx;
            }
        }
// On verifie que les preconditionneurs sont supportes sur GPU:
      if (gpu_ && pc_supported_on_gpu_by_petsc==0)
        {
          Cerr << les_precond[rang] << " is not supported yet by PETSc on GPU." << finl;
          Process::exit();
        }
      if (amgx_ && pc_supported_on_gpu_by_amgx==0)
        {
          Cerr << les_precond[rang] << " is not supported yet by AmgX on GPU." << finl;
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
      // Convergence si residu(it) < MAX (seuil_relatif_ * residu(0), seuil_);
      if (seuil_==0 && seuil_relatif_==0)
        {
          seuil_=1.e-12; // Si aucun seuil defini, on prend un seuil absolu de 1.e-12 (comme avant)
          seuil_relatif_=1.e-14; // To avoid over convergence, typically with: petsc cli { } where no tolerance are given
        }
      KSPSetTolerances(SolveurPetsc_, seuil_relatif_, seuil_, (divtol_==0 ? PETSC_DEFAULT : divtol_), nb_it_max_);
    }
// Change le calcul du test de convergence relative (||Ax-b||/||Ax(0)-b|| au lieu de ||Ax-b||/||b||)
// Peu utilisee dans TRUST car on utilise la convergence sur la norme absolue
// Mais cela corrige une erreur KSP_DIVERGED_DTOL quand ||Ax-b||/||b||>10000=div_tol par defaut dans PETSc (rencontree sur Etude REV_4)
//add_option(sys, "ksp_converged_use_initial_residual_norm",1); // Before PETSc 3.5
  KSPConvergedDefaultSetUIRNorm(SolveurPetsc_); // After PETSc 3.5, a function is available

// Surcharge eventuelle par la ligne de commande
  KSPSetOptionsPrefix(SolveurPetsc_, option_prefix_);
  KSPSetFromOptions(SolveurPetsc_);
  PCSetOptionsPrefix(PreconditionneurPetsc_, option_prefix_);
  PCSetFromOptions(PreconditionneurPetsc_);

// Setting the names:
  KSPType type_ksp;
  KSPGetType(SolveurPetsc_, &type_ksp);
  type_ksp_=(Nom)type_ksp;

  PCType type_pc;
  PCGetType(PreconditionneurPetsc_, &type_pc);
  if (type_pc) type_pc_=(Nom)type_pc;

  // Pas de version CPU de Hypre si PETSc active le support GPU:
#if defined(PETSC_HAVE_CUDA) || defined(PETSC_HAVE_HIP)
  if (type_pc_=="hypre") gpu_ = true;
#endif

// Creation du fichier de config .amgx (NB: les objets PETSc sont crees mais ne seront pas utilises)
  if (amgx_ && Process::je_suis_maitre())
    {
      SFichier s(config());
      // Syntax: See https://github.com/NVIDIA/AMGX/raw/master/doc/AMGX_Reference.pdf
      s << "# AmgX config file" << finl << "config_version=2" << finl;
      add_amgx_option("s:print_config",      limpr() ? "1" : "0");
      add_amgx_option("s:print_solve_stats", limpr() ? "1" : "0");
      add_amgx_option("s:obtain_timings",    limpr() ? "1" : "0");
      add_amgx_option("s:store_res_history","1");
      add_amgx_option("s:monitor_residual","1");
      add_amgx_option("s:max_iters","10000"); // 100 par defaut trop bas...
      if (Process::nproc()<=4)
        add_amgx_option("determinism_flag","1", "15% slower but enabled for NR tests");
#ifdef MPIX_CUDA_AWARE_SUPPORT
      if (getenv("AMGX_USE_MPI_GPU_AWARE")) add_amgx_option("communicator","MPI_DIRECT","Enable GPU direct with MPI Cuda-Aware. No gain for the moment.");
#endif
      s << amgx_options_;
      Cerr << "Writing the AmgX config file: " << config() << finl;
    }
#else
  Cerr << "Error, the code is not built with PETSc support." << finl;
  Cerr << "Contact TRUST support." << finl;
  Process::exit();
#endif

}
const Nom Solv_Petsc::config()
{
  Nom str(Objet_U::nom_du_cas());
#ifdef PETSCKSP_H
  str+=option_prefix_.prefix("_");
#else
  Process::exit("ToDo fix Solv_Petsc::config(): build config filename with numero_solve in the constructor!");
#endif
  str+=amgx_ ? ".amgx" : ".petsc";
  return str;
}

int Solv_Petsc::instance=-1;
int Solv_Petsc::numero_solveur=0;
#ifdef PETSCKSP_H
PetscLogStage Solv_Petsc::Create_Stage_=1;
PetscLogStage Solv_Petsc::KSPSolve_Stage_=2;

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

// Save Matrix and RHS in a .petsc or .mtx (matrix market) file:
void Solv_Petsc::SaveObjectsToFile(const DoubleVect& secmem, DoubleVect& solution)
{
  double start = Statistiques::get_time_now();
  if (save_matrix_==1)
    {
      MatInfo Info;
      MatGetInfo(MatricePetsc_,MAT_GLOBAL_SUM,&Info);
      trustIdType nnz = (trustIdType)Info.nz_allocated;

      Nom filename("Matrix_");
      filename+=(Nom)nb_rows_tot_;
      filename+="_rows_";
      filename+=(Nom)nproc();
      filename+="_cpus.petsc";

      Cerr << "Writing the global PETSc matrix (" << nb_rows_tot_<< " rows and " << nnz << " nnz) in the binary file " << filename << finl;
      if (Process::nproc()>32) Cerr << "It may take some minutes..." << finl;
      PetscViewer viewer;
      PetscViewerCreate(PETSC_COMM_WORLD, &viewer);
// HDF5 issue: the file is empty, don't know why
// So trying to use MPIIO to speedup the write/load...
//#ifdef PETSC_HAVE_HDF5
//      PetscViewerSetType(viewer, PETSCVIEWERHDF5);
//#else
      PetscViewerSetType(viewer, PETSCVIEWERBINARY);
      PetscViewerBinarySetUseMPIIO(viewer, PETSC_TRUE);
//#endif
      PetscViewerFileSetMode(viewer, FILE_MODE_WRITE);
      PetscViewerFileSetName(viewer, filename);
      statistiques().begin_count(sauvegarde_counter_);
      trustIdType bytes = 8 * nnz + 4 * nnz + 4 * nb_rows_tot_;
      MatView(MatricePetsc_, viewer);
      statistiques().end_count(sauvegarde_counter_, bytes);
      Cerr << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to write matrix file." << finl;
      // Save also the RHS if on the host:
      if (SecondMembrePetsc_!=nullptr)
        {
          Cerr << "Writing also the RHS in the file " << filename << finl;
          VecView(SecondMembrePetsc_, viewer);
        }
      else
        Process::exit("You can't export anymore matrix&RHS at PETSc format with AmgX solver. Switch to PETSc solver instead.");
      PetscViewerDestroy(&viewer);

      // ASCII output for small matrix(debugging)
      if (nb_rows_tot_<20)
        {
          Cerr << "The global PETSc matrix is small so we also print it:" << finl;
          MatView(MatricePetsc_, PETSC_VIEWER_STDOUT_WORLD);
        }
    }
  else if (save_matrix_==2)
    {
      // Format matrix market ToDo : method
      if (Process::is_parallel()) Process::exit("Error, matrix market format is not available yet in parallel.");
      Nom filename(Objet_U::nom_du_cas());
      filename += "_matrix";
      filename += (Nom) instance;
      filename += ".mtx";
      SFichier mtx(filename);
      mtx.precision(14);
      mtx.setf(ios::scientific);
      PetscInt rows;
      const PetscInt *ia, *ja;
      PetscBool done;
      MatGetRowIJ(MatricePetsc_, 0, PETSC_FALSE, PETSC_FALSE, &rows, &ia, &ja, &done);
      if (!done) Process::exit("Error in MatGetRowIJ");
      PetscScalar *v;
      MatType mat_type;
      MatGetType(MatricePetsc_, &mat_type);
      Nom type;
      if (strcmp(mat_type, MATSEQAIJ) == 0)
        {
          type = "general";
          MatSeqAIJGetArray(MatricePetsc_, &v);
        }
      else if (strcmp(mat_type, MATSEQSBAIJ) == 0)
        {
          type = "symmetric";
          MatSeqSBAIJGetArray(MatricePetsc_, &v);
        }
      else Process::exit("Matrix type not supported.");
      mtx << "%%MatrixMarket matrix coordinate real " << type << finl;
      Cerr << "Matrix (" << (int)rows << " lines) written into file: " << filename << finl;
      mtx << "%%matrix" << finl;
      mtx << (int)rows << " " << (int)rows << " " << (int)ia[rows] << finl;
      for (int row=0; row<rows; row++)
        for (PetscInt j=ia[row]; j<ia[row+1]; j++)
          mtx << row+1 << " " << (int)ja[j]+1 << " " << v[j] << finl;
      // Sauve b au format matrix market
      filename = Objet_U::nom_du_cas();
      filename += "_rhs";
      filename += (Nom) instance;
      filename += ".mtx";
      SFichier rhs_mtx(filename);
      rhs_mtx.precision(14);
      rhs_mtx.setf(ios::scientific);
      rhs_mtx << "%%MatrixMarket matrix array real general" << finl;
      rhs_mtx << secmem.size_array() << " " << secmem.line_size() << finl;
      for (int row=0; row<rows; row++)
        rhs_mtx << secmem(row) << finl;
      // Provisoire: sauve un vector Petsc au format ASCII pour le RHS
      if (SecondMembrePetsc_!=nullptr)
        {
          PetscViewer viewer;
          Nom rhs_filename(Objet_U::nom_du_cas());
          rhs_filename += "_rhs";
          rhs_filename += (Nom) instance;
          rhs_filename += ".petsc";
          PetscViewerASCIIOpen(PETSC_COMM_WORLD,rhs_filename,&viewer);
          PetscViewerPushFormat(viewer, PETSC_VIEWER_ASCII_MATLAB);
          VecView(SecondMembrePetsc_, viewer);
          Cerr << "Save RHS into " << rhs_filename << finl;
          PetscViewerDestroy(&viewer);
        }
    }
  if (verbose) Cout << "[Petsc] Time to write matrix: \t" << Statistiques::get_time_now() - start << finl;
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
  add_option("viewer_binary_skip_info",""); // Skip reading .info file to avoid -vecload_block_size unused option

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

  Cerr << "Reading the global PETSc matrix in the binary file " << filename << finl;
  if (Process::nproc()>32) Cerr << "It may take some minutes..." << finl;
  PetscViewer viewer;
  PetscViewerCreate(PETSC_COMM_WORLD, &viewer);
//#ifdef PETSC_HAVE_HDF5
//  PetscViewerSetType(viewer, PETSCVIEWERHDF5);
//#else
  PetscViewerSetType(viewer, PETSCVIEWERBINARY);
  PetscViewerBinarySetUseMPIIO(viewer, PETSC_TRUE);
//#endif
  PetscViewerFileSetMode(viewer, FILE_MODE_READ);
  PetscViewerFileSetName(viewer, filename);
  statistiques().begin_count(sauvegarde_counter_);
  MatLoad(MatricePetsc_, viewer);
  MatInfo Info;
  MatGetInfo(MatricePetsc_,MAT_GLOBAL_SUM,&Info);
  trustIdType nnz = (trustIdType)Info.nz_allocated;
  trustIdType bytes = 8 * nnz + 4 * nnz + 4 * nb_rows_tot_;
  statistiques().end_count(sauvegarde_counter_, bytes);
  Cerr << "[IO] " << statistiques().last_time(sauvegarde_counter_) << " s to read matrix file." << finl;

  PetscViewerDestroy(&viewer);
  if (!matrice_symetrique_)
    {
      Cerr << "Reading a non symmetric PETSc matrix is not supported yet in TRUST." << finl;
      exit();
    }
  // Conversion AIJ to SBAIJ:
  MatSetOption(MatricePetsc_, MAT_SYMMETRIC, PETSC_TRUE);
#ifdef PETSC_HAVE_CUDA
  if (gpu_)
    MatConvert(MatricePetsc_, MATAIJCUSPARSE, MAT_INPLACE_MATRIX, &MatricePetsc_);
  else
#endif
#ifdef PETSC_HAVE_HIP
    if (gpu_)
      MatConvert(MatricePetsc_, MATAIJHIPSPARSE, MAT_INPLACE_MATRIX, &MatricePetsc_);
    else
#endif
      MatConvert(MatricePetsc_, MATSBAIJ, MAT_INPLACE_MATRIX, &MatricePetsc_);

  PetscInt nb_rows_tot,nb_cols_tot;
  MatGetSize(MatricePetsc_,&nb_rows_tot,&nb_cols_tot);
  Cerr << "The matrix read has " << (int)nb_rows_tot << " rows." << finl;
  PetscInt nb_local_rows, nb_local_cols;
  MatGetLocalSize(MatricePetsc_, &nb_local_rows, &nb_local_cols);
  if (nb_local_rows != nb_items_to_keep_)
    {
      Cerr << "The matrix read has " << (int)nb_local_rows << " local columns whereas" << finl;
      Cerr << "the RHS/Solution vectors have a size of " << nb_items_to_keep_ << "." << finl;
      Cerr << "Check your data file or the file containing the PETSc matrix." << finl;
      exit();
    }
}

// SV
// Since PETSc 3.10 The option ksp_view is not taken into account for (at least) instance = 2
// I don't understand why ??!!
// So I introduce this fix : if the option ksp_view is in PETSc Options then we call the KSPView function
bool Solv_Petsc::enable_ksp_view()
{
  Nom option="-ksp_view";
  Nom empty="                                                                                                 ";
  char *option_value = strdup( empty );
  PetscBool enable; // enable this option ?
  PetscOptionsGetString( PETSC_NULLPTR, PETSC_NULLPTR, option, option_value, empty.longueur( ), &enable );
  //Nom actual_value( option_value );
  free( option_value );
  return enable ;
}

int Solv_Petsc::add_option(const Nom& astring, const double& value, int cli)
{
  char nom_value[80];
  snprintf(nom_value,80, "%e",value);
  return add_option(astring, (Nom)nom_value, cli);
}

void Solv_Petsc::add_amgx_option(const Nom& key, const Nom& value, const std::string& comment)
{
  if (amgx_ && !amgx_options_.contient(key))
    {
      if (comment!="") amgx_options_+="# "+comment+":\n";
      amgx_options_+=key+"="+value+"\n";
    }
}

void Solv_Petsc::add_amgx_option(const Nom& key_value)
{
  if (amgx_)
    {
      std::string key = key_value.getString().substr(0, key_value.getString().find('='));
      if (!amgx_options_.contient(key)) amgx_options_ += key_value + "\n";
    }
}

bool Solv_Petsc::has_option(const Nom& option, Nom& current_value)
{
  PetscBool flg;
  Nom vide="                                                                                                 ";
  char* tmp=strdup(vide);
  PetscOptionsGetString(PETSC_NULLPTR,PETSC_NULLPTR,option,tmp,vide.longueur(),&flg);
  current_value = tmp;
  free(tmp);
  return current_value!=vide;
}

int Solv_Petsc::add_option(const Nom& astring, const Nom& value, int cli)
{
  Nom option="-";
  // Ajout du prefix si l'option concerne KSP, PC, Mat ou Vec:
  if (astring.debute_par("ksp_") ||
      astring.debute_par("sub_ksp_") ||
      astring.debute_par("pc_") ||
      astring.debute_par("sub_pc_") ||
      astring.debute_par("mat_") ||
      astring.debute_par("vec_") ||
      astring.debute_par("mg_") ||
      cli)
    option+=option_prefix_;

  option+=astring;
  // Attention il ne retourne pas de code d'erreur si l'option est mal orthographiee!!
  // Il ne dit pas non plus qu'elle est unused avec -options_left
  // Nouveau 1.6.3 pour la ligne de commande reste prioritaire, on ne change une option
  // que si elle n'a pas deja ete specifiee...
  Nom current_value;
  if (has_option(option, current_value))
    {
      if (limpr() >= 0) Cerr << "Option Petsc: " << option << " " << value << " not taken cause " << option << " already defined to " << current_value << finl;
      return 0;
    }
  else
    {
      if (value=="")
        {
          PetscOptionsSetValue(PETSC_NULLPTR, option, PETSC_NULLPTR);
          if (limpr() >= 0) Cerr << "Option Petsc: " << option << finl;
        }
      else
        {
          PetscOptionsSetValue(PETSC_NULLPTR, option, value);
          if (limpr() >= 0) Cerr << "Option Petsc: " << option << " " << value << finl;
        }
      return 1;
    }
}

#ifndef PETSC_HAVE_HYPRE
// Pour que le code puisse compiler/tourner si on prend PETSc sans aucun autre package externe:
PetscErrorCode PCHYPRESetType(PC,const char[])
{
  Cerr << "HYPRE preconditioners are not available in this TRUST version." << finl;
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
#ifdef PETSCKSP_H
  std::fenv_t fenv;
  std::feholdexcept(&fenv);
  // Si on utilise un solver petsc on le signale pour les stats finales
  statistiques().begin_count(solv_sys_petsc_counter_);
  statistiques().end_count(solv_sys_petsc_counter_,1,1);
  double start = Statistiques::get_time_now();
  // Attention, bug apres PETSc 3.14 le logging avec PetscLogStage est tres cher pour MatSetValues (appel MPI meme en sequentiel!). Vu sur Flica5 avec appel frequents a Update_matrix
  bool log_Create_Stage = false; // ToDO mettre un test plus intelligent selon taille du cas ou si parallele ?
  if (log_Create_Stage) PetscLogStagePush(Create_Stage_);
  if (nouvelle_matrice())
    {
      // Changement de la taille de matrice, on detruit les objets dont la taille change:
      int hasChanged = mp_max((int)(secmem_sz_!=secmem.size_array()));
      if (MatricePetsc_!=nullptr && hasChanged != 0)
        {
          // Destruction de la matrice de preconditionnement:
          KSPSetOperators(SolveurPetsc_, MatricePetsc_, PETSC_NULLPTR);
          // Destruction des vecteurs
          VecDestroy(&SecondMembrePetsc_);
          SecondMembrePetsc_ = nullptr;
          VecDestroy(&SolutionPetsc_);
          SolutionPetsc_ = nullptr;
          if (LocalSolutionPetsc_!=nullptr)
            {
              VecDestroy(&LocalSolutionPetsc_);
              LocalSolutionPetsc_ = nullptr;
              VecScatterDestroy(&VecScatter_);
            }
          // Destruction matrice
          MatDestroy(&MatricePetsc_);
          MatricePetsc_ = nullptr;
          // Destruction DM
          if (dm_!=nullptr)
            DMDestroy(&dm_);
        }

      matrice_symetrique_ = true;      // On suppose que la matrice est symetrique

      // Construction de la numerotation globale:
      if (MatricePetsc_==nullptr)
        construit_renum(secmem);

      // Matrice morse intermedaire de conversion
      Matrice_Morse matrice_morse_intermediaire;
      if (read_matrix_)
        {
          // Read the PETSc matrix
          RestoreMatrixFromFile();
        }
      else if (sub_type(Matrice_Petsc, la_matrice))
        {
          // Matrice deja au format Petsc
          MatricePetsc_ = ref_cast(Matrice_Petsc, la_matrice).getMat();
          read_matrix_ = 1; // flag reutilise comme si on avait lu la matrice
        }
      else
        construit_matrice_morse_intermediaire(la_matrice, matrice_morse_intermediaire);
      if (verbose) Cout << "[Petsc] Time to convert matrix: \t" << Statistiques::get_time_now() - start << finl;

      // Verification stockage de la matrice
      check_aij(matrice_morse_intermediaire);

      bool la_matrice_est_morse_non_symetrique =
        sub_type(Matrice_Morse, la_matrice) && !sub_type(Matrice_Morse_Sym, la_matrice);
      const Matrice_Morse& matrice_morse = la_matrice_est_morse_non_symetrique ? ref_cast(Matrice_Morse, la_matrice)
                                           : matrice_morse_intermediaire;

      // Verification stencil de la matrice
      nouveau_stencil_ = (MatricePetsc_ == nullptr || rebuild_matrix_ || read_matrix_ ? true : check_stencil(matrice_morse));

      // Build x and b if necessary
      Create_vectors(secmem);
      // Creation de Champs (fields) pour pouvoir utiliser des preconditionneurs PCFIELDSPLIT
      Create_DM(secmem);

      // Construit ou update la matrice
      if (nouveau_stencil_)
        Create_objects(matrice_morse, secmem.line_size());
      else
        Update_matrix(MatricePetsc_, matrice_morse);

      /* reglage de BlockSize avec le line_size() du second membre */
      if (limpr() == 1)
        {
          Cout << "Order of the PETSc matrix : " << nb_rows_tot_ << " (~ "
               << (petsc_cpus_selection_ ? (int) (nb_rows_tot_ / petsc_nb_cpus_) : nb_rows_)
               << " unknowns per PETSc process ) " << (nouveau_stencil_ ? "New stencil." : "Same stencil.") << finl;
        }
    }
  // Update PETSc Vec (vectors) for RHS and solution
  Update_vectors(secmem, solution);

  // Save the matrix and the RHS if the matrix has changed...
  if (nouvelle_matrice() && save_matrix_) SaveObjectsToFile(secmem, solution);
  if (log_Create_Stage) PetscLogStagePop();
  //////////////////////////
  // Solve the linear system
  //////////////////////////
  int size_residu = nb_it_max_ + 1;
  //DoubleTrav residu(size_residu); // bad_alloc sur gros cas, curie pourquoi ?
  ArrOfDouble residu(size_residu);
  int nbiter = solve(residu);
  nb_it_previous_ = nbiter;
  if (limpr()>-1)
    {
      double residu_relatif=(residu[0]>0?residu[nbiter]/residu[0]:residu[nbiter]);
      Cout << finl << "Final residue: " << residu[nbiter] << " ( " << residu_relatif << " )"<<finl;
    }
  Update_solution(solution);
  solution.echange_espace_virtuel();
  fixer_nouvelle_matrice(0);
  // Calcul et verification du vrai residu sur matrice:
  bool check_residual = controle_residu_;
#ifndef NDEBUG
  if (amgx_ || gpu_)
    if (getenv("TRUST_CLOCK_ON")==nullptr)
      {
        Cerr << "Warning checking residual. D2H and H2D copies are possible..." << finl;
        check_residual = true; // En debug uniquement car verification faite sur CPU ce qui est dommage en prod...
      }
#endif
  if (check_residual && !MatricePetsc_ /* Matrice_Petsc::ajouter_multvect() not implemented bouh */)
    {
      DoubleVect test(secmem);
      test*=-1;
      la_matrice.ajouter_multvect(solution,test);
      double vrai_residu = mp_norme_vect(test);
      if (verbose) Cout << "||Ax-b||=" << vrai_residu << finl;
      // Verification de la solution sur la matrice initiale
      if (nbiter>0 && Process::je_suis_maitre())
        {
          double precision_machine=1.e-12;
          if (residu[0]>0 && residu[nbiter]/residu[0]>precision_machine && vrai_residu>10*residu[nbiter])
            {
              Cerr << "Error, computed solution x is false !" << finl;
              Cerr << "True residual (" << vrai_residu << ") is much higher than convergence residual (" << residu[nbiter] << ") !" << finl;
              Process::exit();
            }
        }
    }
  if (solution.isDataOnDevice() && !amgx_) mapToDevice(solution);
  std::fesetenv(&fenv);
  return nbiter;
#else
  return -1;
#endif
}

#ifdef PETSCKSP_H
#include <signal.h>

// Function to handle signals
void handleSignal(True_int signum)
{
  if (signum==8)
    {
      Cerr << "SIGFPE from PETSc KSPSolve() !" << finl;
      throw signum;
    }
  else if (signum==11)
    {
      Cerr << "SIGSEGV from PETSc KSPSolve() !" << finl;
      throw signum;
    }
  else
    {
      fprintf(stderr, "Signal %d received from PETSc. Exiting...\n", signum);
      Process::exit();
    }
}
// Function where signal handlers are set up
void setupSignalHandlers(bool on)
{
  // Configure the sigaction structure for SIGFPE
  struct sigaction sa_fpe;
  sa_fpe.sa_handler = on ? handleSignal : SIG_DFL;
  sigemptyset(&sa_fpe.sa_mask);
  sa_fpe.sa_flags = 0;

  // Install the signal handler for SIGFPE
  if (sigaction(SIGFPE, &sa_fpe, nullptr) == -1)
    {
      fprintf(stderr, "Error installing signal handler for SIGFPE.\n");
      Process::exit(EXIT_FAILURE);
    }

  // Configure the sigaction structure for SIGSEGV
  struct sigaction sa_segv;
  sa_segv.sa_handler = on ? handleSignal : SIG_DFL;
  sigemptyset(&sa_segv.sa_mask);
  sa_segv.sa_flags = 0;

  // Install the signal handler for SIGSEGV
  if (sigaction(SIGSEGV, &sa_segv, nullptr) == -1)
    {
      fprintf(stderr, "Error installing signal handler for SIGSEGV.\n");
      Process::exit(EXIT_FAILURE);
    }
}

int Solv_Petsc::solve(ArrOfDouble& residu)
{
  PetscLogStagePush(KSPSolve_Stage_);
  double start = Statistiques::get_time_now();
  // Affichage par MyKSPMonitor
  if (!solveur_direct_)
    {
      if (limpr() == 1)
        {
          KSPMonitorSet(SolveurPetsc_, MyKSPMonitor, PETSC_NULLPTR, PETSC_NULLPTR);
        }
      else
        KSPMonitorCancel(SolveurPetsc_);
    }
  // Historique du residu
  KSPSetResidualHistory(SolveurPetsc_, residu.addr(), residu.size_array(), PETSC_TRUE);
  // Ksp_view
  if (enable_ksp_view())
    KSPView(SolveurPetsc_, PETSC_VIEWER_STDOUT_WORLD);
  // Keep precond ?
  if (nouvelle_matrice_)
    {
      //set_reuse_preconditioner(false); // Par defaut, precond est refait
      PetscBool flg;
      PetscOptionsHasName(PETSC_NULLPTR,option_prefix_,"-ksp_reuse_preconditioner",&flg);
      if (flg)
        set_reuse_preconditioner(true);
      else if (reuse_preconditioner_nb_it_max_>0)
        {
          bool reuse_precond = (nb_it_previous_ <= reuse_preconditioner_nb_it_max_);
          if (!reuse_precond) Cout << "Matrix preconditioner is recomputed cause previous iterations number>" << reuse_preconditioner_nb_it_max_ << "..." << finl;
          set_reuse_preconditioner(reuse_precond ? true : false);
        }
      if (reuse_preconditioner()) Cout << "Matrix has changed but reusing previous preconditioner..." << finl;
      if (type_pc_ == "shell" && !reuse_preconditioner())
        {
          OWN_PTR(PCShell_base)& pcs=pc_user_.pc_shell;
          pcs->setUpPC(PreconditionneurPetsc_, MatricePetsc_, SolutionPetsc_);
        }
      else
        KSPSetReusePreconditioner(SolveurPetsc_, (PetscBool) reuse_preconditioner()); // Default PETSC_FALSE
    }
  // Solve
  setupSignalHandlers(true);
  if (gpu_) statistiques().begin_count(gpu_library_counter_);
  KSPSolve(SolveurPetsc_, SecondMembrePetsc_, SolutionPetsc_);
  if (gpu_) statistiques().end_count(gpu_library_counter_);
  setupSignalHandlers(false);
  // Analyse de la convergence par Petsc
  KSPConvergedReason Reason;
  KSPGetConvergedReason(SolveurPetsc_, &Reason);
  if (Reason==KSP_DIVERGED_ITS && (convergence_with_nb_it_max_ || ignore_nb_it_max_)) Reason = KSP_CONVERGED_ITS;
  if (Reason<0)
    {
      Cerr << "No convergence on the resolution with the Petsc solver." << finl;
      Cerr << "Reason given by Petsc: ";
      if      (Reason==KSP_DIVERGED_NULL)                Cerr << "KSP_DIVERGED_NULL" << finl;
      else if (Reason==KSP_DIVERGED_DTOL)                Cerr << "KSP_DIVERGED_DTOL" << finl;
      else if (Reason==KSP_DIVERGED_BREAKDOWN)           Cerr << "KSP_DIVERGED_BREAKDOWN" << finl;
      else if (Reason==KSP_DIVERGED_BREAKDOWN_BICG)      Cerr << "KSP_DIVERGED_BREAKDOWN_BICG" << finl;
      else if (Reason==KSP_DIVERGED_NONSYMMETRIC)        Cerr << "KSP_DIVERGED_NONSYMMETRIC" << finl;
      else if (Reason==KSP_DIVERGED_INDEFINITE_PC)       Cerr << "KSP_DIVERGED_INDEFINITE_PC" << finl;
      else if (Reason==KSP_DIVERGED_NANORINF)            Cerr << "KSP_DIVERGED_NANORINF" << finl;
      else if (Reason==KSP_DIVERGED_INDEFINITE_MAT)      Cerr << "KSP_DIVERGED_INDEFINITE_MAT" << finl;
      else if (Reason==KSP_DIVERGED_PC_FAILED)           Cerr << "KSP_DIVERGED_PC_FAILED" << finl;
      else if (Reason==KSP_DIVERGED_ITS)
        {
          Cerr << "KSP_DIVERGED_ITS" << finl;
          Cerr << "That means the solver didn't converge within the maximal iterations number." << finl;
          Cerr << "You can change the maximal number of iterations with the -ksp_max_it option." << finl;
#ifdef MPIX_CUDA_AWARE_SUPPORT
          // Probleme vu avec GPU direct si >= 4 GPUs et preconditinneurs C-AMG ou BOOMERAMG
          // OK pour SA-AMG et Jacobi
          // Il faudrait faire un reproducer a soumettre a PETSc...
          Cerr << "It seems there is a convergence issue (bug?) with MPI GPU Aware library with PETSc CG and some preconditioners." << finl;
          Cerr << "Try using BICGSTAB instead of GCP to bypass the issue." << finl;
          Process::exit();
#endif
        }
      else Cerr << (int)Reason << finl;
      throw Reason;
    }
  if (Reason<0 && !return_on_error_) exit();
  PetscInt nbiter=-1;
  KSPGetIterationNumber(SolveurPetsc_, &nbiter);
  int nbit = (int)nbiter;  // always an int actually
  if (limpr()>-1)
    {
      // MyKSPMonitor ne marche pas pour certains solveurs (residu(0) n'est pas calcule):
      if (solveur_direct_ || type_ksp_ == KSPIBCGS)
        {
          // Calcul de residu(0)=||B||
          VecNorm(SecondMembrePetsc_, NORM_2, &residu[0]);
          // On l'affiche pour les solveurs directs (pour les autres TRUST s'en occupe):
          if (solveur_direct_) MyKSPMonitor(SolveurPetsc_, 0, residu[0], 0);
        }
      // Idem: l'historique du residu est mal evalue pour certains solveurs:
      // donc on le calcul a la derniere iteration:
      if (residu[0] > 0 && (solveur_direct_ || type_ksp_ == KSPIBCGS))
        {
          // Calcul de residu(nbiter)=||Ax-B||
          VecScale(SecondMembrePetsc_, -1);
          MatMultAdd(MatricePetsc_, SolutionPetsc_, SecondMembrePetsc_, SecondMembrePetsc_);
          VecNorm(SecondMembrePetsc_, NORM_2, &residu[nbit]);
        }
    }
  if (verbose) Cout << finl << "[Petsc] Time to solve system:    \t" << Statistiques::get_time_now() - start << finl;
  PetscLogStagePop();
  return Reason < 0 ? (int)Reason : nbit;
}
#endif

#ifdef PETSCKSP_H
void Solv_Petsc::Update_vectors(const DoubleVect& secmem, DoubleVect& solution)
{
  // Assemblage du second membre et de la solution
  double start = Statistiques::get_time_now();
  bool DataOnDevice = solution.checkDataOnDevice(secmem);
  if (gpu_ && DataOnDevice && !isViennaCLVector()) // The 2 arrays are up to date on the device and the solver is a GPU one (fastest strategy)
    {
      // We update PETSc vectors with the arrays on device:
      Update_lhs_rhs<Kokkos::DefaultExecutionSpace>(secmem, solution);
#ifdef PETSC_HAVE_CUDA
      VecCUDAPlaceArray(SecondMembrePetsc_, addrOnDevice(rhs_));
      VecCUDAPlaceArray(SolutionPetsc_, addrOnDevice(lhs_));
#endif
#ifdef PETSC_HAVE_HIP
      VecHIPPlaceArray(SecondMembrePetsc_, addrOnDevice(rhs_));
      VecHIPPlaceArray(SolutionPetsc_, addrOnDevice(lhs_));
#endif
      if (reorder_matrix_) Process::exit("reorder_matrix option is not supported yet on GPU");
      if (different_partition_) Process::exit("different_partition option is not supported yet on GPU");
    }
  else
    {
      // ToDo OpenMP afficher un warning pour dire d'utiliser un solveur GPU si solution est sur le GPU
      secmem.ensureDataOnHost();
      solution.ensureDataOnHost();
      PetscInt size=ix.size_array();
      if (gpu_) statistiques().begin_count(gpu_copytodevice_counter_);
      VecSetOption(SecondMembrePetsc_, VEC_IGNORE_NEGATIVE_INDICES, PETSC_TRUE);
      VecSetValues(SecondMembrePetsc_, size, ix.addr(), secmem.addr(), INSERT_VALUES);
      VecSetOption(SolutionPetsc_, VEC_IGNORE_NEGATIVE_INDICES, PETSC_TRUE);
      VecSetValues(SolutionPetsc_, size, ix.addr(), solution.addr(), INSERT_VALUES);
      VecAssemblyBegin(SecondMembrePetsc_);
      VecAssemblyEnd(SecondMembrePetsc_);
      VecAssemblyBegin(SolutionPetsc_);
      VecAssemblyEnd(SolutionPetsc_);
      if (gpu_) statistiques().end_count(gpu_copytodevice_counter_);
      if (reorder_matrix_)
        {
          VecPermute(SecondMembrePetsc_, colperm, PETSC_FALSE);
          VecPermute(SolutionPetsc_, colperm, PETSC_FALSE);
        }
    }
  if (verbose) Cout << "[Petsc] Time to update vectors: \t" << Statistiques::get_time_now() - start << finl;

//  VecView(SecondMembrePetsc_,PETSC_VIEWER_STDOUT_WORLD);
//  VecView(SolutionPetsc_,PETSC_VIEWER_STDOUT_WORLD);
}

bool Solv_Petsc::isViennaCLVector()
{
  VecType type;
  VecGetType(SecondMembrePetsc_, &type);
  return strcmp(type, VECSEQVIENNACL)==0 || strcmp(type, VECMPIVIENNACL)==0;
}

void Solv_Petsc::Update_solution(DoubleVect& solution)
{
  // Recuperation de la solution
  double start = Statistiques::get_time_now();
  bool DataOnDevice = solution.checkDataOnDevice();
  if (gpu_ && DataOnDevice && !isViennaCLVector()) // solution is on the device to SolutionPetsc_ -> solution update without copy
    {
      Solv_Externe::Update_solution<Kokkos::DefaultExecutionSpace>(solution);
#ifdef PETSC_HAVE_CUDA
      VecCUDAResetArray(SecondMembrePetsc_);
      VecCUDAResetArray(SolutionPetsc_);
#endif
#ifdef PETSC_HAVE_HIP
      VecHIPResetArray(SecondMembrePetsc_);
      VecHIPResetArray(SolutionPetsc_);
#endif
    }
  else
    {
      int size=ix.size_array();
      if (reorder_matrix_)
        VecPermute(SolutionPetsc_, rowperm, PETSC_TRUE);
      // ToDo un seul VecGetValues comme VecSetValues
      if (different_partition_)
        {
          // TRUST and PETSc have different partitions, a local vector LocalSolutionPetsc_ is gathered from the global vector SolutionPetsc_ :
          VecScatterBegin(VecScatter_, SolutionPetsc_, LocalSolutionPetsc_, INSERT_VALUES, SCATTER_FORWARD);
          VecScatterEnd  (VecScatter_, SolutionPetsc_, LocalSolutionPetsc_, INSERT_VALUES, SCATTER_FORWARD);
          // Use the local vector to get the solution:
          PetscInt colonne_locale=0;
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
          if (gpu_) statistiques().begin_count(gpu_copyfromdevice_counter_);
          VecGetValues(SolutionPetsc_, size, ix.addr(), solution.addr());
          if (gpu_) statistiques().end_count(gpu_copyfromdevice_counter_);
        }
    }
  if (verbose) Cout << finl << "[Petsc] Time to update solution: \t" << Statistiques::get_time_now() - start << finl;
}

void Solv_Petsc::check_aij(const Matrice_Morse& mat)
{
  /*******************/
  /* Setting mataij_ */
  /*******************/
  // Matrice non symetrique, on utilise le format aij et non sbaij:
  if (!matrice_symetrique_) mataij_=1;

  // Matrice reordonee necessite le format aij
  if (reorder_matrix_) mataij_=1;

  // Je n'arrive pas a faire marcher le stockage symetrique avec le preconditionneur PCEISENSTAT
  // qui est interessant car necessite 2 fois moins d'operations que le SSOR
  if (type_pc_==PCEISENSTAT) mataij_=1;

  // Reading a Matrix with Hypre (ToDo test if mataij=1 for Hypre is not better, cause here 2 matrix seqsbaij and seqaij)
  if (read_matrix_ && type_pc_==PCHYPRE) mataij_=1;

  // Dans le cas de SUPERLU_DIST pour Cholesky, je n'arrive pas a faire marcher le stockage
  // symetrique donc l'utilisation de SUPERLU_DIST n'est pas encore optimale en RAM...
  if (solveur_direct_==superlu_dist) mataij_=1;
  // IDEM pour UMFPACK qui ne supporte que le format AIJ:
  if (solveur_direct_==umfpack) mataij_=1;
  // IDEM pour UMFPACK qui ne supporte que le format AIJ:
  if (solveur_direct_==strumpack) mataij_=1;

  // Dans le cas GPU, seul le format AIJ est supporte pour le moment:
  if (gpu_ || amgx_) mataij_=1;

#ifdef PETSC_HAVE_OPENMP
  // Dans le cas d'OpenMP, seul le format aij est multithreade:
  // PL (01/2021): plus vrai
  // mataij_=1;
#endif

  // Dans le cas de save_matrix_ en parallele
  // Sinon, cela bloque avec sbaij:
  if (save_matrix_==1 && Process::is_parallel()) mataij_=1;

  // Error in PETSc when read/save the factored matrix if matrix is sbaij
  // so aij is selected instead:
  if (factored_matrix_!="") mataij_=1;

  if (!read_matrix_)
    {
      // Ajout d'un test de verification de la symetrie supposee de la matrice PETSc
      // Ce test a permis de trouver un defaut de parallelisme sur le remplissage
      // de la matrice en pression lors de l'introduction de l'option volume etendu
      bool check_matrice_symetrique_ = matrice_symetrique_;
      // Check cancelled for:
#ifdef PETSC_HAVE_CUDA
      if (!amgx_) check_matrice_symetrique_=false; // Bug with CUDA ?
#endif
#ifdef NDEBUG
      check_matrice_symetrique_=false; // Not done in production
#endif
      if (mataij_ == 0)
        {
          /***************************************/
          /* Test de verification de la symetrie */
          /***************************************/
          if (check_matrice_symetrique_)
            {
              Mat MatricePetscComplete;
              // On construit une matrice PETSc complete sans hypothese sur la symetrie
              Create_MatricePetsc(MatricePetscComplete, 1, mat);
              Mat MatricePetsc;
              Create_MatricePetsc(MatricePetsc, mataij_, mat);
              PetscBool matrices_identiques;
              // On teste l'egalite des 2 matrices en faisant n produits matrice-vecteur
              int n = 10;
              MatMultAddEqual(MatricePetsc, MatricePetscComplete, n, &matrices_identiques);
              if (!matrices_identiques)
                {
                  Cerr << "Error: matrix PETSc are different according to the symmetric storage or not." << finl;
                  if (Process::is_parallel()) Cerr << "Check if the matrix is correct in parallel." << finl;
                  Cerr << "Contact TRUST support team." << finl;
                  if (nb_rows_ < 10)
                    {
                      MatView(MatricePetsc, PETSC_VIEWER_STDOUT_WORLD);
                      MatView(MatricePetscComplete, PETSC_VIEWER_STDOUT_WORLD);
                      exit();
                    }
                }
              MatDestroy(&MatricePetsc);
              MatDestroy(&MatricePetscComplete);
            }
        }
    }
}
// Creation des objets PETSc
void Solv_Petsc::Create_objects(const Matrice_Morse& mat, int blocksize)
{
  // Remplissage d'une matrice de preconditionnement non symetrique
  Mat MatricePrecondionnementPetsc;
  /* Semble plus vrai pour spai dans Petsc 3.10.0:
  if (matrice_symetrique_ && (type_pc_=="hypre" || type_pc_=="spai")) */
  if (matrice_symetrique_ && type_pc_ == "hypre")
    preconditionnement_non_symetrique_ = 1;
  if (mataij_==1) preconditionnement_non_symetrique_ = 0;



  if (preconditionnement_non_symetrique_)
    Create_MatricePetsc(MatricePrecondionnementPetsc, 1, mat);

  // Creation de la matrice Petsc si necessaire
  if (!read_matrix_)
    {
      if (MatricePetsc_!=nullptr) MatDestroy(&MatricePetsc_);
      Create_MatricePetsc(MatricePetsc_, mataij_, mat);
    }
  MatSetBlockSize(MatricePetsc_, blocksize);
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
      MatSetOption(MatricePetsc_, MAT_SYMMETRIC, PETSC_TRUE); // ToDo: ajout option spd pour MAT_SPD ?
      if (type_pc_ == PCLU)
        {
          // PCCHOLESKY is only supported for sbaij format or since PETSc 3.9.2, SUPERLU, CHOLMOD
          if (mataij_ == 0 || solveur_direct_ == superlu_dist || solveur_direct_ == cholmod)
            PCSetType(PreconditionneurPetsc_, PCCHOLESKY); // Precond PCLU -> PCCHOLESKY
        }
      else if (type_pc_ == PCSOR)
        PCSORSetSymmetric(PreconditionneurPetsc_, SOR_LOCAL_SYMMETRIC_SWEEP); // Precond SOR -> SSOR
    }

  /*******************************************/
  /* Choix du package pour le solveur direct */
  /*******************************************/
  static int message_affi = limpr() >= 0;
  if (solveur_direct_ == mumps)
    {
      // Message pour prevenir
      if (message_affi)
        {
          Cout << "The LU decomposition of a matrix with ";
          Cout << "Cholesky from MUMPS may take several minutes, please wait..." << finl;
          Cout
              << "If the decomposition fails/crashes cause a lack of memory, then increase the number of CPUs for your calculation"
              << finl;
          Cout
              << "or add reduce_ram option (syntax: cholesky { reduce_ram }) to suppress preventive memory increase (INCTL(14))"
              << finl;
          Cout
              << "or use Cholesky_out_of_core keyword to write the decomposition on the disk, thus saving memory but with an extra CPU cost during solve."
              << finl;
          Cout
              << "To see the RAM required by the decomposition in the .out file, add impr option to the solver: petsc cholesky { impr }"
              << finl;
          Cout
              << "If an error INFOG(1)=-8|-9|-11|-17|-20 is returned, you can try to increase the ICNTL(14) parameter of MUMPS by using the -mat_mumps_icntl_14 command line option."
              << finl;
          message_affi = 0;
        }
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERMUMPS);
      if (!reduce_ram_)
        {
          // Securite pour MUMPS avec augmentation de la memoire:
          // Par defaut le cas PAR_Canal_incline_VEF plante sur 4 processeurs si -mat_mumps_icntl_14 inferieur a 35...
          // On revient a 75 car parfois VEF_258 plante... C'est pas clair au niveau memoire...
          // Passage a Petsc 3.3 necessite d'augmenter a plus de 75 car sinon Aero_192 crashe...
          // A 90, le cas les_Re180Pr071_T0Q_jdd2 plante sur forchat (32bits)
          // On differencie sequentiel (peu de memoire, mais estimation juste)
          // et le calcul parallele (voir peut etre une separation entre plus et moins de 16 processeurs...)
          // Peut etre equiper le script trust d'une detection des erreurs INFO(1)=-9 ...
          // On passe de 35 a 40 pour faire passer le cas cavite_entrainee_2D_jdd2 (suite passage a MUMPS 5.2.0)
          if (Process::is_sequential())
            add_option("mat_mumps_icntl_14", "40");
          else
            add_option("mat_mumps_icntl_14", "90");
        }
    }
  else if (solveur_direct_ == superlu_dist)
    {
      if (message_affi)
        Cout << "Cholesky from SUPERLU_DIST may take several minutes, please wait..." << finl;
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERSUPERLU_DIST);
    }
  else if (solveur_direct_ == petsc)
    {
      if (message_affi)
        Cout << "Cholesky from PETSc may take several minutes, please wait...";
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERPETSC);
    }
  else if (solveur_direct_ == umfpack)
    {
      if (message_affi)
        Cout << "Cholesky from UMFPACK may take several minutes, please wait...";
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERUMFPACK);
    }
  else if (solveur_direct_ == pastix)
    {
      if (message_affi)
        Cout << "Cholesky from Pastix may take several minutes, please wait...";
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERPASTIX);
    }
  else if (solveur_direct_ == cholmod)
    {
      if (message_affi)
        Cout << "Cholesky from Cholmod may take several minutes, please wait...";
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERCHOLMOD);
    }
  else if (solveur_direct_ == strumpack)
    {
      if (message_affi)
        Cout << "Cholesky from Strumpack may take several minutes, please wait...";
      PCFactorSetMatSolverType(PreconditionneurPetsc_, MATSOLVERSTRUMPACK);
    }
  else if (solveur_direct_ == cli)
    {
      if (message_affi)
        Cout << "LU factorization may take several minutes, please wait...";
    }
  else if (solveur_direct_)
    {
      Cerr << "PCFactorSetMatSolverType not called for direct solver, solveur_direct_=" << solveur_direct_ << finl;
      Cerr << "Contact TRUST support." << finl;
      exit();
    }

  if ((solveur_direct_ > mumps) && (message_affi))
    {
      Cout << " OK " << finl;
      message_affi = 0;
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
      filename += "_Factored_Matrix.petsc";
      if (factored_matrix_ == "read" || factored_matrix_ == "disk")
        {
          int filename_found = 0;
          // Advice: stat file from master and broadcast
          if (Process::je_suis_maitre())
            {
              // struct stat f {}; pb sur gcc 4.8.5
              struct stat f;
              if (!stat(filename, &f)) filename_found = 1;
            }
          envoyer_broadcast(filename_found, 0);
          if (filename_found)
            {
              // File found, we read it:
              factored_matrix_ = "read";
            }
          else
            {
              // File not found, two cases:
              Cerr << "File " << filename << " not found";
              if (factored_matrix_ == "read")
                {
                  Cerr << "!" << finl;
                  exit();
                }
              else
                {
                  Cerr << "." << finl;
                  Cerr << "So we will compute the factored matrix and we will save it to disk." << finl;
                  factored_matrix_ = "save";
                }
            }
        }
      if (factored_matrix_ == "read")
        {
          PetscViewer viewer;
          PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer);
          PCFactorSetUpMatSolverType(PreconditionneurPetsc_);
          Mat FactoredMatrix;
          PCFactorGetMatrix(PreconditionneurPetsc_, &FactoredMatrix);
//	MatCreate(PETSC_COMM_WORLD,&FactoredMatrix);
//	MatSetSizes(FactoredMatrix, nb_rows_, nb_rows_, PETSC_DECIDE, PETSC_DECIDE);
//	MatSetType(FactoredMatrix,MATSEQAIJ);
          Cerr << "Reading the factored matrix in the file " << filename << finl;
          MatLoad(FactoredMatrix, viewer);
          PetscViewerDestroy(&viewer);
        }
      else if (factored_matrix_ == "save")
        {
          Mat FactoredMatrix;
          // Compute the factored matrix:
          PCFactorSetUpMatSolverType(PreconditionneurPetsc_);
          PCSetUp(PreconditionneurPetsc_);
          // Get the factored matrix:
          PCFactorGetMatrix(PreconditionneurPetsc_, &FactoredMatrix);
//NO	MatConvert(FactoredMatrix,MATSEQDENSE,MAT_INITIAL_MATRIX,&FactoredMatrix);
//	MatScalar *a;
//	MatDenseGetArray(FactoredMatrix,&a);
          if (nb_rows_ < 20)
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
          PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_WRITE, &viewer);
          MatView(FactoredMatrix, viewer);
          PetscViewerDestroy(&viewer);

//          PetscViewer viewer2;
//          PetscViewerBinaryOpen(PETSC_COMM_WORLD,filename,FILE_MODE_READ,&viewer2);
//          MatLoad(FactoredMatrix, viewer2);
//          PetscViewerDestroy(&viewer2);
        }
      else if (factored_matrix_ != "")
        {
          Cerr << "Unknown option for factored_matrix option: " << factored_matrix_ << finl;
          Cerr << "Options are: read|save|disk" << finl;
          exit();
        }
    }

  /*************************************/
  /* Mise en place du preconditionneur */
  /*************************************/
  double start = Statistiques::get_time_now();
  KSPSetUp(SolveurPetsc_);
  if (verbose) Cout << "[Petsc] Time to setup solver:    \t" << Statistiques::get_time_now() - start << finl;
}

void Solv_Petsc::Create_vectors(const DoubleVect& b)
{
  if (SecondMembrePetsc_!=nullptr) return; // Deja construit
  if (gpu_)
    {
      // For GPU solvers, allocate 2 arrays on device to avoid 2 H2D and 1 D2H copy later during each solve.
      Create_lhs_rhs_onDevice();
    }
  // Build x
  VecCreate(PETSC_COMM_WORLD,&SecondMembrePetsc_);
  // Set sizes:
  if (petsc_decide_)
    VecSetSizes(SecondMembrePetsc_, PETSC_DECIDE, nb_rows_tot_);
  else if (petsc_cpus_selection_)
    {
      PetscInt nb_rows_petsc = compute_nb_rows_petsc(nb_rows_tot_);
      VecSetSizes(SecondMembrePetsc_, nb_rows_petsc, PETSC_DECIDE);
    }
  else
    VecSetSizes(SecondMembrePetsc_, nb_rows_, PETSC_DECIDE);

  // Set type:
#ifdef PETSC_HAVE_CUDA
  if (gpu_)
    VecSetType(SecondMembrePetsc_, (Process::is_sequential() ? VECSEQCUDA : VECMPICUDA));
  else
#endif
#ifdef PETSC_HAVE_HIP
    if (gpu_)
      VecSetType(SecondMembrePetsc_, (Process::is_sequential() ? VECSEQHIP : VECMPIHIP));
    else
#endif
      VecSetType(SecondMembrePetsc_, (Process::is_sequential() ? VECSEQ : VECMPI));
  VecSetOptionsPrefix(SecondMembrePetsc_, option_prefix_);
  VecSetFromOptions(SecondMembrePetsc_);
  // Build b
  VecDuplicate(SecondMembrePetsc_,&SolutionPetsc_);
  // Initialize x to avoid a crash on GPU later with VecSetValues... (bug PETSc?)
  // if (gpu_) VecSet(SolutionPetsc_,0.0);

  // Only in the case where TRUST and PETSc partitions are not the same
  // VecGetValues can only get values on the same processor, so need to gather values from
  // global vector SolutionPetsc_ to a local vector LocalSolutionPetsc_ before using VecGetValues
  // It will add an extra MPI cost with this operation.
  if (different_partition_)
    {
      // Create the local vector of length nb_rows_:
      VecCreateSeq(PETSC_COMM_SELF, nb_rows_, &LocalSolutionPetsc_);
      // Create the Scatter context to gather from the global solution to the local solution
      ArrOfPetscInt from(nb_rows_);
      for (int i=0; i<nb_rows_; i++)
        from[i]=decalage_local_global_+i; // Global indices in SolutionPetsc_
      IS fromis;
      ISCreateGeneral(PETSC_COMM_WORLD, from.size_array(), from.addr(), PETSC_COPY_VALUES, &fromis);
      VecScatterCreate(SolutionPetsc_, fromis, LocalSolutionPetsc_, PETSC_NULLPTR, &VecScatter_);
      ISDestroy(&fromis);
      // Will permit later with VecScatterBegin/VecScatterEnd something like:
      // LocalSolutionPetsc_[tois[i]]=SolutionPetsc_[fromis[i]]
    }
}

void Solv_Petsc::Create_DM(const DoubleVect& b)
{
  if (dm_!=nullptr) return; // Deja construit
  /* creation de champs Petsc si des MD_Vector_Composite sont trouves dans b, avec recursion! */
  if (sub_type(MD_Vector_composite, b.get_md_vector().valeur()))
    {
      std::map<std::string, std::vector<PetscInt>> champ;
      //liste (MD_Vector_composite, offset de ses elements, multiplicateur (nb d'items du tableau par item du MD_Vector) prefixe des noms de ses champs)
      std::vector<std::tuple<const MD_Vector_composite *, int, int, std::string>>
                                                                               mdc_list =
      {
        std::make_tuple(&ref_cast(MD_Vector_composite, b.get_md_vector().valeur()), 0, b.line_size(),
        std::string("b"))
      };
      while (mdc_list.size()) //remplissage recursif de champs_ -> (nom du champ, indices)
        {
          const MD_Vector_composite& mdc = *std::get<0>(mdc_list.back());
          int idx = std::get<1>(mdc_list.back()), mult = std::get<2>(mdc_list.back()), un = 1;
          std::string prefix = std::get<3>(mdc_list.back());
          mdc_list.pop_back();
          for (int i = 0; i < mdc.nb_parts(); i++)
            {
              const MD_Vector_base& mdb = mdc.get_desc_part(i).valeur();
              int mult2 = mult * std::max(mdc.get_shape(i), un), nb_seq = mdb.nb_items_seq_local() * mult2;
              if (sub_type(MD_Vector_composite, mdb)) //un autre MD_Vector_Composite! on le met dans la liste
                mdc_list.push_back(std::make_tuple(&ref_cast(MD_Vector_composite, mdb), idx, mult2,
                                                   prefix + std::to_string((long long) i)));
              else
                {
                  std::vector<PetscInt> indices;
                  for (int j = 0; j < nb_seq; j++) indices.push_back(decalage_local_global_ + idx + j);
                  if (mdc.get_name(i)!="")
                    champ[mdc.get_name(i).getString()] = indices;
                  else
                    champ[prefix + std::to_string((long long) i)] = indices;
                }
              idx += nb_seq; //mise a jour du decalage (idx)
            }
        }

      /* PetscSection : indique a quel champ appartient chaque variable */
      PetscSection sec;
      PetscSectionCreate(PETSC_COMM_WORLD, &sec);
      PetscSectionSetNumFields(sec, (int)champ.size());
      PetscSectionSetChart(sec, decalage_local_global_,
                           decalage_local_global_ + b.line_size() * b.get_md_vector()->nb_items_seq_local());
      int idx = 0;
      for (auto &&kv : champ)
        {
          PetscSectionSetFieldName(sec, idx, kv.first.c_str());
          if (limpr() >= 0) Cerr << "Field " << kv.first << " available for PCFieldsplit." << finl;
          for (int j = 0; j < (int) kv.second.size(); j++)
            PetscSectionSetDof(sec, kv.second[j], 1), PetscSectionSetFieldDof(sec, kv.second[j], idx, 1);
          idx++;
        }
      PetscSectionSetUp(sec);

      /* DMShell : un objet encapsulant la section */
      DMShellCreate(PETSC_COMM_WORLD, &dm_);
      DMSetLocalSection(dm_, sec);
      DMSetUp(dm_);
      PetscSectionDestroy(&sec);
    }
  if (sub_type(MD_Vector_composite, b.get_md_vector().valeur())) PCSetDM(PreconditionneurPetsc_, dm_);
}

PetscInt Solv_Petsc::compute_nb_rows_petsc(PetscInt nb_rows_tot)
{
  // Case the user specifies a number of CPUs:
  PetscInt nb_rows_petsc = nb_rows_tot / petsc_nb_cpus_;
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

// Creation d'une matrice Petsc depuis une matrice Matrice_Morse
void Solv_Petsc::Create_MatricePetsc(Mat& MatricePetsc, int mataij, const Matrice_Morse& mat_morse)
{
  double start = Statistiques::get_time_now();
  // Recuperation des donnees
  bool journal = nb_rows_tot_ < 20 ? true : false;
  journal = false;
  assert(!sub_type(Matrice_Morse_Sym, mat_morse));
  if (journal)   // Impressions provisoires
    {
      Journal() << "mat=" << finl;
      mat_morse.imprimer_formatte(Journal());
      Journal() << "renum_=" << finl;
      renum_.ecrit(Journal());
      Journal() << "items_to_keep_=" << finl;
      Journal() << items_to_keep_ << finl;
    }

  /////////////////////////////////////
  // On cree et dimensionne la matrice
  /////////////////////////////////////
  // Based on src/ksp/ksp/examples/tutorials/ex2.c
  MatCreate(PETSC_COMM_WORLD, &MatricePetsc);
  if (petsc_decide_)
    MatSetSizes(MatricePetsc, PETSC_DECIDE, PETSC_DECIDE, nb_rows_tot_, nb_rows_tot_);
  else if (petsc_cpus_selection_)
    {
      PetscInt nb_rows_petsc = compute_nb_rows_petsc(nb_rows_tot_);
      Journal() << "Process " << Process::me() << " has " << nb_rows_petsc << " rows of the matrix PETSc." << finl;
      MatSetSizes(MatricePetsc, nb_rows_petsc, nb_rows_petsc, PETSC_DECIDE, PETSC_DECIDE);
    }
  else     // Normal use: partition of PETSc matrix is dicted by TRUST matrix:
    MatSetSizes(MatricePetsc, nb_rows_, nb_rows_, PETSC_DECIDE, PETSC_DECIDE);

  //////////////////////////////////////////////
  // Determination du nombre d'elements non nuls
  //////////////////////////////////////////////
  ArrOfPetscInt nnz(nb_rows_);
  ArrOfPetscInt d_nnz(nb_rows_);
  ArrOfPetscInt o_nnz(nb_rows_);
  //nnz = 0;
  //d_nnz = 0;
  //o_nnz = 0;
  ArrOfTID& renum_array = renum_;  // tableau vu comme lineaire
  const PetscInt premiere_colonne_globale = decalage_local_global_;
  const PetscInt derniere_colonne_globale = nb_rows_ + decalage_local_global_;
  const ArrOfInt& tab1 = mat_morse.get_tab1();
  const ArrOfInt& tab2 = mat_morse.get_tab2();
  //const ArrOfDouble& coeff = mat_morse.get_coeff();
  int cpt = 0;
  const int n = tab1.size_array() - 1;
  for (int i = 0; i < n; i++)
    {
      if (items_to_keep_[i])
        {
          const int k0 = tab1[i] - 1;
          const int k1 = tab1[i + 1] - 1;
          nnz[cpt] = k1 - k0; // Nombre d'elements non nuls sur la ligne i
          for (int k = k0; k < k1; k++)
            {
              const int colonne_locale = tab2[k] - 1;
              const PetscInt colonne_globale = renum_array[colonne_locale];
              if (colonne_globale >= premiere_colonne_globale && colonne_globale < derniere_colonne_globale)
                d_nnz[cpt]++;
              else
                o_nnz[cpt]++;
            }
          cpt++;
        }
    }

  if (journal)
    {
      Journal() << "nnz=" << nnz << finl;
      Journal() << "d_nnz=" << d_nnz << finl;
      Journal() << "o_nnz=" << o_nnz << finl;
    }
  /************************/
  /* Typage de la matrice */
  /************************/
  if (mataij == 0)
    {
      // On utilise SBAIJ pour une matrice symetrique (plus rapide que AIJ)
      MatSetType(MatricePetsc, (Process::is_sequential() ? MATSEQSBAIJ : MATMPISBAIJ));
    }
  else
    {
      // On utilise AIJ car je n'arrive pas a faire marcher avec BAIJ
#ifdef PETSC_HAVE_CUDA
      if (gpu_)
        MatSetType(MatricePetsc, (Process::is_sequential() ? MATSEQAIJCUSPARSE : MATMPIAIJCUSPARSE));
      else
#endif
#ifdef PETSC_HAVE_HIP
        if (gpu_)
          MatSetType(MatricePetsc, (Process::is_sequential() ? MATSEQAIJHIPSPARSE : MATMPIAIJHIPSPARSE));
        else
#endif
          MatSetType(MatricePetsc, (Process::is_sequential() ? MATSEQAIJ : MATMPIAIJ));
    }
  // Surcharge eventuelle par ligne de commande avec -mat_type:
  // Example: now possible to change aijcusparse to aijviennacl via CLI
  MatSetOptionsPrefix(MatricePetsc, option_prefix_);
  MatSetFromOptions(MatricePetsc);
  // If -mat_type aij, update mataij flag:
  MatType mat_type;
  MatGetType(MatricePetsc, &mat_type);
  if (strcmp(mat_type, MATSEQAIJ) == 0 || strcmp(mat_type, MATMPIAIJ) == 0) mataij = 1;

  /********************************************/
  /* Preallocation de la taille de la matrice */
  /********************************************/
  // TRES important pour la vitesse de construction de la matrice
  if (mataij == 0)
    {
      if (different_partition_)
        {
          // If partition of TRUST and PETSc differs, difficult to preallocate the matrix finely so:
          // ToDo, try to optimize:
          int nz = Process::mp_max((nnz.size_array() == 0 ? 0 : (int)max_array(nnz)));  // max_array always an int: max numb of zeros on a line
          MatSeqSBAIJSetPreallocation(MatricePetsc, block_size_, nz, PETSC_NULLPTR);
          MatMPISBAIJSetPreallocation(MatricePetsc, block_size_, nz, PETSC_NULLPTR, nz, PETSC_NULLPTR);
        }
      else
        {
          MatSeqSBAIJSetPreallocation(MatricePetsc, block_size_, PETSC_DEFAULT, nnz.addr());
          // Test on nb_rows==0 is to avoid PAR_docond_anisoproc hangs
          MatMPISBAIJSetPreallocation(MatricePetsc, block_size_, (nb_rows_ == 0 ? 0 : PETSC_DEFAULT), d_nnz.addr(),
                                      (nb_rows_ == 0 ? 0 : PETSC_DEFAULT), o_nnz.addr());
        }
    }
  else
    {
      if (different_partition_)
        {
          // If partition of TRUST and PETSc differs, difficult to preallocate the matrix finely so:
          // ToDo, try to optimize:
          int nz = Process::mp_max((nnz.size_array() == 0 ? 0 : (int)max_array(nnz)));   // max_array always an int: max numb of zeros on a line
          MatSeqAIJSetPreallocation(MatricePetsc, nz, PETSC_NULLPTR);
          MatMPIAIJSetPreallocation(MatricePetsc, nz, PETSC_NULLPTR, nz, PETSC_NULLPTR);
        }
      else
        {
          MatSeqAIJSetPreallocation(MatricePetsc, PETSC_DEFAULT, nnz.addr());
          // Test on nb_rows==0 is to avoid PAR_docond_anisoproc hangs
          MatMPIAIJSetPreallocation(MatricePetsc, (nb_rows_ == 0 ? 0 : PETSC_DEFAULT), d_nnz.addr(),
                                    (nb_rows_ == 0 ? 0 : PETSC_DEFAULT), o_nnz.addr());
        }
    }

  // ToDo: nettoyer la matrice TRUST en amont... Car le nnz des matrices peut varier (ex: implicite, Hyd_Cx_impl ou PolyMAC_P0P1NC)
  // et si on supprime les zeros de la matrice, lors d'un update on peut avoir une allocation -> erreur
  if (mataij_)
    {
      if (clean_matrix_) MatSetOption(MatricePetsc, MAT_IGNORE_ZERO_ENTRIES, PETSC_TRUE); // Ne stocke pas les zeros
      if (verbose)
        {
          ArrOfDouble nonzeros(2); // Pas ArrOfInt car nonzeros peut depasser 2^32 facilement - on n'a pas besoin d'un compte exact
          nonzeros[0] = 0;
          nonzeros[1] = mat_morse.nb_coeff();
          for (int i=0; i<nonzeros[1]; i++)
            if (mat_morse.get_coeff()(i)!=0)
              nonzeros[0]+=1;
          mp_sum_for_each_item(nonzeros);
          if (nonzeros[1] > 0)
            {
              double ratio = 1 - (double) nonzeros[0] / (double) nonzeros[1];
              if (ratio > 0.2) Cout << "Warning! Trust matrix contains a lot of useless stored zeros: " << (int) (ratio * 100) << "% (" << nonzeros[0] << "/" << nonzeros[1] << ")" << finl;
            }
          int zero_discarded = (int)(std::lrint(nonzeros[1] - nonzeros[0]));
          if (zero_discarded) Cout << "[Petsc] Discarding " << zero_discarded << " zeros from TRUST matrix into the PETSc matrix ..." << finl;
        }
    }
  // Genere une erreur (ou pas) si une case de la matrice est remplie sans allocation auparavant:
  MatSetOption(MatricePetsc, MAT_NEW_NONZERO_ALLOCATION_ERR, allow_realloc_ ? PETSC_FALSE : PETSC_TRUE);

  // Hash table (Faster MatAssembly after the first one)
  if (ignore_new_nonzero_)
    MatSetOption(MatricePetsc, MAT_USE_HASH_TABLE, PETSC_TRUE);

  if (verbose) Cout << "[Petsc] Time to create the matrix: \t" << Statistiques::get_time_now() - start << finl;

  // Fill the matrix
  Solv_Petsc::Update_matrix(MatricePetsc, mat_morse);

  // Reorder the matrix
  if (reorder_matrix_)
    {
      Mat Aperm;
      MatOrderingType ordering = MATORDERINGRCM;
      MatGetOrdering(MatricePetsc_, ordering, &rowperm, &colperm);
      MatPermute(MatricePetsc_, rowperm, colperm, &Aperm);
      MatDestroy(&MatricePetsc_);
      MatricePetsc_ = Aperm;
    }
}

void Solv_Petsc::Update_matrix(Mat& MatricePetsc, const Matrice_Morse& mat_morse)
{
  double start = Statistiques::get_time_now();
  bool journal = nb_rows_tot_ < 20 ? true : false;
  journal = false;

  /*****************************/
  /* Remplissage de la matrice */
  /*****************************/
  // ligne par ligne avec un tableau coeff et tab2 qui contiennent
  // les coefficients et les colonnes globales pour chaque ligne
  // On dimensionne ces tableaux a la taille la plus grande possible
  // ToDo : recalcul de nnz utile ?
  ArrOfInt nnz(nb_rows_);
  nnz = 0;
  ArrOfTID& renum_array = renum_;  // tab seen as a flat array (can't use ArrOfPetscInt& because of C++ ref cast...)
  const ArrOfInt& tab1 = mat_morse.get_tab1();
  const ArrOfInt& tab2 = mat_morse.get_tab2();
  int cpt = 0;
  for (int i = 0; i < tab1.size_array() - 1; i++)
    if (items_to_keep_[i])
      {
        nnz[cpt] = tab1[i + 1] - tab1[i]; // Nombre d'elements non nuls sur la ligne i
        cpt++;
      }
  int size = (nb_rows_ == 0 ? 0 : max_array(nnz)); // Test sur nb_rows si nul (cas proc vide) car sinon max_array plante
  ArrOfDouble coeff_tmp(size);
  ArrOfPetscInt tab2_tmp(size);
  const ArrOfDouble& coeff = mat_morse.get_coeff();
  cpt = 0;
  const int n = tab1.size_array() - 1;
  for(int i=0; i < n; i++)
    {
      if (items_to_keep_[i])
        {
          PetscInt ligne_globale = cpt + decalage_local_global_;
          int ncol = 0;
          const int k0 = tab1[i] - 1;
          const int k1 = tab1[i + 1] - 1;
          for (int k = k0; k < k1; k++)
            {
              coeff_tmp[ncol] = coeff[k];
              tab2_tmp[ncol] = renum_array[tab2[k] - 1];
              ncol++;
            }
          assert(ncol == nnz[cpt]);
          if (journal)
            {
              Journal() << (int)ligne_globale << " ";
              for (int j = 0; j < ncol; j++) Journal() << coeff_tmp[j] << " ";
              Journal() << finl;
            }
          try
            {
              MatSetValues(MatricePetsc, 1, &ligne_globale, ncol, tab2_tmp.addr(), coeff_tmp.addr(), INSERT_VALUES);
            }
          catch(...)
            {
              // ToDo: changer car PETSc est en C: pas d'exception lancee
              Cerr << "We detect that the PETSc matrix coefficients are changed without pre-allocation." << finl;
              Cerr << "Try one of the following option:" << finl;
              Cerr << "- Rebuild the matrix each time instead of updating the coefficients (slower)." << finl;
              Cerr << "enable_allocation : Enable re-allocation of coefficients (slow)." << finl;
              Cerr << "- Discard new coefficients (risk!)" << finl;
              Cerr << "Try the two options and select the costly one." << finl;
              Process::exit();
            }
          cpt++;
        }
    }
  /****************************/
  /* Assemblage de la matrice */
  /****************************/
  MatAssemblyBegin(MatricePetsc, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(MatricePetsc, MAT_FINAL_ASSEMBLY);

#ifndef NDEBUG
  if (mataij_)
    {
      // Verifie la non symetrie de la matrice (au moins une fois)
      PetscBool IsSymmetric;
      MatIsSymmetric(MatricePetsc, 0.0, &IsSymmetric);
      if (IsSymmetric && limpr() >= 0 && !amgx_) Cerr << "Warning: The PETSc matrix is aij but is symmetric. May be use sbaij ?" << finl;
    }
#endif
  // Ignore les coefficients ajoutes:
  if (!nouveau_stencil_ && ignore_new_nonzero_)
    MatSetOption(MatricePetsc, MAT_NEW_NONZERO_LOCATIONS, PETSC_FALSE);

  // Recuperation de la memoire max
  /*
  if (limpr()==1)
    {
      MatInfo info;
      MatGetInfo(MatricePetsc,MAT_GLOBAL_MAX,&info);
      Cerr << "Max memory used by matrix on a MPI rank: " << (int)(info.memory/1024/1024) << " MB" << finl;
    }*/
  if (verbose) Cout << "[Petsc] Time to fill the matrix: \t" << Statistiques::get_time_now() - start << finl;
}

bool Solv_Petsc::check_stencil(const Matrice_Morse& mat_morse)
{
  // Est ce un nouveau stencil ?
  double start = Statistiques::get_time_now();
  int new_stencil=0;
  if (!mataij_)
    new_stencil = 1; // Don't how to check the stencil with symmetric ?
  else
    {
      PetscBool done;
      Mat localA;
      PetscInt nRowsLocal;
      const PetscInt *colIndices = nullptr, *rowOffsets = nullptr;
      if (Process::is_sequential()) // sequential AIJ
        {
          // Make localA point to the same memory space as A does
          localA = MatricePetsc_;
        }
      else
        {
          // Get local matrix from redistributed matrix
          MatMPIAIJGetLocalMat(MatricePetsc_, MAT_INITIAL_MATRIX, &localA);
        }
      MatGetRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE, &nRowsLocal, &rowOffsets, &colIndices, &done);

      const ArrOfInt& tab1 = mat_morse.get_tab1();
      const ArrOfInt& tab2 = mat_morse.get_tab2();
      const ArrOfDouble& coeff = mat_morse.get_coeff();
      const ArrOfTID& renum_array = renum_;
      int RowLocal = 0;
      //Journal() << "Provisoire: nb_rows_=" << nb_rows_ << " nb_rows_tot_=" << nb_rows_tot_ << finl;
      const int n = tab1.size_array() - 1;
      for (int i = 0; i < n; i++)
        {
          if (items_to_keep_[i])
            {
              int nnz_row = 0;
              const int k0 = tab1[i] - 1;
              const int k1 = tab1[i + 1] - 1;
              for (int k = k0; k < k1; k++)
                if (coeff[k] != 0) nnz_row++;
              const PetscInt kk0 = rowOffsets[RowLocal];
              const PetscInt kk1 = rowOffsets[RowLocal + 1];
              if (nnz_row != (int)(kk1 - kk0))
                {
                  //Journal() << "Provisoire: Number of non-zero will change from " << rowOffsets[RowLocal + 1] - rowOffsets[RowLocal] << " to " << nnz_row << " on row " << RowLocal << finl;
                  new_stencil = 1;
                  break;
                }
              else
                {
                  for (int k = k0; k < k1; k++)
                    {
                      if (coeff[k] != 0)
                        {
                          bool found = false;
                          const PetscInt col = renum_array[tab2[k] - 1];
                          const PetscInt RowGlobal = decalage_local_global_+RowLocal;
                          for (PetscInt kk = kk0; kk < kk1; kk++)
                            {
                              if (colIndices[kk] == col)
                                {
                                  found = true;
                                  break;
                                }
                            }
                          if (!found)
                            {
                              Journal() << "Provisoire: mat_morse(" << RowGlobal << "," << col << ")!=0 new " << finl;
                              new_stencil = 1;
                              break;
                            }
                        }
                    }
                }
              RowLocal++;
            }
        }
      MatRestoreRowIJ(localA, 0, PETSC_FALSE, PETSC_FALSE, &nRowsLocal, &rowOffsets, &colIndices, &done);
      if (Process::is_parallel()) MatDestroy(&localA);
      new_stencil = mp_max(new_stencil);
    }
  if (verbose) Cout << "[Petsc] Time to check stencil:   \t" << Statistiques::get_time_now() - start << finl;
  return new_stencil;
}
#endif
