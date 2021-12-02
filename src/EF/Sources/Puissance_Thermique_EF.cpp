/****************************************************************************
* Copyright (c) 2019, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Puissance_Thermique_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Puissance_Thermique_EF.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Discretisation_base.h>

Implemente_instanciable_sans_constructeur(Puissance_Thermique_EF,"Puissance_Thermique_EF",Terme_Source_EF_base);

Implemente_It_Sou_EF(Eval_Puiss_Th_EF);

//// printOn
//

Sortie& Puissance_Thermique_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Puissance_Thermique_EF::readOn(Entree& s )
{
  const Equation_base& eqn = equation();
  Terme_Puissance_Thermique::lire_donnees(s,eqn);
  champs_compris_.ajoute_champ(la_puissance);
  //set_fichier("Puissance_Thermique");
  // set_description("Degagement de puissance thermique = Integrale(P*dv)[W]");

  return s;
}


void Puissance_Thermique_EF::associer_zones(const Zone_dis& zone_dis,
                                            const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_EF& zEF = ref_cast(Zone_EF,zone_dis.valeur());
  const Zone_Cl_EF& zclEF = ref_cast(Zone_Cl_EF,zone_cl_dis.valeur());

  iter->associer_zones(zEF, zclEF);

  Eval_Puiss_Th_EF& eval_puis = (Eval_Puiss_Th_EF&) iter.evaluateur();
  eval_puis.associer_zones(zEF, zclEF );
}


void Puissance_Thermique_EF::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.zone_dis(),la_puissance.le_nom(),"W/m3",la_puissance,pb);

  Eval_Puiss_Th_EF& eval_puis = (Eval_Puiss_Th_EF&) iter.evaluateur();
  eval_puis.associer_champs(la_puissance);
}


