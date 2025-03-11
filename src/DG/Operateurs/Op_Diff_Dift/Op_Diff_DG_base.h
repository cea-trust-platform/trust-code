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

#ifndef Op_Diff_DG_base_included
#define Op_Diff_DG_base_included

#include <Op_Diff_Turbulent_base.h>
#include <Operateur_Diff_base.h>
#include <Domaine_DG.h>
#include <TRUST_Ref.h>
#include <SFichier.h>

class Domaine_Cl_DG;

class Op_Diff_DG_base: public Operateur_Diff_base, public Op_Diff_Turbulent_base
{
  Declare_base(Op_Diff_DG_base);
public:
  void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc&) override;

  double calculer_dt_stab() const override;

  void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = diffu; }
  void completer() override;
  const Champ_base& diffusivite() const override { return diffusivite_.valeur(); }
  void mettre_a_jour(double t) override
  {
    Operateur_base::mettre_a_jour(t);
    nu_a_jour_ = 0;
  }

  void update_nu() const; //met a jour nu

  DoubleTab& calculer(const DoubleTab&, DoubleTab&) const override;
  int impr(Sortie& os) const override;

protected:
  REF(Domaine_DG) le_dom_dg_;
  REF(Domaine_Cl_DG) la_zcl_dg_;
  mutable SFichier Flux, Flux_moment, Flux_sum; // Fichiers .out

  REF(Champ_base) diffusivite_;
  mutable int nu_a_jour_ = 0; //si on doit mettre a jour nu
  mutable DoubleTab nu_;
};


#endif /* Op_Diff_DG_base_included */
