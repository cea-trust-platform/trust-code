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
// File:        Aire_interfaciale_Yao_Morel.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Aire_interfaciale_Yao_Morel.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Debog.h>
#include <Frontiere_dis_base.h>
#include <EcritureLectureSpecial.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Navier_Stokes_std.h>
#include <DoubleTrav.h>
#include <Neumann_sortie_libre.h>
#include <Op_Conv_negligeable.h>
#include <Param.h>
#include <Schema_Implicite_base.h>
#include <SETS.h>
#include <EChaine.h>
#include <Neumann_paroi.h>
#include <Scalaire_impose_paroi.h>
#include <Echange_global_impose.h>

#define old_forme

Implemente_instanciable(Aire_interfaciale_Yao_Morel,"Aire_interfaciale_Yao_Morel|Interfacial_area_Yao_Morel",Convection_Diffusion_std);

Sortie& Aire_interfaciale_Yao_Morel::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

Entree& Aire_interfaciale_Yao_Morel::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);

  terme_convectif.set_fichier("Convection_interfacial_area");
  terme_convectif.set_description((Nom)"interfacial area transfer rate=Integral(-A*u*ndS) [kg] if SI units used");

  return is;
}

void Aire_interfaciale_Yao_Morel::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_base& un_fluide = ref_cast(Fluide_base,un_milieu);
  associer_fluide(un_fluide);
}

void Aire_interfaciale_Yao_Morel::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  int N = ref_cast(Pb_Multiphase, probleme()).nb_phases();

  Cerr << "Interfacial area discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  dis.discretiser_champ("temperature",zone_dis(),"interfacialarea","s", N,nb_valeurs_temp,temps,l_inco_ch);//une aire interfaciale par phase
  l_inco_ch.valeur().fixer_nature_du_champ(multi_scalaire);
  l_inco_ch.valeur().fixer_nom_compo(0, Nom("tau"));
  champs_compris_.ajoute_champ(l_inco_ch);

  Cerr << "Bubble diameter discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  Noms noms(N), unites(N);
  noms[0] = "diametre_bulles";
  unites[0] = "m";
  Motcle typeChamp = "champ_elem" ;
  const Zone_dis& z = ref_cast(Zone_dis, probleme().domaine_dis().zone_dis(0));
  dis.discretiser_champ(typeChamp, z.valeur(), multi_scalaire, noms , unites, N, 0, diametre_bulles);

  champs_compris_.ajoute_champ(diametre_bulles);

  Equation_base::discretiser();
  Cerr << "Echelle_temporelle_turbulente::discretiser() ok" << finl;
}

const Milieu_base& Aire_interfaciale_Yao_Morel::milieu() const
{
  return le_fluide.valeur();
}

Milieu_base& Aire_interfaciale_Yao_Morel::milieu()
{
  return le_fluide.valeur();
}

int Aire_interfaciale_Yao_Morel::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

const Motcle& Aire_interfaciale_Yao_Morel::domaine_application() const
{
  static Motcle mot("Interfacialarea");
  return mot;
}

void Aire_interfaciale_Yao_Morel::associer_fluide(const Fluide_base& un_fluide)
{
  le_fluide = un_fluide;
}
