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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Taux_cisaillement_P0_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

#include <Taux_cisaillement_P0_VDF.h>
#include <Zone_VF.h>
#include <Champ_Face.h>
#include <Zone_Cl_VDF.h>

Implemente_instanciable(Taux_cisaillement_P0_VDF,"Taux_cisaillement_P0_VDF",Champ_Fonc_P0_VDF);


//     printOn()
/////

Sortie& Taux_cisaillement_P0_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Taux_cisaillement_P0_VDF::readOn(Entree& s)
{
  return s ;
}

void Taux_cisaillement_P0_VDF::associer_champ(const Champ_Face& la_vitesse, const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VDF  = ref_cast(Zone_Cl_VDF, la_zone_Cl_dis_base);
  vitesse_= la_vitesse;
}

void Taux_cisaillement_P0_VDF::mettre_a_jour(double tps)
{
  int nb_elem = la_zone_VF->nb_elem();
  DoubleVect tmp(nb_elem);
  vitesse_->calcul_S_barre(vitesse_.valeur().valeurs(),tmp,la_zone_Cl_VDF.valeur());
  DoubleTab& S = valeurs(); // Shear rate
  for (int i=0; i<nb_elem; i++)
    S(i) = sqrt(tmp(i));
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

