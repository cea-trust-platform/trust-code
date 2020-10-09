/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Residu_P0_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Residu_P0_VDF_included
#define Residu_P0_VDF_included


#include <Champ_Fonc_P0_VDF.h>
#include <Ref_Champ_P0_VDF.h>
#include <DoubleVect.h>
#include <Zone_dis.h>
#include <Nom.h>

//.DESCRIPTION  classe Residu_P0_VDF
//
// .SECTION voir aussi
// Champ_Fonc_P0_VDF

class Residu_P0_VDF : public Champ_Fonc_P0_VDF
{

  Declare_instanciable(Residu_P0_VDF);

public:

  inline const Champ_P0_VDF& mon_champ() const;
  inline void mettre_a_jour(double );
  void associer_champ(const Champ_P0_VDF& );
  void associer_zone( const Zone_dis& zone_dis );
  void me_calculer(double );

  const Zone_Cl_dis_base& zone_Cl_dis_base() const;


protected:

  REF(Champ_P0_VDF) mon_champ_;
  Zone_dis zone_dis_;
};

inline const Champ_P0_VDF& Residu_P0_VDF::mon_champ() const
{
  return mon_champ_.valeur();
}

inline void Residu_P0_VDF::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}



#endif
