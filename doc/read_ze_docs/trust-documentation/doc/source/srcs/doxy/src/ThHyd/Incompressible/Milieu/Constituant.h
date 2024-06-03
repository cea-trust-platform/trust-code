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

#ifndef Constituant_included
#define Constituant_included

#include <Milieu_base.h>

/*! @brief classe Constituant Cette classe represente le(s) constituant(s) d'un fluide.
 *
 *     Elle contient un champ alpha representant le coefficient de diffusion
 *     du constituant.
 *     Lorsqu'un objet de type Constituant represente plusieurs constituant
 *     le champ alpha est vectoriel avec autant de composantes qu'il y a
 *     de constituants.
 *
 * @sa Milieu_base
 */
class Constituant : public Milieu_base
{
  Declare_instanciable(Constituant);

public :

  void set_param(Param& param) override;
  inline const Champ_Don& diffusivite_constituant() const;
  inline void mettre_a_jour(double) override;
  void discretiser(const Probleme_base& pb, const Discretisation_base& dis) override;
  int initialiser(const double temps) override;
  int nb_constituants() const;
protected :

  Champ_Don D; // Coefficient de diffusion du constituant dans un milieu


};


/*! @brief Renvoie le coefficient de diffusion du constituant.
 *
 * @return (Champ_Don&) le champ representant le coefficient de diffusion
 */
inline const Champ_Don& Constituant::diffusivite_constituant() const
{
  return D;
}


/*! @brief Mise a jour en temps du milieu, i.
 *
 * e. de la coefficient_diffusion du constituant.
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Constituant::mettre_a_jour(double temps)
{
  if (D.non_nul())
    D.mettre_a_jour(temps);
}


/*! @brief NE FAIT RIEN
 *
 */
inline int Constituant::initialiser(const double temps)
{
  if (D.non_nul())
    D.initialiser(temps);
  return 1;
}

#endif
