/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef PCShell_Jacobi_included
#define PCShell_Jacobi_included

#include <PCShell_base.h>
#include <petsc_for_kernel.h>
#ifdef PETSCKSP_H
#include <petscksp.h>
#endif

/* Reproduction of the Jacobi preconditioner as example for pcshell */
class PCShell_Jacobi : public PCShell_base
{
  Declare_instanciable_sans_constructeur_ni_destructeur(PCShell_Jacobi);

public :
#ifdef PETSCKSP_H
  PCShell_Jacobi(): diag_(0)
  {}

  virtual ~PCShell_Jacobi()
  {
    VecDestroy(&diag_);
  }

  Vec get_diag() const
  {
    return diag_;
  }

  PetscErrorCode setUpPC_(PC, Mat, Vec) override;
  PetscErrorCode computePC_(PC, Vec, Vec) override;
  PetscErrorCode destroyPC_(PC) override;
#endif

protected :

#ifdef PETSCKSP_H
  Vec diag_;
#endif
} ;


#endif /* PCShell_Jacobi_included */
