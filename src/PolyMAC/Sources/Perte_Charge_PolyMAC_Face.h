/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Perte_Charge_PolyMAC_Face.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Perte_Charge_PolyMAC_Face_included
#define Perte_Charge_PolyMAC_Face_included

#include <Source_base.h>
#include <Terme_Source_Qdm.h>

#include <Ref_Zone_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>
#include <Ref_Fluide_Incompressible.h>
#include <Ref_Champ_Inc_base.h>


class Probleme_base;

//
// .DESCRIPTION class Perte_Charge_PolyMAC_Face
//

//
// .SECTION voir aussi Source_base
//
//

class Perte_Charge_PolyMAC_Face :  public Source_base,
  public Terme_Source_Qdm


{

  Declare_base(Perte_Charge_PolyMAC_Face);

public:

  virtual DoubleTab& ajouter(DoubleTab& )  const =0;
  virtual DoubleTab& calculer(DoubleTab& ) const =0;
  void associer_pb(const Probleme_base& );
  void mettre_a_jour(double );

protected:

  REF(Champ_Inc_base) la_vitesse;
  REF(Zone_PolyMAC) la_zone_PolyMAC;
  REF(Zone_Cl_PolyMAC) la_zone_Cl_PolyMAC;
  REF(Fluide_Incompressible) le_fluide;
  IntVect num_faces;

  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );



};

#endif
