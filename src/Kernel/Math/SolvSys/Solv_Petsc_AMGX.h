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
// File:        Solv_Petsc_AMGX.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solv_Petsc_AMGX_included
#define Solv_Petsc_AMGX_included

#include <Solv_Petsc.h>
#ifdef PETSC_HAVE_CUDA
#include <AmgXSolver.hpp>
#include <amgx_c.h>
#endif

class Solv_Petsc_AMGX : public Solv_Petsc
{
  Declare_instanciable_sans_constructeur(Solv_Petsc_AMGX);
public :
  inline Solv_Petsc_AMGX()
  {
    amgx_=true;
  };
protected :
  virtual void Create_objects(const Matrice_Morse&, const DoubleVect&);
  virtual int solve(ArrOfDouble& residual);
  virtual void finalize()
  {
#ifdef PETSC_HAVE_CUDA
    if (amgx_initialized_)
      {
        SolveurAmgX_.finalize();
        amgx_initialized_ = false;
      }
#endif
  }
private:
#ifdef PETSC_HAVE_CUDA
  AmgXSolver SolveurAmgX_; // Instance de AmgXWrapper
#endif
};

#endif


