/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Mod_turb_hyd_ss_maille_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_turb_hyd_ss_maille_VEF.h>
#include <distances_VEF.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Zone_Cl_dis.h>

Implemente_base(Mod_turb_hyd_ss_maille_VEF,"Mod_turb_hyd_ss_maille_VEF",Mod_turb_hyd_ss_maille);

//// printOn
//

Sortie& Mod_turb_hyd_ss_maille_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


Entree& Mod_turb_hyd_ss_maille_VEF::readOn(Entree& is )
{
  return Mod_turb_hyd_ss_maille::readOn(is);
}

void Mod_turb_hyd_ss_maille_VEF::associer(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

void Mod_turb_hyd_ss_maille_VEF::calculer_longueurs_caracteristiques()
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem_tot();
  l_.resize(nb_elem);

  calcul_longueur_filtre(l_, methode, zone_VEF);
}
