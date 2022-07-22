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

#include <Champ_Inc.h>

Implemente_deriv(Champ_Inc_base);
Implemente_instanciable(Champ_Inc,"Champ_Inc",DERIV(Champ_Inc_base));


/*! @brief Surcharge Objet_U::printOn(Sortie&) Imprime le champ sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_Inc::printOn(Sortie& os) const
{
  return DERIV(Champ_Inc_base)::printOn(os);
}


/*! @brief Lecture du champ a partir d'un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree sur lequel lire
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Champ_Inc::readOn(Entree& is)
{
  return DERIV(Champ_Inc_base)::readOn(is);
}


/*! @brief Appel a l'objet sous-jacent Fixe le nombre de pas de temps a conserver.
 *
 * @param (int i) le nombre de pas de temps a conserver
 * @return (int) le nombre de pas de temps a conserver
 */
int Champ_Inc::fixer_nb_valeurs_temporelles(int i)
{
  return valeur().fixer_nb_valeurs_temporelles(i);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de pas de temps conserve.
 *
 * @return (int) le nombre de pas de temps conserve
 */
int Champ_Inc::nb_valeurs_temporelles() const
{
  return valeur().nb_valeurs_temporelles() ;
}

/*! @brief Appel a l'objet sous-jacent Fixe le nombre de degres de liberte par composante.
 *
 * @param (int i) le nombre de degres de libertee par composante
 * @return (int) le nombre de degres de libertee par composante
 */
int Champ_Inc::fixer_nb_valeurs_nodales(int i)
{
  return valeur().fixer_nb_valeurs_nodales(i);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de degres de liberte par composante.
 *
 * @return (int) le nombre de degres de liberte par composante
 */
int Champ_Inc::nb_valeurs_nodales() const
{
  return valeur().nb_valeurs_nodales();
}

/*! @brief Appel a l'objet sous-jacent Associe le champ a une zone discretisee.
 *
 * @param (Zone_dis_base& z)
 * @return (Zone_dis_base&) la zone discretise associee au champ
 */
const Zone_dis_base& Champ_Inc::associer_zone_dis_base(const Zone_dis_base& z)
{
  valeur().associer_zone_dis_base(z);
  return z;
}
/*! @brief Appel a l'objet sous-jacent Renvoie la zone discretisee associee au champ.
 *
 * @return (Zone_dis_base&)
 */
const Zone_dis_base& Champ_Inc::zone_dis_base() const
{
  return valeur().zone_dis_base();
}

/*! @brief Appel a l'objet sous-jacent Fixe le temps courant.
 *
 * @param (double& t)
 */
double Champ_Inc::changer_temps(const double t)
{
  return valeur().changer_temps(t);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le temps courant
 *
 * @return (double) le temps courant
 */
double Champ_Inc::temps() const
{
  return valeur().temps();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps courant
 *
 * @return (DoubleTab&) le tableau des valeurs au temps courant
 */
DoubleTab& Champ_Inc::valeurs()
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps courant
 *
 *     (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs au temps courant
 */
const DoubleTab& Champ_Inc::valeurs() const
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps t+i.
 *
 * @param (int i) l'indice de decalage dans le futur
 * @return (DoubleTab&) le tableau des valeurs au temps t+i
 */
DoubleTab& Champ_Inc::futur(int i)
{
  return valeur().futur(i);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps t+i.
 *
 *     (version const)
 *
 * @param (int i) l'indice de decalage dans le futur
 * @return (DoubleTab&) le tableau des valeurs au temps t+i
 */
const DoubleTab& Champ_Inc::futur(int i) const
{
  return valeur().futur(i);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps t-i.
 *
 * @param (int i) l'indice de decalage dans le futur
 * @return (DoubleTab&) le tableau des valeurs au temps t-i
 */
DoubleTab& Champ_Inc::passe(int i)
{
  return valeur().passe(i);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs au temps t-i.
 *
 *     (version const)
 *
 * @param (int i) l'indice de decalage dans le futur
 * @return (DoubleTab&) le tableau des valeurs au temps t-i
 */
const DoubleTab& Champ_Inc::passe(int i) const
{
  return valeur().passe(i);
}

/*! @brief Appel a l'objet sous-jacent Avance le temps courant du champ de i pas de temps.
 *
 * @param (int i) le nombre de pas de temps duquel on avance
 * @return (Champ_Inc_base&) le champ inconnu place au nouveau temps courant
 */
Champ_Inc_base& Champ_Inc::avancer(int i)
{
  return valeur().avancer(i);
}

/*! @brief Appel a l'objet sous-jacent Recule le temps courant du champ de i pas de temps.
 *
 * @param (int i) le nombre de pas de temps duquel on recule
 * @return (Champ_Inc_base&) le champ inconnu place au nouveau temps courant
 */
Champ_Inc_base& Champ_Inc::reculer(int i)
{
  return valeur().reculer(i);
}


/*! @brief Appel a l'objet sous-jacent Lit les valeurs du champs a partir d'un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree
 * @return (int) code de retour propage
 */
int Champ_Inc::lire_donnees(Entree& is)
{
  return valeur().lire_donnees(is);
}

/*! @brief Appel a l'objet sous-jacent Effectue une reprise sur un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (int) code de retour propage
 */
int Champ_Inc::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

/*! @brief Appel a l'objet sous-jacent Effectue une sauvegarde sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) code de retour propage
 */
int Champ_Inc::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

/*! @brief Appel a l'objet sous-jacent calcule l'integrale de l'une des composantes
 *
 *     du champ sur le domaine qui lui correspond
 *
 * @param (int ncomp) le numero de la composante integree en espace
 * @return (double) valeur de l'integrale
 */
double Champ_Inc::integrale_espace(int ncomp) const
{
  return valeur().integrale_espace(ncomp);
}

/*! @brief
 *
 */
void Champ_Inc::mettre_a_jour(double un_temps)
{
  valeur().mettre_a_jour(un_temps);
}

/*! @brief Appel a l'objet sous-jacent Associe le champ a l'equation dont il est une inconnue
 *
 * @param (Equation_base& eqn) l'equation auquel le champ doit s'associer
 */
void Champ_Inc::associer_eqn(const Equation_base& eqn)
{
  valeur().associer_eqn(eqn);
}

/*! @brief Appel a l'objet sous-jacent Calcule la trace du champ sur une frontiere et renvoie
 *
 *     les valeurs.
 *
 * @param (Frontiere_dis_base& fr) frontiere discretisee sur laquelle on veut calculer la trace du champ au temps tps
 * @param (DoubleTab& x, double tps) les valeurs du champ sur la frontiere au temps tps
 * @return (DoubleTab&) les valeurs du champ sur la frontiere au temps tps
 */
DoubleTab& Champ_Inc::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return valeur().trace(fr, x, tps,distant);
}

