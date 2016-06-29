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
// File:        Marqueur_Lagrange_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Marqueur_Lagrange_base.h>
#include <Zone_dis_base.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <Domaine.h>
#include <Ensemble_Lagrange_base.h>

Implemente_base_sans_constructeur(Marqueur_Lagrange_base,"Marqueur_Lagrange_base",Objet_U);

Marqueur_Lagrange_base::Marqueur_Lagrange_base()
{
  t_debut_integr_ = -1.;
}

Sortie& Marqueur_Lagrange_base::printOn(Sortie& os) const
{

  return os;
}

Entree& Marqueur_Lagrange_base::readOn(Entree& is)
{
  return is;
}


// -discretisation du champ densite_particules
// -association de la zone a l ensemble d points suivis
// -t_debut_integr_ fixe par defaut a t_init si pas de valeur lue dans le jdd
void Marqueur_Lagrange_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  const Zone& zone = pb.domaine().zone(0);
  double temps = pb.schema_temps().temps_courant();
  Nom nom="densite_particules";
  Nom unite="sans_dimension";
  dis.discretiser_champ("champ_elem",zone_dis,nom,unite,1,temps,densite_particules_);
  champs_compris_.ajoute_champ(densite_particules_.valeur());

  ensemble_points().associer_zone(zone);
  if (t_debut_integr_==-1.)
    t_debut_integr_ = pb.schema_temps().temps_init();
}


void Marqueur_Lagrange_base::mettre_a_jour(double temps)
{
  calculer_valeurs_champs();
  densite_particules_.changer_temps(temps);
}

void Marqueur_Lagrange_base::creer_champ(const Motcle& motlu)
{

}

const Champ_base& Marqueur_Lagrange_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}

void Marqueur_Lagrange_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<" Marqueur_Lagrange_base : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}

