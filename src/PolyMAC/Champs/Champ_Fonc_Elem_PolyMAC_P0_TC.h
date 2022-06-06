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

#ifndef Champ_Fonc_Elem_PolyMAC_P0_TC_included
#define Champ_Fonc_Elem_PolyMAC_P0_TC_included

#include <Champ_Fonc_Face_PolyMAC.h>
#include <Champ_Fonc_Elem_PolyMAC.h>
#include <Champ_Fonc.h>
#include <Ref_Zone_Cl_PolyMAC.h>
#include <Ref_Champ_Face_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Fonc.h>
#include <Ref_Champ_Fonc.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    class Champ_Fonc_Elem_PolyMAC_P0_TC for the calculation of the shear rate (taux de cisaillement)
//      This field is a Champ_Fonc_Elem_PolyMAC_P0 with 1 value per element and per phase :
//      Champ_Fonc_Elem_PolyMAC_P0_TC::valeurs()(e, n) returns the value of phase n in element e
//      The shear rate is calculated using
//        shear rate = sqrt(2*Sij*Sij)
//        Sij = 1/2(grad(u)+t grad(u))
//
//////////////////////////////////////////////////////////////////////////////

class Champ_Fonc_Elem_PolyMAC_P0_TC : public Champ_Fonc_Elem_PolyMAC

{

  Declare_instanciable(Champ_Fonc_Elem_PolyMAC_P0_TC);

public:

  inline void mettre_a_jour(double ) override;
  inline void associer_champ(const Champ_Face_PolyMAC_P0& );
  void me_calculer(double tps);

  inline virtual       Champ_Face_PolyMAC_P0& champ_a_deriver()      ;
  inline virtual const Champ_Face_PolyMAC_P0& champ_a_deriver() const;

protected:

  REF(Champ_Face_PolyMAC_P0) champ_;
};

inline void Champ_Fonc_Elem_PolyMAC_P0_TC::mettre_a_jour(double tps)
{
  if (temps()!=tps) me_calculer(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

inline void Champ_Fonc_Elem_PolyMAC_P0_TC::associer_champ(const Champ_Face_PolyMAC_P0& ch)
{
  Cerr << "On associe le taux de cisaillement au champ de vitesse " << finl ;
  champ_= ch;
}

inline Champ_Face_PolyMAC_P0& Champ_Fonc_Elem_PolyMAC_P0_TC::champ_a_deriver()
{
  return champ_.valeur();
}
inline const Champ_Face_PolyMAC_P0& Champ_Fonc_Elem_PolyMAC_P0_TC::champ_a_deriver() const
{
  return champ_.valeur();
}

#endif
