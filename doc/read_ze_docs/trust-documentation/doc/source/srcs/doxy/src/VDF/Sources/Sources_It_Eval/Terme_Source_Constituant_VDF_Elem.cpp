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

#include <Convection_Diffusion_Concentration.h>
#include <Terme_Source_Constituant_VDF_Elem.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Domaine_Cl_VDF.h>
#include <Domaine_VDF.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Constituant_VDF_Elem,"Source_Constituant_VDF_P0_VDF",Terme_Source_VDF_base);

Sortie& Terme_Source_Constituant_VDF_Elem::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Constituant_VDF_Elem::readOn(Entree& s)
{
  Terme_Source_Constituant::lire_donnees(s);
  set_fichier("Source_Constituant");
  set_description("Injection rate = Integral(source_C*dv) [mol/s]");
  return s;
}

void Terme_Source_Constituant_VDF_Elem::completer()
{
  Terme_Source_VDF_base::completer();
  col_width_ = Terme_Source_Constituant::completer(equation().inconnue().valeur());
}

void Terme_Source_Constituant_VDF_Elem::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
{
  const Domaine_VDF& zvdf = ref_cast(Domaine_VDF,domaine_dis.valeur());
  const Domaine_Cl_VDF& zclvdf = ref_cast(Domaine_Cl_VDF,domaine_cl_dis.valeur());
  iter->associer_domaines(zvdf, zclvdf);
  Eval_Source_C_VDF_Elem& eval_puis = static_cast<Eval_Source_C_VDF_Elem&> (iter->evaluateur());
  eval_puis.associer_domaines(zvdf,zclvdf);
}

void Terme_Source_Constituant_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(),la_source_constituant.le_nom(),"",la_source_constituant,pb);
  Eval_Source_C_VDF_Elem& eval_puis = static_cast<Eval_Source_C_VDF_Elem&> (iter->evaluateur());
  eval_puis.associer_champs(la_source_constituant);
}
