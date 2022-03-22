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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Precond_local.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Precond_local_included
#define Precond_local_included

#include <Matrice_Morse_Sym.h>
#include <Ref_Matrice_Base.h>
#include <Precond_base.h>
#include <SolveurSys.h>

class Precond_local : public Precond_base
{
  Declare_instanciable(Precond_local);

protected :
  void prepare_(const Matrice_Base&, const DoubleVect&) override;
  int preconditionner_(const Matrice_Base&, const DoubleVect&, DoubleVect&) override;
  int precond(const Matrice_Base&, const DoubleVect&, DoubleVect&);

  SolveurSys le_precond_local_;
  Matrice_Morse_Sym la_matrice_locale_;
  Matrice_Morse_Sym matrice_de_travail_;

  REF(Matrice_Base) matrice_a_resoudre_;

  void res_syst_loc_simple(const Matrice_Morse_Sym& mat, const DoubleVect& secmem, DoubleVect& solution, const Champ_Inc_base& champ);
  void res_syst_loc_hybride(const Matrice_Morse_Sym& mat, const DoubleVect& secmem, DoubleVect& solution, const Champ_Inc_base& champ);
};

#endif /* Precond_local_included */
