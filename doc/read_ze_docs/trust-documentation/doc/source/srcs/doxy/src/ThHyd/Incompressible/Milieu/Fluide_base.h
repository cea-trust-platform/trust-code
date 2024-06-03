/****************************************************************************
* Copyright (c) 2023, CEA
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
  Declare_instanciable_sans_constructeur(Fluide_base);

public :

  Fluide_base();
  inline const Champ_Don& viscosite_cinematique() const;
  inline const Champ_Don& viscosite_dynamique() const;
  inline const Champ_Don& beta_c() const;
  inline Champ_Don& viscosite_cinematique();
  inline Champ_Don& viscosite_dynamique();
  inline Champ_Don& beta_c();

  virtual const Champ_base& energie_interne() const;
  virtual Champ_base&       energie_interne();
  virtual const Champ_base& enthalpie() const;
  virtual Champ_base&       enthalpie();

  // Modif CHD 07/05/03 Ajout des parametres pour un fluide semi
  // transparent on les ramene ici pour ne plus avoir a utiliser
  // de Fluide incompressible semi transparent.
  int is_rayo_semi_transp() const override;
  int is_rayo_transp() const override;
  void fixer_type_rayo();
  void reset_type_rayo();
  int longueur_rayo_is_discretised();
  inline Champ_Don& kappa();
  inline const Champ_Don& kappa() const;
  inline Champ_Don& indice();
  inline const Champ_Don& indice() const;
  inline Champ_Don& longueur_rayo();
  inline const Champ_Don& longeur_rayo() const;

  void set_param(Param& param) override;
  void verifier_coherence_champs(int& err,Nom& message) override;
  bool initTimeStep(double dt) override;
  void mettre_a_jour(double ) override;
  int initialiser(const double temps) override;
  void creer_champs_non_lus() override;
  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;
  virtual void set_h0_T0(double h0, double T0);
  virtual int is_incompressible() const { return 0; }

protected :

  void creer_e_int() const; //creation sur demande de e_int / h
  mutable int e_int_auto_ = 0; //1 si on a cree e_int
  static void calculer_e_int(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);//fonction de calcul par defaut

  mutable Champ e_int; //pour la creation sur demande
  Champ h;
  Champ_Don mu, nu, beta_co;
  double h0_ = 0, T0_ = 0;

  // Parametres du fluide rayonnant semi transparent
  Champ_Don coeff_absorption_;
  Champ_Don indice_refraction_;
  // Longueur caractaristique de la longueur de penetration
  // du rayonnement dans le milieu semi transparent definie
  // comme l = 1/(3*kappa)
  Champ_Don longueur_rayo_;

  void creer_nu();
  virtual void calculer_nu();
};


/*! @brief Renvoie la viscosite dynamique, mu.
 *
 * (version const)
 *
 * @return (Champ_Don&) la viscosite dynamique, mu
 */
inline const Champ_Don& Fluide_base::viscosite_dynamique() const
{
  return mu;
}


/*! @brief Renvoie la viscosite dynamique, mu.
 *
 * @return (Champ_Don&) la viscosite dynamique, mu
 */
inline Champ_Don& Fluide_base::viscosite_dynamique()
{
  return mu;
}


/*! @brief Renvoie la viscosite cinematique, nu.
 *
 * (vesrion const)
 *
 * @return (Champ_Don&) la viscosite cinematique, nu
 */
inline const Champ_Don& Fluide_base::viscosite_cinematique() const
{
  return nu;
}


/*! @brief Renvoie la viscosite cinematique, nu.
 *
 * @return (Champ_Don&) la viscosite cinematique, nu
 */
inline Champ_Don& Fluide_base::viscosite_cinematique()
{
  return nu;
}


/*! @brief Renvoie la dilatabilite du constituant, beta_co.
 *
 * (version const)
 *
 * @return (Champ_Don&) la dilatabilite du constituant, beta_co
 */
inline const Champ_Don& Fluide_base::beta_c() const
{
  return beta_co;
}


/*! @brief Renvoie la dilatabilite du constituant, beta_co.
 *
 * @return (Champ_Don&) la dilatabilite du constituant, beta_co
 */
inline Champ_Don& Fluide_base::beta_c()
{
  return beta_co;
}


/*! @brief Renvoie le coefficient d'absorbtion du fluide
 *
 * @return (Champ_Don&) le coefficient d'absorbtion du fluide
 */
inline Champ_Don& Fluide_base::kappa()
{
  return coeff_absorption_;
}

/*! @brief Renvoie le coefficient d'absorbtion du fluide
 *
 * @return (Champ_Don&) le coefficient d'absorbtion du fluide
 */
inline const Champ_Don& Fluide_base::kappa() const
{
  return coeff_absorption_;
}

/*! @brief Renvoie l'indice de refraction du fluide
 *
 * @return (Champ_Don&) l'indice de refraction du fluide
 */
inline Champ_Don& Fluide_base::indice()
{
  return indice_refraction_;
}

/*! @brief Renvoie l'indice de refraction du fluide
 *
 * @return (Champ_Don&) l'indice de refraction du fluide
 */
inline const Champ_Don& Fluide_base::indice() const
{
  return indice_refraction_;
}

/*! @brief Renvoie la longueur de penetration du rayonnement dans le fluide definie comme l = 1/(3*kappa)
 *
 * @return (Champ_Don&) la longueur de penetration du rayonnement dans le fluide definie comme l = 1/(3*kappa)
 */
inline Champ_Don& Fluide_base::longueur_rayo()
{
  return longueur_rayo_;
}


/*! @brief Renvoie la longueur de penetration du rayonnement dans le fluide definie comme l = 1/(3*kappa)
 *
 * @return (Champ_Don&) la longueur de penetration du rayonnement dans le fluide definie comme l = 1/(3*kappa)
 */
inline const Champ_Don& Fluide_base::longeur_rayo() const
{
  return longueur_rayo_;
}


#endif
