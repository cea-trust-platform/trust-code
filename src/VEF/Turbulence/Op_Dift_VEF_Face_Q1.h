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
// File:        Op_Dift_VEF_Face_Q1.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Dift_VEF_Face_Q1_included
#define Op_Dift_VEF_Face_Q1_included

#include <Op_Dift_VEF_base.h>
#include <Ref_Champ_Q1NC.h>
#include <Ref_Champ_Uniforme.h>
#include <Matrice_Morse.h>

//
// .DESCRIPTION class Op_Dift_VEF_FaceQ1
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VEF_Face_Q1 : public Op_Dift_VEF_base
{
  Declare_instanciable(Op_Dift_VEF_Face_Q1);

public:

  void associer_diffusivite(const Champ_base&);
  const Champ_base& diffusivite() const;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  double calculer_dt_stab() const;


  // Methodes pour l implicite.

  inline void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  void contribue_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;

  inline void remplir_nu(DoubleTab& ) const
  {
    Cerr<<__FILE__<<":"<<__LINE__<<finl;
    exit();
  } ;


protected :
  REF(Champ_Uniforme) diffusivite_;
};
// Description:
// on dimensionne notre matrice.

inline  void Op_Dift_VEF_Face_Q1::dimensionner(Matrice_Morse& matrice) const
{
  Op_VEF_Face::dimensionner(la_zone_vef.valeur(), la_zcl_vef.valeur(), matrice);
}

inline void Op_Dift_VEF_Face_Q1::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VEF_Face::modifier_pour_Cl(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice, secmem);
}


//Description:
//on assemble la matrice.

inline void Op_Dift_VEF_Face_Q1::contribuer_a_avec(const DoubleTab& inco,
                                                   Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Dift_VEF_Face_Q1::contribuer_au_second_membre(DoubleTab& resu) const
{
  contribue_au_second_membre(resu);
}


#endif

