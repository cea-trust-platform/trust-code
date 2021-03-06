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

#include <Terme_Source_Constituant_VEF_Face.h>
#include <Milieu_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>

Implemente_instanciable_sans_constructeur(Terme_Source_Constituant_VEF_Face,"source_Constituant_VEF_P1NC",Terme_Source_VEF_base);
implemente_It_Sou_VEF_Face(Eval_Source_C_VEF_Face)

//// printOn
//

Sortie& Terme_Source_Constituant_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Terme_Source_Constituant_VEF_Face::readOn(Entree& s )
{
  Terme_Source_Constituant::lire_donnees(s);
  set_fichier("Source_Constituant");
  set_description("Injection rate = Integral(source_C*dv) [mol/s]");

  return s;
}


void Terme_Source_Constituant_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                                       const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF,zone_cl_dis.valeur());

  iter->associer_zones(zvef, zclvef);

  Eval_Source_C_VEF_Face& eval_puis = (Eval_Source_C_VEF_Face&) iter.evaluateur();
  eval_puis.associer_zones(zvef, zclvef );
}


void Terme_Source_Constituant_VEF_Face::associer_pb(const Probleme_base& pb)
{
  const Equation_base& eqn = pb.equation(0);
  eqn.discretisation().nommer_completer_champ_physique(eqn.zone_dis(),la_source_constituant.le_nom(),"",la_source_constituant,pb);
  Eval_Source_C_VEF_Face& eval_puis = (Eval_Source_C_VEF_Face&) iter.evaluateur();
  eval_puis.associer_champs(la_source_constituant);
}


