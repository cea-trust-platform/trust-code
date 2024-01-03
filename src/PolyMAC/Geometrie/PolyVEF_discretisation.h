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

#ifndef PolyVEF_discretisation_included
#define PolyVEF_discretisation_included

#include <PolyMAC_P0_discretisation.h>

class PolyVEF_discretisation : public PolyMAC_P0_discretisation
{
  Declare_base(PolyVEF_discretisation);
public :
  void grad_u(const Domaine_dis_base& z,const Domaine_Cl_dis_base& zcl,const Champ_Inc_base& ch_vitesse,OWN_PTR(Champ_Fonc_base)& ) const override;
  void taux_cisaillement(const Domaine_dis_base&, const Domaine_Cl_dis_base& ,const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const override;
  void creer_champ_vorticite(const Schema_Temps_base& ,const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const override;

  bool is_polyvef() const override { return true; }
};

class PolyVEF_P0_discretisation : public PolyVEF_discretisation
{
  Declare_instanciable(PolyVEF_P0_discretisation);
public :
  bool is_polyvef_p0() const override { return true; }
};

class PolyVEF_P0P1_discretisation : public PolyVEF_discretisation
{
  Declare_instanciable(PolyVEF_P0P1_discretisation);
public :
  bool is_polyvef_p0p1() const override { return true; }
};

#endif /* PolyVEF_discretisation_included */
