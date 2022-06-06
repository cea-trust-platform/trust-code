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

#include <VEF_1D.h>
#include <Zone_VEF.h>
#include <Champ_Ostwald_VEF.h>
#include <Navier_Stokes_std.h>
#include <Tri_VEF.h>
#include <Tetra_VEF.h>
#include <Quadri_VEF.h>
#include <Hexa_VEF.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(VEF_1D,"VEF_1D",VEF_discretisation);

// printOn et readOn

Sortie& VEF_1D::printOn(Sortie& s ) const
{
  return s;
}

Entree& VEF_1D::readOn(Entree& is )
{
  return is;
}

void VEF_1D::vitesse(const Schema_Temps_base& sch,
                     Zone_dis& z,
                     Champ_Inc& ch) const
{
  Cerr << "Discretisation de la vitesse" << finl;
  Zone_VEF& zone_VEF=ref_cast(Zone_VEF, z.valeur());
  if (sub_type(Tri_VEF,zone_VEF.type_elem().valeur()) || sub_type(Tetra_VEF,zone_VEF.type_elem().valeur()))
    {
      ch.typer("Champ_P1NC");
      Champ_P1NC& ch_vitesse=ref_cast(Champ_P1NC, ch.valeur());
      ch_vitesse.associer_zone_dis_base(zone_VEF);
      ch_vitesse.nommer("vitesse");
      ch_vitesse.fixer_nb_comp(1);
      ch_vitesse.fixer_nb_valeurs_temporelles(sch.nb_valeurs_temporelles());
      ch_vitesse.fixer_nb_valeurs_nodales(zone_VEF.nb_faces());
      ch_vitesse.fixer_unite("m/s");
      ch_vitesse.changer_temps(sch.temps_courant());
    }
  else if (sub_type(Quadri_VEF,zone_VEF.type_elem().valeur()) || sub_type(Hexa_VEF,zone_VEF.type_elem().valeur()))
    {
      ch.typer("Champ_Q1NC");
      Champ_Q1NC& ch_vitesse=ref_cast(Champ_Q1NC, ch.valeur());
      ch_vitesse.associer_zone_dis_base(zone_VEF);
      ch_vitesse.nommer("vitesse");
      ch_vitesse.fixer_nb_comp(1);
      ch_vitesse.fixer_nb_valeurs_temporelles(sch.nb_valeurs_temporelles());
      ch_vitesse.fixer_nb_valeurs_nodales(zone_VEF.nb_faces());
      ch_vitesse.fixer_unite("m/s");
      ch_vitesse.changer_temps(sch.temps_courant());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans VEF_discretisation::vitesse" << finl;
      exit();
    }
}


