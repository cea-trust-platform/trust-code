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

#include <Source_Neutronique_VDF.h>
#include <Probleme_base.h>
#include <Domaine_Cl_VDF.h>
#include <Champ_Inc.h>
#include <Domaine_VDF.h>

Implemente_instanciable_sans_constructeur(Source_Neutronique_VDF,"Source_Neutronique_VDF_P0_VDF",Source_Neutronique);

Sortie& Source_Neutronique_VDF::printOn(Sortie& s) const { return s << que_suis_je() ; }
Entree& Source_Neutronique_VDF::readOn(Entree& s) { return Source_Neutronique::readOn(s); }

void Source_Neutronique_VDF::completer()
{
  Source_Neutronique::completer();
  iter_->completer_();
}

double Source_Neutronique_VDF::calculer_Tmoyenne()
{
  double T = 0.;
  const int nb_elem = le_dom->nb_elem();
  const DoubleTab& temp = equation().inconnue()->valeurs();
  for (int i = 0; i < nb_elem; i++) T += temp(i);

  T = T / nb_elem;
  return T;
}

void Source_Neutronique_VDF::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis)
{
  const Domaine_VDF& zvdf = ref_cast(Domaine_VDF,domaine_dis);
  const Domaine_Cl_VDF& zclvdf = ref_cast(Domaine_Cl_VDF,domaine_cl_dis);
  le_dom = zvdf;
  iter_->associer_domaines(zvdf, zclvdf);
  Eval_Puiss_Neutr_VDF_Elem& eval_puis = static_cast<Eval_Puiss_Neutr_VDF_Elem&> (iter_->evaluateur());
  eval_puis.associer_domaines(zvdf, zclvdf );
}

void Source_Neutronique_VDF::associer_pb(const Probleme_base& pb)
{
  Eval_Puiss_Neutr_VDF_Elem& eval_puis = static_cast<Eval_Puiss_Neutr_VDF_Elem&> (iter_->evaluateur());
  eval_puis.associer_champs(la_puissance);
  eval_puis.associer_repartition(repartition(),nom_ssz());
}
