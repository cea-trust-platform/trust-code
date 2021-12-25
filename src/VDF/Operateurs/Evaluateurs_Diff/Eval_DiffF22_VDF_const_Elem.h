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
// File:        Eval_DiffF22_VDF_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_DiffF22_VDF_const_Elem_included
#define Eval_DiffF22_VDF_const_Elem_included

#include <Eval_Diff_VDF_const.h>
#include <Eval_Diff_VDF_Elem.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>

// .DESCRIPTION class Eval_DiffF22_VDF_const_Elem
// Evaluateur VDF pour la diffusion dans l'equation de F22 (modele V2F)
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.
// .SECTION voir aussi Eval_DiffF22_VDF_const
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
  REF(Champ_Inc) KEps;
  REF(Champ_Inc) v2;
};

#endif /* Eval_DiffF22_VDF_const_Elem_included */
