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
// File:        Op_Conv_Centre_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Conv
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Conv_Centre_VDF_Elem_included
#define Op_Conv_Centre_VDF_Elem_included

#include <Eval_Conv_VDF_Elem_leaves.h>
#include <Op_VDF_Elem.h>
#include <ItVDFEl.h>

// .DESCRIPTION class Op_Conv_Centre_VDF_Elem
//  Cette classe represente l'operateur de convection associe a une equation de
//  transport d'un scalaire.
//  La discretisation est VDF
//  Le champ convecte est scalaire
//  Le schema de convection est du type Centre (sur 2 points)
//  L'iterateur associe est de type Iterateur_VDF_Elem
//  L'evaluateur associe est de type Eval_Centre_VDF_Elem

declare_It_VDF_Elem(Eval_Centre_VDF_Elem)

class Op_Conv_Centre_VDF_Elem : public Op_Conv_VDF_base, public Op_VDF_Elem
{
  Declare_instanciable_sans_constructeur(Op_Conv_Centre_VDF_Elem);

public:

  Op_Conv_Centre_VDF_Elem();
  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc&  );
  Champ_Inc_base& vitesse();
  const Champ_Inc_base& vitesse() const;
  inline void associer_vitesse(const Champ_base& );
  inline void dimensionner(Matrice_Morse& ) const;
  inline void dimensionner_bloc_vitesse(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
};

// Description:
// associe le champ de vitesse a l'evaluateur
inline void Op_Conv_Centre_VDF_Elem::associer_vitesse(const Champ_base& ch_vit)
{
  const Champ_Face& vit = (Champ_Face&) ch_vit;

  Eval_Centre_VDF_Elem& eval_conv = dynamic_cast<Eval_Centre_VDF_Elem&> (iter.evaluateur());
  eval_conv.associer(vit );                // Eval_Conv_VDF::associer
}

inline  void Op_Conv_Centre_VDF_Elem::dimensionner(Matrice_Morse& matrice) const
{
  Op_VDF_Elem::dimensionner(iter.zone(), iter.zone_Cl(), matrice);
}

inline  void Op_Conv_Centre_VDF_Elem::dimensionner_bloc_vitesse(Matrice_Morse& matrice) const
{
  Op_VDF_Elem::dimensionner_bloc_vitesse(iter.zone(), iter.zone_Cl(), matrice);
}

inline void Op_Conv_Centre_VDF_Elem::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Elem::modifier_pour_Cl(iter.zone(), iter.zone_Cl(), matrice, secmem);
}

#endif /* Op_Conv_Centre_VDF_Elem_included */
