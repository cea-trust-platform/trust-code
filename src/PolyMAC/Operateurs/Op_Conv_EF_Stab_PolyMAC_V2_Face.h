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
// File:        Op_Conv_EF_Stab_PolyMAC_V2_Face.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Conv_EF_Stab_PolyMAC_V2_Face_included
#define Op_Conv_EF_Stab_PolyMAC_V2_Face_included

#include <Op_Conv_PolyMAC_base.h>
#include <Matrice_Morse.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Conv_EF_Stab_PolyMAC_V2_Face
//
// <Description of class Op_Conv_EF_Stab_PolyMAC_V2_Face>
//
/////////////////////////////////////////////////////////////////////////////

class Op_Conv_EF_Stab_PolyMAC_V2_Face : public Op_Conv_PolyMAC_base
{

  Declare_instanciable( Op_Conv_EF_Stab_PolyMAC_V2_Face ) ;

public :
  void completer() override;
  void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override { };
  double calculer_dt_stab() const override;

  /* interface ajouter_blocs */
  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;

  void check_multiphase_compatibility() const override { }; //of course
  void set_incompressible(const int flag) override
  {
    incompressible_ = flag;
  }

protected :
  double alpha = -1e8; //alpha = 0 -> centre, alpha = 1 -> amont
  DoubleVect porosite_f, porosite_e; //pour F5
};

class Op_Conv_Amont_PolyMAC_V2_Face : public Op_Conv_EF_Stab_PolyMAC_V2_Face
{
  Declare_instanciable( Op_Conv_Amont_PolyMAC_V2_Face ) ;
};

class Op_Conv_Centre_PolyMAC_V2_Face : public Op_Conv_EF_Stab_PolyMAC_V2_Face
{
  Declare_instanciable( Op_Conv_Centre_PolyMAC_V2_Face ) ;
};

#endif /* Op_Conv_EF_Stab_PolyMAC_V2_Face_included */
