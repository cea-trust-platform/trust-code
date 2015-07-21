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
// File:        Op_Conv_EF_VEF_P1NC.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Conv_EF_VEF_P1NC_included
#define Op_Conv_EF_VEF_P1NC_included

#include <Matrice_Morse.h>
#include <Op_Conv_VEF_base.h>

//
// .DESCRIPTION class Op_Conv_EF_VEF_P1NC
//
//  Cette classe represente l'operateur de convection associe a une equation de
//  transport d'un scalaire.
//  La discretisation est VEF
//  Le champ convecte est scalaire ou vecteur de type Champ_P1NC
//  Le schema de convection est du type Amont
//  Les methodes pour l'implicite sont codees.

//
// .SECTION voir aussi
// Operateur_Conv_base


class Op_Conv_EF_VEF_P1NC : public Op_Conv_VEF_base
{

  Declare_instanciable(Op_Conv_EF_VEF_P1NC);

public:

  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const;

  // Methodes pour l implicite.
  inline void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  void contribue_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;

protected:
  int transportant_bar;
  int transporte_bar;
  int filtrer_resu;
  int antisym;
};


// Description:
// on dimensionne notre matrice au moyen de la methode dimensionner de la classe
// Op_VEF_Face.

inline  void Op_Conv_EF_VEF_P1NC::dimensionner(Matrice_Morse& matrice) const
{
  Op_VEF_Face::dimensionner(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice);
}


// Description:
// On modifie le second membre et la matrice dans le cas des
// conditions de dirichlet.

inline void Op_Conv_EF_VEF_P1NC::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VEF_Face::modifier_pour_Cl(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice, secmem);
}


//Description:
//on assemble la matrice des inconnues implicite.

inline void Op_Conv_EF_VEF_P1NC::contribuer_a_avec(const DoubleTab& inco,
                                                   Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Conv_EF_VEF_P1NC::contribuer_au_second_membre(DoubleTab& resu) const
{
  contribue_au_second_membre(resu);
}

#endif
