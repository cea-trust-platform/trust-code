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

#ifndef Eval_Dift_VDF_included
#define Eval_Dift_VDF_included

#include <Ref_Turbulence_paroi_scal.h>
#include <Turbulence_paroi_scal.h>
#include <Ref_Champ_Fonc.h>
#include <Eval_Diff_VDF.h>
#include <Champ_Fonc.h>
#include <TRUSTVects.h>

class Eval_Dift_VDF : public Eval_Diff_VDF
{
public:
  inline void mettre_a_jour() override
  {
    Eval_Diff_VDF::mettre_a_jour();
    update_equivalent_distance();
  }

  inline double get_equivalent_distance(int boundary_index, int local_face) const override
  {
    return equivalent_distance[boundary_index](local_face);
  }

  inline double compute_heq_impl(double d0, int i, double d1, int j, int compo) const
  {
    const double heq_lam = Eval_Diff_VDF::compute_heq_impl(d0, i, d1, j, compo);
    const double heq_turb = 0.5 * (tab_diffusivite_turbulente(i, is_multi_ * compo) + tab_diffusivite_turbulente(j, is_multi_ * compo)) / (d1 + d0);
    return heq_lam + heq_turb;
  }

  inline double nu_t_impl(int i, int compo) const { return tab_diffusivite_turbulente(i, is_multi_ * compo); }

  inline double nu_lam_impl_face(int i, int j, int k, int l, int compo) const
  {
    return Eval_Diff_VDF::nu_2_impl_face(i, j, k, l, compo);
  }

  inline double nu_lam_impl_face2(int i, int j, int compo) const
  {
    return Eval_Diff_VDF::nu_1_impl_face(i, j, compo);
  }

  inline double nu_1_impl(int i, int compo) const
  {
    const double nu_lam = Eval_Diff_VDF::nu_1_impl(i, compo);
    const double nu_turb = tab_diffusivite_turbulente(i, is_multi_ * compo);
    return nu_lam + nu_turb;
  }

  inline double nu_2_impl(int i, int compo) const
  {
    return Eval_Diff_VDF::nu_2_impl(i, compo);
  }

  inline double nu_1_impl_face(int i, int j, int compo) const
  {
    return 0.5 * (tab_diffusivite_turbulente(i, is_multi_ * compo) + tab_diffusivite_turbulente(j, is_multi_ * compo));
  }

  inline double nu_2_impl_face(int i, int j, int k, int l, int compo) const
  {
    return 0.25 * (tab_diffusivite_turbulente(i, is_multi_ * compo) + tab_diffusivite_turbulente(j, is_multi_ * compo) +
                   tab_diffusivite_turbulente(k, is_multi_ * compo) + tab_diffusivite_turbulente(l, is_multi_ * compo));
  }

  void update_equivalent_distance()
  {
    if (loipar.non_nul())
      {
        int s = loipar->tab_equivalent_distance_size();
        equivalent_distance.dimensionner(s);
        for (int i = 0; i < s; i++) equivalent_distance[i].ref(loipar->tab_equivalent_distance(i));
      }
  }

  inline const Champ_Fonc& diffusivite_turbulente() const { return ref_diffusivite_turbulente_.valeur(); }

  inline void associer_diff_turb(const Champ_Fonc& diff_turb)
  {
    ref_diffusivite_turbulente_ = diff_turb;
    tab_diffusivite_turbulente.ref(diff_turb.valeurs());
    is_multi_ = (diff_turb.valeurs().dimension(1) > 1) ? 1 : 0;
  }

  inline virtual void associer_loipar(const Turbulence_paroi_scal& loi_paroi) { loipar = loi_paroi; }
  inline virtual void init_ind_fluctu_term() { /* do nothing */}

protected:
  int is_multi_ = 0;
  REF(Champ_Fonc) ref_diffusivite_turbulente_;
  REF(Turbulence_paroi_scal) loipar;
  DoubleVects equivalent_distance;
  DoubleTab tab_diffusivite_turbulente;
};

#endif /* Eval_Dift_VDF_included */
