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

#ifndef Op_Dift_VDF_Face_leaves_included
#define Op_Dift_VDF_Face_leaves_included

#include <Eval_Dift_VDF_Face_leaves.h>
#include <Op_Dift_VDF_Face_Axi_base.h>
#include <Op_Dift_VDF_Face_base.h>
#include <Op_Diff_Dift_VDF.h>

/// \cond DO_NOT_DOCUMENT
class Op_Dift_VDF_Face_leaves
{ };
/// \endcond

class Mod_turb_hyd_base;
class Champ_Fonc;

//////////////// CONST /////////////////

// ===========================================================================================================================================
declare_It_VDF_Face(Eval_Dift_VDF_const_Face)
class Op_Dift_VDF_Face : public Op_Dift_VDF_Face_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_Face>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Face);
public:
  Op_Dift_VDF_Face();
  inline void completer() override { completer_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_const_Face>(); }
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_const_Face>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_const_Face>(ch); }
  inline void associer_diffusivite(const Champ_base& ch) override { associer_diffusivite_impl<Eval_Dift_VDF_const_Face>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_const_Face>(); }
};

// ===========================================================================================================================================
class Op_Dift_VDF_Face_Axi : public Op_Dift_VDF_Face_Axi_base
{
  Declare_instanciable(Op_Dift_VDF_Face_Axi);
public:
  inline bool is_VAR() const override { return false; }
  inline double nu_(const int ) const override { return diffusivite_.valeur()(0,0); }
  inline double nu_mean_2_pts_(const int i, const int ) const override { return nu_(i); }
  inline double nu_mean_4_pts_(const int i, const int ) const override { return nu_(i); }
  inline double nu_mean_4_pts_(const int i, const int , const int , const int ) const override { return nu_(i); }
  inline void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = ref_cast(Champ_Uniforme, diffu); }
  inline void mettre_a_jour_var(double ) const override { /* do nothing */}
  inline const Champ_base& diffusivite() const override { return diffusivite_; }

protected:
  REF(Champ_Uniforme) diffusivite_;
};

//////////////// VAR /////////////////

// ===========================================================================================================================================
declare_It_VDF_Face(Eval_Dift_VDF_var_Face)
class Op_Dift_VDF_var_Face : public Op_Dift_VDF_Face_base, public Op_Diff_Dift_VDF<Op_Dift_VDF_var_Face>
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_var_Face);

public:
  Op_Dift_VDF_var_Face();
  inline void completer() override { completer_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_var_Face>(); }
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd, const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_var_Face>(zd,zcd,ch); }
  inline void associer_diffusivite_turbulente(const Champ_Fonc& ch) { associer_diffusivite_turbulente_impl<Type_Operateur::Op_DIFT_FACE,Eval_Dift_VDF_var_Face>(ch); }
  inline void associer_diffusivite(const Champ_base& ch) override { associer_diffusivite_impl<Eval_Dift_VDF_var_Face>(ch); }
  inline const Champ_base& diffusivite() const override { return diffusivite_impl<Eval_Dift_VDF_var_Face>(); }
};

// ===========================================================================================================================================
class Op_Dift_VDF_var_Face_Axi : public Op_Dift_VDF_Face_Axi_base
{
  Declare_instanciable(Op_Dift_VDF_var_Face_Axi);
public:

  inline bool is_VAR() const override { return true; }
  inline double nu_(const int i) const override { return diffusivite_->valeurs()(i); }
  inline double nu_mean_2_pts_(const int i, const int j) const override { return 0.5*(diffusivite_->valeurs()(i)+diffusivite_->valeurs()(j)); }
  inline double nu_mean_4_pts_(const int , const int ) const override;
  inline double nu_mean_4_pts_(const int i, const int j, const int k, const int l) const override
  {
    return 0.25*( diffusivite_->valeurs()(i) + diffusivite_->valeurs()(j) + diffusivite_->valeurs()(k) + diffusivite_->valeurs()(l));
  }
  inline void associer_diffusivite(const Champ_base& diffu) override { diffusivite_ = diffu; }
  inline void mettre_a_jour_var(double t) const override { ref_cast_non_const(Op_Dift_VDF_var_Face_Axi,(*this)).mettre_a_jour(t); }
  inline const Champ_base& diffusivite() const override { return diffusivite_; }

protected:
  REF(Champ_base) diffusivite_;
};

inline double Op_Dift_VDF_var_Face_Axi::nu_mean_4_pts_(const int i, const int j) const
{
  int element;
  double d_visco_lam = 0;
  if ((element=face_voisins(i,0)) != -1) d_visco_lam += diffusivite_->valeurs()(element);
  if ((element=face_voisins(i,1)) != -1) d_visco_lam += diffusivite_->valeurs()(element);
  if ((element=face_voisins(j,0)) != -1) d_visco_lam += diffusivite_->valeurs()(element);
  if ((element=face_voisins(j,1)) != -1) d_visco_lam += diffusivite_->valeurs()(element); // XXX : BUG FIXED HERE

  d_visco_lam /= 3.0;
  return d_visco_lam;
}

#endif /* Op_Dift_VDF_Face_leaves_included */
