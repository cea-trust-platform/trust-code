/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <PCShell_Jacobi.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(PCShell_Jacobi,"PC_Jacobi",PCShell_base);

Sortie& PCShell_Jacobi::printOn(Sortie& s ) const
{
  return PCShell_base::printOn(s);
}

Entree& PCShell_Jacobi::readOn(Entree& is )
{
  return is;
}

#ifdef PETSCKSP_H
PetscErrorCode PCShell_Jacobi::setUpPC_(PC pc, Mat pmat, Vec x)
{

  if (diag_==nullptr)
    VecDuplicate(x, &diag_);
  MatGetDiagonal(pmat, diag_);
  VecReciprocal(diag_);
  return 0;
}

PetscErrorCode PCShell_Jacobi::computePC_(PC pc, Vec x, Vec y)
{
  VecPointwiseMult(y, x, diag_);
  return 0;
}

PetscErrorCode PCShell_Jacobi::destroyPC_(PC pc)
{
  VecDestroy(&diag_);
  return 0;
}


#endif

