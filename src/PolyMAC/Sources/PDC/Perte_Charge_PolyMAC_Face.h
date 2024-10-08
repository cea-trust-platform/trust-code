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

#ifndef Perte_Charge_PolyMAC_Face_included
#define Perte_Charge_PolyMAC_Face_included

#include <Domaine_Cl_PolyMAC.h>
#include <Terme_Source_Qdm.h>
#include <Source_base.h>
#include <TRUST_Ref.h>

class Domaine_PolyMAC;
class Champ_Inc_base;
class Probleme_base;
class Fluide_base;

class Perte_Charge_PolyMAC_Face: public Source_base, public Terme_Source_Qdm
{
  Declare_base(Perte_Charge_PolyMAC_Face);
public:
  void associer_pb(const Probleme_base&) override;
  void mettre_a_jour(double) override { }

protected:
  OBS_PTR(Champ_Inc_base) la_vitesse;
  OBS_PTR(Domaine_PolyMAC) le_dom_PolyMAC;
  OBS_PTR(Domaine_Cl_PolyMAC) le_dom_Cl_PolyMAC;
  OBS_PTR(Fluide_base) le_fluide;
  IntVect num_faces;

  void associer_domaines(const Domaine_dis_base&, const Domaine_Cl_dis_base&) override;
};

#endif /* Perte_Charge_PolyMAC_Face_included */
