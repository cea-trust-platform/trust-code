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
// File:        Terme_Source_Constituant_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Source_Constituant_PolyMAC_Elem_included
#define Terme_Source_Constituant_PolyMAC_Elem_included



#include <Terme_Source_Constituant.h>
#include <Terme_Source_PolyMAC_base.h>
#include <Eval_Source_C_PolyMAC_Elem.h>
#include <ItSouPolyMACEl.h>

declare_It_Sou_PolyMAC_Elem(Eval_Source_C_PolyMAC_Elem)

//.DESCRIPTION class Terme_Source_Constituant_PolyMAC_Elem
//
// Cette classe represente un terme source de l'equation de la thermique
// du type degagement volumique de puissance thermique uniforme sur une zone
//
//.SECTION
// voir aussi Terme_Source_Constituant, Terme_Source_PolyMAC_base

class Terme_Source_Constituant_PolyMAC_Elem : public Terme_Source_Constituant,
  public Terme_Source_PolyMAC_base
{
  Declare_instanciable_sans_constructeur(Terme_Source_Constituant_PolyMAC_Elem);

public:

  inline Terme_Source_Constituant_PolyMAC_Elem();
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  void associer_pb(const Probleme_base& );
  void mettre_a_jour(double temps)
  {
    Terme_Source_Constituant::mettre_a_jour(temps);
  }
};


//
// Fonctions inline de la classe Terme_Source_Constituant_PolyMAC_Elem
//

inline Terme_Source_Constituant_PolyMAC_Elem::Terme_Source_Constituant_PolyMAC_Elem()
  : Terme_Source_Constituant(),Terme_Source_PolyMAC_base(It_Sou_PolyMAC_Elem(Eval_Source_C_PolyMAC_Elem)())
{
}


#endif
