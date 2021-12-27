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
// File:        Op_Diff_VDF_Face_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_VDF_Face_leaves_included
#define Op_Diff_VDF_Face_leaves_included

#include <Eval_Diff_VDF_Face_leaves.h>
#include <Op_Diff_VDF_Face_base.h>
#include <Op_Diff_VDF.h>

#ifdef DOXYGEN_SHOULD_SKIP_THIS // seulement un truc inutile pour check_source ...
class Op_Diff_VDF_Face_leaves
{ };
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

//////////////// CONST /////////////////

declare_It_VDF_Face(Eval_Diff_VDF_const_Face)
// .DESCRIPTION class Op_Diff_VDF_Face
//  Cette classe represente l'operateur de diffusion associe a une equation de la quantite de mouvement.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite est uniforme
//  L'iterateur associe est de type Iterateur_VDF_Face. L'evaluateur associe est de type Eval_Diff_VDF_const_Face
class Op_Diff_VDF_Face : public Op_Diff_VDF_Face_base, public Op_Diff_VDF<Op_Diff_VDF_Face>
{
  Declare_instanciable_sans_constructeur(Op_Diff_VDF_Face);
public:
  Op_Diff_VDF_Face();
  inline Eval_VDF_Face& get_eval_face() { return get_eval_face_impl<Eval_Diff_VDF_const_Face>(); }
};

//////////////// VAR /////////////////

declare_It_VDF_Face(Eval_Diff_VDF_var_Face)
// .DESCRIPTION class Op_Diff_VDF_var_Face
//  Cette classe represente l'operateur de diffusion associe a une equation de la quantite de mouvement.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite n'est pas uniforme
//  L'iterateur associe est de type Iterateur_VDF_Face. L'evaluateur associe est de type Eval_Diff_VDF_var_Face
class Op_Diff_VDF_var_Face : public Op_Diff_VDF_Face_base, public Op_Diff_VDF<Op_Diff_VDF_var_Face>
{
  Declare_instanciable_sans_constructeur(Op_Diff_VDF_var_Face);
public:
  Op_Diff_VDF_var_Face();
  inline Eval_VDF_Face& get_eval_face() { return get_eval_face_impl<Eval_Diff_VDF_var_Face>(); }
};


#endif /* Op_Diff_VDF_Face_leaves_included */
