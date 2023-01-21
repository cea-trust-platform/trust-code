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

#ifndef Eval_Diff_VDF_included
#define Eval_Diff_VDF_included

#include <Champ_Uniforme.h>
#include <Ref_Champ_base.h>
#include <Champ_base.h>

class Eval_Diff_VDF
{
public:
  virtual ~Eval_Diff_VDF() { }

  inline const int& is_var() const { return is_var_; }

  inline virtual const Champ_base& diffusivite() const final { return get_diffusivite(); }

  inline virtual const Champ_base& get_diffusivite() const final
  {
    assert(ref_diffusivite_.non_nul());
    return ref_diffusivite_.valeur();
  }

  inline virtual void associer(const Champ_base& diffu)
  {
    ref_diffusivite_ = diffu;
    is_var_ = sub_type(Champ_Uniforme, diffu) ? 0 : 1;
    tab_diffusivite_.ref(diffu.valeurs());
  }

  virtual void mettre_a_jour() // surcharger pour Multi-incos ...
  {
    ref_diffusivite_->valeurs().echange_espace_virtuel();
    tab_diffusivite_.ref(ref_diffusivite_->valeurs());
  }

  // Methods used by the flux computation in template class:
  inline double compute_heq_impl(double d0, int i, double d1, int j, int compo) const
  {
    return 1. / (d0 / tab_diffusivite_(is_var_ * i, compo) + d1 / tab_diffusivite_(is_var_ * j, compo));
  }

  inline double nu_1_impl(int i, int compo) const { return tab_diffusivite_(is_var_ * i, compo); }
  inline double nu_2_impl(int i, int compo) const { return tab_diffusivite_(is_var_ * i, compo); }

  inline double nu_1_impl_face(int i, int j, int compo) const
  {
    return 0.5 * (tab_diffusivite_(is_var_ * i, compo) + tab_diffusivite_(is_var_ * j, compo));
  }

  inline double nu_2_impl_face(int i, int j, int k, int l, int compo) const
  {
    return 0.25 * (tab_diffusivite_(is_var_ * i, compo) + tab_diffusivite_(is_var_ * j, compo) + tab_diffusivite_(is_var_ * k, compo) + tab_diffusivite_(is_var_ * l, compo));
  }

  inline double nu_lam_impl_face(int i, int j, int k, int l, int compo) const { return nu_2_impl_face(i, j, k, l, compo); }
  inline double nu_lam_impl_face2(int i, int j, int compo) const { return nu_1_impl_face(i, j, compo); }

  // These methods will be overloaded in DIFT operators (See Eval_Dift_VDF_const_Elem for example ...)
  inline int get_ind_Fluctu_Term() const { return 0; }
  inline double get_dv_mvol(const int i) const { throw; } /* seulement pour K-Eps */
  inline virtual double get_equivalent_distance(int boundary_index,int local_face) const { return 0; }
  inline double nu_t_impl(int i, int compo) const { return 0.; }
  inline double tau_tan_impl(int i, int j) const { return 0.; }
  inline bool uses_wall() const { return false; }
  inline bool uses_mod() const { return false; }
  inline const DoubleTab& get_k_elem() const { throw; } // pour F5 seulement ...

protected:
  int is_var_ = 0;
  REF(Champ_base) ref_diffusivite_;
  DoubleTab tab_diffusivite_;
};

#endif /* Eval_Diff_VDF_included */
