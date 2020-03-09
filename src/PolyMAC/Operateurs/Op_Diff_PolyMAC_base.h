/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_PolyMAC_base.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_PolyMAC_base_included
#define Op_Diff_PolyMAC_base_included

#include <Operateur_Diff_base.h>
#include <Op_Diff_Turbulent_base.h>
#include <Ref_Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>
class Champ_Fonc;


//
// .DESCRIPTION class Op_Diff_PolyMAC_base
//
// Classe de base des operateurs de diffusion PolyMAC

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_PolyMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_PolyMAC_base : public Operateur_Diff_base, public Op_Diff_Turbulent_base
{


  Declare_base(Op_Diff_PolyMAC_base);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& );

  void associer_diffusivite(const Champ_base& );
  void completer();
  const Champ_base& diffusivite() const;

  void remplir_nu(DoubleTab& nu) const;
  void remplir_nu_fac() const;
  const DoubleTab& get_nu() const
  {
    return nu_;
  }
  const DoubleTab& get_nu_fac() const
  {
    return nu_fac;
  }

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual int impr(Sortie& os) const;

protected:
  REF(Zone_PolyMAC) la_zone_poly_;
  REF(Zone_Cl_PolyMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;
  mutable DoubleTab nu_;
  //facteur pour moduler la conductivite par face : le flux a la face f est multiplie par nu_fac(f)^2
  mutable DoubleTab nu_fac;
};



//
// Fonctions inline de la classe Op_Diff_PolyMAC_base
//


#endif
