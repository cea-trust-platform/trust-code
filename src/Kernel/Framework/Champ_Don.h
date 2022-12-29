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

#ifndef Champ_Don_included
#define Champ_Don_included

#include <Champ_Don_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Champ_Don Classe generique de la hierarchie des champs donnes, un objet
 *
 *      Champ_Don peut referencer n'importe quel objet derivant de
 *      Champ_Don_base.
 *      La plupart des methodes appellent les methodes de l'objet Probleme
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Champ_Don_base Ch_proto
 */
class Champ_Don : public DERIV(Champ_Don_base), public Champ_Proto
{
  Declare_instanciable(Champ_Don);
public :

  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override;
  inline const DoubleTab& valeurs() const override;
  inline operator DoubleTab& () = delete;
  inline operator const DoubleTab& () const = delete;
  inline int fixer_nb_valeurs_nodales(int );
  inline int nb_valeurs_nodales() const;
  inline int nb_comp() const;
  inline Champ_base& affecter_(const Champ_base& ch);
  inline Champ_base& affecter_compo(const Champ_base& , int );
  inline void mettre_a_jour(double temps);
  inline int initialiser(const double temps);
};

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le tableau des valeurs.
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline DoubleTab& Champ_Don::valeurs()
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le tableau des valeurs.
 *     (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline const DoubleTab& Champ_Don::valeurs() const
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent Fixe le nombre de degres de liberte par composante
 *
 * @param (int nb_noeuds) le nombre de degres de liberte par composante
 * @return (int) le nombre de degres de liberte par composante
 */
inline int Champ_Don::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  return valeur().fixer_nb_valeurs_nodales(nb_noeuds);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de composantes du champ.
 *
 * @return (int) le nombre de composantes du champ
 */
inline int Champ_Don::nb_comp() const
{
  return valeur().nb_comp();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de degres de liberte par composante
 *
 * @return (int) le nombre de degres de liberte par composante
 */
inline int Champ_Don::nb_valeurs_nodales() const
{
  return valeur().nb_valeurs_nodales();
}

/*! @brief Appel a l'objet sous-jacent Affectation d'un Champ_Base dans un Champ_Don
 *
 * @param (Champ_base& ch) le champ partie droite de l'affectation
 * @return (Champ_base&) le champ affecte (upcaste en Champ_base&)
 */
inline Champ_base& Champ_Don::affecter_(const Champ_base& ch)
{
  return valeur().affecter(ch);
}


/*! @brief Appel a l'objet sous-jacent Affectation d'une composante d'un Champ_Base dans la meme
 *
 *     composante d'un Champ_Don.
 *
 * @param (Champ_base& ch) le champ partie droite de l'affectation
 * @param (int compo) l'indice de la composante du champ a affecte
 * @return (Champ_base&) le champ affecte (upcaste en Champ_base&)
 */
inline Champ_base& Champ_Don::affecter_compo(const Champ_base& ch, int compo)
{
  return valeur().affecter_compo(ch, compo);
}
/*! @brief Appel a l'objet sous-jacent Provoque la mise a jour du champ si il est instationnaire.
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_Don::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

/*! @brief Appel a l'objet sous-jacent Provoque l'initialisation du champ si necessaire
 *
 */
inline int Champ_Don::initialiser(const double temps)
{
  return valeur().initialiser(temps);
}

#endif
