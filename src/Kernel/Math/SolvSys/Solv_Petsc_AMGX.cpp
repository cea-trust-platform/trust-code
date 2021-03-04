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
// File:        Solv_Petsc_AMGX.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/95
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_Petsc_AMGX.h>

Implemente_instanciable_sans_constructeur(Solv_Petsc_AMGX,"Solv_Petsc_AMGX",Solv_Petsc);

// printOn
Sortie& Solv_Petsc_AMGX::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_Petsc_AMGX::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

// Creation des objets PETSc
void Solv_Petsc_AMGX::Create_objects(const Matrice_Morse& mat, const DoubleVect&)
{
#ifdef PETSC_HAVE_CUDA
  // Creation de la matrice Petsc si necessaire
  if (!read_matrix_)
    Create_MatricePetsc(MatricePetsc_, mataij_, mat);

  if (!amgx_initialized())
    {
      Nom filename(Objet_U::nom_du_cas());
      filename += ".amgx";
      Nom AmgXmode = "dDDI"; // dDDI:GPU hDDI:CPU (not supported yet by AmgXWrapper)
      /* Possible de jouer avec simple precision peut etre:
      1. (lowercase) letter: whether the code will run on the host (h) or device (d).
      2. (uppercase) letter: whether the matrix precision is float (F) or double (D).
      3. (uppercase) letter: whether the vector precision is float (F) or double (D).
      4. (uppercase) letter: whether the index type is 32-bit int (I) or else (not currently supported).
      typedef enum { AMGX_mode_hDDI, AMGX_mode_hDFI, AMGX_mode_hFFI, AMGX_mode_dDDI, AMGX_mode_dDFI, AMGX_mode_dFFI } AMGX_Mode; */
      Cerr << "Initializing Amgx and reading the " << filename << " file." << finl;
      SolveurAmgX_.initialize(PETSC_COMM_WORLD, AmgXmode.getString(), filename.getString());
      amgx_initialized_ = true;
    }
  SolveurAmgX_.setA(MatricePetsc_);
#endif
}
int Solv_Petsc_AMGX::solve(ArrOfDouble& residu)
{
#ifdef PETSC_HAVE_CUDA
  int nbiter = -1;
  SolveurAmgX_.solve(SolutionPetsc_, SecondMembrePetsc_);
  SolveurAmgX_.getIters(nbiter);
  if (limpr() > -1)
    {
      SolveurAmgX_.getResidual(0, residu(0));
      SolveurAmgX_.getResidual(nbiter - 1, residu(nbiter));
    }
  return nbiter;
#endif
}
