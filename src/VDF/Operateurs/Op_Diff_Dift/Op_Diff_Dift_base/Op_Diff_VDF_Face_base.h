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

#ifndef Op_Diff_VDF_Face_base_included
#define Op_Diff_VDF_Face_base_included

#include <Iterateur_VDF_Face.h>
#include <Op_Diff_VDF_base.h>
#include <Eval_Diff_VDF.h>
#include <Op_VDF_Face.h>

class Eval_VDF_Face;

class Op_Diff_VDF_Face_base : public Op_Diff_VDF_base, public Op_VDF_Face
{
  Declare_base(Op_Diff_VDF_Face_base);
public:
  inline Op_Diff_VDF_Face_base(const Iterateur_VDF_base& iterateur) : Op_Diff_VDF_base(iterateur)
  {
    declare_support_masse_volumique(1);
  }

  double calculer_dt_stab() const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_VDF_Face::modifier_pour_Cl(iter_->domaine(), iter_->domaine_Cl(), matrice, secmem); }
};

#endif /* Op_Diff_VDF_Face_base_included */
