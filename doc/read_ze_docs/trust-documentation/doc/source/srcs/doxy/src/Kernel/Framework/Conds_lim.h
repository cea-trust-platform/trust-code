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

#ifndef Conds_lim_included
#define Conds_lim_included

#include <TRUST_Vector.h>
#include <Cond_lim.h>

class Domaine_dis;

/*! @brief classe Conds_lim Cette classe represente un vecteur de conditions aux limites.
 *
 *      Un objet de ce type est porte par chaque Domaine_Cl_dis_base associee
 *      a une equation. Une classe representant un vecteur d'objet est
 *      declaree grace a la macro VECT(classe_X)
 *
 * @sa Cond_lim Domaine_Cl_dis_base
 */
class Conds_lim : public VECT(Cond_lim)
{
  Declare_instanciable(Conds_lim);
public:

  inline int initialiser(double temps);
  inline void mettre_a_jour(double temps);
  inline void calculer_coeffs_echange(double temps);
  void completer(const Domaine_dis&);
  inline int compatible_avec_eqn(const Equation_base&) const;
  inline int compatible_avec_discr(const Discretisation_base&) const;

  inline void set_modifier_val_imp(int);
};


inline int Conds_lim::initialiser(double temps)
{
  int ok = 1;
  for (auto &itr : *this)
    ok = ok && itr->initialiser(temps);
  return ok;
}

/*! @brief Mise a jour en temps de toutes les conditions aux limites du vecteur.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Conds_lim::mettre_a_jour(double temps)
{
  for (auto& itr : *this) itr->mettre_a_jour(temps);
}

/*! @brief Calcul des coefficients d'echange pour toutes les conditions aux limites du vecteur.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Conds_lim::calculer_coeffs_echange(double temps)
{
  for (auto& itr : *this) itr->calculer_coeffs_echange(temps);
}

/*! @brief Renvoie si TOUTES les conditions aux limites du vecteurs sont compatibles avec l'equation passee en parametre.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle on va verifier la compatibilite
 * @return (int) 1 si toutes les conditions aux limites sont compatibles avec l'equation, 0 sinon.
 */
inline int Conds_lim::compatible_avec_eqn(const Equation_base& eqn) const
{
  int ok = 1;
  for (auto& itr : *this) ok *= itr->compatible_avec_eqn(eqn);
  return ok;
}

/*! @brief Renvoie si TOUTES les conditions aux limites du vecteurs sont compatibles avec la discretisation passee en parametre.
 *
 * @param la discretisation avec laquelle on va verifier la compatibilite
 * @return (int) 1 si toutes les conditions aux limites sont compatibles avec la discretisation, 0 sinon.
 */
inline int Conds_lim::compatible_avec_discr(const Discretisation_base& dis) const
{
  int ok = 1;
  for (auto& itr : *this) ok *= itr->compatible_avec_discr(dis);
  return ok;
}

/*! @brief Positionnement du drapeau modifier_val_imp de toutes les conditions aux limites du vecteur.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Conds_lim::set_modifier_val_imp(int drap)
{
  for (auto& itr : *this) itr->set_modifier_val_imp(drap);
}

#endif /* Conds_lim_included */
