/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Fonc_P0_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Fonc_P0_PolyMAC_included
#define Champ_Fonc_P0_PolyMAC_included

#include <Champ_Fonc_P0_base.h>
#include <Ref_Zone_PolyMAC.h>

//.DESCRIPTION classe Champ_Fonc_P0_PolyMAC
//

//.SECTION voir aussi
// Champ_Fonc_P0

class Champ_Fonc_P0_PolyMAC: public Champ_Fonc_P0_base
{

  Declare_instanciable(Champ_Fonc_P0_PolyMAC);

public :

  inline void associer_zone_dis_base(const Zone_dis_base&);
  const Zone_dis_base& zone_dis_base() const;
  virtual void mettre_a_jour(double );
  double valeur_au_bord(int face) const;
  //   DoubleVect moyenne() const;
  //   double moyenne(int ) const;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  DoubleVect moyenne() const;
  double moyenne(int ) const;
  int imprime(Sortie& , int ) const;
protected:

  REF(Zone_PolyMAC) la_zone_PolyMAC;
};

inline void Champ_Fonc_P0_PolyMAC::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_PolyMAC = (const Zone_PolyMAC&) la_zone_dis_base;
}

#endif
