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
// File:        Sparskit.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#include <Sparskit.h>
#include <TRUSTVect.h>

extern "C"
{
  // distdot renomme distdot2 dans SPARSKIT car conflit avec distdot
  // livre avec la librairie PARMS ($TRUST_ROOT/lib/src/LIBNP)
#ifndef F77_Majuscule
  double F77NAME(distdot2)(const integer* const n, const double* const x,
                           const integer* const ix, const double* const y,
                           const integer* const iy)
  {
    double resu=0.;
    resu = F77NAME(ddot)(n,x,ix,y,iy);
    return Process::mp_sum(resu);
  }
#else
  double F77NAME(DISTDOT2)(const integer* const a, const double* const b,
                           const integer* const c, const double* const d,
                           const integer* const e)
  {
    double resu=0.;
    resu = F77NAME(DDOT)(a,b,c,d,e);
    return Process::mp_sum(resu);
  }
#endif
}
