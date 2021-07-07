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
// File:        EOS_Tools_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EOS_Tools_base_included
#define EOS_Tools_base_included

#include <Objet_U.h>
#include <Ref_Fluide_Dilatable_base.h>

class Fluide_Dilatable_base;
class DoubleTab;
class Zone_Cl_dis;
class Zone_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe EOS_Tools_base
// .SECTION voir aussi
//
//////////////////////////////////////////////////////////////////////////////

class EOS_Tools_base : public Objet_U
{
  Declare_base(EOS_Tools_base);
public :
  void associer_fluide(const Fluide_Dilatable_base&);
  inline virtual void mettre_a_jour(double ) { }
  // Methodes virtuelles pure
  virtual const DoubleTab& rho_discvit() const=0;
  virtual const DoubleTab& rho_face_n() const=0;
  virtual const DoubleTab& rho_face_np1() const=0;
  virtual void associer_zones(const Zone_dis&,const Zone_Cl_dis&) =0;
  virtual void divu_discvit(const DoubleTab&, DoubleTab&)=0;
  virtual void secmembre_divU_Z(DoubleTab& ) const =0;
  virtual void calculer_rho_face_np1(const DoubleTab& rho)=0;
  virtual double moyenne_vol(const DoubleTab&) const =0;

protected :
  REF(Fluide_Dilatable_base) le_fluide_;
};

#endif /* EOS_Tools_base_included */
