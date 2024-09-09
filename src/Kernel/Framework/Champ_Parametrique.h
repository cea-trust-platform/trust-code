/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Champ_Parametrique_included
#define Champ_Parametrique_included

#include <Champ_Don.h>
#include <TRUST_List.h>

/*! @brief : class Champ_Parametrique
 *
 *  <Description of class Champ_Parametrique>
 */
class Champ_Parametrique : public Champ_Don_base
{
  Declare_instanciable( Champ_Parametrique ) ;
public:
  // Methodes surchargees:
  void mettre_a_jour(double temps) override { champ()->mettre_a_jour(temps); }
  int imprime(Sortie& os, int j) const override { return champ()->imprime(os,j); }
  int fixer_nb_valeurs_nodales(int nb_noeuds) override { return champ()->fixer_nb_valeurs_nodales(nb_noeuds); }
  int reprendre(Entree& is) override { return champ()->reprendre(is); }
  int sauvegarder(Sortie& os) const override { return champ()->sauvegarder(os); }
  void resetTime(double time) override { champ()->resetTime(time); }
  inline DoubleTab& valeurs() override { return champ()->valeurs(); }
  inline const DoubleTab& valeurs() const override { return champ()->valeurs(); }
  inline DoubleTab& valeurs(double temps) override { return champ()->valeurs(temps); }
  inline const DoubleTab& valeurs(double temps) const override { return champ()->valeurs(temps); }
  Champ_base& affecter_(const Champ_base& ch) override { return champ()->affecter(ch); }
  Champ_base& affecter_compo(const Champ_base& ch, int i) override { return champ()->affecter_compo(ch, i); }
  inline int nb_valeurs_nodales() const override { return champ()->valeurs().dimension(0); }
  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const override { return champ()->valeur_aux(positions, valeurs); }
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override { return champ()->valeur_aux_elems(positions, les_polys, valeurs); };
  bool instationnaire() const override { return champ()->instationnaire(); }

  // Methodes surchargees avec boucles sur les champs
  int initialiser(const double temps) override { for (auto& ch : champs_) ch->initialiser(temps); return 1; }

  // Methodes specifiques:
  std::string newCompute() const;
  static bool enabled;
  static std::string dirnameDefault;
  static std::string dirnameCompute(int compute);
protected :
  LIST(Champ_Don) champs_;
  Champ_Don& champ() { return champs_[index_-1]; }
  const Champ_Don& champ() const { return champs_[index_-1]; }
  mutable int index_=0;
};

#endif
