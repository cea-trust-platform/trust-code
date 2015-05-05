/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Terme_Puissance_Thermique_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Puissance_Thermique_VEF_Face.h>
#include <Milieu_base.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>

Implemente_instanciable_sans_constructeur(Terme_Puissance_Thermique_VEF_Face,"Puissance_Thermique_VEF_P1NC",Terme_Puissance_Thermique_VEF_base);
implemente_It_Sou_VEF_Face(Eval_Puiss_Th_VEF_Face)


Sortie& Terme_Puissance_Thermique_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Terme_Puissance_Thermique_VEF_Face::readOn(Entree& s )
{
  Terme_Puissance_Thermique_VEF_base::readOn(s);
  set_fichier("Puissance_Thermique");
  set_description("Heat power release = Integral(P*dv) [W]");
  return s;
}

void Terme_Puissance_Thermique_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis& zone_cl_dis)
{
  Terme_Puissance_Thermique_VEF_base::associer_zones(zone_dis,zone_cl_dis);
  Eval_Puiss_Th_VEF_Face& eval_puis = (Eval_Puiss_Th_VEF_Face&) iter.evaluateur();
  eval_puis.associer_zones(zone_dis.valeur(),zone_cl_dis.valeur());
}


void Terme_Puissance_Thermique_VEF_Face::associer_pb(const Probleme_base& pb)
{
  preparer_source(pb);
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.zone_dis(),"Puissance_volumique","W/m3",la_puissance,pb);
  Eval_Puiss_Th_VEF_Face& eval_puis = (Eval_Puiss_Th_VEF_Face&) iter.evaluateur();
  eval_puis.associer_champs(rho_ref.valeur(),Cp.valeur(),la_puissance);
}


