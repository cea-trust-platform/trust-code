/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef SolveurSys_included
#define SolveurSys_included

//
// .DESCRIPTION class SolveurSys
//  Un SolveurSys represente n'importe qu'elle classe
//  derivee de la classe SolveurSys_base
//
// .SECTION voir aussi
//  Jacobi Gauss_Seidel SOR SolveurSys_base

#include <SolveurSys_base.h>
#include <Matrice_Base.h>

Declare_deriv(SolveurSys_base);

class SolveurSys : public DERIV(SolveurSys_base)
{
  Declare_instanciable(SolveurSys);
public:
  int resoudre_systeme(const Matrice_Base &matrice, const DoubleVect &secmem, DoubleVect &solution);
  int resoudre_systeme(const Matrice_Base &matrice, const DoubleVect &secmem, DoubleVect &solution, int niter_max);

  inline int supporte_matrice_morse_sym() { return valeur().supporte_matrice_morse_sym(); }
  inline const Nom& le_nom() const override { return nom_; }
  inline void nommer(const Nom& nom) override { nom_ = nom; }

private:
  Nom nom_;
};

#endif /* SolveurSys_included */
