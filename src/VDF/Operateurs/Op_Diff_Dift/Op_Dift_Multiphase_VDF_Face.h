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

#ifndef Op_Dift_Multiphase_VDF_Face_included
#define Op_Dift_Multiphase_VDF_Face_included

#include <Op_Dift_Multiphase_proto.h>
#include <Op_Dift_VDF_Face_base.h>
#include <Eval_Dift_VDF_leaves.h>
#include <Op_Diff_Dift_VDF.h>

class Op_Dift_Multiphase_VDF_Face : public Op_Dift_VDF_Face_base, public Op_Diff_Dift_VDF<Op_Dift_Multiphase_VDF_Face>, public Op_Dift_Multiphase_proto
{
  Declare_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Face);
public:
  Op_Dift_Multiphase_VDF_Face() : Op_Dift_VDF_Face_base(Iterateur_VDF_Face<Eval_Dift_Multiphase_VDF_Face>()) { }

  void creer_champ(const Motcle& motlu) override;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;

  void completer() override;
  void mettre_a_jour(double ) override;
  void preparer_calcul() override;
  void modifier_mu(DoubleTab& ) const { throw; }
  void associer_diffusivite_turbulente(const Champ_Fonc& ch) = delete;

  double calculer_dt_stab() const override;
  bool is_turb() const override { return true; }
  const Correlation* correlation_viscosite_turbulente() const override { return &corr_; }

  inline void associer(const Domaine_dis_base& zd, const Domaine_Cl_dis_base& zcd, const Champ_Inc_base& ch) override
  {
    associer_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(zd, zcd, ch);
  }

  inline void associer_diffusivite(const Champ_base& ch) override
  {
    associer_diffusivite_impl<Eval_Dift_Multiphase_VDF_Face>(ch);
  }

  inline const Champ_base& diffusivite() const override
  {
    return diffusivite_impl<Eval_Dift_Multiphase_VDF_Face>();
  }

  inline const DoubleTab& get_diffusivite_turbulente() const
  {
    return get_diffusivite_turbulente_multiphase_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE,Eval_Dift_Multiphase_VDF_Face>();
  }
};

#endif /* Op_Dift_Multiphase_VDF_Face_included */
