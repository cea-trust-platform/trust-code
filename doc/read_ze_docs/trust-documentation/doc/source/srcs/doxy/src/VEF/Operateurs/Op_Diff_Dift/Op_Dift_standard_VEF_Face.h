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

#ifndef Op_Dift_standard_VEF_Face_included
#define Op_Dift_standard_VEF_Face_included

#include <Op_Dift_VEF_base.h>
#include <Operateur_Div.h>

class Op_Dift_standard_VEF_Face: public Op_Dift_VEF_base
{
  Declare_instanciable(Op_Dift_standard_VEF_Face);
public:
  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;

private:
  void calcul_divergence(DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab& ) const;
  void ajouter_cas_vectoriel(const DoubleTab&, DoubleTab&, const DoubleTab&, const DoubleTab& ) const;

  REF(Champ_Inc) divergence_U;
  int grad_Ubar = 1, nu_lu = 1, nut_lu = 1;
  int nu_transp_lu = 1, nut_transp_lu = 1, filtrer_resu = 1;
  DoubleTab grad_;
};

#endif /* Op_Dift_standard_VEF_Face_included */
