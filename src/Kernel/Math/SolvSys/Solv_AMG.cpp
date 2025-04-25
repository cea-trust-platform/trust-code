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

#include <Solv_AMG.h>
#include <EChaine.h>
#include <Motcle.h>
#include <Solv_AMGX.h>
#include <Solv_Petsc_GPU.h>
#ifdef TRUST_USE_ROCM
#include <rocm-core/rocm_version.h>
#endif

Implemente_instanciable(Solv_AMG,"Solv_AMG",SolveurSys_base);
// XD amg solveur_sys_base amg 0 Wrapper for AMG preconditioner-based solver which switch for the best one on CPU/GPU Nvidia/GPU AMD
// XD attr solveur chaine solveur 0 not_set
// XD attr option_solveur bloc_lecture option_solveur 0 not_set

// printOn
Sortie& Solv_AMG::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

/**
 * @brief Reads the configuration for the AMG solver from the input stream.
 *
 * This function parses the input stream to configure the AMG solver parameters,
 * including the relative tolerance (RTOL), absolute tolerance (ATOL), and whether
 * to print additional information (IMPR). It supports different solver libraries
 * based on the available hardware (CPU or GPU).
 *
 * The expected input format is:
 *   amg solver { rtol value [impr] }
 *
 * @param is The input stream from which to read the solver configuration.
 * @return The input stream after reading the configuration.
 *
 * @throws Process::exit if the input syntax is incorrect or if an unsupported
 * library is specified.
 */
Entree& Solv_AMG::readOn(Entree& is)
{
  // amg solver { rtol value [impr] }
  double rtol=0;
  double atol=0;
  bool impr = false;
  Nom library;
  Nom options_petsc("");
  Motcle motcle;
  is >> motcle;
  while (motcle != "}")
    {
      if (motcle=="GCP" || motcle=="BICGSTAB" || motcle=="GMRES") chaine_lue_=motcle;
      else if (motcle=="{") {}
      else if (motcle=="RTOL") is >> rtol;
      else if (motcle=="ATOL") is >> atol;
      else if (motcle=="IMPR") impr = true;
      else options_petsc+=motcle;
      is >> motcle;
    }
  // We select the more efficient/robust one:
#if defined(TRUST_USE_CUDA) || defined(TRUST_USE_ROCM)
  library = "petsc_gpu";
  chaine_lue_ += " { precond boomeramg { }"; // Best GPU solver
#else
  library = "petsc";
  chaine_lue_ += " { precond boomeramg { }"; // Best CPU solver
#endif
  const char* value = std::getenv("ROCM_ARCH");
  if (value != nullptr && std::string(value) == "gfx1100")
    {
      library = "petsc_gpu";
      chaine_lue_ += " { precond sa-amg { }";    // GPU Solver for gfx1100 (Hypre crash)
    }
  /*
  #if defined(MPIX_CUDA_AWARE_SUPPORT) && (OMPI_MAJOR_VERSION == 4)
  library = "amgx";
  chaine_lue_ += " { precond sa-amg { }";    // Best GPU solver on Nvidia if MPI-GPU Aware OpenMPI 4.x (Hypre diverge...)
  #endif
  */
  if (rtol>0)
    {
      chaine_lue_ += " rtol ";
      chaine_lue_ += Nom(rtol, "%e");
    }
  if (atol>0)
    {
      chaine_lue_ += " atol ";
      chaine_lue_ += Nom(atol, "%e");
    }
  if (impr) chaine_lue_ += " impr";
  if (options_petsc!="")
    {
      chaine_lue_ += " ";
      chaine_lue_ += options_petsc;
    }
  chaine_lue_ += " }";
  Cerr << "====================================================================" << finl;
  Cerr << "Creating AMG solver: " << library << " " << chaine_lue_ << finl;
  Cerr << "====================================================================" << finl;
  EChaine entree(chaine_lue_);
  Nom nom_solveur("Solv_");
  nom_solveur+=library;
  solveur_.typer(nom_solveur);
  if (library=="amgx")
    ref_cast(Solv_AMGX, solveur_.valeur()).create_solver(entree);
  else if (library=="petsc")
    ref_cast(Solv_Petsc, solveur_.valeur()).create_solver(entree);
  else if (library=="petsc_gpu")
    ref_cast(Solv_Petsc_GPU, solveur_.valeur()).create_solver(entree);
  else
    Process::exit("Unsupported case in Solv_AMG::readOn");
  return is;
}
