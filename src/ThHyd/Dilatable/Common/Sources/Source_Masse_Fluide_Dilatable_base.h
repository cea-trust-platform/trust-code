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

#ifndef Source_Masse_Fluide_Dilatable_base_included
#define Source_Masse_Fluide_Dilatable_base_included

#include <Domaine_Cl_dis.h>
#include <Champ_front.h>
#include <TRUST_Ref.h>

class Convection_Diffusion_Fluide_Dilatable_base;
class Fluide_Dilatable_base;

/*! @brief : classe Source_Masse_Fluide_Dilatable_base Une source speciale pour l'equation de masse (utilisee seulement lors de la projection/correction donc).
 *
 */
class Source_Masse_Fluide_Dilatable_base: public Objet_U
{
  Declare_base(Source_Masse_Fluide_Dilatable_base);
public :
  void completer();

  // Managing properly the champ_front - not done automatically because we are not in the scope of boundary conditions.
  // The three methods below take care of this.
  // This is all inspired by what happens in Conds_lim::completer() and Conds_lim::initialiser()
  // and Equation_base::initTimeStep() and Equation_base::updateGivenFields()
  void mettre_a_jour(double temps);
  void changer_temps_futur(double temps, int i);
  void set_temps_defaut(double temps);

  virtual void associer_domaine_cl(const Domaine_Cl_dis&);

  inline int nb_comp() { return ncomp_; }

  // methodes virtuelles pures
  virtual void ajouter_eq_espece(const Convection_Diffusion_Fluide_Dilatable_base& eqn, const Fluide_Dilatable_base& fluide, const bool is_expl, DoubleVect& resu) const = 0;
  virtual void ajouter_projection(const Fluide_Dilatable_base& fluide, DoubleVect& resu) const = 0;

protected :
  int ncomp_ = -1;
  Nom nom_bord_;
  Champ_front ch_front_source_;
  REF(Domaine_Cl_dis) domaine_cl_dis_;
};

#endif /* Source_Masse_Fluide_Dilatable_base_included */
