/****************************************************************************
* Copyright (c) 2022, CEA
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


#ifndef Champ_front_included
#define Champ_front_included


#include <Champ_front_base.h>

Declare_deriv(Champ_front_base);


/*! @brief classe Champ_front Classe generique de la hierarchie des champs aux frontieres,
 *
 *      un objet Champ_front peut referencer n'importe quel objet derivant de
 *      Champ_front_base.
 *      La plupart des methodes appellent les methodes de l'objet Probleme
 *      sous-jacent via la methode valeur() declaree grace a la macro
 * Declare_deriv().;
 *
 * @sa Champ_front_base
 */
class Champ_front : public DERIV(Champ_front_base), public Champ_Proto
{

  Declare_instanciable(Champ_front);

public:

//  Champ_front(const Champ_front_base& x);
  Champ_front(const Champ_front&) = default;
  inline Champ_front& operator=(const Champ_front_base& ch_base) ;
  inline Champ_front& operator=(const Champ_front& ch_base) ;
  inline void nommer(const Nom& name) override;
  inline const Nom& le_nom() const override;
  inline void associer_fr_dis_base(const Frontiere_dis_base& ) ;
  inline const Frontiere_dis_base& frontiere_dis() const ;
  inline Frontiere_dis_base& frontiere_dis() ;
  inline int nb_comp() const ;
  inline void mettre_a_jour(double temps);
  inline void calculer_coeffs_echange(double temps);

  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override;
  inline const DoubleTab& valeurs() const override;

  inline const Noms& get_synonyms() const;
  inline void add_synonymous(const Nom& nom);
};


/*! @brief Constructeur par copie d'un Champ_front_base dans un Champ_front.
 *
 * @param (Champ_front_base& x) le champ a copier
 */
/*
inline Champ_front::Champ_front(const Champ_front_base& x)
  :DERIV(Champ_front_base)(x)
{}
*/

/*! @brief Operateur d'affectation d'un Champ_front_base dans un Champ_front.
 *
 * @param (Champ_front_base& ch_base) le champ partie droite de l'affectation
 * @return (Champ_front&) le resultat de l'affectation
 */
inline Champ_front& Champ_front::operator=(const Champ_front_base& ch_base)
{
  DERIV(Champ_front_base)::operator=(ch_base);
  return *this;
}


/*! @brief Operateur d'affectation d'un Champ_front_base dans un Champ_front.
 *
 * @param (Champ_front_base& ch_base) le champ partie droite de l'affectation
 * @return (Champ_front&) le resultat de l'affectation
 */
inline Champ_front& Champ_front::operator=(const Champ_front& ch_front)
{
  DERIV(Champ_front_base)::operator=(ch_front.valeur());
  return *this;
}

/*! @brief Appel a l'objet sous-jacent Donne un nom au champ
 *
 * @param (Nom& name) le nom a donner au champ
 */
inline void Champ_front::nommer(const Nom& name)
{
  valeur().nommer(name) ;
}


/*! @brief Appel a l'objet sous-jacent Renvoie le nom du champ
 *
 * @return (Nom&) le nom du champ
 */
inline const Nom& Champ_front::le_nom() const
{
  return valeur().le_nom();
}


/*! @brief Appel a l'objet sous-jacent Renvoie la frontiere discretisee associee au champ
 *
 *     (version const)
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee au champ
 */
inline const Frontiere_dis_base& Champ_front::frontiere_dis() const
{
  return valeur().frontiere_dis();
}


/*! @brief Appel a l'objet sous-jacent Renvoie la frontiere discretisee associee au champ
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee au champ
 */
inline Frontiere_dis_base& Champ_front::frontiere_dis()
{
  return valeur().frontiere_dis();
}


/*! @brief Appel a l'objet sous-jacent Associe une frontiere discretisee au champ.
 *
 * @param (Frontiere_dis_base& fr) la frontiere discretisee a associer
 */
inline void Champ_front::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  valeur().associer_fr_dis_base(fr);
}


/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de composantes du champ
 *
 * @return (int) le nombre de composantes du champ
 */
inline int Champ_front::nb_comp() const
{
  return valeur().nb_comp();
}


/*! @brief Appel a l'objet sous-jacent Effectue une mise a jour en temps.
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_front::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


/*! @brief Calcul (si necessaire) les coefficients d'echange pour un couplage
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_front::calculer_coeffs_echange(double temps)
{
  valeur().calculer_coeffs_echange(temps);
}


/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs du champ
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline DoubleTab& Champ_front::valeurs()
{
  return valeur().valeurs();
}


/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs du champ
 *
 *     (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline const DoubleTab& Champ_front::valeurs() const
{
  return valeur().valeurs();
}

inline const Noms& Champ_front::get_synonyms() const
{
  return valeur().get_synonyms();
}

inline void Champ_front::add_synonymous(const Nom& nom)
{
  valeur().add_synonymous(nom);
}
#endif
