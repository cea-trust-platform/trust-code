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

#ifndef Op_Dift_VDF_Elem_leaves_included
#define Op_Dift_VDF_Elem_leaves_included

#include <Op_Dift_VDF_Elem_base.h>
#include <Eval_Dift_VDF_leaves.h>
#include <Op_Diff_Dift_VDF.h>

/// \cond DO_NOT_DOCUMENT
class Op_Dift_VDF_Elem_leaves
{ };
/// \endcond

class Turbulence_paroi_scal;
class Champ_Fonc;
class Champ_base;

class Op_Dift_VDF_Elem : public Op_Dift_VDF_Elem_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Elem);
public:
  Op_Dift_VDF_Elem();
  inline double calculer_dt_stab() const override { return calculer_dt_stab_elem(); }
  inline double alpha_(const int i) const override { return alpha_impl<Eval_Dift_VDF_Elem>(i); }
  inline void associer_loipar(const Turbulence_paroi_scal& lp ) { associer_loipar_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem>(lp); }
  inline void associer(const Domaine_dis& zd, const Domaine_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem>(ch); }
  inline void associer_diffusivite(const Champ_base& ch ) override { associer_diffusivite_impl<Eval_Dift_VDF_Elem>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_Elem>(); }
  inline void completer() override
  {
    completer_impl<Type_Operateur::Op_DIFT_ELEM, Eval_Dift_VDF_Elem>();
    associer_pb<Eval_Dift_VDF_Elem>(equation().probleme());
  }
};

// ===========================================================================================================================================

class Op_Dift_VDF_Elem_Axi : public Op_Dift_VDF_Elem_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_Elem_Axi>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Elem_Axi);
public:
  Op_Dift_VDF_Elem_Axi();
  inline double calculer_dt_stab() const override { return calculer_dt_stab_elem_axi(); }
  inline double alpha_(const int i) const override { return alpha_impl<Eval_Dift_VDF_Elem_Axi>(i); }
  inline void associer_loipar(const Turbulence_paroi_scal& lp ) { associer_loipar_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem_Axi>(lp); }
  inline void associer(const Domaine_dis& zd, const Domaine_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem_Axi>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Elem_Axi>(ch); }
  inline void associer_diffusivite(const Champ_base& ch ) override { associer_diffusivite_impl<Eval_Dift_VDF_Elem_Axi>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_Elem_Axi>(); }
  inline void completer() override
  {
    completer_impl<Type_Operateur::Op_DIFT_ELEM, Eval_Dift_VDF_Elem_Axi>();
    associer_pb<Eval_Dift_VDF_Elem_Axi>(equation().probleme());
  }
};

// ===========================================================================================================================================

class Op_Dift_VDF_Multi_inco_Elem : public Op_Dift_VDF_Elem_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_Multi_inco_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Multi_inco_Elem);
public:
  Op_Dift_VDF_Multi_inco_Elem();
  inline double calculer_dt_stab() const override { return calculer_dt_stab_elem(); }
  inline double alpha_(const int i) const override
  {
    const DoubleTab& alpha = diffusivite_pour_pas_de_temps().valeurs();
    const int is_var = sub_type(Champ_Uniforme, diffusivite()) ? 0 : 1;
    double alpha_lam = alpha(is_var * i,0);
    for (int k = 1; k < alpha.line_size(); k++) alpha_lam = std::max(alpha_lam, alpha(is_var * i,k));
    return alpha_lam;
  }
  inline void associer_loipar(const Turbulence_paroi_scal& lp ) { associer_loipar_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem>(lp); }
  inline void associer(const Domaine_dis& zd, const Domaine_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem>(ch); }
  inline void associer_diffusivite(const Champ_base& ch ) override { associer_diffusivite_impl<Eval_Dift_VDF_Multi_inco_Elem>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_Multi_inco_Elem>(); }
  inline void completer() override
  {
    completer_impl<Type_Operateur::Op_DIFT_ELEM, Eval_Dift_VDF_Multi_inco_Elem>();
    associer_pb<Eval_Dift_VDF_Multi_inco_Elem>(equation().probleme());
  }
};

// ===========================================================================================================================================

class Op_Dift_VDF_Multi_inco_Elem_Axi : public Op_Dift_VDF_Elem_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_Multi_inco_Elem_Axi>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Multi_inco_Elem_Axi);
public:
  Op_Dift_VDF_Multi_inco_Elem_Axi();
  inline double calculer_dt_stab() const override { return calculer_dt_stab_elem_axi(); }
  inline double alpha_(const int i) const override
  {
    const DoubleTab& alpha = diffusivite().valeurs();
    const int is_var = sub_type(Champ_Uniforme, diffusivite()) ? 0 : 1;
    double alpha_lam = alpha(is_var * i,0);
    for (int k = 1; k < alpha.line_size(); k++) alpha_lam = std::max(alpha_lam, alpha(i,k));
    return alpha_lam;
  }

  inline void associer_loipar(const Turbulence_paroi_scal& lp ) { associer_loipar_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem_Axi>(lp); }
  inline void associer(const Domaine_dis& zd, const Domaine_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem_Axi>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_ELEM,Eval_Dift_VDF_Multi_inco_Elem_Axi>(ch); }
  inline void associer_diffusivite(const Champ_base& ch ) override { associer_diffusivite_impl<Eval_Dift_VDF_Multi_inco_Elem_Axi>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_Multi_inco_Elem_Axi>(); }
  inline void completer() override
  {
    completer_impl<Type_Operateur::Op_DIFT_ELEM, Eval_Dift_VDF_Multi_inco_Elem_Axi>();
    associer_pb<Eval_Dift_VDF_Multi_inco_Elem_Axi>(equation().probleme());
  }
};

#endif /* Op_Dift_VDF_Elem_leaves_included */
