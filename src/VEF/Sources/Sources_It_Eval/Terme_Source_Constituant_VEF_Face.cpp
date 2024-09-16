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

#include <Convection_Diffusion_Concentration.h>
#include <Terme_Source_Constituant_VEF_Face.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Domaine_Cl_VEF.h>
#include <Milieu_base.h>
#include <Domaine_VEF.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Constituant_VEF_Face, "source_Constituant_VEF_P1NC", Terme_Source_VEF_base);

Sortie& Terme_Source_Constituant_VEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Constituant_VEF_Face::readOn(Entree& s)
{
  Terme_Source_Constituant::lire_donnees(s);
  set_fichier("Source_Constituant");
  set_description("Injection rate = Integral(source_C*dv) [mol/s]");
  return s;
}

void Terme_Source_Constituant_VEF_Face::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis)
{
  const Domaine_VEF& zvef = ref_cast(Domaine_VEF, domaine_dis);
  const Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF, domaine_cl_dis);
  iter_->associer_domaines(zvef, zclvef);
  Eval_Source_C_VEF_Face& eval_puis = dynamic_cast<Eval_Source_C_VEF_Face&> (iter_->evaluateur());
  eval_puis.associer_domaines(zvef, zclvef);
}

void Terme_Source_Constituant_VEF_Face::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(), la_source_constituant.le_nom(), "", la_source_constituant, pb);
  Eval_Source_C_VEF_Face& eval_puis = dynamic_cast<Eval_Source_C_VEF_Face&> (iter_->evaluateur());
  eval_puis.associer_champs(la_source_constituant);
}
