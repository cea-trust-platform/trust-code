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
// File:        Op_Grad_PolyMAC_Face.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Grad_PolyMAC_Face_included
#define Op_Grad_PolyMAC_Face_included

#include <Operateur_Grad.h>
#include <Ref_Zone_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>
#include <Ref_Champ_Face_PolyMAC.h>

//
// .DESCRIPTION class Op_Grad_PolyMAC_Face
//
//  Cette classe represente l'operateur de gradient
//  La discretisation est PolyMAC
//  On calcule le gradient d'un champ_Elem_PolyMAC (la pression)
//

// .SECTION voir aussi
// Operateur_Grad_base
//

class Op_Grad_PolyMAC_Face : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_PolyMAC_Face);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc& ) override;
  void completer() override;

  /* interface {dimensionner,ajouter}_blocs -> cf Equation_base.h */
  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;

  int impr(Sortie& os) const override;

  void check_multiphase_compatibility() const override { }; //ok

private:

  REF(Zone_PolyMAC) ref_zone;
  REF(Zone_Cl_PolyMAC) ref_zcl;
};

#endif
