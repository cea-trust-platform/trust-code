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

#ifndef Op_Diff_Dift_VDF_included
#define Op_Diff_Dift_VDF_included

#include <Modele_turbulence_scal_base.h>
#include <TRUST_type_traits.h>
#include <Mod_turb_hyd_base.h>
#include <Iterateur_VDF.h>
#include <Champ_P0_VDF.h>
#include <Correlation.h>

class Turbulence_paroi_scal;
class Champ_Fonc;
class Champ_base;

template <typename OP_TYPE>
class Op_Diff_Dift_VDF
{
protected:

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFF_ELEM || _TYPE_ == Type_Operateur::Op_DIFT_ELEM || _TYPE_ == Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, void>
  associer_impl(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis, const Champ_Inc& ch_diffuse)
  {
    const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
    associer_<EVAL_TYPE>(domaine_dis,domaine_cl_dis).associer_inconnue(inco);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFF_FACE || _TYPE_ == Type_Operateur::Op_DIFT_FACE || _TYPE_ == Type_Operateur::Op_DIFT_MULTIPHASE_FACE, void>
  associer_impl(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis, const Champ_Inc& ch_diffuse)
  {
    const Champ_Face_VDF& inco = ref_cast(Champ_Face_VDF,ch_diffuse.valeur());
    associer_<EVAL_TYPE>(domaine_dis,domaine_cl_dis).associer_inconnue(inco);
  }

  template <typename EVAL_TYPE>
  void associer_diffusivite_impl(const Champ_base& ch_diff)
  {
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    eval_diff_turb.associer(ch_diff);
  }

  template <typename EVAL_TYPE>
  void associer_pb(const Probleme_base& pb)
  {
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    eval_diff_turb.associer_pb(pb);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_MULTIPHASE_FACE, void>
  associer_corr_impl(const Correlation& corr)
  {
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    return eval_diff_turb.associer_corr(corr);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_MULTIPHASE_FACE, void>
  set_nut_impl(const DoubleTab& nut)
  {
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    return eval_diff_turb.set_nut(nut);
  }

  template <typename EVAL_TYPE>
  const Champ_base& diffusivite_impl() const
  {
    const EVAL_TYPE& eval_diff_turb = static_cast<const EVAL_TYPE&>(iter_()->evaluateur());
    return eval_diff_turb.get_diffusivite();
  }

  template <typename EVAL_TYPE>
  const double& alpha_impl(const int i) const // TODO : FIXME : pour multiphase faut ajouter compo
  {
    const EVAL_TYPE& eval_diff_turb = static_cast<const EVAL_TYPE&>(iter_()->evaluateur());
    const Champ_base& diffu = eval_diff_turb.get_diffusivite();
    const int is_var = eval_diff_turb.is_var();
    return diffu.valeurs()(is_var * i);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFF_FACE, void>
  mettre_a_jour_impl()
  {
    EVAL_TYPE& eval_diff = static_cast<EVAL_TYPE&> (iter_()->evaluateur());
    eval_diff.mettre_a_jour();
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_ELEM || _TYPE_ == Type_Operateur::Op_DIFT_FACE, void>
  associer_diffusivite_turbulente_impl(const Champ_Fonc& visc_ou_diff_turb)
  {
    static_cast<OP_TYPE *>(this)->associer_diffusivite_turbulente_base(visc_ou_diff_turb); // hohohoho
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    eval_diff_turb.associer_diff_turb(visc_ou_diff_turb);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_ELEM, void>
  associer_loipar_impl(const Turbulence_paroi_scal& loi_paroi)
  {
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&>(iter_()->evaluateur());
    eval_diff_turb.associer_loipar(loi_paroi);
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_ELEM, void> completer_impl()
  {
    static_cast<OP_TYPE *>(this)->completer_Op_Dift_VDF_base();
    const RefObjU& modele_turbulence = static_cast<OP_TYPE *>(this)->equation().get_modele(TURBULENCE);
    if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
      {
        const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
        const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
        associer_diffusivite_turbulente_impl<_TYPE_,EVAL_TYPE>(lambda_t); // YES !

        const Turbulence_paroi_scal& loipar = mod_turb.loi_paroi();
        if (loipar.non_nul()) associer_loipar_impl<_TYPE_,EVAL_TYPE>(loipar); // Et YES !

        EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&> (iter_()->evaluateur());
        eval_diff_turb.init_ind_fluctu_term(); // utile juste pour Const/Var Elem... sinon on fait rien
      }
    else // bizarre mais V2 (on fait comme le cas de l'Op_FACE mais sans assoscier un modele ...)
      {
        const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
        const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
        associer_diffusivite_turbulente_impl<_TYPE_,EVAL_TYPE>(alpha_t);
      }
  }

  template <Type_Operateur _TYPE_ ,typename EVAL_TYPE>
  inline enable_if_t_<_TYPE_ == Type_Operateur::Op_DIFT_FACE, void> completer_impl()
  {
    static_cast<OP_TYPE *>(this)->completer_Op_Dift_VDF_base();
    const RefObjU& modele_turbulence = static_cast<OP_TYPE *>(this)->equation().get_modele(TURBULENCE);
    const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
    const Champ_Fonc& visc_turb = mod_turb.viscosite_turbulente();
    associer_diffusivite_turbulente_impl<_TYPE_,EVAL_TYPE>(visc_turb);
    EVAL_TYPE& eval_diff_turb = static_cast<EVAL_TYPE&> (iter_()->evaluateur());
    eval_diff_turb.associer_modele_turbulence(mod_turb);
  }

private:
  // CRTP pour recuperer l'iter
  inline const Iterateur_VDF& iter_() const { return static_cast<const OP_TYPE *>(this)->get_iter(); }
  inline Iterateur_VDF& iter_() { return static_cast<OP_TYPE *>(this)->get_iter(); }

  template <typename EVAL_TYPE>
  inline EVAL_TYPE& associer_(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
  {
    const Domaine_VDF& zvdf = ref_cast(Domaine_VDF,domaine_dis.valeur());
    const Domaine_Cl_VDF& zclvdf = ref_cast(Domaine_Cl_VDF,domaine_cl_dis.valeur());
    iter_()->associer(zvdf,zclvdf,static_cast<OP_TYPE&>(*this));
    EVAL_TYPE& eval_diff = static_cast<EVAL_TYPE&> (iter_()->evaluateur());
    eval_diff.associer_domaines(zvdf,zclvdf);
    return eval_diff;
  }
};

#endif /* Op_Diff_Dift_VDF_included */
