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
// File:        Eval_Dift_VDF_const2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Dift_VDF_const2_included
#define Eval_Dift_VDF_const2_included

#include <Eval_Diff_VDF_const2.h>
#include <Ref_Champ_Fonc.h>
#include <Champ_Fonc.h>
//
// .DESCRIPTION class Eval_Dift_VDF_const2
//
// Cette classe represente un evaluateur de flux diffusif
// total (diffusion laminaire et turbulente)
// avec une diffusivite constante en espace.

//.SECTION
// voir aussi Eval_Diff_VDF_const2

class Eval_Dift_VDF_const2 : public Eval_Diff_VDF_const2
{

public:

  inline void associer_diff_turb(const Champ_Fonc& diff_turb)
  {
    diffusivite_turbulente_ = diff_turb;
    dv_diffusivite_turbulente.ref(diff_turb.valeurs());
  }

  inline const Champ_Fonc& diffusivite_turbulente() const
  {
    return diffusivite_turbulente_.valeur();
  }

  // Overloaded methods used by the flux computation in template class:
  inline double nu_1_impl(int i, int compo) const
  {
    //#define nu_1(i) (db_diffusivite+dv_diffusivite_turbulente(i))
    return get_diffusivite()(0,0)+dv_diffusivite_turbulente(i);
  }

  inline double nu_2_impl(int i, int compo) const
  {
    //#define nu_2(i) db_diffusivite
    return get_diffusivite()(0,0);
  }

  inline double compute_heq_impl(double d0, int i, double d1, int j, int compo) const
  {
    //#define f_heq(d0,i,d1,j) heq=(nu_2(i) +
    // 0.5*(dv_diffusivite_turbulente(i)+dv_diffusivite_turbulente(j)))/(d1+d0);
    double heq = (get_diffusivite()(0,0) +
                  0.5*(dv_diffusivite_turbulente(i)+dv_diffusivite_turbulente(j)))/(d1+d0);
    return heq;
  }

protected:

  REF(Champ_Fonc) diffusivite_turbulente_;
  DoubleVect dv_diffusivite_turbulente;

};

#endif /* Eval_Dift_VDF_const2_included */
