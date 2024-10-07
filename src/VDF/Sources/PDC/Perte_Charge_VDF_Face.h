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

#ifndef Perte_Charge_VDF_Face_included
#define Perte_Charge_VDF_Face_included

#include <Source_dep_inco_base.h>
#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>

class Champ_Face_VDF;
class Probleme_base;
class Domaine_Cl_VDF;
class Domaine_VDF;
class Fluide_base;

class Perte_Charge_VDF_Face : public Source_dep_inco_base
{
  Declare_base(Perte_Charge_VDF_Face);
public:
  void associer_pb(const Probleme_base& ) override;
  void mettre_a_jour(double temps) override { }

  inline int has_interface_blocs() const override { return 1; }

protected:
  REF(Champ_Face_VDF) la_vitesse;
  REF(Fluide_base) le_fluide;
  REF(Domaine_VDF) le_dom_VDF;
  REF(Domaine_Cl_VDF) le_dom_Cl_VDF;
  IntVect num_faces;
  DoubleVect corr_front_ss;
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
};

#endif /* Perte_Charge_VDF_Face_included */
