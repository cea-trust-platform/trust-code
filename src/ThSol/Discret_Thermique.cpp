/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Discret_Thermique.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_Tabule.h>

#include <Milieu_base.h>


Implemente_base(Discret_Thermique, "Discret_Thermique", Discretisation_base);

Sortie& Discret_Thermique::printOn(Sortie& s) const { return s; }

Entree& Discret_Thermique::readOn(Entree& s) { return s; }

void Discret_Thermique::temperature(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch, int nb_comp) const
{
  Cerr << "Discretisation de la temperature" << finl;
  discretiser_champ("temperature", z, "temperature", "K", nb_comp, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
}

void Discret_Thermique::enthalpie(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch, int nb_comp) const
{
  Cerr << "Discretisation de l'enthalpie" << finl;
  discretiser_champ("temperature" /* comme T */, z, "enthalpie", "J/kg", nb_comp, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
}

void Discret_Thermique::Fluctu_Temperature(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch) const
{
  Cerr << "Discretisation des fluctuations en temperature" << finl;
  Noms noms(2), unit(2);
  noms[0] = "variance_temperature";
  noms[1] = "taux_dissipation_temperature";
  unit[0] = "K2";
  unit[1] = "m2s-3";
  discretiser_champ("temperature", z, multi_scalaire, noms, unit, 2, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
  ch->nommer("Fluctu_Temperature");
}
void Discret_Thermique::Flux_Chaleur_Turb(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch) const
{
  Cerr << "Discretisation du flux de chaleur turbulente" << finl;
  discretiser_champ("vitesse", z, "Flux_Chaleur_Turbulente", "truc1", dimension, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
}

void Discret_Thermique::flux_neutronique(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch, int nb_comp) const
{
  Cerr << "Discretisation du flux neutronique" << finl;
  discretiser_champ("temperature", z, "flux_neutronique", "m-2.s-1", nb_comp, sch.nb_valeurs_temporelles(), sch.temps_courant(), ch);
}
