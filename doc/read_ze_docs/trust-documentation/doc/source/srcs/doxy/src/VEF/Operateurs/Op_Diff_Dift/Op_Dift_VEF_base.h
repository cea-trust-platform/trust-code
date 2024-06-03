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

#ifndef Op_Dift_VEF_base_included
#define Op_Dift_VEF_base_included

#include <Op_Diff_Turbulent_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Op_Diff_VEF_base.h>
#include <TRUST_Ref.h>

class Modele_turbulence_scal_base;

class Op_Dift_VEF_base : public Op_Diff_VEF_base, public Op_Diff_Turbulent_base
{
  Declare_base(Op_Dift_VEF_base);
public:

  double calculer_dt_stab() const override;
  void calculer_pour_post(Champ& espace_stockage, const Nom& option, int comp) const override;

  void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = diffu; }
  inline const Champ_base& diffusivite() const override { return diffusivite_.valeur(); }

  inline double diffusivite(int) const
  {
    return (diffusivite().valeurs().nb_dim() == 1) ? (diffusivite().valeurs())(0) : (diffusivite().valeurs())(0, 0);
  }

  inline void dimensionner(Matrice_Morse& matrice) const override
  {
    Op_VEF_Face::dimensionner(le_dom_vef.valeur(), la_zcl_vef.valeur(), matrice);
  }

  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override
  {
    Op_VEF_Face::modifier_pour_Cl(le_dom_vef.valeur(), la_zcl_vef.valeur(), matrice, secmem);
  }

  void calculer_borne_locale(DoubleVect& ,double ,double ) const override;
  void associer_modele_turbulence(const Modele_turbulence_hyd_base& );
  void mettre_a_jour(double temps) override;
  void associer(const Domaine_dis& , const Domaine_Cl_dis& , const Champ_Inc& ) override;
  void completer() override;

  const DoubleTab& get_tau_tan() const { return tau_tan_; }
  const Modele_turbulence_hyd_base& get_modele_turbulence() const { return le_modele_turbulence.valeur(); }

protected:
  REF(Modele_turbulence_hyd_base) le_modele_turbulence; // A deplacer dans Op_Diff_turb ?
  REF(Champ_base) diffusivite_;
  DoubleTab tau_tan_;

  double calculer_dt_stab_P1NCP1B() const;
};

#endif /* Op_Dift_VEF_base_included */
