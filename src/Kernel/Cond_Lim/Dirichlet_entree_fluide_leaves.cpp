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

#include <Dirichlet_entree_fluide_leaves.h>

Implemente_base(Dirichlet_entree_fluide, "Dirichlet_entree_fluide", Dirichlet);
Sortie& Dirichlet_entree_fluide::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Dirichlet_entree_fluide::readOn(Entree& s) { return Cond_lim_base::readOn(s); }

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_Flux_Chaleur_Turbulente_imposee, "Frontiere_ouverte_Flux_Chaleur_Turbulente_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_Flux_Chaleur_Turbulente_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_Flux_Chaleur_Turbulente_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_vitesse_imposee, "Frontiere_ouverte_vitesse_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_vitesse_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_vitesse_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_vitesse_imposee_libre, "Frontiere_ouverte_vitesse_imposee_sortie", Entree_fluide_vitesse_imposee);
Sortie& Entree_fluide_vitesse_imposee_libre::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_vitesse_imposee_libre::readOn(Entree& s)
{
  return Entree_fluide_vitesse_imposee::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_alpha_impose, "Frontiere_ouverte_alpha_impose", Dirichlet_entree_fluide);
Sortie& Entree_fluide_alpha_impose::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_alpha_impose::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Fraction_volumique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_concentration_imposee, "Frontiere_ouverte_concentration_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_concentration_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_concentration_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Concentration"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_Fluctu_Temperature_imposee, "Frontiere_ouverte_Fluctu_Temperature_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_Fluctu_Temperature_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_Fluctu_Temperature_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_fraction_massique_imposee, "Frontiere_ouverte_fraction_massique_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_fraction_massique_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_fraction_massique_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Fraction_massique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_temperature_imposee, "Frontiere_ouverte_temperature_imposee", Dirichlet_entree_fluide);
Sortie& Entree_fluide_temperature_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_temperature_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}

/* ========================================================================================= */

Implemente_instanciable(Entree_fluide_V2_impose, "Frontiere_ouverte_V2_impose", Dirichlet_entree_fluide);
Sortie& Entree_fluide_V2_impose::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Entree_fluide_V2_impose::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Transport_V2"), Motcle("indetermine") };
  return Dirichlet_entree_fluide::readOn(s);
}
