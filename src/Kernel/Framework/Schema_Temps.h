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

#ifndef Schema_Temps_included
#define Schema_Temps_included

#include <Schema_Temps_base.h>
#include <Deriv.h>

Declare_deriv(Schema_Temps_base);

/*! @brief classe Schema_Temps Classe generique de la hierarchie des schemas en temps, un objet
 *
 *      Schema_Temps peut referencer n'importe quel objet derivant de
 *      Schema_Temps_base.
 *      La plupart des methodes appellent les methodes de l'objet
 *      Schema_Temps_base sous-jacent via la methode valeur() declaree grace
 * alamacroDeclare_deriv().;
 *
 * @sa Sch_Tps_base
 */
class Schema_Temps : public DERIV(Schema_Temps_base)
{
  Declare_instanciable(Schema_Temps);
  //
public :
  inline int sauvegarder(Sortie& ) const override;
  inline int reprendre(Entree& ) override;
  inline int faire_un_pas_de_temps_eqn_base(Equation_base& );
  inline bool iterateTimeStep(bool& converged);
  inline int mettre_a_jour();
  inline void imprimer(Sortie& os) const;
  inline double pas_de_temps() const;

  inline void corriger_dt_calcule(double& ) const;
  inline bool initTimeStep(double dt);
  inline double pas_temps_min() const;
  inline double& pas_temps_min();
  inline double pas_temps_max() const;
  inline double& pas_temps_max();
  inline int nb_pas_dt() const;
  inline double temps_courant() const;
  inline double temps_calcul() const;
  inline void changer_temps_courant(const double );
  inline double facteur_securite_pas() const;
  inline double& facteur_securite_pas();
  inline int stop();
  inline int impr(Sortie& ) const;
  inline int lsauv() const;
  inline Schema_Temps& operator=(const Schema_Temps_base&);
  //inline Schema_Temps& operator=(const Schema_Temps&);
};


/*! @brief Operateur d'affectation d'un Schema_Temp_base dans un Schema_temps.
 *
 * @return (Schema_Temps&) renvoie une reference sur *this: le schema nouvellement affecte.
 */

inline Schema_Temps& Schema_Temps::operator=(const Schema_Temps_base& sch)
{
  DERIV(Schema_Temps_base)::operator=(sch);
  return *this;
}
/*
inline Schema_Temps& Schema_Temps::operator=(const Schema_Temps& sch)
{
  *this=sch;
  return *this;
}
*/
/*! @brief Appel a l'objet sous-jacent.
 *
 * Sauvegarde le schema en temps sur un flot de sortie.
 *
 * @param (Sortie& os) le flot de sortie
 * @return (int) code de retour sous-jacent
 */
inline int Schema_Temps::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Effectue une reprise (lecture) du schema en temps a partir
 *     d'un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree
 * @return (int) code de retour sous-jacent
 */
inline int Schema_Temps::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Effectue un pas de temps sur une equation.
 *
 * @param (Equation_base& eqn) l'equation sur laquelle on effectue un pas de temps
 * @return (int) code de retour sous-jacent
 */
inline int Schema_Temps::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  return valeur().faire_un_pas_de_temps_eqn_base(eqn);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Effectue un pas de temps sur le probleme
 *
 * @param (Probleme_base& pb) le probleme sur lequel on effectue un pas de temps
 * @return (int) code de retour sous-jacent
 */
inline bool Schema_Temps::iterateTimeStep(bool& converged)
{
  return valeur().iterateTimeStep(converged);
}

/*! @brief Appel a l'objet sous-jacent Mise a jour du temps et du nombre de pas de temps.
 *
 * .
 *
 * @return (int) code de retour sous-jacent
 */
inline int Schema_Temps::mettre_a_jour()
{
  return valeur().mettre_a_jour();
}
/*! @brief Appel a l'objet sous-jacent Renvoie le pas en temps courant.
 *
 * @return (double) le pas de temps courant.
 */
inline double Schema_Temps::pas_de_temps() const
{
  return valeur().pas_de_temps();
}
/*! @brief Appel a l'objet sous-jacent Corrige le pas de temps : dtmin <= dt <= dtmax.
 *
 * @param (double& dt) le pas de temps a corriger
 */
inline void Schema_Temps::corriger_dt_calcule(double& dt) const
{
  valeur().corriger_dt_calcule(dt);
}
/*! @brief Appel a l'objet sous-jacent Pretraitement eventuel.
 *
 */
inline bool Schema_Temps::initTimeStep(double dt)
{
  return valeur().initTimeStep(dt);
}
/*! @brief Appel a l'objet sous-jacent Renvoie le pas de temps minimum.
 *
 * @return (double) le pas de temps minimum
 */
inline double Schema_Temps::pas_temps_min() const
{
  return valeur().pas_temps_min();
}
/*! @brief Appel a l'objet sous-jacent Renvoie une reference sur le pas de temps minimum.
 *
 * @return (double&) le pas de temps minimum
 */
inline double& Schema_Temps::pas_temps_min()
{
  return valeur().pas_temps_min();
}
/*! @brief Appel a l'objet sous-jacent Renvoie le pas de temps maximum.
 *
 * @return (double) le pas de temps maximum
 */
inline double Schema_Temps::pas_temps_max() const
{
  return valeur().pas_temps_max();
}
/*! @brief Appel a l'objet sous-jacent Renvoie une reference sur le pas de temps maximum.
 *
 * @return (double&) le pas de temps maximum
 */
inline double& Schema_Temps::pas_temps_max()
{
  return valeur().pas_temps_max();
}
/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de pas de temps effectues.
 *
 * @return (int) le nombre de pas de temps effectues
 */
inline int Schema_Temps::nb_pas_dt() const
{
  return valeur().nb_pas_dt();
}
/*! @brief Appel a l'objet sous-jacent Renvoie le temps courant
 *
 * @return (double) le temps courant
 */
inline double Schema_Temps::temps_courant() const
{
  return valeur().temps_courant();
}
/*! @brief Appel a l'objet sous-jacent Renvoie le temps ecoule calcule: (temps courant - temps debut)
 *
 * @return (double) le temps ecoule calcule
 */
inline double Schema_Temps::temps_calcul() const
{
  return valeur().temps_calcul();
}
/*! @brief Appel a l'objet sous-jacent Change le temps courant
 *
 * @param (double& t) la nouvelle valeur du temps courant
 */
inline void Schema_Temps::changer_temps_courant(const double t)
{
  valeur().changer_temps_courant(t);
}
/*! @brief Appel a l'objet sous-jacent Renvoie le facteur de securite ou multiplicateur de pas de temps.
 *
 * @return (double) le facteur de securite du schema en temps
 */
inline double Schema_Temps::facteur_securite_pas() const
{
  return valeur().facteur_securite_pas();
}
/*! @brief Appel a l'objet sous-jacent Renvoie une reference sur le facteur de securite ou multiplicateur de
 *
 *     pas de temps
 *
 * @return (double&) le facteur de securite du schema en temps
 */
inline double& Schema_Temps::facteur_securite_pas()
{
  return valeur().facteur_securite_pas();
}
/*! @brief Appel a l'objet sous-jacent Renvoie 1 si il y lieu de stopper le calcul pour differente raisons:
 *
 *         - le temps final est atteint
 *         - le nombre de pas de temps maximum est depasse
 *         - l'etat stationnaire est atteint
 *         - indicateur d'arret fichier
 *     Renvoie 0 sinon
 *
 * @return (entier) 1 si il y a lieu de s'arreter 0 sinon
 */
inline int Schema_Temps::stop()
{
  return valeur().stop();
}
/*! @brief Appel a l'objet sous-jacent Renvoie 1 s'il y a lieu de faire une impression.
 *
 * @return (int) code de retour sous-jacent
 */
inline int Schema_Temps::impr(Sortie& os) const
{
  return valeur().impr(os);
}
/*! @brief Appel a l'objet sous-jacent Imprime le schema en temp sur un flot de sortie (si il y a lieu).
 *
 * @param (Sortie& os) le flot de sortie
 */
inline void Schema_Temps::imprimer(Sortie& os) const
{
  valeur().imprimer(os);
}
/*! @brief Appel a l'objet sous-jacent: Renvoie 1 s'il y a lieu de faire une sauvegarde
 *
 *     0 sinon.
 *
 * @return (int) 1 si il faut faire une sauvegarde 0 sinon
 */
inline int Schema_Temps::lsauv() const
{
  return valeur().lsauv();
}
#endif
