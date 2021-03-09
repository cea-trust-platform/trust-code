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
// File:        Eval_Diff_VDF_Multi_inco_const2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_Multi_inco_const2_included
#define Eval_Diff_VDF_Multi_inco_const2_included

#include <Eval_Diff_VDF2.h>
#include <Champ_Face.h>
#include <Champ_Uniforme.h>
#include <Ref_Champ_Uniforme.h>

class Champ_base;

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const2
//
// Cette classe represente un evaluateur de flux diffusif
// pour un vecteur d'inconnues avec une diffusivite par
// inconnue. Le champ de diffusivite associe a chaque inconnue
// est constant.

//
// .SECTION voir aussi Evaluateur_VDF
//

class Eval_Diff_VDF_Multi_inco_const2 : public Eval_Diff_VDF2
{
public:

  inline void associer(const Champ_base& );
  inline const Champ_base& get_diffusivite() const;

  virtual inline void mettre_a_jour() {}

  // Methods used by the flux computation in template class:
  inline double nu_1_impl(int i, int compo) const
  {
    return dv_diffusivite(compo);
  }

  inline double nu_2_impl(int i, int compo) const
  {
    return dv_diffusivite(compo);
  }

  inline double compute_heq_impl(double d0, int i, double d1, int j, int compo) const
  {
    return dv_diffusivite(compo)/(d0+d1);
  }

  inline double nu_1_impl_face(int i, int j) const
  {
    return dv_diffusivite(j);
  }

  inline double nu_2_impl_face(int i, int j, int k, int l) const
  {
    return dv_diffusivite(j);
  }

protected:

  REF(Champ_Uniforme) diffusivite_;
  DoubleVect dv_diffusivite;  // nb of components = nb of multi-inco

};

inline void Eval_Diff_VDF_Multi_inco_const2::associer(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
  dv_diffusivite.ref(diffu.valeurs());
}
inline const Champ_base& Eval_Diff_VDF_Multi_inco_const2::get_diffusivite() const
{
  return diffusivite_;
}

#endif /* Eval_Diff_VDF_Multi_inco_const2_included */
