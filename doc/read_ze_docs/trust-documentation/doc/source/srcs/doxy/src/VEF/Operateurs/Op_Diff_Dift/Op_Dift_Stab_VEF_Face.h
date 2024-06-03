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

#ifndef Op_Dift_Stab_VEF_Face_included
#define Op_Dift_Stab_VEF_Face_included

#include <Op_Dift_VEF_Face_Gen.h>
#include <Op_Dift_VEF_base.h>
#include <Operateur_Div.h>

class Op_Dift_Stab_VEF_Face: public Op_Dift_VEF_base, public Op_Dift_VEF_Face_Gen<Op_Dift_Stab_VEF_Face>
{
  Declare_instanciable(Op_Dift_Stab_VEF_Face);
public:
  void completer() override;

  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override; // pour l'explicite

  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override; // pour l'implicite

private:
  IntTab is_dirichlet_faces_;
  REF(Champ_Inc) divergence_U;

  int nu_lu_ = 1, nut_lu_ = 1, nu_transp_lu_ = 0, nut_transp_lu_ = 1;
  int new_jacobian_ = 0, standard_ = 0;

  void ajouter_cas_scalaire(const DoubleTab& inconnue, const DoubleTab& nu, const DoubleTab& nu_turb_m, DoubleTab& resu2) const;
  void ajouter_cas_vectoriel(const DoubleTab& inconnue, const DoubleTab& nu, const DoubleTab& nu_turb_m, DoubleTab& resu2) const;

  void calculer_min_max(const DoubleTab&, int&, DoubleTab&, const bool is_max) const;

  void ajouter_diffusion(const DoubleTab&, const DoubleTab&, DoubleTab&, const bool ) const;
  void ajouter_antidiffusion(const DoubleTab&, const DoubleTab&, DoubleTab&, const bool ) const;

  double calculer_gradients(int, const DoubleTab&) const;
};

#endif /* Op_Dift_Stab_VEF_Face_included */
