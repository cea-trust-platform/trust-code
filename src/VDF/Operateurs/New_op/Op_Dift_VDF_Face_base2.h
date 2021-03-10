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
// File:        Op_Dift_VDF_Face_base2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_op
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Dift_VDF_Face_base2_included
#define Op_Dift_VDF_Face_base2_included

#include <Op_Dift_VDF_base2.h>
#include <ItVDFFa.h>
#include <Op_VDF_Face.h>

class Eval_VDF_Face2;
class Champ_Fonc;
class Mod_turb_hyd_base;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Dift_VDF_Face_base2
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VDF_Face_base2 : public Op_Dift_VDF_base2, public Op_VDF_Face
{
  Declare_base(Op_Dift_VDF_Face_base2);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );
  void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  void associer_loipar(const Turbulence_paroi& );
  const Champ_base& diffusivite() const;
  double calculer_dt_stab() const;
  double calculer_dt_stab(const Zone_VDF&) const;
  void calculer_borne_locale(DoubleVect& ,double , double ) const;
  inline Op_Dift_VDF_Face_base2(const Iterateur_VDF_base& );
  inline virtual Eval_VDF_Face2& get_eval_face();
};

inline Eval_VDF_Face2& Op_Dift_VDF_Face_base2::get_eval_face()
{
  Cerr<<"get_eval_face doit etre surcharge par "<<que_suis_je();
  exit();
  return (Eval_VDF_Face2&) iter.evaluateur();
}

inline void Op_Dift_VDF_Face_base2::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Face::modifier_pour_Cl(iter.zone(), iter.zone_Cl(), matrice, secmem);
}

inline Op_Dift_VDF_Face_base2::Op_Dift_VDF_Face_base2(const Iterateur_VDF_base& iter_base)
  : Op_Dift_VDF_base2(iter_base)
{}

#endif /* Op_Dift_VDF_Face_base2_included */
