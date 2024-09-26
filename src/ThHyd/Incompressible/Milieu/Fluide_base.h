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

#ifndef Fluide_base_included
#define Fluide_base_included

#include <Milieu_base.h>
#include <TRUST_Ref.h>

class Champ_base;

/*! @brief classe Fluide_base Cette classe represente un d'un fluide incompressible ainsi que
 *
 *     ses proprietes:
 *         - viscosite cinematique, (mu)
 *         - viscosite dynamique,   (nu)
 *         - masse volumique,       (rho)
 *         - diffusivite,           (alpha)
 *         - conductivite,          (lambda)
 *         - capacite calorifique,  (Cp)
 *         - dilatabilite thermique du constituant (beta_co)
 *
 * @sa Milieu_base
 */
class Fluide_base : public Milieu_base
{
  Declare_instanciable(Fluide_base);
public :
  void set_param(Param& param) override;
  void verifier_coherence_champs(int& err, Nom& message) override;
  bool initTimeStep(double dt) override;
  void mettre_a_jour(double) override;
  int initialiser(const double temps) override;
  void creer_champs_non_lus() override;
  void discretiser(const Probleme_base& pb, const Discretisation_base& dis) override;
  virtual void set_h0_T0(double h0, double T0);
  virtual int is_incompressible() const { return 0; }

  const Champ_base& energie_interne() const;
  Champ_base& energie_interne();
  const Champ_base& enthalpie() const;
  Champ_base& enthalpie();
  const Champ_base& temperature_multiphase() const;
  Champ_base& temperature_multiphase();

  inline const Champ_Don& viscosite_cinematique() const { return ch_nu_; }
  inline Champ_Don& viscosite_cinematique() { return ch_nu_; }
  inline const Champ_Don& viscosite_dynamique() const { return ch_mu_; }
  inline Champ_Don& viscosite_dynamique() { return ch_mu_; }

  // Renvoie la dilatabilite du constituant, beta_co.
  inline const Champ_Don& beta_c() const { return ch_beta_co_; }
  inline Champ_Don& beta_c() { return ch_beta_co_; }

  // Renvoie le coefficient d'absorbtion du fluide
  inline Champ_Don& kappa() { return coeff_absorption_; }
  inline const Champ_Don& kappa() const { return coeff_absorption_; }

  // Renvoie l'indice de refraction du fluide
  inline Champ_Don& indice() { return indice_refraction_; }
  inline const Champ_Don& indice() const { return indice_refraction_; }

  //  Renvoie la longueur de penetration du rayonnement dans le fluide definie comme l = 1/(3*kappa)
  inline Champ_Don& longueur_rayo() { return longueur_rayo_; }
  inline const Champ_Don& longeur_rayo() const { return longueur_rayo_; }

  // Modif CHD 07/05/03 Ajout des parametres pour un fluide semi transparent on les ramene ici pour ne plus avoir a utiliser de Fluide incompressible semi transparent.
  int is_rayo_semi_transp() const override;
  int is_rayo_transp() const override;
  void fixer_type_rayo();
  void reset_type_rayo();
  int longueur_rayo_is_discretised();
protected :
  void creer_e_int() const; // creation sur demande de e_int / h
  void creer_temperature_multiphase() const; // seulement si Energie_Multiphase_Enthalpie
  void calculer_temperature_multiphase() const; // seulement si Energie_Multiphase_Enthalpie

  mutable int e_int_auto_ = 0; //1 si on a cree e_int
  static void calculer_e_int(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv); // fonction de calcul par defaut

  mutable OWN_PTR(Champ_base) ch_e_int_, ch_h_ou_T_; //pour la creation sur demande : h is Energie_Multiphase et T si Energie_Multiphase_Enthalpie
  Champ_Don ch_mu_, ch_nu_, ch_beta_co_;
  double h0_ = 0, T0_ = 0;

  // Parametres du fluide rayonnant semi transparent
  Champ_Don coeff_absorption_, indice_refraction_;

  // Longueur caractaristique de la longueur de penetration du rayonnement dans le milieu semi transparent definie comme l = 1/(3*kappa)
  Champ_Don longueur_rayo_;

  void creer_nu();
  virtual void calculer_nu();
};

#endif /* Fluide_base_included */
