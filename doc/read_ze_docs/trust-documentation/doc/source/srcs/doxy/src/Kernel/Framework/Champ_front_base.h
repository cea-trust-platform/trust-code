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

#ifndef Champ_front_base_included
#define Champ_front_base_included

#include <Field_base.h>
#include <Champ_Proto.h>
#include <TRUST_Ref.h>
#include <Roue.h>

class Frontiere_dis_base;
class Champ_Inc_base;
class Domaine_dis_base;
class Cond_lim_base;

/*! @brief classe Champ_front_base Classe de base pour la hierarchie des champs aux frontieres.
 *
 * Un
 *      objet Champ_front_base definit un champ sur la frontiere d'un
 *      domaine. Un objet de type Champ_front_base sera associe a
 *      chaque condition aux limites.
 *      Champ_front_base derive de Champ_Proto, afin d'avoir une interface
 *      conforme a tous les champs, et d'heriter des operations courantes sur
 *      les Champs.
 *      Les deux methodes principales sont initialiser et
 *      mettre_a_jour.  Ce sont les deux seules qui peuvent modifier les
 *      valeurs du champ.
 *      La methode initialiser est appelee une fois au debut du
 *      calcul. Elle ne doit pas dependre de donnees exterieures a
 *      l'equation qui porte la CL (en effet, rien ne garantit que ces
 *      donnees sont initialisees). En revanche, les valeurs de
 *      l'inconnue sur laquelle porte la CL peuvent etre utiles pour
 *      l'initialisation => l'inconnue est passee en parametre en
 *      lecture seule.
 *      La methode mettre_a_jour est appelee au debut de chaque pas de
 *      temps ou sous-pas-de-temps, elle peut utiliser des donnees
 *      exterieures a l'equation. A charge a l'algorithme de s'assurer
 *      que ces donnees sont pertinentes...
 *      Dans le cas de champs stationnaires, la methode mettre_a_jour
 *      n'a rien a faire et les valeurs sont remplies une fois pour
 *      toutes par la methode initialiser.
 *      Dans le cas de champs instationnaires, il y a plusieurs valeurs en temps
 *      et chacune peut etre mise a jour.
 *      Les Champ_front sont divises en :
 *       * Champ_front_uniforme, constant dans le temps et l'espace
 *       * Champ_front_instationnaire_base, uniformes en espace mais variables en temps
 *       * Champ_front_var, variables en espace.
 *      Les Champ_front_var sont ensuite classes selon qu'ils sont
 *      stationnaires ou instationnaires.
 *
 *      Les valeurs sont stockees dans une roue de DoubleTab.
 *      Si le champ est uniforme en espace, les DoubleTab sont
 *      dimensionnes a 1.
 *      S'il est stationnaire, la roue n'a qu'une valeur temporelle
 *      et le temps qui lui est assigne n'a pas de sens.
 *      S'il est instationnaire, les valeurs temporelles sont celles
 *      de l'inconnue de l'equation a laquelle se rapporte le champ.
 *
 *
 * @sa Champ_Proto Frontiere_dis_base, Classe abstraite, Methode abstraite:, Champ_front_base& affecter_(const Champ_front_base& ch)
 */
class Champ_front_base : public Field_base, public Champ_Proto
{
  Declare_base_sans_constructeur(Champ_front_base);
public:
  Champ_front_base();
  inline virtual void completer() { }
  virtual int initialiser(double temps, const Champ_Inc_base& inco);
  virtual void associer_fr_dis_base(const Frontiere_dis_base&);
  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override;
  inline const DoubleTab& valeurs() const override;
  virtual DoubleTab& valeurs_au_temps(double temps)=0;
  virtual const DoubleTab& valeurs_au_temps(double temps) const = 0;
  inline const Frontiere_dis_base& frontiere_dis() const;
  inline Frontiere_dis_base& frontiere_dis();
  const Domaine_dis_base& domaine_dis() const;
  virtual Champ_front_base& affecter_(const Champ_front_base& ch) = 0;
  virtual void fixer_nb_valeurs_temporelles(int nb_cases);
  virtual void mettre_a_jour(double temps);
  virtual void calculer_coeffs_echange(double temps);
  virtual void valeurs_face(int, DoubleVect&) const;
  virtual inline void verifier(const Cond_lim_base& la_cl) const;
  double get_temps_defaut() const { return temps_defaut; }
  void set_temps_defaut(double temps) { temps_defaut = temps; }
  virtual void changer_temps_futur(double temps, int i);
  virtual int avancer(double temps);
  virtual int reculer(double temps);

protected:
  double temps_defaut ; // Le temps pris par defaut quand le parametre
  // n'est pas specifie. Ce sera en particulier
  // celui utilise par les operateurs et les
  // solveurs.
  REF(Frontiere_dis_base) la_frontiere_dis;
  Roue_ptr les_valeurs; // Les valeurs du champ
};


/*! @brief Renvoie la frontiere discretisee associee au champ.
 *
 * (version const)
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee au champ
 */
inline const Frontiere_dis_base& Champ_front_base::frontiere_dis() const
{
  return la_frontiere_dis.valeur();
}


/*! @brief Renvoie la frontiere discretisee associee au champ.
 *
 * @return (Frontiere_dis_base&) la frontiere discretisee associee au champ
 */
inline Frontiere_dis_base& Champ_front_base::frontiere_dis()
{
  return la_frontiere_dis.valeur();
}


/*! @brief Renvoie le tableau des valeurs du champ.
 *
 * @return (DoubleTab&) le tableau des valeurs du champ au temps par defaut.
 */
inline DoubleTab& Champ_front_base::valeurs()
{
  return valeurs_au_temps(temps_defaut);
}


/*! @brief Renvoie le tableau des valeurs du champ au temps par defaut.
 *
 * (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline const DoubleTab& Champ_front_base::valeurs() const
{
  return valeurs_au_temps(temps_defaut);
}

// devra etre surchargee par les champ_front voulant verifier les arguments
inline void Champ_front_base::verifier(const Cond_lim_base& la_cl) const
{
  return;
}

#endif
