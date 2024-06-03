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

#ifndef Terme_Puissance_Thermique_Echange_Impose_Elem_base_included
#define Terme_Puissance_Thermique_Echange_Impose_Elem_base_included

#include <Source_base.h>
#include <TRUST_Ref.h>
#include <Champ_Don.h>
#include <Parser_U.h>

class Domaine_Cl_dis_base;
class Probleme_base;

class Terme_Puissance_Thermique_Echange_Impose_Elem_base: public Source_base
{
  Declare_base(Terme_Puissance_Thermique_Echange_Impose_Elem_base);
public:
  int has_interface_blocs() const override { return 1; }
  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override { } //rien
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  void associer_pb(const Probleme_base& ) override { }
  void mettre_a_jour(double ) override;
  void pid_process();
  int initialiser(double temps) override;

protected:
  REF(Domaine_VF) le_dom;
  REF(Domaine_Cl_dis_base) le_dom_Cl;
  Champ_Don himp_,Text_;
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  // PID controler
  int regul_ = 0;
  Parser_U pow_cible_, Kp_, Ki_, Kd_;
  double DT_regul_ = 0.0, p_error = 0.0;
};

#endif
