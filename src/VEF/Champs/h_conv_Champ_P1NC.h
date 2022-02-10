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
// File:        h_conv_Champ_P1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef h_conv_Champ_P1NC_included
#define h_conv_Champ_P1NC_included


#include <Champ_h_conv.h>
#include <Champ_Fonc_P0_VEF.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Zone_Cl_VEF.h>

//.DESCRIPTION  classe h_conv_Champ_P1NC
//
// .SECTION voir aussi
// Champ_h_conv Champ_Fonc_P0_VEF

class h_conv_Champ_P1NC : public Champ_h_conv,
  public Champ_Fonc_P0_VEF

{

  Declare_instanciable(h_conv_Champ_P1NC);

public:

  inline const Champ_P1NC& mon_champ() const;
  inline int& temp_ref();
  inline const int& temp_ref() const;
  inline void mettre_a_jour(double ) override;
  void associer_champ(const Champ_P1NC& );
  void me_calculer(double ) override;

  inline void associer_zone_Cl_dis_base(const Zone_Cl_dis_base&);
  const Zone_Cl_dis_base& zone_Cl_dis_base() const;


protected:

  REF(Champ_P1NC) mon_champ_;
  REF(Zone_Cl_VEF) la_zone_Cl_VEF;
  int temp_ref_;
};

inline const Champ_P1NC& h_conv_Champ_P1NC::mon_champ() const
{
  return mon_champ_.valeur();
}

inline int& h_conv_Champ_P1NC::temp_ref()
{
  return temp_ref_;
}
inline const int& h_conv_Champ_P1NC::temp_ref() const
{
  return temp_ref_;
}


inline void h_conv_Champ_P1NC::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

inline void h_conv_Champ_P1NC::associer_zone_Cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VEF  = (const Zone_Cl_VEF&) la_zone_Cl_dis_base;
}



#endif
