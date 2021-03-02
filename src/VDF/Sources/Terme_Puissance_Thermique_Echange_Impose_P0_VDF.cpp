/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Terme_Puissance_Thermique_Echange_Impose_P0_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Puissance_Thermique_Echange_Impose_P0_VDF.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Terme_Puissance_Thermique_Echange_Impose_P0_VDF,"terme_puissance_thermique_echange_impose_VDF_P0_VDF",Source_base);



//// printOn
//

Sortie& Terme_Puissance_Thermique_Echange_Impose_P0_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}


//// readOn
//

Entree& Terme_Puissance_Thermique_Echange_Impose_P0_VDF::readOn(Entree& s )
{
  Param param(que_suis_je());
  param.ajouter("himp",&himp_,Param::REQUIRED);
  param.ajouter("Text",&Text_,Param::REQUIRED);
  param.lire_avec_accolades_depuis(s);
  set_fichier("Terme_Puissance_Thermique_Echange_Impose");
  set_description("Power (W)");
  return s ;
}

void Terme_Puissance_Thermique_Echange_Impose_P0_VDF::mettre_a_jour(double temps)
{
  int nb_elem = la_zone_VDF.valeur().nb_elem();
  const Zone_VF& zone = la_zone_VDF.valeur();
  const DoubleVect& volumes = zone.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();

  bilan()(0) = 0;

  for (int e = 0; e < nb_elem; e++)
    bilan()(0) += himp.addr()[himp.dimension(0) > 1 ? e : 0] * volumes(e) * (Text.addr()[Text.dimension(0) > 1 ? e : 0] - T(e));

  himp_.mettre_a_jour(temps);
  Text_.mettre_a_jour(temps);
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Terme_Puissance_Thermique_Echange_Impose_P0_VDF
//
////////////////////////////////////////////////////////////////////

void Terme_Puissance_Thermique_Echange_Impose_P0_VDF::associer_zones(const Zone_dis& zone_dis,
                                                                     const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}


DoubleTab& Terme_Puissance_Thermique_Echange_Impose_P0_VDF::ajouter(DoubleTab& resu )  const
{
  int nb_elem=la_zone_VDF.valeur().nb_elem();
  const Zone_VF&     zone               = la_zone_VDF.valeur();
  const DoubleVect& volumes = zone.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();

  for (int e = 0; e < nb_elem; e++)
    resu(e) -= volumes(e) * himp.addr()[himp.dimension(0) > 1 ? e : 0] * (T(e) - Text.addr()[Text.dimension(0) > 1 ? e : 0]);

  return resu;
}
DoubleTab& Terme_Puissance_Thermique_Echange_Impose_P0_VDF::calculer(DoubleTab& resu) const
{
  resu=0;
  ajouter(resu);
  return resu;
}
void Terme_Puissance_Thermique_Echange_Impose_P0_VDF::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  int nb_elem=la_zone_VDF.valeur().nb_elem();
  const Zone_VF&     zone               = la_zone_VDF.valeur();
  const DoubleVect& volumes = zone.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();

  for (int e = 0; e < nb_elem; e++)
    matrice(e, e) += volumes(e) * himp.addr()[himp.dimension(0) > 1 ? e : 0];
}
