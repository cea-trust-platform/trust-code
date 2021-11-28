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
// File:        Eval_Diff_VDF_Face_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Face_leaves_included
#define Eval_Diff_VDF_Face_leaves_included

#include <Eval_Diff_VDF_const.h>
#include <Eval_Diff_VDF_Face.h>
#include <Eval_Diff_VDF_var.h>

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/*
 * xxx xxx
 * THIS IS ONLY SPECIFIC TO TRUST CHECK_SOURCE :
 * WE ARE OBLIGED TO HAVE THE 1ST CLASS WITH THE SAME NAME AS THE FILE
 * OTHERWISE DO NOT COMPILE
 *
 * We use DOXYGEN_SHOULD_SKIP_THIS macro to skip this class in the doxygen documentation
 */
class Eval_Diff_VDF_Face_leaves: public Eval_Diff_VDF_Face<Eval_Diff_VDF_Face_leaves>,
  public Eval_Diff_VDF_const {};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

// .DESCRIPTION class Eval_Diff_VDF_const_Face
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite est constant.
class Eval_Diff_VDF_const_Face : public Eval_Diff_VDF_Face<Eval_Diff_VDF_const_Face>,
  public Eval_Diff_VDF_const { };

// .DESCRIPTION class Eval_Diff_VDF_var_Face
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite n"est pas constant.
// Dans le cas de la methode IMPLICITE les evaluateurs calculent la quantite qui figure
// dans le premier membre de l'equation, nous ne prenons pas par consequent l'oppose en
// ce qui concerne les termes pour la matrice, par contre pour le second membre nous
// procedons comme en explicite mais en ne fesant intervenir que les valeurs fournies
// par les conditions limites.
class Eval_Diff_VDF_var_Face : public Eval_Diff_VDF_Face<Eval_Diff_VDF_var_Face>,
  public Eval_Diff_VDF_var
{
public:
  static constexpr bool IS_VAR = true;
};

#endif /* Eval_Diff_VDF_Face_leaves_included */
