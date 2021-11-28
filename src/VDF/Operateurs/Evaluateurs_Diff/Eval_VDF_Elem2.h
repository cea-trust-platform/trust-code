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
// File:        Eval_VDF_Elem2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_VDF_Elem2_included
#define Eval_VDF_Elem2_included

#include <Ref_Champ_base.h>
#include <Champ_P0_VDF.h>
class Echange_externe_impose;

// .DESCRIPTION class Eval_VDF_Elem
// Cette classe represente le prototype fonctionnel
// des evaluateurs de flux associes aux equations de
// conservation integrees sur les elements
class Eval_VDF_Elem2
{
public:
  inline void associer_inconnue(const Champ_base& );

  // contribution de la derivee en vitesse d'une equation scalaire
  template <typename BC_TYPE>
  double coeffs_face_bloc_vitesse(const DoubleTab&, int , const BC_TYPE&, int ) const { return 0.; }

  double coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const { return 0.; }
  double coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int ) const { return 0.; }

protected:
  REF(Champ_base) inconnue;
};

inline void Eval_VDF_Elem2::associer_inconnue(const Champ_base& inco)
{
  assert(sub_type(Champ_P0_VDF,inco));
  inconnue=inco;
}

#endif /* Eval_VDF_Elem2_included */
