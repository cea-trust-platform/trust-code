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

#ifndef Champ_front_calc_included
#define Champ_front_calc_included

#include <Ch_front_var_instationnaire_dep.h>
#include <TRUST_Ref.h>
#include <Motcle.h>

class Domaine_Cl_dis_base;
class Domaine_dis_base;
class Champ_Inc_base;
class Front_dis_base;
class Equation_base;
class Zone_dis_base;
class Milieu_base;

/*! @brief classe Champ_front_calc Classe derivee de Champ_front_var qui represente les
 *
 *      champs a la frontiere obtenus en prenant la trace
 *      d'un objet de type Champ_Inc (champ inconnue d'une equation)
 *
 * @sa Champ_front_var_instationnaire Champ_Inc
 */
class Champ_front_calc : public Ch_front_var_instationnaire_dep
{
  Declare_instanciable_sans_constructeur(Champ_front_calc);
public:
  Champ_front_calc();
  int initialiser(double, const Champ_Inc_base&) override;
  void associer_ch_inc_base(const Champ_Inc_base&);
  Champ_front_base& affecter_(const Champ_front_base& ch) override;
  void mettre_a_jour(double temps) override;
  void creer(const Nom&, const Nom&, const Motcle&);
  void verifier(const Cond_lim_base& la_cl) const override;

  // Methodes pour acceder aux objets opposes:
  const Champ_Inc_base& inconnue() const;
  const Nom& nom_bord_oppose() const;
  const Equation_base& equation() const;
  const Milieu_base& milieu() const;
  const Domaine_dis_base& domaine_dis() const;
  const Domaine_Cl_dis_base& domaine_Cl_dis() const;
  const Frontiere_dis_base& front_dis() const;
  inline void set_distant(int d) { distant_=d ; }

protected :
  REF2(Champ_Inc_base) l_inconnue;          // L'inconnue du probleme oppose
  Nom nom_autre_bord_,nom_autre_pb_;       // Nom du bord et du probleme oppose
  Motcle nom_inco_;
  bool via_readon_ = false;
  int distant_;                            // par defaut distant_ vaut 1
};

#endif /* Champ_front_calc_included */
