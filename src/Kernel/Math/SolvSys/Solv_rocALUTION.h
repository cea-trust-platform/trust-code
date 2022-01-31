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
// File:        Solv_rocALUTION.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_rocALUTION_included
#define Solv_rocALUTION_included

#include <SolveurSys_base.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <rocalution.hpp>
#pragma GCC diagnostic pop
using namespace rocalution;

class Solv_rocALUTION : public SolveurSys_base
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION);

public :
  Solv_rocALUTION();
  ~Solv_rocALUTION() override;
  inline int solveur_direct() const override { return 0; };
  inline int resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x, int niter_max) override { return resoudre_systeme(a,b,x); };

  int resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x) override;

protected :
  LocalVector<double> sol;
  LocalVector<double> rhs;
  LocalVector<double> res;
  LocalMatrix<double> mat;
};
#endif


