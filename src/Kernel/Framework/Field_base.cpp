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

#include <Field_base.h>

Implemente_base_sans_constructeur(Field_base,"Field_base",Objet_U);
// XD field_base objet_u champ_base -1 Basic class of fields.


Field_base::Field_base() : nb_compo_(0), nature_(scalaire) { }

Sortie& Field_base::printOn(Sortie& s ) const { return s; }
Entree& Field_base::readOn(Entree& is ) { return is; }

/*! @brief Renvoie le nom du champ.
 *
 */
const Nom& Field_base::le_nom() const
{
  return nom_;
}

/*! @brief Donne un nom au champ
 *
 * @param (Nom& name) le nom a donner au champ
 */
void Field_base::nommer(const Nom& name)
{
  nom_ = name ;
}

/*! @brief Fixe le nombre de composantes du champ.
 *
 * Le champ est vectoriel s'il est de meme dimension que l'espace.
 *
 * @param (int i) le nombre de composantes du champs
 */
void Field_base::fixer_nb_comp(int i)
{
  // Interdiction de changer la nature du champ une fois le nb_valeurs_nodales fixe.
  nb_compo_ = i;
  noms_compo_.dimensionner(i);

  if (i == dimension) fixer_nature_du_champ(vectoriel);
  else if (i > dimension) fixer_nature_du_champ(multi_scalaire);
}

/*! @brief Fixe le nom des composantes du champ
 *
 * @param (Noms& noms) le tableau des noms a donner aux composantes du champ
 * @return (Noms&) le tableau des noms des composantes du champ
 */
const Noms& Field_base::fixer_noms_compo(const Noms& noms)
{
  return noms_compo_ = noms;
}

/*! @brief Renvoie le tableau des noms des composantes du champ
 *
 */
const Noms& Field_base::noms_compo() const
{
  return noms_compo_;
}

/*! @brief Fixe le nom de la i-eme composante du champ
 *
 * @param (int i) l'index de la composante du champ dont on veut specifier le nom
 * @param (Nom& nom) le nom a donner a la i-eme composante du champ
 * @return (Nom&) le nom de i-eme composante du champ
 * @throws index de la composante du champ invalide
 */
const Nom& Field_base::fixer_nom_compo(int i, const Nom& nom)
{
  assert(i < nb_comp());
  return noms_compo_[i] = nom;
}

/*! @brief Renvoie le nom de la ieme composante du champ
 *
 * @param (int i) l'index de la composante du champ dont on veut specifier le nom
 * @return (Nom& nom) le nom de i-eme composante du champ
 * @throws index de la composante du champ invalide
 */
const Nom& Field_base::nom_compo(int i) const
{
  assert(i < nb_comp());
  return noms_compo_[i];
}

/*! @brief Fixe le nom d'un champ scalaire
 *
 * @param (Nom& nom) le nom a donner au champ (scalaire)
 * @return (Nom&) le nom du champ scalaire
 * @throws le champ n'est pas scalaire
 */
const Nom& Field_base::fixer_nom_compo(const Nom& nom)
{
  assert(nb_comp() == 1);
  nommer(nom);
  noms_compo_.dimensionner_force(1);
  return noms_compo_[0] = nom;
}

/*! @brief Renvoie le nom d'un champ scalaire
 *
 */
const Nom& Field_base::nom_compo() const
{
  assert(nb_comp() == 1);
  return le_nom();
}

/*! @brief Specifie les unites des composantes du champ.
 *
 * Ces unites sont specifiees grace a un tableau de Nom et peuvent etre differentes pour chaque composante du champ.
 *
 * @param (Noms& noms) les noms des unites des composantes du champ
 */
const Noms& Field_base::fixer_unites(const Noms& noms)
{
  unite_.dimensionner_force(nb_comp());
  fixer_nature_du_champ(multi_scalaire);
  return unite_ = noms;
}

/*! @brief Renvoie les unites des composantes du champ
 *
 * @return (Noms&) les noms des unites des composantes du champ
 */
const Noms& Field_base::unites() const
{
  return unite_;
}

/*! @brief Specifie l'unite de la i-eme composante du champ Signification: l'index de la composante du champ dont on veut specifier l'unite
 *
 * @param (Nom& nom) le type de l'unite a specifier
 * @return (Nom&) le type de l'unite de la i-eme composante du champ
 */
const Nom& Field_base::fixer_unite(int i, const Nom& nom)
{
  fixer_nature_du_champ(multi_scalaire);
  return unite_[i] = nom;
}

/*! @brief Renvoie l'unite de la i-eme composante du champ
 *
 * @param (int i) l'index de la composante du champ dont on veut connaitre l'unite
 * @return (Nom&) l'unite de la i-eme composante du champ
 */
const Nom& Field_base::unite(int i) const
{
  return unite_[i];
}

/*! @brief Specifie l'unite d'un champ scalaire ou dont toutes les composantes ont la meme unite
 *
 * @param (Nom& nom) l'unite a specifier
 * @return (Nom&) l'unite du champ
 */
const Nom& Field_base::fixer_unite(const Nom& nom)
{
  unite_.dimensionner_force(1);
  return unite_[0] = nom;
}

/*! @brief Renvoie l'unite d'un champ scalaire dont toutes les composantes ont la meme unite
 *
 * @return (Nom&) l'unite (commune) des composantes du champ
 */
const Nom& Field_base::unite() const
{
  assert(unite_.size() == 1);
  return unite_[0];
}

/*! @brief Fixer la nature d'un champ: scalaire, multiscalaire, vectoriel.
 *
 * Le type (enum) Nature_du_champ est defini dans Ch_base.h.
 *
 * @param (Nature_du_champ n) la nature a donner au champ
 */
Nature_du_champ Field_base::fixer_nature_du_champ(Nature_du_champ n)
{
  return nature_ = n;
}
