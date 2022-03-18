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
// File:        Terme_Source_Canal_perio_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Terme_Source_Canal_perio_VDF_Face_included
#define Terme_Source_Canal_perio_VDF_Face_included



//
// .DESCRIPTION class Terme_Source_Canal_perio_VDF_Face
//  Cette classe permet de conserver le debit dans une simulation
//  temporelle de Canal
//
// .SECTION voir aussi
//  Terme_Source_Canal_perio

#include <Terme_Source_Canal_perio.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Ref_Zone_VDF.h>

class Probleme_base;
class Navier_Stokes_std;


// La classe derive de Source_base et peut etre d'un terme source

class Terme_Source_Canal_perio_VDF_Face : public Terme_Source_Canal_perio
{
  Declare_instanciable(Terme_Source_Canal_perio_VDF_Face);

public :
  DoubleTab& ajouter(DoubleTab& ) const override;

protected :
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override;

  void calculer_debit(double&) const override;
};
class Terme_Source_Canal_perio_QC_VDF_Face : public Terme_Source_Canal_perio_VDF_Face
{
  Declare_instanciable(Terme_Source_Canal_perio_QC_VDF_Face);
};
class Terme_Source_Canal_perio_VDF_Front_Tracking : public Terme_Source_Canal_perio_VDF_Face
{
  Declare_instanciable(Terme_Source_Canal_perio_VDF_Front_Tracking);
};
#endif
