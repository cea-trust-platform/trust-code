/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Conv_centre4_CoviMAC_Elem.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Conv_centre4_CoviMAC_Elem_included
#define Op_Conv_centre4_CoviMAC_Elem_included

#include <ItCoviMACEl.h>
#include <Eval_centre4_CoviMAC_Elem.h>

//
// .DESCRIPTION class Op_Conv_centre4_CoviMAC_Elem
//
//  Cette classe represente l'operateur de convection associe a une equation de
//  transport d'un scalaire.
//  La discretisation est CoviMAC
//  Le champ convecte est scalaire
//  Le schema de convection est du type Centre (sur 4 points)
//  L'iterateur associe est de type Iterateur_CoviMAC_Elem
//  L'evaluateur associe est de type Eval_centre4_CoviMAC_Elem

//
// .SECTION voir aussi
//
//
declare_It_CoviMAC_Elem(Eval_centre4_CoviMAC_Elem)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Conv_centre4_CoviMAC_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Op_Conv_centre4_CoviMAC_Elem : public Op_Conv_CoviMAC_iterateur_base
{
  Declare_instanciable_sans_constructeur(Op_Conv_centre4_CoviMAC_Elem);

public:

  Op_Conv_centre4_CoviMAC_Elem();
  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc&  );
  inline void associer_vitesse(const Champ_base& );
  const Champ_Inc_base& vitesse() const;
  Champ_Inc_base& vitesse();
};

// Description:
// associe le champ de vitesse a l'evaluateur
inline void Op_Conv_centre4_CoviMAC_Elem::associer_vitesse(const Champ_base& ch_vit)
{
  const Champ_Face& vit = (Champ_Face&) ch_vit;

  Eval_centre4_CoviMAC_Elem& eval_conv = (Eval_centre4_CoviMAC_Elem&) iter.evaluateur();
  eval_conv.associer(vit );                // Eval_Conv_CoviMAC::associer
}

#endif
