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

#ifndef Eval_VDF_Face_included
#define Eval_VDF_Face_included

#include <CL_Types_Aretes_enum.h> // For SFINAE later ...
#include <Ref_Champ_base.h>
#include <Champ_Face.h>
#include <type_traits> // For SFINAE later ...

// .DESCRIPTION class Eval_VDF_Face
// Cette classe represente le prototype fonctionnel des evaluateurs
// de flux associes aux equations de conservation integrees
// sur les volumes entrelaces
class Eval_VDF_Face
{
public:
  static constexpr bool CALC_FA7_SORTIE_LIB = false, CALC_ARR_FL = true, CALC_ARR_BORD = true, CALC_ARR_COIN_FL = false,
                        CALC_ARR_INT = true, CALC_ARR_MIXTE = true, CALC_ARR_PERIO = true, CALC_ARR_PAR_FL = true, CALC_ARR_PAR = true,
                        CALC_ARR_SYMM_PAR = true, CALC_ARR_SYMM_FL = true, CALC_ARR_SYMM = false;
  inline void associer_inconnue(const Champ_base& );

protected:
  REF(Champ_base) inconnue;
};

inline void Eval_VDF_Face::associer_inconnue(const Champ_base& inco)
{
  assert(sub_type(Champ_Face,inco));
  inconnue=ref_cast(Champ_Face,inco);
}

#endif /* Eval_VDF_Face_included */
