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

#include <Terme_Puissance_Thermique_DG_Elem.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Milieu_base.h>

Implemente_instanciable_sans_constructeur(Terme_Puissance_Thermique_DG_Elem, "Puissance_Thermique_Elem_DG", Terme_Puissance_Thermique_DG_base);
Add_synonym(Terme_Puissance_Thermique_DG_Elem, "Puissance_Thermique_Elem_DG_P0");

Sortie& Terme_Puissance_Thermique_DG_Elem::printOn(Sortie& s) const { return s << que_suis_je(); }
Entree& Terme_Puissance_Thermique_DG_Elem::readOn(Entree& s) { return Terme_Puissance_Thermique_DG_base::readOn(s); }

void Terme_Puissance_Thermique_DG_Elem::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis)
{
  Terme_Puissance_Thermique_DG_base::associer_domaines(domaine_dis, domaine_cl_dis);
  Eval_Puiss_Th_DG_Elem& eval_puis = dynamic_cast<Eval_Puiss_Th_DG_Elem&> (iter_->evaluateur());
  eval_puis.associer_domaines(domaine_dis, domaine_cl_dis);
}

void Terme_Puissance_Thermique_DG_Elem::associer_pb(const Probleme_base& pb)
{
  Eval_Puiss_Th_DG_Elem& eval_puis = dynamic_cast<Eval_Puiss_Th_DG_Elem&> (iter_->evaluateur());
  eval_puis.associer_champs(la_puissance);
}
