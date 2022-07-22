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

#ifndef Equation_included
#define Equation_included

#include <Equation_base.h>




Declare_deriv(Equation_base);

/*! @brief classe Equation Classe generique de la hierarchie des equations.
 *
 * Un objet Equation peut
 *      referencer n'importe quel objet derivant de Equation_base.
 *      La plupart des methodes appellent les methodes de l'objet Probleme
 *      sous-jacent via la methode valeur() declaree grace a la macro
 * Declare_deriv().;
 *
 * @sa Eqn_base Operateur Proprietes Champ_Inc
 */
class Equation : public DERIV(Equation_base)
{
  Declare_instanciable(Equation);
public :
  //  inline Equation(const Equation_base&);
  inline int nombre_d_operateurs() const;
  inline const Operateur& operateur(int) const;
  inline Operateur& operateur(int);
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();
  inline void discretiser();
  //
  inline int sauvegarder(Sortie& ) const override;
  inline int reprendre(Entree& ) override;
  //
  inline void associer_sch_tps_base(const Schema_Temps_base&);
  inline void associer_milieu_base(const Milieu_base&);
  inline const Milieu_base& milieu() const;
  inline Milieu_base& milieu();
};
/*
inline Equation::Equation(const Equation_base& eqn)
  : DERIV(Equation_base)(eqn)
{
}
*/


/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le nombre d'operateurs de l'equation.
 *
 * @return (int) le nombre d'operateur de l'equation
 */
inline int Equation::nombre_d_operateurs() const
{
  return valeur().nombre_d_operateurs();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le i-eme operateur.
 *     (version const)
 *
 * @param (int) indice de l'operateur
 * @return (Operateur&) i-eme operateur de l'equation
 */
inline const Operateur& Equation::operateur(int i) const
{
  return valeur().operateur(i);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le i-eme operateur.
 *
 * @param (int) indice de l'operateur
 * @return (Operateur&) le i-eme operateur de l'equation
 */
inline Operateur& Equation::operateur(int i)
{
  return valeur().operateur(i);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le champ inconnue.
 *     (version const)
 *
 * @return (Champ_Inc&) le champ inconnue de l'equation
 */
inline const Champ_Inc& Equation::inconnue() const
{
  return valeur().inconnue();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Discretise l'equation: type l'inconnue.
 *
 */
inline void Equation::discretiser()
{
  valeur().discretiser();
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le champ inconnue.
 *
 * @return (Champ_Inc&) le champ inconnue de l'equation
 */
inline Champ_Inc& Equation::inconnue()
{
  return valeur().inconnue();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Effectue une sauvegarde des inconnues de l'equation.
 *
 * @param (Sortie& os) le flot de sortie de sauvegarde
 * @return (int) code de retour propage
 */
inline int Equation::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Effectue une reprise apres une sauvegarde.
 *
 * @param (Entree& is) flot d'entre pour la reprise
 * @return (int) code de retour propage (1 si reprise OK)
 */
inline int Equation::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Associe un schema en temps a l'equation.
 *
 * @param (Schema_Temps_base& un_schema_temps) le schema en temps a associer
 */
inline void Equation::associer_sch_tps_base(const Schema_Temps_base& un_schema_temps)
{
  valeur().associer_sch_tps_base(un_schema_temps);
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Associe un milieu physique a l'equation.
 *     NOTE: cette methode est abstraite dans Equation_base et prend un
 *           dans ses derivees. Voir par exemple: Navier_Stokes_std
 *
 * @param (Milieu_base& mil) le milieu physique de l'equation
 */
inline void Equation::associer_milieu_base(const Milieu_base& mil)
{
  valeur().associer_milieu_base(mil);
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le milieu physique de l'equation.
 *     (version const)
 *
 * @return (Milieu_base&) le milieu physique de l'equation
 */
inline const Milieu_base& Equation::milieu() const
{
  return valeur().milieu();
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le milieu physique de l'equation.
 *
 * @return (Milieu_base&) le milieu physique de l'equation
 */
inline Milieu_base& Equation::milieu()
{
  return valeur().milieu();
}

#endif
