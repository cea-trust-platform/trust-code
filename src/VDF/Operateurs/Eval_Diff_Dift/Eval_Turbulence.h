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
// File:        Eval_Turbulence.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Diff_Dift
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Turbulence_included
#define Eval_Turbulence_included

#include <Ref_Turbulence_paroi_scal.h>
#include <Turbulence_paroi_scal.h>
#include <DoubleVects.h>

// .DESCRIPTION class Eval_Turbulence
// Implements all stuff related to turbulence for VDF evaluators.
class Eval_Turbulence
{

public:
  virtual ~Eval_Turbulence() {}
  inline virtual void associer_loipar(const Turbulence_paroi_scal& loi_paroi) { loipar = loi_paroi; }
  inline virtual void init_ind_fluctu_term() { /* do nothing */}
  void update_equivalent_distance() ;

protected:
  REF(Turbulence_paroi_scal) loipar;
  VECT(DoubleVect) equivalent_distance;
};

#endif /* Eval_Turbulence_included */
