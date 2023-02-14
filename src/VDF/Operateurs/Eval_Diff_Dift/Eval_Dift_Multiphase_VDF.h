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

#ifndef Eval_Dift_Multiphase_VDF_included
#define Eval_Dift_Multiphase_VDF_included

#include <Eval_Dift_VDF.h>
#include <Correlation.h>

class Eval_Dift_Multiphase_VDF : public Eval_Dift_VDF
{
public:
  void associer_corr(const Correlation& c ) { corr_ = c; is_multi_ = 1; /* XXX */ }

  inline void mettre_a_jour() override
  {
    Eval_Diff_VDF::mettre_a_jour();
    update_diffusivite_turbulente();
  }

  inline void set_nut(const DoubleTab& nut)
  {
    tab_diffusivite_turbulente = nut;
  }

  inline virtual void update_diffusivite_turbulente() final
  {
    tab_alpha_.ref(ref_cast(Pb_Multiphase, ref_probleme_.valeur()).eq_masse.inconnue().passe());
    for (int e = 0; e < tab_diffusivite_turbulente.dimension(0); e++)
      for (int n = 0; n < tab_diffusivite_turbulente.dimension(1); n++)
        tab_diffusivite_turbulente(e, n) = std::max(tab_alpha_(e, n), 1e-8) * ref_diffusivite_->valeurs()(is_var_ * e, n);

    tab_diffusivite_turbulente.echange_espace_virtuel();
  }

  const Champ_Fonc& diffusivite_turbulente() const { throw; }

  void associer_diff_turb(const Champ_Fonc& diff_turb) { throw; }

  void associer_loipar(const Turbulence_paroi_scal& loi_paroi) override { throw; }

  // pour CRTP : TODO : appel depuis eval mere
  inline const Correlation& get_corr() const { return corr_; }

protected:
  REF(Correlation) corr_; //attention REF + DERIV => 2 valeur() deso
};

#endif /* Eval_Dift_Multiphase_VDF_included */
