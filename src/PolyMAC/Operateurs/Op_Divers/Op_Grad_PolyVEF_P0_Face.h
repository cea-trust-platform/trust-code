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

#ifndef Op_Grad_PolyVEF_P0_Face_included
#define Op_Grad_PolyVEF_P0_Face_included

#include <Domaine_PolyVEF_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Operateur_Grad.h>
#include <TRUST_Ref.h>
#include <cfloat>

class Champ_Face_PolyVEF_P0;

/*! @brief class Op_Grad_PolyVEF_P0_Face
 *
 *   Cette classe represente l'operateur de gradient
 *   La discretisation est PolyVEF_P0
 *   On calcule le gradient d'un champ_Elem_PolyVEF_P0 (la pression)
 *
 *
 * @sa Operateur_Grad_base
 */
class Op_Grad_PolyVEF_P0_Face : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_PolyVEF_P0_Face);

public:

  void associer(const Domaine_dis& , const Domaine_Cl_dis& , const Champ_Inc& ) override;
  void completer() override;

  /* interface {dimensionner,ajouter}_blocs -> cf Equation_base.h */
  int has_interface_blocs() const override { return 1; };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;

  int impr(Sortie& os) const override;

  void update_grad(int full_stencil = 0) const;

  /* public pour utilisation par Assembleur_P_PolyVEF_P0 : [grad p]_f */
  mutable IntTab fgrad_d, fgrad_e;
  mutable DoubleTab fgrad_c;

  void check_multiphase_compatibility() const override { }; //ok

private:

  mutable double last_gradp_ = -DBL_MAX; //dernier temps utilise pour interpoler grad p (mis a DBL_MAX si grad p non reinterpole)
  REF(Domaine_PolyVEF_P0) ref_domaine;
  REF(Domaine_Cl_PolyMAC) ref_dcl;
};

#endif
