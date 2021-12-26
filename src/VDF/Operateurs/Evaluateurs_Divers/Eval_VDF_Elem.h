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
// File:        Eval_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_VDF_Elem_included
#define Eval_VDF_Elem_included

#include <Ref_Champ_base.h>
#include <Champ_P0_VDF.h>
class Echange_externe_impose;

// .DESCRIPTION class Eval_VDF_Elem
// Cette classe represente le prototype fonctionnel
// des evaluateurs de flux associes aux equations de
// conservation integrees sur les elements
class Eval_VDF_Elem
{
public:
  // To overload if needed ... see Eval_XXX in leaves files
  static constexpr bool CALC_FLUX_FACES_ECH_EXT_IMP = true, CALC_FLUX_FACES_ECH_GLOB_IMP = true, CALC_FLUX_FACES_ENTREE_FL  = true,
                        CALC_FLUX_FACES_PAR = true, CALC_FLUX_FACES_PAR_ADIAB = false, CALC_FLUX_FACES_PAR_DEFIL = false, CALC_FLUX_FACES_PAR_FIXE = true,
                        CALC_FLUX_FACES_SORTIE_LIB = false, CALC_FLUX_FACES_SYMM = false, CALC_FLUX_FACES_PERIO = true;

  template <typename BC_TYPE>
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , const BC_TYPE&, int , ArrOfDouble& ) const { /* Do nothing */ }

  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int , ArrOfDouble& ) const { /* Do nothing */ }
  inline void coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int , ArrOfDouble& ) const { /* Do nothing */ }
  inline void associer_inconnue(const Champ_base& );

protected:
  REF(Champ_base) inconnue;
};

inline void Eval_VDF_Elem::associer_inconnue(const Champ_base& inco)
{
  assert(sub_type(Champ_P0_VDF,inco));
  inconnue=inco;
}

#endif /* Eval_VDF_Elem_included */
