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

#include <Puissance_Thermique_EF.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Domaine_Cl_EF.h>
#include <Domaine_EF.h>

Implemente_instanciable_sans_constructeur(Puissance_Thermique_EF, "Puissance_Thermique_EF", Terme_Source_EF_base);

Sortie& Puissance_Thermique_EF::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Puissance_Thermique_EF::readOn(Entree& s)
{
  const Equation_base& eqn = equation();
  Terme_Puissance_Thermique::lire_donnees(s, eqn);
  champs_compris_.ajoute_champ(la_puissance);
  champs_don_.add(la_puissance);
  champs_don_.add(la_puissance_lu);
  return s;
}

void Puissance_Thermique_EF::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis)
{
  const Domaine_EF& zEF = ref_cast(Domaine_EF, domaine_dis);
  const Domaine_Cl_EF& zclEF = ref_cast(Domaine_Cl_EF, domaine_cl_dis);
  iter_->associer_domaines(zEF, zclEF);
  Eval_Puiss_Th_EF& eval_puis = dynamic_cast<Eval_Puiss_Th_EF&> (iter_->evaluateur());
  eval_puis.associer_domaines(zEF, zclEF);
}

void Puissance_Thermique_EF::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(), la_puissance.le_nom(), "W/m3", la_puissance, pb);
  Eval_Puiss_Th_EF& eval_puis = dynamic_cast<Eval_Puiss_Th_EF&> (iter_->evaluateur());
  eval_puis.associer_champs(la_puissance);
}
