/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_Diff_VDF_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_leaves_included
#define Eval_Diff_VDF_leaves_included

#include <Eval_Diff_VDF_Elem.h>
#include <Eval_Diff_VDF_const2.h>
#include <Eval_Diff_VDF_var2.h>
#include <Eval_Diff_VDF_var_aniso.h>
#include <Eval_Diff_VDF_Multi_inco_const2.h>
#include <Eval_Diff_VDF_Multi_inco_var2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/*
 * xxx xxx
 * THIS IS ONLY SPECIFIC TO TRUST 5CHECK_SOURCE :
 * WE ARE OBLIGED TO HAVE THE 1ST CLASS WITH THE SAME NAME AS THE FILE
 * OTHERWISE DO NOT COMPILE
 */
class Eval_Diff_VDF_leaves: public Eval_Diff_VDF_Elem<Eval_Diff_VDF_leaves>,
  public Eval_Diff_VDF_const2 {};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*
 * ******************************
 * CAS SCALAIRE - const/var/aniso
 * ******************************
 */

//
// .DESCRIPTION class Eval_Diff_VDF_const_Elem_Axi
//
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.

//
// .SECTION voir aussi Eval_Diff_VDF_const


class Eval_Diff_VDF_const_Elem_Axi : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_const_Elem_Axi>,
  public Eval_Diff_VDF_const2
{
public:
  static constexpr bool IS_AXI = true;
};

//
// .DESCRIPTION class Eval_Diff_VDF_const_Elem
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.

//
// .SECTION voir aussi Eval_Diff_VDF_const


class Eval_Diff_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_const_Elem>,
  public Eval_Diff_VDF_const2 {};


//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem_Axi
//
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var

class Eval_Diff_VDF_var_Elem_Axi :public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_Axi>,
  public Eval_Diff_VDF_var2
{
public:
  static constexpr bool IS_AXI = true;
};

//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var

class Eval_Diff_VDF_var_Elem : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem>,
  public Eval_Diff_VDF_var2 {};


//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem_aniso
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var_aniso

class Eval_Diff_VDF_var_Elem_aniso : public Eval_Diff_VDF_Elem<Eval_Diff_VDF_var_Elem_aniso>,
  public Eval_Diff_VDF_var_aniso
{
public:
  static constexpr bool IS_ANISO= true;
};


/*
 * ******************************
 * CAS VECTORIEL - const/var
 * ******************************
 */

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const_Elem_Axi
//
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue
// Le champ de diffusivite associe a chaque inconnue est constant.

// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_const
class Eval_Diff_VDF_Multi_inco_const_Elem_Axi :
  public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_const_Elem_Axi>,
  public Eval_Diff_VDF_Multi_inco_const2
{
public:
  static constexpr bool IS_MULTD = false;
  static constexpr bool IS_AXI = true;
};

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_const_Elem
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue
// Le champ de diffusivite associe a chaque inconnue est constant.

// .SECTION voir aussi Eval_Diff_VDF_Multi_inco_const
class Eval_Diff_VDF_Multi_inco_const_Elem :
  public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_const_Elem>,
  public Eval_Diff_VDF_Multi_inco_const2
{
public:
  static constexpr bool IS_MULTD = false;
};

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_var_Elem_Axi
//
// Evaluateur VDF pour la diffusion en coordonnees cylindriques
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue
// Le champ de diffusivite associe a chaque inconnue n'est pas constant.

//.SECTION voir aussi Eval_Diff_VDF_Multi_inco_var
class Eval_Diff_VDF_Multi_inco_var_Elem_Axi :
  public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_var_Elem_Axi>,
  public Eval_Diff_VDF_Multi_inco_var2
{
public:
  static constexpr bool IS_MULTD = false;
  static constexpr bool IS_AXI = true;
};

//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_var_Elem
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues
// Il y a une diffusivite par inconnue
// Le champ de diffusivite associe a chaque inconnue n'est pas constant.

//.SECTION voir aussi Eval_Diff_VDF_Multi_inco_var
class Eval_Diff_VDF_Multi_inco_var_Elem :
  public Eval_Diff_VDF_Elem<Eval_Diff_VDF_Multi_inco_var_Elem>,
  public Eval_Diff_VDF_Multi_inco_var2
{
public:
  static constexpr bool IS_MULTD = false;
};

#endif /* Eval_Diff_VDF_leaves_included */
