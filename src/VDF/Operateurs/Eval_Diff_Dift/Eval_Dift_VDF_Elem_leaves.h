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
// File:        Eval_Dift_VDF_Elem_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Diff_Dift
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dift_VDF_Elem_leaves_included
#define Eval_Dift_VDF_Elem_leaves_included

#include <Ref_Modele_turbulence_scal_base.h>
#include <Eval_Dift_VDF_Multi_inco_const.h>
#include <Eval_Dift_VDF_Multi_inco_var.h>
#include <Ref_Turbulence_paroi_scal.h>
#include <Turbulence_paroi_scal.h>
#include <Eval_Dift_VDF_const.h>
#include <Eval_Diff_VDF_Elem.h>
#include <Eval_Dift_VDF_var.h>
#include <DoubleVects.h>

/// \cond DO_NOT_DOCUMENT
class Eval_Dift_VDF_Elem_leaves : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_Elem_leaves>,
  public Eval_Dift_VDF_const {};
/// \endcond

/*
 * ******************************
 * CAS SCALAIRE - const/var/aniso
 * ******************************
 */

// .DESCRIPTION class Eval_Dift_VDF_const_Elem_Axi
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente) en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite est constant en espace.
// .SECTION voir aussi Eval_Dift_VDF_const
class Eval_Dift_VDF_const_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_const_Elem_Axi>, public Eval_Dift_VDF_const
{
public:
  static constexpr bool IS_DEQUIV = true, IS_AXI = true;
};

// .DESCRIPTION class Eval_Dift_VDF_const_Elem
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite est constant.
// .SECTION voir aussi Eval_Dift_VDF_const
class Eval_Dift_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_const_Elem>, public Eval_Dift_VDF_const
{
public :
  inline Eval_Dift_VDF_const_Elem() : ind_Fluctu_Term(1) {}
  static constexpr bool IS_MODIF_DEQ = true;

  inline int get_ind_Fluctu_Term() const { return ind_Fluctu_Term; }

  inline void associer_loipar(const Turbulence_paroi_scal& loi_paroi)
  {
    Eval_Dift_VDF_const::associer_loipar(loi_paroi);
    ind_Fluctu_Term = 0;
  }

  void init_ind_fluctu_term()
  {
    ind_Fluctu_Term = 0;
    if (!loipar.non_nul()) ind_Fluctu_Term = 1;
  }

private:
  int ind_Fluctu_Term;
};

// .DESCRIPTION class Eval_Dift_VDF_var_Elem_Axi
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente) en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite n'est pas constant en espace.
// .SECTION voir aussi Eval_Dift_VDF_var
class Eval_Dift_VDF_var_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_var_Elem_Axi>, public Eval_Dift_VDF_var
{
public:
  static constexpr bool IS_DEQUIV = true, IS_AXI = true;
};

// .DESCRIPTION class Eval_Dift_VDF_var_Elem
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite n'est pas constant.
// .SECTION voir aussi Eval_Dift_VDF_var
class Eval_Dift_VDF_var_Elem : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_var_Elem>, public Eval_Dift_VDF_var
{
public:
  inline Eval_Dift_VDF_var_Elem( ): ind_Fluctu_Term(1) {}
  static constexpr bool IS_MODIF_DEQ = true;

  inline int get_ind_Fluctu_Term() const { return ind_Fluctu_Term; }

  inline void init_ind_fluctu_term()
  {
    ind_Fluctu_Term = 0;
    if (!loipar.non_nul()) ind_Fluctu_Term = 1;
  }

  inline virtual void associer_loipar(const Turbulence_paroi_scal& loi_paroi)
  {
    Eval_Dift_VDF_var::associer_loipar(loi_paroi);
    ind_Fluctu_Term = 0;
  }

private:
  int ind_Fluctu_Term;
};

/*
 * ******************************
 * CAS VECTORIEL - const/var
 * ******************************
 */

// .DESCRIPTION class Eval_Dift_VDF_Multi_inco_const_Elem_Axi
// Evaluateur VDF pour la diffusion totale en coordonnees cylindriques (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue est constant.
// .SECTION voir aussi Eval_Dift_VDF_Multi_inco_const_Elem
class Eval_Dift_VDF_Multi_inco_const_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_Multi_inco_const_Elem_Axi>, public Eval_Dift_VDF_Multi_inco_const
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true, IS_AXI = true;
};

// .DESCRIPTION class Eval_Dift_VDF_Multi_inco_const_Elem
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite laminaire associe a chaque inconnue est constant.
// .SECTION voir aussi Eval_Dift_VDF_Multi_inco_const
class Eval_Dift_VDF_Multi_inco_const_Elem : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_Multi_inco_const_Elem>, public Eval_Dift_VDF_Multi_inco_const
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true;
};

class Eval_Dift_VDF_Multi_inco_var_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_Multi_inco_var_Elem_Axi>, public Eval_Dift_VDF_Multi_inco_var
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true, IS_AXI = true;
};

// .DESCRIPTION class Eval_Dift_VDF_Multi_inco_var_Elem
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue n'est pas constant.
//.SECTION voir aussi Eval_Dift_VDF_Multi_inco_var
class Eval_Dift_VDF_Multi_inco_var_Elem : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_Multi_inco_var_Elem>, public Eval_Dift_VDF_Multi_inco_var
{
public:
  static constexpr bool IS_MULTD = false, IS_DEQUIV = true;
};

#endif /* Eval_Dift_VDF_Elem_leaves_included */
