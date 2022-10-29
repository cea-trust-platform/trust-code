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

#include <Terme_Puissance_Thermique_VEF_base.h>
#include <Zone_VEF.h>
#include <Champ_val_tot_sur_vol_base.h>

Implemente_base(Terme_Puissance_Thermique_VEF_base,"Terme_Puissance_Thermique_VEF_base",Terme_Source_VEF_base);

////printOn
Sortie& Terme_Puissance_Thermique_VEF_base::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

////readOn
Entree& Terme_Puissance_Thermique_VEF_base::readOn(Entree& s)
{
  const Equation_base& eqn = equation();
  Terme_Puissance_Thermique::lire_donnees(s,eqn);
  champs_compris_.ajoute_champ(la_puissance);
  Nom name_file("Puissance_Thermique");
  modify_name_file(name_file);
  set_fichier(name_file);
  set_description("Heat power release = Integral(P*dv) [W]");
  return s ;
}

void Terme_Puissance_Thermique_VEF_base::associer_zones(const Zone_dis& zone_dis,
                                                        const Zone_Cl_dis& zone_cl_dis)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF,zone_cl_dis.valeur());
  iter->associer_zones(zvef,zclvef);
}

int Terme_Puissance_Thermique_VEF_base::initialiser(double temps)
{
  Terme_Source_VEF_base::initialiser(temps);
  initialiser_champ_puissance(equation());
  return 1;
}


