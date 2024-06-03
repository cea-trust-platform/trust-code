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

#ifndef Champ_Ostwald_included
#define Champ_Ostwald_included

#include <Champ_Fonc_P0_base.h>
#include <Champ_Don_base.h>
#include <TRUST_Ref.h>

class Fluide_Ostwald;

/*! @brief classe Champ_Ostwald Represente un champ qui varie en fonction de la consistance et
 *
 *         de l'indice de structure.
 *    Classe mere qui gere le champ utilise pour le fluide d'Ostwald dans les deux
 *    discretisations.
 *    Possede les fonctions generiques aux deux discretisations.
 *    Fait reference a un fluide d'Ostwald pour pouvoir utiliser les deux
 *    parametres du fluide d'Ostwald : K et N.
 *
 * @sa Champ_Don_base
 */
class Champ_Ostwald : public Champ_Fonc_P0_base
{
  Declare_instanciable(Champ_Ostwald);
public :
  void mettre_a_jour(double temps) override;
  int initialiser(const double temps) override;
  int fixer_nb_valeurs_nodales(int nb_noeuds) override;
  Champ_base& affecter_(const Champ_base& ) override;
  virtual void me_calculer(double tps);

  inline virtual const Fluide_Ostwald& mon_fluide() const { return mon_fluide_.valeur(); }

  inline void associer_fluide(const Fluide_Ostwald& le_fluide)
  {
    mon_fluide_ = le_fluide;
  }

protected:
  REF(Fluide_Ostwald) mon_fluide_;  // pour obtenir K et N
};

#endif /* Champ_Ostwald_included */
