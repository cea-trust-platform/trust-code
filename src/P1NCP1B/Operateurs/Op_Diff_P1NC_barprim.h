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
// File:        Op_Diff_P1NC_barprim.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_P1NC_barprim_included
#define Op_Diff_P1NC_barprim_included

#include <Operateur_Diff_base.h>
#include <Ref_Champ_Uniforme.h>
#include <Ref_Zone_VEF_PreP1b.h>
#include <Ref_Zone_Cl_VEFP1B.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <Op_VEF_Face.h>
#include <Zone_VEF_PreP1b.h>
#include <Zone_Cl_VEFP1B.h>
#include <Ref_Champ_Inc.h>



//
// .DESCRIPTION class Op_Diff_VEF_Face
//  Cette classe represente l'operateur de diffusion
//  La discretisation est VEF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_P1NC_barprim : public Operateur_Diff_base, public Op_VEF_Face
{
  Declare_instanciable(Op_Diff_P1NC_barprim);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );
  void completer();
  const Champ_base& diffusivite() const;

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;

  // Methodes pour l implicite.

  inline void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  void contribue_au_second_membre(DoubleTab& ) const;

  double calculer_dt_stab() const;



protected :

  void calculer_divergence(const DoubleTab&, const DoubleVect&, DoubleTab&) const;
  REF(Zone_VEF_PreP1b) la_zone_vef;
  REF(Zone_Cl_VEFP1B) la_zcl_vef;
  REF(Champ_Uniforme) diffusivite_;
  //REF(Champ_Inc) inconnue_;

};

// Description:
// on dimensionne notre matrice.

inline  void Op_Diff_P1NC_barprim::dimensionner(Matrice_Morse& matrice) const
{
  // Op_VEF_Face::dimensionner(la_zone_vef.valeur(), la_zcl_vef.valeur(), matrice);
  return;
}

inline void Op_Diff_P1NC_barprim::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  // Op_VEF_Face::modifier_pour_Cl(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice, secmem);
  return;
}


//Description:
//on assemble la matrice.

inline void Op_Diff_P1NC_barprim::contribuer_a_avec(const DoubleTab& inco,
                                                    Matrice_Morse& matrice) const
{
  /*    ajouter_contribution(inco, matrice);    */
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_P1NC_barprim::contribuer_au_second_membre(DoubleTab& resu) const
{
  //contribue_au_second_membre(resu);
}

#endif
