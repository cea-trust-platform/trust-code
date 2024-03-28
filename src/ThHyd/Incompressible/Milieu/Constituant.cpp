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

#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Constituant.h>
#include <Param.h>

Implemente_instanciable(Constituant, "Constituant", Milieu_base);

Sortie& Constituant::printOn(Sortie& os) const { return os; }

Entree& Constituant::readOn(Entree& is) { return Milieu_base::readOn(is); }

void Constituant::set_param(Param& param)
{
  Milieu_base::set_param(param);
  param.ajouter("coefficient_diffusion", &D_, Param::REQUIRED);
}

void Constituant::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  discretiser_multi_concentration(Nom("coefficient_diffusion"), pb, dis);
}

void Constituant::discretiser_multi_concentration(const Nom& nm, const Probleme_base& pb, const Discretisation_base& dis)
{
  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  dis.nommer_completer_champ_physique(domaine_dis, nm, "m2/s", D_.valeur(), pb);
  champs_compris_.ajoute_champ(D_.valeur());
  Milieu_base::discretiser(pb, dis);
}
