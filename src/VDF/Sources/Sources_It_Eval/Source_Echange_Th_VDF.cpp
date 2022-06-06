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

#include <Convection_Diffusion_Temperature.h>
#include <Source_Echange_Th_VDF.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Interprete.h>
#include <Conduction.h>

Implemente_instanciable_sans_constructeur(Source_Echange_Th_VDF,"Echange_thermique_h_imp_VDF_P0_VDF",Terme_Source_VDF_base);
implemente_It_Sou_VDF_Elem(Eval_Echange_Himp_VDF_Elem)

Sortie& Source_Echange_Th_VDF::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Source_Echange_Th_VDF::readOn(Entree& s)
{
  Nom nom_pb;
  s >> nom_pb >> h;
  pb_voisin = ref_cast(Probleme_base,Interprete::objet(nom_pb));
  set_fichier("Ech_Thermique");
  set_description("Heat power release = h*Integral(delta_T(elem)*dv) [W]");
  return s;
}

void Source_Echange_Th_VDF::associer_zones(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter->associer_zones(zvdf, zclvdf);
  Eval_Echange_Himp_VDF_Elem& eval = static_cast<Eval_Echange_Himp_VDF_Elem&> (iter->evaluateur());
  eval.associer_zones(zvdf, zclvdf );
}

void Source_Echange_Th_VDF::associer_pb(const Probleme_base& pb)
{
  REF(Equation_base) eqn;
  Eval_Echange_Himp_VDF_Elem& eval = static_cast<Eval_Echange_Himp_VDF_Elem&> (iter.evaluateur());
  bool eq_OK = false;

  for (int i = 0; i < pb_voisin->nombre_d_equations(); i++)
    if (sub_type(Conduction,pb_voisin->equation(i)) ||sub_type(Convection_Diffusion_Temperature,pb_voisin->equation(i)))
      {
        eqn = pb_voisin->equation(i);
        eq_OK = true;
      }

  if (!eq_OK)
    {
      Cerr << "Source_Echange_Th_VDF is not coupled to a problem solving temperature." << finl;
      Process::exit();
    }

  eval.associer_champs(equation().inconnue(), eqn->inconnue(),h);
}
