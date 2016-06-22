/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        PrecondA.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PrecondA_included
#define PrecondA_included

#include <ArrOfInt.h>
#include <Precond.h>

class Matrice_Morse_Sym;
class Matrice_Bloc_Sym;

class PrecondA : public Precond_base
{
  Declare_instanciable_sans_constructeur(PrecondA);
public:
  PrecondA();
  int supporte_matrice_morse_sym()
  {
    return 0;
  }; // Matrice_Morse_Sym non supporte

protected:
  int preconditionner_(const Matrice_Base&, const DoubleVect& src, DoubleVect& solution);
  void   prepare_(const Matrice_Base&, const DoubleVect& src);

  Precond le_precond_0;
  Precond le_precond_1;
  Precond le_precond_a;
  double alpha_0;
  double alpha_1;
  double alpha_a;
};

#endif
