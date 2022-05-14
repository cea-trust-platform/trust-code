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
// File:        Masse_PolyMAC_P0_Face.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Solveurs
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Masse_PolyMAC_P0_Face_included
#define Masse_PolyMAC_P0_Face_included


#include <Ref_Zone_Cl_PolyMAC.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Zone_PolyMAC_P0.h>
#include <Solveur_Masse.h>

class Masse_PolyMAC_P0_Face : public Solveur_Masse_base
{

  Declare_instanciable(Masse_PolyMAC_P0_Face);

public:

  void associer_zone_dis_base(const Zone_dis_base& ) override;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& ) override;
  void completer() override;

  DoubleTab& appliquer_impl(DoubleTab& ) const override;

  DoubleTab& corriger_solution(DoubleTab& x, const DoubleTab& y, int incr = 0) const override;

  /* interface ajouter_blocs */
  int has_interface_blocs() const override
  {
    return 1;
  }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const override;

  void check_multiphase_compatibility() const override { };
private:

  REF(Zone_PolyMAC_P0) la_zone_PolyMAC_P0;
  REF(Zone_Cl_PolyMAC) la_Zone_Cl_PolyMAC;
};

#endif





