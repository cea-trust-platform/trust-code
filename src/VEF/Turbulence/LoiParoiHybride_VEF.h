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
// File:        LoiParoiHybride_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////



#ifndef LoiParoiHybride_VEF_included
#define LoiParoiHybride_VEF_included

#include <LoiParoiHybride.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
class Zone_dis_base;
class Zone_Cl_dis_base;


//.DESCRIPTION
//
// CLASS: LoiParoiHybride_VEF
//

//.SECTION  voir aussi
// Turbulence_paroi_base

class LoiParoiHybride_VEF : public Turbulence_paroi_base, LoiParoiHybride
{

  Declare_instanciable(LoiParoiHybride_VEF);

public:

  void associer(const Zone_dis&, const Zone_Cl_dis& );
  virtual int init_lois_paroi();
  int calculer_hyd(DoubleTab& );
  int calculer_hyd(DoubleTab& , DoubleTab& );

protected:


private:

  REF(Zone_VEF) la_zone_VEF;
  REF(Zone_Cl_VEF) la_zone_Cl_VEF;

};

///////////////////////////////////////////////////////////
//
//  Fonctions inline de la classe Paroi_std_hyd_VEF
//
///////////////////////////////////////////////////////////



#endif
