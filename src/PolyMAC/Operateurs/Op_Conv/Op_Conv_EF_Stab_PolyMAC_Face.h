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

#ifndef Op_Conv_EF_Stab_PolyMAC_Face_included
#define Op_Conv_EF_Stab_PolyMAC_Face_included

#include <Op_Conv_PolyMAC_base.h>

class Matrice_Morse;

class Op_Conv_EF_Stab_PolyMAC_Face : public Op_Conv_PolyMAC_base
{
  Declare_instanciable( Op_Conv_EF_Stab_PolyMAC_Face ) ;
public :
  void completer() override;

  DoubleTab& ajouter(const DoubleTab& inco, DoubleTab& resu) const override;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;

  void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override { }
  void dimensionner(Matrice_Morse& mat) const override;
  void set_incompressible(const int flag) override;

protected :
  double alpha = 1; //alpha = 0 -> centre, alpha = 1 -> amont (par defaut, on fait l'amont)
  DoubleVect porosite_f, porosite_e; //pour F5

private :
  IntTab equiv_; //equiv(f, i, j) = f2 si la face f1 = e_f(f_e(f, i), j) est equivalente a la face f2 de l'autre cote
};

class Op_Conv_Amont_PolyMAC_Face : public Op_Conv_EF_Stab_PolyMAC_Face
{
  Declare_instanciable( Op_Conv_Amont_PolyMAC_Face ) ;
};

class Op_Conv_Centre_PolyMAC_Face : public Op_Conv_EF_Stab_PolyMAC_Face
{
  Declare_instanciable( Op_Conv_Centre_PolyMAC_Face ) ;
};

#endif /* Op_Conv_EF_Stab_PolyMAC_Face_included */
