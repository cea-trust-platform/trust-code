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

#ifndef PCShell_included
#define PCShell_included

#include <petsc_for_kernel.h>
#ifdef PETSCKSP_H
#include <petscksp.h>
#endif

/* Define context for user-provided preconditioner */
class PCShell
{
public :
#ifdef PETSCKSP_H
  PCShell(): diag_(0)
  {}

  virtual ~PCShell()
  {
    VecDestroy(&diag_);
  }

  virtual PetscErrorCode setUpPC(PC, Mat, Vec);
  virtual PetscErrorCode computePC(PC, Vec, Vec);
  virtual PetscErrorCode destroyPC(PC);
#endif

protected :

#ifdef PETSCKSP_H
  Vec diag_;
#endif
} ;

/* Struct to associate Petsc preconditionner to user-provided preconditioner */
typedef struct
{
  PCShell* pc_shell_ptr;
} PCstruct;


//inline PetscErrorCode PCShellUserApply(PC pc_apply, Vec x, Vec y)
//{
//  PCstruct *shell;
//
//  PCShellGetContext(pc_apply,(void**) &shell);
//  PCShell *ptr=shell->pc_shell_ptr;
//  return ptr->computePC(pc_apply,x,y);
//}
//
//inline PetscErrorCode PCShellUserDestroy(PC pc_apply)
//{
//  PCstruct *shell;
//
//  PCShellGetContext(pc_apply,(void**) &shell);
//  PCShell *ptr=shell->pc_shell_ptr;
//  return ptr->destroyPC(pc_apply);
//}


#endif /* PCShell_included */
