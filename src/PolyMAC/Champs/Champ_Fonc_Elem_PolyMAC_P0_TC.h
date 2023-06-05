/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_Fonc_Elem_PolyMAC_P0P1NC.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <TRUST_Ref.h>

/*! @brief class Champ_Fonc_Elem_PolyMAC_P0_TC for the calculation of the shear rate (taux de cisaillement)
 *
 *    This field is a Champ_Fonc_Elem_PolyMAC_P0 with 1 value per element and per phase :
 *
 *       Champ_Fonc_Elem_PolyMAC_P0_TC::valeurs()(e, n) returns the value of phase n in element e
 *       The shear rate is calculated using
 *         shear rate = sqrt(2*Sij*Sij)
 *         Sij = 1/2(grad(u)+t grad(u))
 *
 */

class Champ_Fonc_Elem_PolyMAC_P0_TC: public Champ_Fonc_Elem_PolyMAC_P0P1NC
{
  Declare_instanciable(Champ_Fonc_Elem_PolyMAC_P0_TC);
public:
  void mettre_a_jour(double) override;
  void me_calculer(double tps);

  inline void associer_champ(const Champ_Face_PolyMAC_P0& ch) { champ_ = ch; }
  inline virtual Champ_Face_PolyMAC_P0& champ_a_deriver() { return champ_.valeur(); }
  inline virtual const Champ_Face_PolyMAC_P0& champ_a_deriver() const { return champ_.valeur(); }

protected:
  REF(Champ_Face_PolyMAC_P0) champ_;
};

#endif /* Champ_Fonc_Elem_PolyMAC_P0_TC_included */
