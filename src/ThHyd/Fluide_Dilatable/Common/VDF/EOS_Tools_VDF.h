/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        EOS_Tools_VDF.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common/VDF
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EOS_Tools_VDF_included
#define EOS_Tools_VDF_included

#include <Ref_Zone_Cl_dis.h>
#include <EOS_Tools_base.h>
#include <Ref_Zone_VDF.h>
#include <DoubleTab.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe EOS_Tools_VDF
//     Cette classe et specifique a discretisation de type VDF.
// .SECTION voir aussi
//     Fluide_Dilatable_base EOS_Tools_base
//////////////////////////////////////////////////////////////////////////////

class EOS_Tools_VDF: public EOS_Tools_base
{
  Declare_instanciable(EOS_Tools_VDF);
public :
  const DoubleTab& rho_discvit() const;
  void associer_zones(const Zone_dis&,const Zone_Cl_dis&);
  void divu_discvit(const DoubleTab&, DoubleTab&);
  void secmembre_divU_Z(DoubleTab& ) const;
  void mettre_a_jour(double temps);
  void calculer_rho_face_np1(const DoubleTab& rho);
  double moyenne_vol(const DoubleTab&) const ;
  inline const DoubleTab& rho_face_n() const { return tab_rho_face; }
  inline const DoubleTab& rho_face_np1() const { return tab_rho_face_np1; }
  inline const Fluide_Dilatable_base& le_fluide() const { return le_fluide_.valeur(); }

protected :
  REF(Zone_VDF) la_zone;
  REF(Zone_Cl_dis) la_zone_Cl;
  DoubleTab tab_rho_face, tab_rho_face_demi, tab_rho_face_np1;
};

#endif /* EOS_Tools_VDF_included */
