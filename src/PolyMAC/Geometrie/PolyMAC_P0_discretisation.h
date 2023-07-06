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

#ifndef PolyMAC_P0_discretisation_included
#define PolyMAC_P0_discretisation_included

#include <PolyMAC_P0P1NC_discretisation.h>

class PolyMAC_P0_discretisation : public PolyMAC_P0P1NC_discretisation
{
  Declare_instanciable(PolyMAC_P0_discretisation);
public :
  void grad_u(const Domaine_dis& z,const Domaine_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const override;
  void taux_cisaillement(const Domaine_dis&, const Domaine_Cl_dis& ,const Champ_Inc&, Champ_Fonc&) const override;
  void creer_champ_vorticite(const Schema_Temps_base& ,const Champ_Inc&, Champ_Fonc& ) const override;

  bool is_polymac_p0() const override { return true; }
  bool is_polymac_p0p1nc() const override { return false; } // attention heritage !
  bool is_polymac() const override { return false; } // attention heritage !
};

#endif /* PolyMAC_P0_discretisation_included */
