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

#ifndef Domaine_dis_included
#define Domaine_dis_included

#include <Domaine.h>
#include <Domaine_dis_base.h>
#include <TRUST_Deriv.h>

class Domaine_Cl_dis_base;
class Sous_domaine_dis;

/*! @brief classe Domaine_dis Classe generique de la hierarchie des domaines discretisees.
 *
 * Un objet
 *      de type Domaine_dis peut referencer n'importe quel objet derivant de
 *      Domaine_dis_base.
 *      La plupart des methodes appellent les methodes de l'objet Domaine
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Domaine_dis_base
 */
class Domaine_dis : public DERIV(Domaine_dis_base)
{
  Declare_instanciable(Domaine_dis);

public :
  inline void associer_domaine(const Domaine&);

  int nombre_de_sous_domaines_dis() const { return valeur().nombre_de_sous_domaines_dis(); }

  const Sous_domaine_dis& sous_domaine_dis(int i) const;
  Sous_domaine_dis& sous_domaine_dis(int i);

  void typer(const Nom& nom);

  inline const Domaine& domaine() const;
  inline Domaine& domaine();
  inline void creer_elements_fictifs(const Domaine_Cl_dis_base&);
  inline const Frontiere_dis_base& frontiere_dis(int ) const;
};


/*! @brief Appel a l'objet sous-jacent.
 *
 * Associe une domaine (non discretisee) a l'objet.
 *
 * @param (Domaine& une_domaine)
 */
inline void Domaine_dis::associer_domaine(const Domaine& une_domaine)
{
  valeur().associer_domaine(une_domaine);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie la domaine associee.
 *     (version const)
 *
 * @return (Domaine&) la domaine associee
 */
inline const Domaine& Domaine_dis::domaine() const
{
  return valeur().domaine();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie la domaine associee.
 *
 * @return (Domaine&) la domaine associee
 */
inline Domaine& Domaine_dis::domaine()
{
  return valeur().domaine();
}

inline void Domaine_dis::creer_elements_fictifs(const Domaine_Cl_dis_base& zcl)
{
  valeur().creer_elements_fictifs( zcl);
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie la ieme frontiere
 *
 * @param (int i) l'index de la frontiere a renvoyer
 * @return (Frontiere_dis_base&) la i-eme frontiere de la domaine discretisee
 */
inline const Frontiere_dis_base& Domaine_dis::frontiere_dis(int i) const
{
  return valeur().frontiere_dis(i);
}


#endif
