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

#ifndef Eval_Dift_VDF_leaves_included
#define Eval_Dift_VDF_leaves_included

#include <Eval_Dift_Multiphase_VDF.h>
#include <Eval_Diff_VDF_Face_Gen.h>
#include <Eval_Diff_VDF_Elem_Gen.h>
#include <Mod_turb_hyd_base.h>
#include <Eval_Dift_VDF.h>
#include <TRUST_Ref.h>

class Turbulence_paroi_base;

/// \cond DO_NOT_DOCUMENT
class Eval_Dift_VDF_leaves
{};
/// \endcond

/*! @brief class Eval_Dift_VDF_Elem_Axi Evaluateur VDF pour la diffusion totale (laminaire et turbulente) en coordonnees cylindriques
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF).
 *
 * @sa Eval_Dift_VDF_var
 */
class Eval_Dift_VDF_Elem_Axi : public Eval_Diff_VDF_Elem_Gen<Eval_Dift_VDF_Elem_Axi>, public Eval_Dift_VDF
{
public:
  static constexpr bool IS_DEQUIV = true, IS_AXI = true;
};

/*! @brief class Eval_Dift_VDF_Elem Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF).
 *
 * @sa Eval_Dift_VDF_var
 */
class Eval_Dift_VDF_Elem : public Eval_Diff_VDF_Elem_Gen<Eval_Dift_VDF_Elem>, public Eval_Dift_VDF
{
public:
  static constexpr bool IS_MODIF_DEQ = true;
  inline int get_ind_Fluctu_Term() const { return ind_Fluctu_Term; }
  void init_ind_fluctu_term() override;
  void associer_loipar(const Turbulence_paroi_scal& loi_paroi) override;

private:
  int ind_Fluctu_Term = 1;
};

class Eval_Dift_VDF_Multi_inco_Elem_Axi : public Eval_Diff_VDF_Elem_Gen<Eval_Dift_VDF_Multi_inco_Elem_Axi>, public Eval_Dift_VDF
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true, IS_AXI = true;
  inline void mettre_a_jour() override { update_equivalent_distance(); }
};

/*! @brief class Eval_Dift_VDF_Multi_inco_Elem Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues. Il y a une diffusivite par inconnue.
 *
 * @sa Eval_Dift_VDF_Multi_inco_var
 */
class Eval_Dift_VDF_Multi_inco_Elem : public Eval_Diff_VDF_Elem_Gen<Eval_Dift_VDF_Multi_inco_Elem>, public Eval_Dift_VDF
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true;
  inline void mettre_a_jour() override { update_equivalent_distance(); }
};

/*! @brief class Eval_Dift_VDF_Face Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
 *
 *  Le champ diffuse est un Champ_Face_VDF
 *
 */
class Eval_Dift_VDF_Face : public Eval_Diff_VDF_Face_Gen<Eval_Dift_VDF_Face>, public Eval_Dift_VDF
{
public:
  static constexpr bool IS_TURB = true, CALC_FA7_SORTIE_LIB = true, CALC_ARR_PAR_FL = false;
  inline void associer_modele_turbulence(const Mod_turb_hyd_base& mod) { le_modele_turbulence = mod;  }
  inline bool uses_wall() const { return le_modele_turbulence.valeur().utiliser_loi_paroi(); }
  void mettre_a_jour() override;
  double tau_tan_impl(int face,int k) const;

private:
  REF(Mod_turb_hyd_base) le_modele_turbulence;
  REF(Turbulence_paroi_base) loipar;
  DoubleTab tau_tan_;
};

class Eval_Dift_Multiphase_VDF_Face : public Eval_Diff_VDF_Face_Gen<Eval_Dift_Multiphase_VDF_Face>, public Eval_Dift_Multiphase_VDF
{
public:
  static constexpr bool IS_TURB = true, CALC_FA7_SORTIE_LIB = true, CALC_ARR_PAR_FL = false;
  inline bool uses_wall() const { return false; }
};

#endif /* Eval_Dift_VDF_leaves_included */
