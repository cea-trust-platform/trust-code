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
// File:        Op_Dift_EF_Q1.h
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Dift_EF_Q1_included
#define Op_Dift_EF_Q1_included

#include <Op_Dift_EF_base.h>
#include <Ref_Champ_Inc.h>
#include <Ref_Champ_Uniforme.h>
#include <Ref_Zone_EF.h>
#include <Ref_Zone_Cl_EF.h>
#include <Matrice_Morse.h>
#include <Op_EF_base.h>
#include <Champ_Don.h>

//
// .DESCRIPTION class Op_Dift_EF_Q1
//  Cette classe represente l'operateur de diffusion
//  La discretisation est EF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
//
//////////////////////////////////////////////////////////////////////////////


class Op_Dift_EF_Q1 : public Op_Dift_EF_base
{
  Declare_instanciable(Op_Dift_EF_Q1);

public:

  void associer_diffusivite(const Champ_base& ) override;
  inline const Champ_base& diffusivite() const override
  {
    return diffusivite_;
  };
  inline double diffusivite(int) const
  {
    return (diffusivite().valeurs().nb_dim()==1)?(diffusivite().valeurs())(0):(diffusivite().valeurs())(0,0);
  };
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& ajouter_new(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  double calculer_dt_stab() const override;
  void calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const override;
  void verifier() const;
  void remplir_nu(DoubleTab&) const override;
  void remplir_marqueur_elem_CL_paroi(ArrOfInt& ,const Zone_EF& ,const Zone_Cl_EF& ) const;

  // Methodes pour l implicite.

  inline void dimensionner(Matrice_Morse& ) const override;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  void contribuer_au_second_membre(DoubleTab& ) const override;
  void ajouter_bords(const DoubleTab&, DoubleTab& ,int contrib_interne=1) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contributions_bords(Matrice_Morse& matrice ) const;
  void ajouter_contribution_new(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_cas_scalaire(const DoubleTab& inconnue,
                            DoubleTab& resu, DoubleTab& flux_bords,
                            DoubleTab& nu,
                            const Zone_Cl_EF& zone_Cl_EF,
                            const Zone_EF& zone_EF ) const;
  void ajouter_cas_vectoriel(const DoubleTab& inconnue,
                             DoubleTab& resu, DoubleTab& flux_bords,
                             DoubleTab& nu,
                             const Zone_Cl_EF& zone_Cl_EF,
                             const Zone_EF& zone_EF,
                             int nb_comp) const;


protected :
  int transpose_;   // vaurt zero si on ne veut pas calculer grad u transpose
  int transpose_partout_ ; // vaut 1 si on veut calculer grad_u_transpose meme au bord
  int nouvelle_expression_;
  REF(Champ_base) diffusivite_;
  DoubleTab& ajouter_vectoriel_dim3_nbn_8(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_scalaire_dim3_nbn_8(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_vectoriel_dim2_nbn_4(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_scalaire_dim2_nbn_4(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_vectoriel_gen(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& ajouter_scalaire_gen(const DoubleTab& ,  DoubleTab& ) const;
};

class Op_Dift_EF_Q1_option : public Op_Dift_EF_Q1
{
  Declare_instanciable(Op_Dift_EF_Q1_option);
};


// Description:
// on dimensionne notre matrice.

inline  void Op_Dift_EF_Q1::dimensionner(Matrice_Morse& matrice) const
{
  Op_EF_base::dimensionner(la_zone_EF.valeur(), la_zcl_EF.valeur(), matrice);
}

inline void Op_Dift_EF_Q1::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_EF_base::modifier_pour_Cl(la_zone_EF.valeur(),la_zcl_EF.valeur(), matrice, secmem);
}


//Description:
//on assemble la matrice.

inline void Op_Dift_EF_Q1::contribuer_a_avec(const DoubleTab& inco,
                                             Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}


#endif
