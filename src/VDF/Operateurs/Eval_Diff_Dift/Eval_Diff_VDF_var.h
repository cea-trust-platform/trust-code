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

#ifndef Eval_Diff_VDF_var_included
#define Eval_Diff_VDF_var_included

#include <Champ_Uniforme.h>
#include <Ref_Champ_base.h>
#include <Eval_Diff_VDF.h>
#include <Champ_base.h>
#include <Zone_VDF.h>

// .DESCRIPTION class Eval_Diff_VDF_var
// Cette classe represente un evaluateur de flux diffusif
// avec un coefficient de diffusivite qui n'est pas constant en espace.
//.SECTION voir aussi Evaluateur_VDF
class Eval_Diff_VDF_var : public Eval_Diff_VDF
{
public:
  inline void associer(const Champ_base&) override;
  inline void mettre_a_jour() override;
  inline const Champ_base& get_diffusivite() const override;
  inline const Champ_base& diffusivite() const { return get_diffusivite(); }

  // Methods used by the flux computation in template class:
  inline double nu_1_impl(int i, int compo) const { return dv_diffusivite(i); }
  inline double nu_2_impl(int i, int compo) const { return dv_diffusivite(i); }
  inline double nu_t_impl(int i, int compo) const { return 0.; }
  inline double nu_lam_impl_face(int i, int j, int k, int l, int compo) const { return nu_2_impl_face(i,j,k,l,compo); }
  inline double nu_lam_impl_face2(int i, int j, int compo) const { return nu_1_impl_face(i,j,compo); }
  inline double tau_tan_impl(int i, int j) const { return 0.; }
  inline bool uses_wall() const { return false; }
  inline bool uses_mod() const { return false; }
  inline const DoubleTab& get_k_elem() const { throw; } // pour F5 seulement ...

  inline double compute_heq_impl(double d0, int i, double d1, int j, int compo) const
  {
    return 1./(d0/dv_diffusivite(i) + d1/dv_diffusivite(j));
  }

  inline double nu_1_impl_face(int i, int j, int compo) const { return 0.5*(dv_diffusivite(i)+dv_diffusivite(j)); }
  inline double nu_2_impl_face(int i, int j, int k, int l, int compo) const
  {
    return 0.25*(dv_diffusivite(i)+dv_diffusivite(j)+dv_diffusivite(k)+dv_diffusivite(l));
  }

protected:
  REF(Champ_base) diffusivite_;
  DoubleVect dv_diffusivite;
};

inline void Eval_Diff_VDF_var::associer(const Champ_base& diffu)
{
  diffusivite_ = diffu;
  dv_diffusivite.ref(diffu.valeurs());
}

inline void Eval_Diff_VDF_var::mettre_a_jour()
{
  diffusivite_->valeurs().echange_espace_virtuel();
  dv_diffusivite.ref(diffusivite_->valeurs());
}

inline const Champ_base& Eval_Diff_VDF_var::get_diffusivite() const
{
  assert(diffusivite_.non_nul());
  return diffusivite_.valeur();
}

#endif /* Eval_Diff_VDF_var_included */
