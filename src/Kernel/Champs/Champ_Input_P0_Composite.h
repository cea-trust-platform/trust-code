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

#ifndef Champ_Input_P0_Composite_included
#define Champ_Input_P0_Composite_included

#include <Champ_input_P0.h>

using ICoCo::TrioField;

class Champ_Input_P0_Composite : public Champ_Fonc_P0_base
{
  Declare_instanciable(Champ_Input_P0_Composite);
public:
  // champ utilise pour l'initialisation
  bool is_initialized() { return champ_initial_.non_nul(); }
  const DoubleTab& initial_values() { return champ_initial_->valeurs(); }

  // champ input classique (faut plus des methodes pt etre ?)
  using Champ_Proto::valeurs;

  Champ_input_P0& input_field() { return champ_input_; }
  DoubleTab& valeurs() override { return champ_input_.valeurs(); }
  const DoubleTab& valeurs() const override { return champ_input_.valeurs(); }

  void mettre_a_jour(double) override { }
  int initialiser(const double t) override { return champ_input_.initialiser(t); }
  double changer_temps(const double t) override { return champ_input_.changer_temps(t); }

  const Nom& fixer_unite(const Nom& unit) override { return champ_input_.fixer_unite(unit); }
  const Nom& fixer_unite(int i, const Nom& unit) override { return champ_input_.fixer_unite(i,unit); }

  void associer_domaine_dis_base(const Domaine_dis_base& zdb) override { champ_input_.associer_domaine_dis_base(zdb); }
  const Domaine_dis_base& domaine_dis_base() const override { return champ_input_.domaine_dis_base(); }

  void getTemplate(TrioField& afield) const { champ_input_.getTemplate(afield); }
  void setValue(const TrioField& afield) { champ_input_.setValue(afield); }
  void setDoubleValue(const double val) { champ_input_.setDoubleValue(val); }

private:
  Champ_input_P0 champ_input_;
  OWN_PTR(Champ_Don_base) champ_initial_;
};

#endif /* Champ_Input_P0_Composite_included */
