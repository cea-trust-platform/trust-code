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

#ifndef VEF_discretisation_included
#define VEF_discretisation_included

/*! @brief class VEF_discretisation (schema de base) Classe qui gere la dicretisation VEF du probleme
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

class VEF_discretisation : public Discret_Thyd
{
  Declare_instanciable(VEF_discretisation);
public :
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, int nb_pas_dt, double temps, OWN_PTR(Champ_Inc_base)& champ,
                         const Nom& sous_type = NOM_VIDE) const override;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, OWN_PTR(Champ_Fonc_base)& champ) const override;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, OWN_PTR(Champ_Don_base)& champ) const override;
  void distance_paroi(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void distance_paroi_globale(const Schema_Temps_base&, Domaine_dis_base&, OWN_PTR(Champ_Fonc_base)&) const override; // Distance paroi definie sur tout le domaine
  void proprietes_physiques_fluide_Ostwald(const Domaine_dis_base&, Fluide_Ostwald&, const Navier_Stokes_std&, const Champ_Inc_base&) const override;
  Nom domaine_cl_dis_type() const override { return "Domaine_Cl_VEF"; }

  void vorticite(Domaine_dis_base&, const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const;
  void creer_champ_vorticite(const Schema_Temps_base& ,const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const override;
  void critere_Q(const Domaine_dis_base& z,const Domaine_Cl_dis_base&,const Champ_Inc_base& vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void y_plus(const Domaine_dis_base& z,const Domaine_Cl_dis_base&,const Champ_Inc_base& vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void grad_T(const Domaine_dis_base& z,const Domaine_Cl_dis_base&,const Champ_Inc_base& temperature, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void grad_u(const Domaine_dis_base& z,const Domaine_Cl_dis_base&,const Champ_Inc_base& temperature, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void h_conv(const Domaine_dis_base& z,const Domaine_Cl_dis_base&,const Champ_Inc_base& temperature, OWN_PTR(Champ_Fonc_base)& ch, Motcle& nom, int temp_ref) const override;
  void taux_cisaillement(const Domaine_dis_base&, const Domaine_Cl_dis_base&,const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)&) const override;
  void residu(const Domaine_dis_base& , const Champ_Inc_base&, OWN_PTR(Champ_Fonc_base)& ) const override;

  bool is_vef() const override { return true; }

  inline int get_P1Bulle() const { return P1Bulle_; }
  inline int get_alphaE() const  { return alphaE_;  }
  inline int get_alphaS() const  { return alphaS_;  }
  inline int get_alphaA() const  { return alphaA_;  }
  inline int get_alphaRT() const { return alphaRT_;  }
  inline int get_modif_div_face_dirichlet() const  { return modif_div_face_dirichlet_;  }
  inline int get_cl_pression_sommet_faible() const { return cl_pression_sommet_faible_; }

private:
  int P1Bulle_ = 1, alphaE_ = 1, alphaS_ = 1, alphaA_ = 0, alphaRT_ = 0, modif_div_face_dirichlet_ = 0;
  int cl_pression_sommet_faible_ = 0; // determine si les cl de pression sont imposees de facon faible ou forte -> voir divergence et assembleur, zcl

  void discretiser_champ_(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, int nb_pas_dt, double temps, OWN_PTR(Champ_Inc_base)& champ,
                          const Nom& sous_type = NOM_VIDE) const;

  void discretiser_champ_fonc_don(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Objet_U& champ) const;

  void discretiser_champ_fonc_don_(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Objet_U& champ) const;

  void modifier_champ_tabule(const Domaine_dis_base& domaine_dis,Champ_Fonc_Tabule& ch_tab,const VECT(REF(Champ_base))& ch_inc) const override;

  Domaine_dis_base& discretiser() const override;
};

#endif /* VEF_discretisation_included */
