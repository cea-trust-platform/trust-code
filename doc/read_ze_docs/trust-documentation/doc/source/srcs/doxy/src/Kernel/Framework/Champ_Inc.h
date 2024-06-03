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

#ifndef Champ_Inc_included
#define Champ_Inc_included

#include <Champ_Inc_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Champ_Inc Classe generique de la hierarchie des champs inconnue, un objet
 *
 *      Champ_Inc peut referencer n'importe quel objet derivant de
 *      Champ_Inc_base.
 *      La plupart des methodes appellent les methodes de l'objet champ
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Ch_Inc_base Domaine_dis Ch_proto
 */

class Champ_Inc : public DERIV(Champ_Inc_base), public Champ_Proto
{
  Declare_instanciable(Champ_Inc);
public:
  //
  // Methodes reimplementees (Champ_Proto)
  //
  using Champ_Proto::valeurs;
  DoubleTab& valeurs() override;
  const DoubleTab& valeurs() const override;
  int reprendre(Entree&) override;
  int sauvegarder(Sortie&) const override;
  //
  // Nouvelles methodes
  //
  int fixer_nb_valeurs_temporelles(int);
  int nb_valeurs_temporelles() const;
  int fixer_nb_valeurs_nodales(int);
  int nb_valeurs_nodales() const;
  const Domaine_dis_base& associer_domaine_dis_base(const Domaine_dis_base&);
  const Domaine_dis_base& domaine_dis_base() const;
  double changer_temps(const double);
  double temps() const;
  DoubleTab& futur(int i = 1) override;
  const DoubleTab& futur(int i = 1) const override;
  DoubleTab& passe(int i = 1) override;
  const DoubleTab& passe(int i = 1) const override;
  Champ_Inc_base& avancer(int i = 1);
  Champ_Inc_base& reculer(int i = 1);
  int lire_donnees(Entree&);

  double integrale_espace(int ) const;
  void mettre_a_jour(double temps);
  void associer_eqn(const Equation_base&);
  // methode supprimee volontairement
  //DoubleTab& trace(const Frontiere_dis_base&, DoubleTab& ,int distant) const;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double ,int distant) const;
  // Operateur de conversion implicite
  operator DoubleTab& () = delete;
  operator const DoubleTab& () const = delete;
};

#endif
