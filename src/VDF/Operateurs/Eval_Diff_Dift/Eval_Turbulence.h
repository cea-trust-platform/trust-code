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

#ifndef Eval_Turbulence_included
#define Eval_Turbulence_included

#include <Ref_Turbulence_paroi_scal.h>
#include <Turbulence_paroi_scal.h>
#include <Ref_Champ_Fonc.h>
#include <Champ_Fonc.h>
#include <TRUSTVects.h>

/*! @brief class Eval_Turbulence Implements all stuff related to turbulence for VDF evaluators.
 *
 */
class Eval_Turbulence
{

public:
  virtual ~Eval_Turbulence() { }

  inline const Champ_Fonc& diffusivite_turbulente() const { return ref_diffusivite_turbulente_.valeur(); }

  inline void associer_diff_turb(const Champ_Fonc& diff_turb)
  {
    ref_diffusivite_turbulente_ = diff_turb;
    dv_diffusivite_turbulente.ref(diff_turb.valeurs());
  }

  inline virtual void associer_loipar(const Turbulence_paroi_scal& loi_paroi) { loipar = loi_paroi; }
  inline virtual void init_ind_fluctu_term() { /* do nothing */}
  void update_equivalent_distance() ;

protected:
  REF(Champ_Fonc) ref_diffusivite_turbulente_;
  REF(Turbulence_paroi_scal) loipar;
  DoubleVects equivalent_distance;
  DoubleVect dv_diffusivite_turbulente;
};

#endif /* Eval_Turbulence_included */
