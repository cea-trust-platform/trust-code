/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Eval_Dift_Multiphase_VDF_included
#define Eval_Dift_Multiphase_VDF_included

#include <Correlation_base.h>
#include <Eval_Dift_VDF.h>
#include <Pb_Multiphase.h>

class Eval_Dift_Multiphase_VDF : public Eval_Dift_VDF
{
public:
  void associer_corr(const Correlation& c ) { corr_ = c; is_multi_ = 1; /* XXX */ }

  inline void mettre_a_jour() override
  {
    Eval_Diff_VDF::mettre_a_jour();
    update_diffusivite_turbulente();
  }

  inline void set_nut(const DoubleTab& nut, const bool need_ar) { nu_t_ = nut; need_alpha_rho_ = need_ar; }

  inline virtual void update_diffusivite_turbulente() final
  {
    const DoubleTab& rho = ref_probleme_->milieu().masse_volumique()->passe();
    const int cR = rho.dimension(0) == 1;
    tab_diffusivite_turbulente = nu_t_;

    if (need_alpha_rho_ && sub_type(Pb_Multiphase, ref_probleme_.valeur()))
      {
        for (int e = 0; e < nu_t_->dimension(0); e++)
          for (int n = 0; n < nu_t_->dimension(1); n++)
            tab_diffusivite_turbulente(e, n) = tab_alpha_(e, n) * rho(!cR * e, n) * nu_t_.valeur()(e, n);
      }

    tab_diffusivite_turbulente.echange_espace_virtuel();
    tab_diff_turb_first_update_ = false;
  }

  const DoubleTab& get_diffusivite_turbulente_multiphase() const
  {
    if (tab_diff_turb_first_update_)
      const_cast<Eval_Dift_Multiphase_VDF&>(*this).update_diffusivite_turbulente();

    return tab_diffusivite_turbulente;
  }

  const Champ_Fonc& diffusivite_turbulente() const { throw; }
  const DoubleTab& tab_nu_t() const { return nu_t_.valeur(); }

  void associer_diff_turb(const Champ_Fonc& diff_turb) { throw; }

  void associer_loipar(const Turbulence_paroi_scal_base& loi_paroi) override { throw; }

  // pour CRTP : TODO : appel depuis eval mere
  inline const Correlation& get_corr() const { return corr_; }

protected:
  REF(Correlation) corr_; //attention REF + OWN_PTR => 2 valeur() deso
  REF(DoubleTab) nu_t_;
  bool need_alpha_rho_ = true, tab_diff_turb_first_update_ = true;
};

#endif /* Eval_Dift_Multiphase_VDF_included */
