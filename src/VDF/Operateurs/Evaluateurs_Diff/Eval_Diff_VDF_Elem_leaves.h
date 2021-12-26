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
// File:        Eval_Diff_VDF_Elem_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Elem_leaves_included
#define Eval_Diff_VDF_Elem_leaves_included

#include <Eval_Diff_VDF_Multi_inco_const.h>
#include <Eval_Diff_VDF_Multi_inco_var.h>
#include <Eval_Diff_VDF_var_aniso.h>
#include <Eval_Diff_VDF_const.h>
#include <Eval_Diff_VDF_Elem.h>
#include <Eval_Diff_VDF_var.h>
#include <Ref_Champ_Inc.h>

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/*
 * THIS IS ONLY SPECIFIC TO TRUST CHECK_SOURCE : WE ARE OBLIGED TO HAVE THE 1ST CLASS WITH THE SAME NAME AS THE FILE OTHERWISE DO NOT COMPILE
 * We use DOXYGEN_SHOULD_SKIP_THIS macro to skip this class in the doxygen documentation
 */
class Eval_Diff_VDF_Elem_leaves: public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Elem_leaves>,
  public Eval_Diff_VDF_const {};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*
 * ******************************
 * CAS SCALAIRE - const/var/aniso
 * ******************************
 */

// .DESCRIPTION class Eval_Diff_VDF_const_Elem_Axi
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite est constant.
// .SECTION voir aussi Eval_Diff_VDF_const
class Eval_Diff_VDF_const_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_const_Elem_Axi>, public Eval_Diff_VDF_const
{
public:
  static constexpr bool IS_AXI = true;
};

// .DESCRIPTION class Eval_Diff_VDF_const_Elem
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite est constant.
// .SECTION voir aussi Eval_Diff_VDF_const
class Eval_Diff_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_const_Elem>, public Eval_Diff_VDF_const {};


// .DESCRIPTION class Eval_Diff_VDF_var_Elem_Axi
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite n'est pas constant.
// .SECTION voir aussi Eval_Diff_VDF_var
class Eval_Diff_VDF_var_Elem_Axi :public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_Axi>, public Eval_Diff_VDF_var
{
public:
  static constexpr bool IS_AXI = true;
};

// .DESCRIPTION class Eval_Diff_VDF_var_Elem
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite n'est pas constant.
// .SECTION voir aussi Eval_Diff_VDF_var
class Eval_Diff_VDF_var_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem>, public Eval_Diff_VDF_var {};

// .DESCRIPTION class Eval_Diff_VDF_var_Elem_aniso
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite n'est pas constant.
// .SECTION voir aussi Eval_Diff_VDF_var_aniso
class Eval_Diff_VDF_var_Elem_aniso : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>, public Eval_Diff_VDF_var_aniso
{
public:
  static constexpr bool IS_ANISO= true;
};

// .DESCRIPTION class Eval_DiffF22_VDF_const_Elem
// Evaluateur VDF pour la diffusion dans l'equation de F22 (modele V2F)
// Le champ diffuse est scalaire (Champ_P0_VDF). Le champ de diffusivite est constant.
class Eval_DiffF22_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_DiffF22_VDF_const_Elem>, public Eval_Diff_VDF_const
{
public:
  // Overload Eval_VDF_Elem
  static constexpr bool CALC_FLUX_FACES_ECH_EXT_IMP = false, CALC_FLUX_FACES_ECH_GLOB_IMP = false, CALC_FLUX_FACES_PAR = false,
                        CALC_FLUX_FACES_SORTIE_LIB = true, CALC_FLUX_FACES_SYMM = true, CALC_FLUX_FACES_PERIO = false;

  inline void associer_keps(const Champ_Inc& keps, const Champ_Inc& champv2)
  {
    KEps = keps;
    v2 = champv2;
  }

private:
  REF(Champ_Inc) KEps, v2;
};

/*
 * ******************************
 * CAS VECTORIEL - const/var
 * ******************************
 */

// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const_Elem_Axi
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue est constant.
// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_const
class Eval_Diff_VDF_Multi_inco_const_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_const_Elem_Axi>, public Eval_Diff_VDF_Multi_inco_const
{
public:
  static constexpr bool IS_MULTD = false, IS_AXI = true;
};

// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const_Elem
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue est constant.
// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_const
class Eval_Diff_VDF_Multi_inco_const_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_const_Elem>, public Eval_Diff_VDF_Multi_inco_const
{
public:
  static constexpr bool IS_MULTD = false;
};

// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_var_Elem_Axi
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue n'est pas constant.
//.SECTION voir aussi Eval_Diff_VDF_Multi_inco_var
class Eval_Diff_VDF_Multi_inco_var_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_var_Elem_Axi>, public Eval_Diff_VDF_Multi_inco_var
{
public:
  static constexpr bool IS_MULTD = false, IS_AXI = true;
};

// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_var_Elem
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue. Le champ de diffusivite associe a chaque inconnue n'est pas constant.
//.SECTION voir aussi Eval_Diff_VDF_Multi_inco_var
class Eval_Diff_VDF_Multi_inco_var_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_var_Elem>, public Eval_Diff_VDF_Multi_inco_var
{
public:
  static constexpr bool IS_MULTD = false;
};

#endif /* Eval_Diff_VDF_Elem_leaves_included */
