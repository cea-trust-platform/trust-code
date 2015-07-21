/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Turbulence_hyd_sous_maille_Smago_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/36
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_Smago_VEF.h>
#include <Champ_P1NC.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Zone_VEF.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_Smago_VEF,"Modele_turbulence_hyd_sous_maille_Smago_VEF",Mod_turb_hyd_ss_maille_VEF);

Turbulence_hyd_sous_maille_Smago_VEF::Turbulence_hyd_sous_maille_Smago_VEF()
{
  cs=0.18;
}

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_Smago_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Turbulence_hyd_sous_maille_Smago_VEF::readOn(Entree& is )
{
  Mod_turb_hyd_ss_maille_VEF::readOn(is);
  return is;
}

void Turbulence_hyd_sous_maille_Smago_VEF::set_param(Param& param)
{
  Mod_turb_hyd_ss_maille_VEF::set_param(param);
  param.ajouter("cs",&cs);
  param.ajouter_condition("value_of_cs_ge_0","sous_maille_smago model constant must be positive.");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_Smago_VEF
//
//////////////////////////////////////////////////////////////////////////////

Champ_Fonc& Turbulence_hyd_sous_maille_Smago_VEF::calculer_viscosite_turbulente()
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  SMA_barre.resize(nb_elem_tot);

  calculer_S_barre();

  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }
  for (int elem=0 ; elem<nb_elem ; elem++)
    visco_turb(elem)=cs*cs*l_(elem)*l_(elem)*sqrt(SMA_barre[elem]);

  double temps = mon_equation->inconnue().temps();
  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_Smago_VEF::calculer_S_barre()
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();

  Champ_P1NC::calcul_S_barre(la_vitesse,SMA_barre,zone_Cl_VEF);
}


