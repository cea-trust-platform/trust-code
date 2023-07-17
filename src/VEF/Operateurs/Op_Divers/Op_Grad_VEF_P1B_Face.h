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

#ifndef Op_Grad_VEF_P1B_Face_included
#define Op_Grad_VEF_P1B_Face_included

#include <Op_Grad_VEF_Face.h>
#include <Champ_P1_isoP1Bulle.h>

/*! @brief class Op_Grad_VEF_P1B_Face
 *
 *   Cette classe represente l'operateur de gradient
 *   La discretisation est VEF
 *   On calcule le gradient d'un champ_P1B_VEF (la pression)
 *
 *
 *
 * @sa Operateur_Grad_base
 */
class Domaine_VEF_PreP1b;
class Domaine_Cl_VEF;

class Op_Grad_VEF_P1B_Face : public Op_Grad_VEF_Face
{

  Declare_instanciable(Op_Grad_VEF_P1B_Face);

public:

  const Domaine_VEF_PreP1b& domaine_Vef() const;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& modifier_grad_pour_Cl(DoubleTab& ) const;
  DoubleTab& ajouter_elem(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_som(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_aretes(const DoubleTab& ,  DoubleTab& ) const;
  int impr(Sortie& ) const override ;

  static double calculer_coef_som(int elem,  const Domaine_Cl_VEF& zcl,
                                  const Domaine_VEF_PreP1b& domaine_VEF) ;
  void calculer_flux_bords() const override;

private:
  mutable ArrOfDouble coeff_som_;
  mutable IntTab som_;

};

#endif