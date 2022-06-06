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

#ifndef EOS_Tools_VEF_included
#define EOS_Tools_VEF_included

#include <EOS_Tools_base.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_dis.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe EOS_Tools_VEF
//     Cette classe et specifique a discretisation de type VEF.
// .SECTION voir aussi
//     Fluide_Dilatable_base EOS_Tools_base
//////////////////////////////////////////////////////////////////////////////

class EOS_Tools_VEF: public EOS_Tools_base
{
  Declare_instanciable(EOS_Tools_VEF);
public :
  const DoubleTab& rho_discvit() const override;
  const DoubleTab& rho_face_n() const override;
  const DoubleTab& rho_face_np1() const override;
  void associer_zones(const Zone_dis&,const Zone_Cl_dis&) override;
  void divu_discvit(const DoubleTab& , DoubleTab& ) override;
  double moyenne_vol(const DoubleTab&) const override;
  void secmembre_divU_Z(DoubleTab& ) const override;
  inline void calculer_rho_face_np1(const DoubleTab& rho) override { /* Do nothing */ }
  inline const Fluide_Dilatable_base& le_fluide() const { return le_fluide_.valeur(); }

protected :
  REF(Zone_VEF) la_zone;
  REF(Zone_Cl_dis) la_zone_Cl;
};

#endif /* EOS_Tools_VEF_included */
