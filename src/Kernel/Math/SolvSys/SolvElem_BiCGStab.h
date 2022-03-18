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
// File:        SolvElem_BiCGStab.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SolvElem_BiCGStab_included
#define SolvElem_BiCGStab_included

#include <SolvElem_base.h>
#include <TRUSTVect.h>
#include <SPV2.h>

class SolvElem_BiCGStab: public SolvElem_base
{
  Declare_instanciable(SolvElem_BiCGStab);
public:

  inline void iteration(int n, const DoubleVect& secmem, DoubleVect& solution, ArrOfInt& ipar, ArrOfDouble& fpar, ArrOfDouble& w) override;
  inline ArrOfDouble& dimensionne_wks(int n, ArrOfDouble& w) override;
};

inline void SolvElem_BiCGStab::iteration(int n, const DoubleVect& secmem, DoubleVect& solution, ArrOfInt& ipar, ArrOfDouble& fpar, ArrOfDouble& w)
{
  F77NAME(BCGSTABV2)(&n, secmem.addr(), solution.addr(), ipar.addr(), fpar.addr(), w.addr());
}

inline ArrOfDouble& SolvElem_BiCGStab::dimensionne_wks(int n, ArrOfDouble& w)
{
  w.resize_array(8 * n);
  return w;
}

#endif
