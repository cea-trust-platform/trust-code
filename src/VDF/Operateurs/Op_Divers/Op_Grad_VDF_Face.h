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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Grad_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Divers
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Grad_VDF_Face_included
#define Op_Grad_VDF_Face_included

#include <Op_Grad_VDF_Face_base.h>
#include <Matrice_Morse.h>
#include <Zone_VDF.h>
#include <SFichier.h>

// .DESCRIPTION class Op_Grad_VDF_Face
//  Cette classe represente l'operateur de gradient
//  La discretisation est VDF. On calcule le gradient d'un champ_P0_VDF (la pression)
class Op_Grad_VDF_Face : public Op_Grad_VDF_Face_base
{
  Declare_instanciable(Op_Grad_VDF_Face);
public:
  int impr(Sortie& os) const override;
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;

protected:
  mutable SFichier Flux_grad, Flux_grad_moment, Flux_grad_sum;
};

#endif /* Op_Grad_VDF_Face_included */
