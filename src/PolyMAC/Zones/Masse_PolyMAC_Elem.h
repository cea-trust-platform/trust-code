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
// File:        Masse_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Masse_PolyMAC_Elem_included
#define Masse_PolyMAC_Elem_included


#include <Solveur_Masse.h>
#include <Ref_Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Masse_PolyMAC_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Masse_PolyMAC_Elem : public Solveur_Masse_base
{

  Declare_instanciable(Masse_PolyMAC_Elem);

public:

  void associer_zone_dis_base(const Zone_dis_base& );
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& );

  virtual void dimensionner(Matrice_Morse& matrix) const;
  DoubleTab& ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation = 1) const;
  virtual Matrice_Base& ajouter_masse(double dt, Matrice_Base& matrice, int penalisation = 1) const;

  DoubleTab& appliquer_impl(DoubleTab& ) const;
  void completer();
  void appliquer_coef(DoubleVect& coef) const;

private:

  bool no_diff_;
  REF(Zone_PolyMAC) la_zone_PolyMAC;
  REF(Zone_Cl_PolyMAC) la_zone_Cl_PolyMAC;
};

#endif





