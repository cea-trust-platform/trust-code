/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Diff_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VDF_Face_included
#define Op_Diff_VDF_Face_included

#include <Op_Diff_VDF_Face_base.h>
#include <Eval_Diff_VDF_const_Face.h>
class Champ_Inc;
//
// .DESCRIPTION class Op_Diff_VDF_Face
//
//  Cette classe represente l'operateur de diffusion associe a une equation de
//  la quantite de mouvement.
//  La discretisation est VDF
//  Le champ diffuse est un Champ_Face
//  Le champ de diffusivite est uniforme
//  L'iterateur associe est de type Iterateur_VDF_Face
//  L'evaluateur associe est de type Eval_Diff_VDF_const_Face

//
// .SECTION voir aussi
//
//
declare_It_VDF_Face(Eval_Diff_VDF_const_Face)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VDF_Face
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VDF_Face : public Op_Diff_VDF_Face_base
{

  Declare_instanciable_sans_constructeur(Op_Diff_VDF_Face);

public:
  inline Eval_VDF_Face& get_eval_face();
  Op_Diff_VDF_Face();
};

//
// Fonctions inline de la classe Op_Diff_VDF_Face
//


// Description renvoit l'evaluateur caste en Ecal_VDF_Face corretement
inline Eval_VDF_Face& Op_Diff_VDF_Face::get_eval_face()
{
  Eval_Diff_VDF_const_Face& eval_diff = (Eval_Diff_VDF_const_Face&) iter.evaluateur();
  return (Eval_VDF_Face&) eval_diff;
}
#endif
