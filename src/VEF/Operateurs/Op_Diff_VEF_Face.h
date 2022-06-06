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


#ifndef Op_Diff_VEF_Face_included
#define Op_Diff_VEF_Face_included

#include <Op_Diff_VEF_base.h>
#include <Ref_Champ_Inc.h>
#include <Matrice_Morse.h>
#include <Champ_base.h>

//
// .DESCRIPTION class Op_Diff_VEF_Face
//  Cette classe represente l'operateur de diffusion
//  La discretisation est VEF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
//
//////////////////////////////////////////////////////////////////////////////


class Op_Diff_VEF_Face : public Op_Diff_VEF_base
{
  Declare_instanciable(Op_Diff_VEF_Face);

public:


  void associer_diffusivite(const Champ_base& ) override;
  void completer() override;
  const Champ_base& diffusivite() const override;

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  void verifier() const;

  // Methodes pour l implicite.

  inline void dimensionner(Matrice_Morse& ) const override;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  inline void contribuer_au_second_membre(DoubleTab& ) const override;
  void contribue_au_second_membre(DoubleTab& ) const;

  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_multi_scalaire(const DoubleTab&, Matrice_Morse& ) const;

  void ajouter_cas_scalaire(const DoubleTab& inconnue,
                            DoubleTab& resu, DoubleTab& flux_bords,
                            DoubleTab& nu,
                            const Zone_Cl_VEF& zone_Cl_VEF,
                            const Zone_VEF& zone_VEF ) const;
  void ajouter_cas_vectoriel(const DoubleTab& inconnue,
                             DoubleTab& resu, DoubleTab& flux_bords,
                             DoubleTab& nu,
                             const Zone_Cl_VEF& zone_Cl_VEF,
                             const Zone_VEF& zone_VEF,
                             int nb_comp) const;
  void ajouter_cas_multi_scalaire(const DoubleTab& inconnue,
                                  DoubleTab& resu, DoubleTab& flux_bords,
                                  DoubleTab& nu,
                                  const Zone_Cl_VEF& zone_Cl_VEF,
                                  const Zone_VEF& zone_VEF,
                                  int nb_comp) const;


protected :


  REF(Champ_base) diffusivite_;

  //DoubleVect porosite_face;
};

// Description:
// on dimensionne notre matrice.

inline  void Op_Diff_VEF_Face::dimensionner(Matrice_Morse& matrice) const
{
  Op_VEF_Face::dimensionner(la_zone_vef.valeur(), la_zcl_vef.valeur(), matrice);
}

inline void Op_Diff_VEF_Face::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VEF_Face::modifier_pour_Cl(la_zone_vef.valeur(),la_zcl_vef.valeur(), matrice, secmem);
}


//Description:
//on assemble la matrice.

inline void Op_Diff_VEF_Face::contribuer_a_avec(const DoubleTab& inco,
                                                Matrice_Morse& matrice) const
{
  const Champ_base& inconnue = equation().inconnue().valeur();
  const Nature_du_champ nature_champ = inconnue.nature_du_champ();
  if (nature_champ!=multi_scalaire)
    ajouter_contribution(inco, matrice);
  else
    ajouter_contribution_multi_scalaire(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_VEF_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  contribue_au_second_membre(resu);
}

#endif
