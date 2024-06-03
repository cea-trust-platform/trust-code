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

#include <Taux_cisaillement_P0_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <Champ_P1NC.h>
#include <Domaine_VF.h>

Implemente_instanciable(Taux_cisaillement_P0_VEF, "Taux_cisaillement_P0_VEF", Champ_Fonc_P0_VEF);

Sortie& Taux_cisaillement_P0_VEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Taux_cisaillement_P0_VEF::readOn(Entree& s) { return s; }

void Taux_cisaillement_P0_VEF::associer_champ(const Champ_P1NC& la_vitesse, const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
{
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, le_dom_Cl_dis_base);
  vitesse_ = la_vitesse;
}

void Taux_cisaillement_P0_VEF::mettre_a_jour(double tps)
{
  int nb_elem = le_dom_VF->nb_elem();
  DoubleVect tmp(nb_elem);
  vitesse_->calcul_S_barre(vitesse_.valeur().valeurs(), tmp, le_dom_Cl_VEF.valeur());
  DoubleTab& S = valeurs(); // Shear rate
  for (int i = 0; i < nb_elem; i++) S(i) = sqrt(tmp(i));
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

