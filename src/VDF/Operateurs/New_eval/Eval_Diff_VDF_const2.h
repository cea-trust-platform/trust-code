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
// File:        Eval_Diff_VDF_const.h
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_const2_included
#define Eval_Diff_VDF_const2_included

#include <Eval_Diff_VDF.h>
#include <Champ_Face.h>
#include <Champ_base.h>
#include <Ref_Champ_Uniforme.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
class Champ_base;

//
// .DESCRIPTION class Eval_Diff_VDF_const
//
// Cette classe represente un evaluateur de flux diffusif
// avec un coefficient de diffusivite qui est isotrope
// et constant en espace.

//.SECTION voir aussi Evaluateur_VDF


class Eval_Diff_VDF_const2 : public Eval_Diff_VDF

{
public:

  inline Eval_Diff_VDF_const2();
  inline void associer(const Champ_base& );
  inline const Champ_base& get_diffusivite() const;
  inline const Champ_Uniforme& diffusivite() const;

  inline void mettre_a_jour( );

protected:

  REF(Champ_Uniforme) diffusivite_;
  double db_diffusivite;

  inline double dist_face(int, int, int) const;
  inline double dist_face_period(int, int, int) const;

};

inline Eval_Diff_VDF_const2::Eval_Diff_VDF_const2():db_diffusivite(-1.0e+300)
{
}

inline const Champ_Uniforme& Eval_Diff_VDF_const2::diffusivite() const
{
  return diffusivite_.valeur();
}

inline const Champ_base& Eval_Diff_VDF_const2::get_diffusivite() const
{
  return diffusivite();
}

// Description:
// renvoie la distance entre les faces fac1 et fac2 dans la direction k
inline double Eval_Diff_VDF_const2::dist_face(int fac1, int fac2, int k) const
{
  return xv(fac2,k) - xv(fac1,k);
}

inline double Eval_Diff_VDF_const2::dist_face_period(int fac1, int fac2, int k) const
{
  return la_zone->dist_face_period(fac1,fac2,k);
}


// Description:
// associe le champ de diffusivite
inline void Eval_Diff_VDF_const2::associer(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
  db_diffusivite = diffusivite_.valeur()(0,0);
}

// Description:
// mise a jour de double diffusivite
inline void Eval_Diff_VDF_const2::mettre_a_jour( )
{
  db_diffusivite = diffusivite_.valeur()(0,0);
}


#endif
