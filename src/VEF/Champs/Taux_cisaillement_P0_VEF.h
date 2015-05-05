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
// File:        Taux_cisaillement_P0_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Taux_cisaillement_P0_VEF_included
#define Taux_cisaillement_P0_VEF_included


#include <Champ_Fonc_P0_VEF.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Zone_Cl_VEF.h>

//.DESCRIPTION  classe Taux_cisaillement_P0_VEF
//

class Taux_cisaillement_P0_VEF : public Champ_Fonc_P0_VEF
{

  Declare_instanciable(Taux_cisaillement_P0_VEF);

public:

  void mettre_a_jour(double );
  void associer_champ(const Champ_P1NC&, const Zone_Cl_dis_base&);

private:

  REF(Zone_Cl_VEF) la_zone_Cl_VEF;
  REF(Champ_P1NC) vitesse_;
};

#endif
