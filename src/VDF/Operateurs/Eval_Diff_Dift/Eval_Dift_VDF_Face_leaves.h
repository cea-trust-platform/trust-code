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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Dift_VDF_Face_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Diff_Dift
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dift_VDF_Face_leaves_included
#define Eval_Dift_VDF_Face_leaves_included

#include <Ref_Turbulence_paroi_base.h>
#include <Eval_Dift_VDF_const.h>
#include <Eval_Diff_VDF_Face.h>
#include <Eval_Dift_VDF_var.h>
#include <Mod_turb_hyd_base.h>

/// \cond DO_NOT_DOCUMENT
class Eval_Dift_VDF_Face_leaves : public Eval_Diff_VDF_Face<Eval_Dift_VDF_Face_leaves>,
  public Eval_Dift_VDF_const {};
/// \endcond

// .DESCRIPTION class Eval_Dift_VDF_const_Face
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite est constant.
class Eval_Dift_VDF_const_Face : public Eval_Diff_VDF_Face<Eval_Dift_VDF_const_Face>,
  public Eval_Dift_VDF_const
{
public:
  static constexpr bool IS_TURB = true, CALC_FA7_SORTIE_LIB = true, CALC_ARR_PAR_FL = false;
  inline void associer_modele_turbulence(const Mod_turb_hyd_base& mod) { le_modele_turbulence= mod; }
  inline void mettre_a_jour() override;

  // overload methods (see implementations in Eval_Diff_VDF_const)
  inline double tau_tan_impl(int face,int k) const;
  inline bool uses_wall() const { return le_modele_turbulence.valeur().utiliser_loi_paroi(); }

protected:
  REF(Mod_turb_hyd_base) le_modele_turbulence;
  DoubleTab tau_tan_;
};

inline void Eval_Dift_VDF_const_Face::mettre_a_jour()
{
  Eval_Dift_VDF_const::mettre_a_jour( ) ;
  if (le_modele_turbulence->loi_paroi().non_nul()) tau_tan_.ref(le_modele_turbulence->loi_paroi()->Cisaillement_paroi());
}

inline double Eval_Dift_VDF_const_Face::tau_tan_impl(int face, int k) const
{
  const int nb_faces = la_zone.valeur().nb_faces();
  const ArrOfInt& ind_faces_virt_bord = la_zone.valeur().ind_faces_virt_bord();
  int f = (face >= tau_tan_.dimension(0)) ? ind_faces_virt_bord[face-nb_faces] : face;

  if(f >= tau_tan_.dimension_tot(0))
    {
      Cerr << "Erreur dans tau_tan " << finl;
      Cerr << "dimension : " << tau_tan_.dimension(0) << finl;
      Cerr << "dimension_tot : " << tau_tan_.dimension_tot(0) << finl;
      Cerr << "face : " << face << finl;
      Process::exit();
    }
  return tau_tan_(f,k);
}

// .DESCRIPTION class Eval_Dift_VDF_var_Face
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite n'est pas constant.
class Eval_Dift_VDF_var_Face : public Eval_Diff_VDF_Face<Eval_Dift_VDF_var_Face>,
  public Eval_Dift_VDF_var
{
public:
  static constexpr bool IS_VAR = true, IS_TURB = true, CALC_FA7_SORTIE_LIB = true, CALC_ARR_PAR_FL = false;
  inline void associer_modele_turbulence(const Mod_turb_hyd_base& mod) { le_modele_turbulence = mod;  }
  inline void mettre_a_jour() override;

  // overload methods (see implementations in Eval_Diff_VDF_const)
  inline double tau_tan_impl(int face,int k) const;
  inline bool uses_wall() const { return le_modele_turbulence.valeur().utiliser_loi_paroi(); }

private:
  REF(Mod_turb_hyd_base) le_modele_turbulence;
  REF(Turbulence_paroi_base) loipar;
  DoubleTab tau_tan_;
};

inline void Eval_Dift_VDF_var_Face::mettre_a_jour()
{
  Eval_Dift_VDF_var::mettre_a_jour();
  if (le_modele_turbulence->loi_paroi().non_nul())
    {
      // Modif E. Saikali : on fait le ref seulement si le tableau a ete initialise, sinon pointeur nulle
      const DoubleTab& tab = le_modele_turbulence->loi_paroi().valeur().Cisaillement_paroi();
      if (tab.size_array() > 0) tau_tan_.ref(tab);
    }
}

inline double Eval_Dift_VDF_var_Face::tau_tan_impl(int face, int k) const
{
  const int nb_faces = la_zone.valeur().nb_faces();
  const ArrOfInt& ind_faces_virt_bord = la_zone.valeur().ind_faces_virt_bord();
  int f = (face >= tau_tan_.dimension(0)) ? ind_faces_virt_bord[face-nb_faces] : face;
  if(f >= tau_tan_.dimension_tot(0))
    {
      Cerr << "Erreur dans tau_tan " << finl;
      Cerr << "dimension : " << tau_tan_.dimension(0) << finl;
      Cerr << "dimension_tot : " << tau_tan_.dimension_tot(0) << finl;
      Cerr << "face : " << face << finl;
      Process::exit();
    }
  return tau_tan_(f,k);
}

#endif /* Eval_Dift_VDF_Face_leaves_included */
