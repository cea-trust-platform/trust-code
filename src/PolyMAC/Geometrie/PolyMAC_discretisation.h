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

#ifndef PolyMAC_discretisation_included
#define PolyMAC_discretisation_included

/*! @brief class PolyMAC_discretisation (schema de base) Classe qui gere la dicretisation PolyMAC du probleme
 *
 *     c'est ici que :
 *     * l'on discretise les proprietes physiques du fluide et notamment celui du fluide
 *        d'Ostwald( K, N, Mu, ...)
 *     * l'on associe le champ d'Ostwald et l'equation hydraulique, le fluide et la
 *     domaine_dis_base
 *     ** ( nouvelles procedures :
 *   void proprietes_physiques_fluide_Ostwald(Domaine_dis_base& ,Fluide_Ostwald& ,
 *            const Navier_Stokes_std& , const Champ_Inc_base& ) const;
 *    a besoin de la classe Fluide_Ostwald pour avoir acces au fluide etudie
 *                          Navier_Stokes_Std pour avoir acces a l'equation hydraulique
 *
 * @sa Discret_Thyd_Turb
 */

#include <Discret_Thyd.h>

class Fluide_Incompressible;
class Navier_Stokes_std;
class Fluide_Ostwald;

class PolyMAC_discretisation: public Discret_Thyd
{
  Declare_instanciable(PolyMAC_discretisation);

public:
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, int nb_pas_dt, double temps, OWN_PTR(Champ_Inc_base)& champ,
                         const Nom& sous_type = NOM_VIDE) const override;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, OWN_PTR(Champ_Fonc_base)& champ) const override;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Champ_Don& champ) const override;
  Nom domaine_cl_dis_type() const override { return "Domaine_Cl_PolyMAC"; }

  void distance_paroi(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void distance_paroi_globale(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const override;
  void proprietes_physiques_fluide_Ostwald(const Domaine_dis_base&, Fluide_Ostwald&, const Navier_Stokes_std&, const Champ_Inc_base&) const override;

  void vorticite(Domaine_dis_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void creer_champ_vorticite(const Schema_Temps_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const override;
  void critere_Q(const Domaine_dis_base& z, const Domaine_Cl_dis_base&, const Champ_Inc_base& vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void y_plus(const Domaine_dis_base& z, const Domaine_Cl_dis_base&, const Champ_Inc_base& vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void grad_T(const Domaine_dis_base& z, const Domaine_Cl_dis_base&, const Champ_Inc_base& temperature, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void grad_u(const Domaine_dis_base& z, const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& ch_vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void h_conv(const Domaine_dis_base& z, const Domaine_Cl_dis_base&, const Champ_Inc_base& temperature, OWN_PTR(Champ_Fonc_base)& ch, Motcle& nom, int temp_ref) const override;
  inline type_calcul_du_residu codage_du_calcul_du_residu() const override { return VIA_AJOUTER; }

  Nom get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur, const Equation_base& eqn, const REF(Champ_base) &champ_sup) const override;

  bool is_polymac() const override { return true; }
  bool is_polymac_family() const override { return true; }

private:
  virtual void discretiser_champ_fonc_don(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Objet_U& champ) const;
  void modifier_champ_tabule(const Domaine_dis_base& domaine_vdf, Champ_Fonc_Tabule& lambda_tab, const VECT(REF(Champ_base)) &ch_temper) const override;
};

#endif /* PolyMAC_discretisation_included */
