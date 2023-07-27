/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Energie_Multiphase_Enthalpie.h>
#include <Pb_Multiphase_Enthalpie.h>
#include <Discret_Thyd.h>

Implemente_instanciable(Energie_Multiphase_Enthalpie, "Energie_Multiphase_Enthalpie|Energie_Multiphase_h", Energie_Multiphase);

Sortie& Energie_Multiphase_Enthalpie::printOn(Sortie& is) const { return Energie_Multiphase::printOn(is); }

Entree& Energie_Multiphase_Enthalpie::readOn(Entree& is)
{
  Energie_Multiphase::readOn(is);
  l_inco_ch->nommer("enthalpie");
  return is;
}

void Energie_Multiphase_Enthalpie::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Energy-enthalpy equation discretization " << finl;
  const Pb_Multiphase_Enthalpie& pb = ref_cast(Pb_Multiphase_Enthalpie, probleme());
  dis.enthalpie(schema_temps(), domaine_dis(), l_inco_ch, pb.nb_phases());
  l_inco_ch.valeur().fixer_nature_du_champ(pb.nb_phases() == 1 ? scalaire : pb.nb_phases() == dimension ? vectoriel : multi_scalaire); //pfft
  for (int i = 0; i < pb.nb_phases(); i++)
    l_inco_ch.valeur().fixer_nom_compo(i, Nom("enthalpie_") + pb.nom_phase(i));
  champs_compris_.ajoute_champ(l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Energie_Multiphase_Enthalpie::discretiser() ok" << finl;
}

const Champ_Don& Energie_Multiphase_Enthalpie::diffusivite_pour_transport() const
{
  return milieu().diffusivite_fois_rho();
}

const Champ_base& Energie_Multiphase_Enthalpie::diffusivite_pour_pas_de_temps() const
{
  return milieu().diffusivite();
}
