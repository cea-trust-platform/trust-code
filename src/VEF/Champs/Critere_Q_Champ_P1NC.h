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
// File:        Critere_Q_Champ_P1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Critere_Q_Champ_P1NC_included
#define Critere_Q_Champ_P1NC_included


#include <Champ_Critere_Q.h>
#include <Champ_Fonc_P0_VEF.h>
#include <Ref_Champ_P1NC.h>

//.DESCRIPTION  classe Critere_Q_Champ_P1NC
//
// .SECTION voir aussi
// Champ_Critere_Q Champ_Fonc_P0_VDF

class Critere_Q_Champ_P1NC : public Champ_Critere_Q,
  public Champ_Fonc_P0_VEF

{

  Declare_instanciable(Critere_Q_Champ_P1NC);

public:

  inline const Champ_P1NC& mon_champ() const;
  inline void mettre_a_jour(double );
  void associer_champ(const Champ_P1NC& );
  void me_calculer(double );

  //  inline void associer_zone_Cl_dis_base(const Zone_Cl_dis_base&);
  //  const Zone_Cl_dis_base& zone_Cl_dis_base() const;


protected:

  REF(Champ_P1NC) mon_champ_;
  //  REF(Zone_Cl_VEF) la_zone_Cl_VEF;
};

inline const Champ_P1NC& Critere_Q_Champ_P1NC::mon_champ() const
{
  return mon_champ_.valeur();
}

inline void Critere_Q_Champ_P1NC::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

/* inline void Critere_Q_Champ_P1NC::associer_zone_Cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)   */
/* { */
/*   la_zone_Cl_VEF  = (const Zone_Cl_VDF&) la_zone_Cl_dis_base; */
/* } */



#endif
