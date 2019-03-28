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
// File:        Op_Div_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Div_PolyMAC_included
#define Op_Div_PolyMAC_included

#include <Operateur_Div.h>
#include <Ref_Zone_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>

//
// .DESCRIPTION class Op_Div_PolyMAC
//
//  Cette classe represente l'operateur de divergence
//  La discretisation est PolyMAC
//  On calcule la divergence d'un champ_P1NC (la vitesse)
//


//
// .SECTION voir aussi
// Operateur_Div_base
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Div_PolyMAC
//
//////////////////////////////////////////////////////////////////////////////
class MAtrice_Morse;
class Op_Div_PolyMAC : public Operateur_Div_base
{

  Declare_instanciable(Op_Div_PolyMAC);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis&,const Champ_Inc&);
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual int impr(Sortie& os) const;
  void volumique(DoubleTab& ) const;

  void dimensionner(Matrice_Morse& matrice) const;
  void contribuer_a_avec(const DoubleTab&,Matrice_Morse& matrice) const;

protected:

  REF(Zone_PolyMAC) la_zone_PolyMAC;
  REF(Zone_Cl_PolyMAC) la_zcl_PolyMAC;

  DoubleVect porosite_face;
};

#endif
