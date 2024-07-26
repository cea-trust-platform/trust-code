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

#include <Source_Dirac_VDF_Elem.h>
#include <Fluide_Incompressible.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable_sans_constructeur(Source_Dirac_VDF_Elem,"Dirac_VDF_P0_VDF",Terme_Puissance_Thermique_VDF_base);
// XD dirac source_base dirac 0 Class to define a source term corresponding to a volume power release in the energy equation.
// XD attr position list position 0 not_set
// XD attr ch field_base ch 0 Thermal power field type. To impose a volume power on a domain sub-area, the Champ_Uniforme_Morceaux (partly_uniform_field) type must be used. NL2 Warning : The volume thermal power is expressed in W.m-3.

Sortie& Source_Dirac_VDF_Elem::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Source_Dirac_VDF_Elem::readOn(Entree& is)
{
  point = 0;
  point.resize(dimension);
  is >> point;

  Terme_Puissance_Thermique_VDF_base::readOn(is);
  set_fichier("Puissance_Thermique");
  set_description("Heat power release = nb_dirac*P*V [W]");
  return is;
}

void Source_Dirac_VDF_Elem::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
{
  Terme_Puissance_Thermique_VDF_base::associer_domaines(domaine_dis,domaine_cl_dis);
  Eval_Dirac_VDF_Elem& eval_dirac = static_cast<Eval_Dirac_VDF_Elem&> (iter_->evaluateur());
  eval_dirac.associer_domaines(domaine_dis.valeur(),domaine_cl_dis.valeur());
  const int nb_elem = domaine_dis->nb_elem();
  const Domaine& mon_dom = domaine_dis->domaine();
  nb_dirac = 0;

  for (int elem = 0; elem < nb_elem; elem++)
    {
      int test =  mon_dom.type_elem()->contient(point,elem) ;
      if (test == 1) nb_dirac++;
    }

  eval_dirac.associer_nb_elem_dirac(nb_dirac);
}

void Source_Dirac_VDF_Elem::associer_pb(const Probleme_base& pb)
{
  Eval_Dirac_VDF_Elem& eval_dirac = static_cast<Eval_Dirac_VDF_Elem&> (iter_->evaluateur());
  eval_dirac.associer_champs(la_puissance);
  eval_dirac.le_point.copy(point);
}
