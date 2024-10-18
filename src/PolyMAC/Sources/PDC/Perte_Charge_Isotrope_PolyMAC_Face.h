/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Perte_Charge_Isotrope_PolyMAC_Face_included
#define Perte_Charge_Isotrope_PolyMAC_Face_included

#include <Perte_Charge_PolyMAC.h>
#include <PDC_PolyMAC_impl.h>

//!  Perte de charge isotrope (proportionnelle a -u )
/**
 du/dt = - lambda(Re,x,y,z,t) * u * ||u|| / 2 Dh

 Lecture des arguments :

 Perte_Charge_Isotrope_PolyMAC_Face diametre_hydraulique {
 lambda expression(Re,x,y,z,t)
 diam_hydr champ_don
 [sous_domaine nom]
 }
 */

class Perte_Charge_Isotrope_PolyMAC_Face: public Perte_Charge_PolyMAC, public PDC_Isotrope_PolyMAC
{
  Declare_instanciable(Perte_Charge_Isotrope_PolyMAC_Face);
public:
  void mettre_a_jour(double temps) override { diam_hydr->mettre_a_jour(temps); }

protected:

  //! Implemente le calcul effectif de la perte de charge pour un lieu donne
  void coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u, double dh, double nu, double reynolds, double& coeff_ortho, double& coeff_long, double& u_l,
                           DoubleVect& v_valeur) const override;
};

class Perte_Charge_Isotrope_PolyMAC_P0P1NC_Face: public Perte_Charge_Isotrope_PolyMAC_Face
{
  Declare_instanciable(Perte_Charge_Isotrope_PolyMAC_P0P1NC_Face);

public:
  int has_interface_blocs() const override { return 1; }
  void check_multiphase_compatibility() const override { }
};

#endif /* Perte_Charge_Isotrope_PolyMAC_Face_included */
