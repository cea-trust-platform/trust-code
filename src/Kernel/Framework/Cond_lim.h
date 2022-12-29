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

#ifndef Cond_lim_included
#define Cond_lim_included

#include <Cond_lim_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Cond_lim Classe generique servant a representer n'importe quelle classe
 *
 *     derivee de la classe Cond_lim_base
 *     La plupart des methodes appellent les methodes de l'objet Probleme
 *     sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Cond_lim_base
 */
class Cond_lim : public DERIV(Cond_lim_base)
{

  Declare_instanciable(Cond_lim);

public:

  //  Cond_lim(const Cond_lim_base& cl_base) : DERIV(Cond_lim_base)(cl_base) {}
  Cond_lim(const Cond_lim&) = default;
  inline Cond_lim& operator=(const Cond_lim_base& cl_base);
  inline Cond_lim& operator=(const Cond_lim& cl);
  void adopt(Cond_lim& cl);
  inline Frontiere_dis_base& frontiere_dis();
  inline const Frontiere_dis_base& frontiere_dis() const;
  inline int operator == (const Cond_lim& x) const;
  inline void mettre_a_jour(double temps);
  inline void calculer_coeffs_echange(double temps);
  inline void completer();
  inline void injecter_dans_champ_inc(Champ_Inc& ) const;
  inline int compatible_avec_eqn(const Equation_base&) const;
  inline int compatible_avec_discr(const Discretisation_base&) const;

  inline void set_modifier_val_imp(int);

};


/*! @brief Operateur d'affectation d'un objet de type Cond_lim_base& dans une objet de type Cond_lim& .
 *
 * @param (Cond_lim_base& cl_base) la partie droite de l'affectation
 * @return (Cond_lim&) renvoie (*this)
 */
inline Cond_lim& Cond_lim::operator=(const Cond_lim_base& cl_base)
{
  DERIV(Cond_lim_base)::operator=(cl_base);
  return *this;
}

/*! @brief Operateur d'affectation d'un objet de type Cond_lim& dans une objet de type Cond_lim& .
 *
 * @param (Cond_lim_base& cl_base) la partie droite de l'affectation
 * @return (Cond_lim&) renvoie (*this)
 */
inline Cond_lim& Cond_lim::operator=(const Cond_lim& cl)
{
  DERIV(Cond_lim_base)::operator=(cl.valeur());
  return *this;
}


/*! @brief Appel a l'objet sous-jacent Renvoie la frontiere discretisee associee a la condition aux limites.
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee a la condition aux limites
 */
inline Frontiere_dis_base& Cond_lim::frontiere_dis()
{
  return valeur().frontiere_dis();
}


/*! @brief Appel a l'objet sous-jacent Renvoie la frontiere discretisee associee a la condition aux limites.
 *
 *     (version const)
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee a la condition aux limites
 */
inline const Frontiere_dis_base& Cond_lim::frontiere_dis() const
{
  return valeur().frontiere_dis();
}

/*! @brief Operateur d'egalite logique (comparaison) entre 2 conditions aux limites.
 *
 * @param (Cond_lim& x) l'operande droit de l'operateur d'egalite
 * @return (int) 1 si l'objet est egal a l'objet condition aux limites passe en parametre 0 sinon.
 */
inline int Cond_lim::operator == (const Cond_lim& x) const
{
  return (DERIV(Cond_lim_base)&) x == (DERIV(Cond_lim_base)&) *this ;
}


/*! @brief Appel a l'objet sous-jacent Mise a jour en temps de la condition aux limites.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Cond_lim::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


/*! @brief Appel a l'objet sous-jacent Calcul des coeffs d'echange de la condition aux limites.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Cond_lim::calculer_coeffs_echange(double temps)
{
  valeur().calculer_coeffs_echange(temps);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 */
inline void Cond_lim::completer()
{
  valeur().completer();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * permet a une condition aux limites de s'injecter dans le Champ_Inc
 *     avec l'equation passee en paramtre.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle on doit verifier la compatibilite
 * @return 1 si compatible 0 sinon.
 */
inline void Cond_lim::injecter_dans_champ_inc(Champ_Inc& ch) const
{
  valeur().injecter_dans_champ_inc(ch);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie 1 si les conditions aux limites sont compatibles
 *     avec l'equation passee en paramtre.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle on doit verifier la compatibilite
 * @return (int) 1 si compatible 0 sinon.
 */
inline int Cond_lim::compatible_avec_eqn(const Equation_base& eqn) const
{
  return valeur().compatible_avec_eqn(eqn);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie 1 si les conditions aux limites sont compatibles
 *     avec la discretisation passee en paramtre.
 *
 * @param (Discretisation_base& dis) la discretisation avec laquelle on doit verifier la compatibilite
 * @return (int) 1 si compatible 0 sinon
 */
inline int Cond_lim::compatible_avec_discr(const Discretisation_base& dis) const
{
  return valeur().compatible_avec_discr(dis);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Positionnement du drapeau modifier_val_imp.
 *
 */
inline void Cond_lim::set_modifier_val_imp(int drap)
{
  valeur().set_modifier_val_imp(drap);
}

#endif
