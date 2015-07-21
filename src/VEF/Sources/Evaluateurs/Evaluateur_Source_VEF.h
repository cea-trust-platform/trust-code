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
// File:        Evaluateur_Source_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Evaluateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Evaluateur_Source_VEF_included
#define Evaluateur_Source_VEF_included


#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Evaluateur_Source_VEF
//
//
//////////////////////////////////////////////////////////////////////////////

class Evaluateur_Source_VEF
{

public:

  inline Evaluateur_Source_VEF();
  inline virtual ~Evaluateur_Source_VEF() {};
  inline Evaluateur_Source_VEF(const Evaluateur_Source_VEF& );
  inline void associer_zones(const Zone_dis_base&,const Zone_Cl_dis_base&);
  virtual void mettre_a_jour() =0;
  virtual void completer() = 0;

protected:

  REF(Zone_VEF) la_zone;
  REF(Zone_Cl_VEF) la_zone_cl;

};

//
//   Fonctions inline de Evaluateur_Source_VEF
//

inline Evaluateur_Source_VEF::Evaluateur_Source_VEF() {}

inline Evaluateur_Source_VEF::Evaluateur_Source_VEF(const Evaluateur_Source_VEF& eval)
  : la_zone(eval.la_zone),la_zone_cl(eval.la_zone_cl)
{
}

inline void Evaluateur_Source_VEF::associer_zones(const Zone_dis_base& zone_vef,
                                                  const Zone_Cl_dis_base& zone_cl_vef)
{
  la_zone = ref_cast(Zone_VEF,zone_vef);
  la_zone_cl = ref_cast(Zone_Cl_VEF,zone_cl_vef);
  completer();
}

#endif
