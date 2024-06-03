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

#ifndef Op_Div_VEFP1B_Elem_included
#define Op_Div_VEFP1B_Elem_included

#include <Operateur_Div.h>
#include <Domaine_VEF.h>
#include <TRUST_Ref.h>

class Domaine_VEF;
class FMatrice_Morse;
class Domaine_Cl_VEF;

/*! @brief class Op_Div_VEFP1B_Elem
 *
 *   Cette classe represente l'operateur de divergence. La discretisation est VEF
 *   On calcule la divergence d'un champ_P1NC (la vitesse)
 *
 *
 * @sa Operateur_Div_base
 */
class Op_Div_VEFP1B_Elem: public Operateur_Div_base
{
  Declare_instanciable(Op_Div_VEFP1B_Elem);
public:
  void degres_liberte() const;
  int impr(Sortie& os) const override;
  void volumique(DoubleTab&) const override;
  void associer(const Domaine_dis& , const Domaine_Cl_dis&,const Champ_Inc&) override;

  // Explicite
  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;
  DoubleTab& ajouter_elem(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_som(const DoubleTab&, DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_aretes(const DoubleTab&, DoubleTab&) const;

  // Implicite
  void dimensionner(FMatrice_Morse&) const { }
  void dimensionner(Matrice_Morse&) const override { }
  void modifier_pour_Cl(FMatrice_Morse&, DoubleTab&) const { }
  void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override { }
  void contribuer_a_avec(const DoubleTab&, FMatrice_Morse&) const { }
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override { }
  void contribuer_au_second_membre(DoubleTab&) const override { }

private:
  REF(Domaine_VEF) le_dom_vef;
  REF(Domaine_Cl_VEF) la_zcl_vef;

  void volumique_P0(DoubleTab&) const;

  mutable IntVect nb_degres_liberte;
  mutable IntTab som_;
};

#endif /* Op_Div_VEFP1B_Elem_included */
