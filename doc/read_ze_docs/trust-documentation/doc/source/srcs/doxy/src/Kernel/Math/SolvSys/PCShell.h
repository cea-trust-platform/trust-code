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

#include <PCShell_base.h>
#include <TRUST_Deriv.h>

/*! @brief class PCShell Un PCShell represente n'importe qu'elle classe
 *
 *   derivee de la classe PCShell_base
 *
 *
 * @sa Jacobi PCShell_base
 */
class PCShell : public DERIV(PCShell_base)
{
  Declare_instanciable(PCShell);
public:
#ifdef PETSCKSP_H

  inline PetscErrorCode setUpPC(PC, Mat, Vec);
  inline PetscErrorCode computePC(PC, Vec, Vec);
  inline PetscErrorCode destroyPC(PC);
#endif
};

#ifdef PETSCKSP_H

inline PetscErrorCode PCShell::setUpPC(PC pc, Mat pmat, Vec x)
{
  PCShell_base& p = valeur();
  return p.setUpPC_(pc, pmat, x);
}

inline PetscErrorCode PCShell::computePC(PC pc, Vec x, Vec y)
{
  PCShell_base& p = valeur();
  return p.computePC_(pc, x, y);
}

inline PetscErrorCode PCShell::destroyPC(PC pc)
{
  PCShell_base& p = valeur();
  return p.destroyPC_(pc);
}
#endif

#endif /* PCShell_included */
