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

#ifndef Eval_Diff_VDF_Face_leaves_included
#define Eval_Diff_VDF_Face_leaves_included

#include <Eval_Diff_VDF_const.h>
#include <Eval_Diff_VDF_Face.h>
#include <Eval_Diff_VDF_var.h>

/// \cond DO_NOT_DOCUMENT
class Eval_Diff_VDF_Face_leaves: public Eval_Diff_VDF_Face<Eval_Diff_VDF_Face_leaves>,
  public Eval_Diff_VDF_const {};
/// \endcond

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
