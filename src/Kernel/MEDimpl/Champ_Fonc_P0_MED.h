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
// File:        Champ_Fonc_P0_MED.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_P0_MED_included
#define Champ_Fonc_P0_MED_included



#include <Champ_Fonc_P0_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_Fonc_P0_MED
//    permet de faire un champ P0 pour le post med
//////////////////////////////////////////////////////////////////////////////

class Champ_Fonc_P0_MED: public Champ_Fonc_P0_base
{
  Declare_instanciable(Champ_Fonc_P0_MED);

public:
  void associer_zone_dis_base(const Zone_dis_base&);
  const Zone_dis_base& zone_dis_base() const;
  void mettre_a_jour(double );
private:
  REF(Zone_dis_base) zonebidon;
};
#endif
