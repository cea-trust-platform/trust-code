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
// File:        Op_Diff_K_Eps_VEF_Face_Q1.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


//
// .DESCRIPTION class Op_Diff_K_Eps_VEF_FaceQ1
//   Cette classe represente l'operateur de diffusion turbulente
//   La discretisation est VEF
//  Les methodes pour l'implicite sont codees.

#ifndef Op_Diff_K_Eps_VEF_Face_Q1_included
#define Op_Diff_K_Eps_VEF_Face_Q1_included

#define PRDT_K_DEFAUT 1
#define PRDT_EPS_DEFAUT 1.3

#include <Op_Diff_K_Eps_VEF_base.h>
#include <Ref_Champ_Q1NC.h>

class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc;


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_K_Eps_VEF_FaceQ1
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_VEF_Face_Q1 : public Op_Diff_K_Eps_VEF_base, public Op_VEF_Face
{

  Declare_instanciable(Op_Diff_K_Eps_VEF_Face_Q1);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  void associer_diffusivite_turbulente();
  const Champ_Fonc& diffusivite_turbulente() const;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;

  // Methodes pour l implicite.

  inline void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  void contribue_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;

protected :
  REF(Zone_VEF) la_zone_vef;
  REF(Zone_Cl_VEF) la_zcl_vef;
  REF(Champ_Q1NC) inconnue_;
  //DoubleVect porosite_face;
};

//
// Fonctions inline de la classe Op_Diff_K_Eps_VEF_FaceQ1
//
// Description:
// on dimensionne notre matrice au moyen de la methode dimensionner de la classe
// Op_VEF_Face.

inline  void Op_Diff_K_Eps_VEF_Face_Q1::dimensionner(Matrice_Morse& matrice) const
{
  Op_VEF_Face::dimensionner(la_zone_vef.valeur(), la_zcl_vef.valeur(), matrice);
}

// Description:
// On modifie le second membre et la matrice dans le cas des
// conditions de dirichlet.

inline void Op_Diff_K_Eps_VEF_Face_Q1::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VEF_Face::modifier_pour_Cl(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice, secmem);
}


//Description:
//on assemble la matrice des inconnues implicite.

inline void Op_Diff_K_Eps_VEF_Face_Q1::contribuer_a_avec(const DoubleTab& inco,
                                                         Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_K_Eps_VEF_Face_Q1::contribuer_au_second_membre(DoubleTab& resu) const
{
  contribue_au_second_membre(resu);
}


#endif
