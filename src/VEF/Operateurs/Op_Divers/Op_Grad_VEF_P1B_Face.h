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

#ifndef Op_Grad_VEF_P1B_Face_included
#define Op_Grad_VEF_P1B_Face_included

#include <Champ_P1_isoP1Bulle.h>
#include <Operateur_Grad.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <TRUST_Ref.h>

/*! @brief class Op_Grad_VEF_P1B_Face
 *
 *   Cette classe represente l'operateur de gradient. La discretisation est VEF
 *   On calcule le gradient d'un champ_P1B_VEF (la pression)
 *
 * @sa Operateur_Grad_base
 */
class Op_Grad_VEF_P1B_Face: public Operateur_Grad_base
{
  Declare_instanciable(Op_Grad_VEF_P1B_Face);
public:
  void associer(const Domaine_dis_base& , const Domaine_Cl_dis&,const Champ_Inc&) override;
  const Domaine_VEF& domaine_vef() const;
  int impr(Sortie&) const override;

  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;
  DoubleTab& modifier_grad_pour_Cl(DoubleTab&) const;
  DoubleTab& ajouter_elem(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_som(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_aretes(const DoubleTab&, DoubleTab&) const;

  static double calculer_coef_som(int elem, const Domaine_Cl_VEF& zcl, const Domaine_VEF& domaine_VEF);
  void calculer_flux_bords() const override;

private:
  REF(Domaine_VEF) le_dom_vef;
  REF(Domaine_Cl_VEF) la_zcl_vef;
  mutable ArrOfDouble coeff_som_;
  mutable IntTab som_;
};

#endif /* Op_Grad_VEF_P1B_Face_included */
