/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Discret_Thermique_included
#define Discret_Thermique_included

#include <Discretisation_base.h>



class Schema_Temps_base;
class Milieu_base;

/*! @brief Class Discret_Thermique Cette classe est la classe de base representant une discretisation
 *
 *     spatiale appliquee aux problemes thermiques.
 *     Les methodes virtuelles pures sont a implementer dans les classes
 *     derivees pour typer et discretiser les champs portes par les
 *     equations liees a la discretisation.
 *
 * @sa Discretisation_base, Classe abstraite, Methodes abstraites, void temperature(const Schema_Temps_base&, Domaine_dis_base&, Champ_Inc_base&) const, void proprietes_physiques_milieu(Domaine_dis_base& ,Milieu_base& ,const Champ_Inc_base& ) const
 */
class Discret_Thermique : public Discretisation_base
{
  Declare_base(Discret_Thermique);

public :

  void temperature(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&, int nb_comp = 1) const;
  void enthalpie(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&, int nb_comp = 1) const;
  void flux_neutronique(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Inc_base)& ch, int nb_comp=1) const;
  void Fluctu_Temperature(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&) const ;
  void Flux_Chaleur_Turb(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Inc_base)&) const;
  virtual void t_paroi(const Domaine_dis_base& z,const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& , OWN_PTR(Champ_Fonc_base)& ch) const
  {
    Cerr << "Discret_Thermique::t_paroi() does nothing" << finl;
    Cerr << que_suis_je() << " needs to overload it !" << finl;
    Process::exit();
  }
};

#endif
