/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Perte_Charge_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Perte_Charge_VDF_Face_included
#define Perte_Charge_VDF_Face_included

#include <Source_dep_inco_base.h>
#include <Ref_Fluide_Incompressible.h>
#include <Ref_Champ_Face.h>
#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>
#include <IntVect.h>
#include <DoubleVect.h>

class Probleme_base;

//
// .DESCRIPTION class Perte_Charge_VDF_Face
//

//
// .SECTION voir aussi Source_base
//
//

class Perte_Charge_VDF_Face : public Source_dep_inco_base
{

  Declare_base(Perte_Charge_VDF_Face);

public:

  void associer_pb(const Probleme_base& );
  void mettre_a_jour(double temps)
  {
    ;
  }

protected:

  REF(Champ_Face) la_vitesse;
  REF(Fluide_Incompressible) le_fluide;
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  IntVect num_faces;
  DoubleVect corr_front_ss;
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );

};

#endif
