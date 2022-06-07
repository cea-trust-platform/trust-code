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

#ifndef Sous_zone_dis_included
#define Sous_zone_dis_included

#include <Sous_zone_dis_base.h>

Declare_deriv(Sous_zone_dis_base);


//! Classe generique de la hierarchie des sous-zones discretisees.
/**
   Un objet de type Sous_zone_dis peut referencer n'importe quel objet
   derivant de Sous_zone_dis_base.
*/

class Sous_zone_dis : public DERIV(Sous_zone_dis_base)
{

  Declare_instanciable(Sous_zone_dis);

public:

  inline void associer_sous_zone(const Sous_Zone& ssz)
  {
    valeur().associer_sous_zone(ssz);
  }
  inline void associer_zone_dis(const Zone_dis_base& zd)
  {
    valeur().associer_zone_dis(zd);
  }
  inline void discretiser()
  {
    valeur().discretiser();
  }
};

#endif
