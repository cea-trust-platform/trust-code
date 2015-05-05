/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Modifier_nut_pour_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/Turbulence
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Modifier_nut_pour_QC.h>
#include <Mod_turb_hyd_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Probleme_base.h>

void Correction_nut_et_cisaillement_paroi_si_qc(Mod_turb_hyd_base& mod)
{
  // on recgarde si on a un fluide QC
  if (sub_type(Fluide_Quasi_Compressible,mod.equation().probleme().milieu()))
    {
      const  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,mod.equation().probleme().milieu());
      // 1 on multiplie nu_t par rho

      DoubleTab& nut=ref_cast_non_const(DoubleTab, mod.viscosite_turbulente().valeurs());
      multiplier_diviser_rho(nut, le_fluide, 0 /* multiplier */);

      // 2  On modifie le ciasaillement paroi
      const DoubleTab& cisaillement_paroi=mod.loi_paroi().valeur().Cisaillement_paroi();
      DoubleTab& cisaillement=ref_cast_non_const(DoubleTab, cisaillement_paroi);
      multiplier_diviser_rho(cisaillement, le_fluide, 0 /* multiplier */);
      cisaillement.echange_espace_virtuel();
    }
}
