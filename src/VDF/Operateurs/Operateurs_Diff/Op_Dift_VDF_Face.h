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
// File:        Op_Dift_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Dift_VDF_Face_included
#define Op_Dift_VDF_Face_included

#include <Op_Dift_VDF_Face_base.h>
#include <Eval_Dift_VDF_const_Face.h>

declare_It_VDF_Face(Eval_Dift_VDF_const_Face)

class Champ_Fonc;
class Mod_turb_hyd_base;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Dift_VDF_Face
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VDF_Face : public Op_Dift_VDF_Face_base
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Face);

public:
  Op_Dift_VDF_Face();
  void associer_diffusivite_turbulente(const Champ_Fonc& );
  void completer();
  inline Eval_VDF_Face2& get_eval_face();
};

// Description renvoit l'evaluateur caste en Ecal_VDF_Face corretement
inline Eval_VDF_Face2& Op_Dift_VDF_Face::get_eval_face()
{
  Eval_Dift_VDF_const_Face& eval_diff = dynamic_cast<Eval_Dift_VDF_const_Face&> (iter.evaluateur());
  return eval_diff;
}

#endif /* Op_Dift_VDF_Face_included */
