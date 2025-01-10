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
#include <hip/hip_version.h>
#define HIP_VERSION_MAJOR (HIP_VERSION / 100000)
#endif

Implemente_instanciable(Solv_AMG,"Solv_AMG",SolveurSys_base);

// printOn
Sortie& Solv_AMG::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

Entree& Solv_AMG::readOn(Entree& is)
{
  // amg solver { rtol value [impr] }
  double rtol=0;
  double atol=0;
  bool impr = false;
  Nom library;
  Motcle motcle;
  is >> motcle;
  while (motcle != "}")
    {
      if (motcle=="GCP" || motcle=="{") {}
      else if (motcle=="RTOL") is >> rtol;
      else if (motcle=="ATOL") is >> atol;
      else if (motcle=="IMPR") impr = true;
      else
        {
          Cerr << "Reading: " << motcle << finl;
          Process::exit("Error! Waiting syntax: GCP { rtol value [impr] }");
        }
      is >> motcle;
    }
  // We select the more efficient/robust one:
#if defined(TRUST_USE_CUDA) || defined(TRUST_USE_ROCM)
  library = "petsc_gpu";
  chaine_lue_ = "gcp { precond boomeramg { }"; // Best GPU solver
#else
  library = "petsc";
  chaine_lue_ = "gcp { precond boomeramg { }"; // Best CPU solver
#endif
#if defined(ROCM_VERSION_MAJOR) && (ROCM_VERSION_MAJOR >= 6)
  library = "petsc_gpu";
  chaine_lue_ = "gcp { precond sa-amg { }";    // GPU Solver for ROCm >= 6 (Hypre crash)
#endif
#if defined(MPIX_CUDA_AWARE_SUPPORT) && (OMPI_MAJOR_VERSION == 4)
  library = "amgx";
  chaine_lue_ = "gcp { precond sa-amg { }";    // Best GPU solver on Nvidia if MPI-GPU Aware OpenMPI 4.x (Hypre diverge...)
#endif
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
  chaine_lue_ += " }";
  Cerr << "====================================================================" << finl;
  Cerr << "Creating AMG solver: " << library << " " << chaine_lue_ << finl;
  Cerr << "====================================================================" << finl;
  EChaine entree(chaine_lue_);
  Nom nom_solveur("Solv_");
  nom_solveur+=library;
  solveur.typer(nom_solveur);
  if (library=="amgx")
    ref_cast(Solv_AMGX, solveur.valeur()).create_solver(entree);
  else if (library=="petsc")
    ref_cast(Solv_Petsc, solveur.valeur()).create_solver(entree);
  else if (library=="petsc_gpu")
    ref_cast(Solv_Petsc_GPU, solveur.valeur()).create_solver(entree);
  else
    Process::exit("Unsupported case in Solv_AMG::readOn");
  return is;
}
