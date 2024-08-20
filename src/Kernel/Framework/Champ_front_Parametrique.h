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


#ifndef Champ_front_Parametrique_included
#define Champ_front_Parametrique_included

#include <Champ_front.h>
#include <TRUST_List.h>

/*! @brief : class Champ_front_Parametrique
 *
 *  <Description of class Champ_front_Parametrique>
 */
class Champ_front_Parametrique : public Champ_front_base
{
  Declare_instanciable(Champ_front_Parametrique);

public:
  // Methodes surchargees:
  using Champ_Proto::valeurs;
  inline virtual DoubleTab& valeurs() override { return champ()->valeurs(); }
  inline virtual const DoubleTab& valeurs() const override { return champ()->valeurs(); }
  DoubleTab& valeurs_au_temps(double temps) override { return champ()->valeurs_au_temps(temps); }
  const DoubleTab& valeurs_au_temps(double temps) const override { return champ()->valeurs_au_temps(temps); }
  const Frontiere_dis_base& frontiere_dis() const override { return champ()->frontiere_dis(); }
  Frontiere_dis_base& frontiere_dis() override { return champ()->frontiere_dis(); }
  const Domaine_dis_base& domaine_dis() const override { return champ()->domaine_dis(); }
  Champ_front_base& affecter_(const Champ_front_base& ch) override { return champ()->affecter_(ch); }
  void mettre_a_jour(double temps) override { champ()->mettre_a_jour(temps); }
  void calculer_coeffs_echange(double temps) override { champ()->calculer_coeffs_echange(temps); };
  void valeurs_face(int i, DoubleVect& val) const override { champ()->valeurs_face(i,val); }
  inline void verifier(const Cond_lim_base& la_cl) const override { champ()->verifier(la_cl); };
  double get_temps_defaut() const override { return champ()->get_temps_defaut(); }
  void set_temps_defaut(double temps) override { champ()->set_temps_defaut(temps); }
  void changer_temps_futur(double temps, int i) override { champ()->changer_temps_futur(temps, i); }
  int avancer(double temps) override { return champ()->avancer(temps); }
  int reculer(double temps) override { return champ()->reculer(temps); }
  void fixer_nb_comp(int i) override { champ()->fixer_nb_comp(i); }
  int nb_comp() const override { return champ()->nb_comp(); }
  Nature_du_champ nature_du_champ() const override { return champ()->nature_du_champ(); }
  Nature_du_champ fixer_nature_du_champ(Nature_du_champ nat) override { return champ()->fixer_nature_du_champ(nat); }

  // Methodes surchargees avec boucles sur les champs
  void associer_fr_dis_base(const Frontiere_dis_base& fr) override { for (auto& ch : champs_) ch->associer_fr_dis_base(fr); }
  int initialiser(double temps, const Champ_Inc_base& inco) override { for (auto& ch : champs_) ch->initialiser(temps, inco); return 1; }
  virtual void completer() override { for (auto& ch : champs_) ch->completer(); }
  void fixer_nb_valeurs_temporelles(int nb_cases) override { for (auto& ch : champs_) ch->fixer_nb_valeurs_temporelles(nb_cases); }

  // Methodes surchargees avec calcul specifique de la derivee en temps du champ_front_parametrique (Gpoint_):
  void calculer_derivee_en_temps(double t1, double t2) override;
  const DoubleTab& derivee_en_temps() const override;
  bool instationnaire() const override { return index_==1 ? champ()->instationnaire() : true; } // Le premier champ peut etre stationnaire mais ensuite instationnaire forcement

  // Methodes specifiques:
  std::string newCompute() const;
  Champ_front& champ() { return champs_[index_-1]; }
  const Champ_front& champ() const { return champs_[index_-1]; }

private:
  LIST(Champ_front) champs_;
  mutable int index_=0;
  mutable double last_t2_ = DMAXFLOAT; // Pour gerer les changements de champs dans calculer_derivee_en_temps
};

#endif
