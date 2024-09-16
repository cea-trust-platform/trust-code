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

#include <Terme_Source_Constituant_PolyMAC_Elem.h>
#include <Convection_Diffusion_Concentration.h>
#include <Discretisation_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Milieu_base.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Constituant_PolyMAC_Elem, "Source_Constituant_Elem_PolyMAC|Source_Constituant_Elem_PolyMAC_P0P1NC", Terme_Source_PolyMAC_base);
Add_synonym(Terme_Source_Constituant_PolyMAC_Elem, "Source_Constituant_Elem_PolyMAC_P0");

Sortie& Terme_Source_Constituant_PolyMAC_Elem::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Constituant_PolyMAC_Elem::readOn(Entree& s)
{
  Terme_Source_Constituant::lire_donnees(s);
  set_fichier("Source_Constituant");
  set_description("Injection rate = Integral(source_C*dv) [mol/s]");
  return s;
}

void Terme_Source_Constituant_PolyMAC_Elem::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
{
  const Domaine_PolyMAC& zvdf = ref_cast(Domaine_PolyMAC, domaine_dis);
  const Domaine_Cl_PolyMAC& zclvdf = ref_cast(Domaine_Cl_PolyMAC, domaine_cl_dis.valeur());
  iter_->associer_domaines(zvdf, zclvdf);
  Eval_Source_C_PolyMAC_Elem& eval_puis = dynamic_cast<Eval_Source_C_PolyMAC_Elem&> (iter_->evaluateur());
  eval_puis.associer_domaines(zvdf, zclvdf);
}

void Terme_Source_Constituant_PolyMAC_Elem::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(), la_source_constituant.le_nom(), "", la_source_constituant, pb);
  Eval_Source_C_PolyMAC_Elem& eval_puis = dynamic_cast<Eval_Source_C_PolyMAC_Elem&> (iter_->evaluateur());
  eval_puis.associer_champs(la_source_constituant);
}
