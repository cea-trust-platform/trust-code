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
// File:        Op_Conv_VDF_Elem_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Conv
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
//
#ifndef Op_Conv_VDF_Elem_leaves_included
#define Op_Conv_VDF_Elem_leaves_included

#include <Eval_Conv_VDF_Face_leaves.h> // pour les compilos (templates :-) )
#include <Eval_Conv_VDF_Elem_leaves.h>
#include <Op_Conv_VDF.h>
#include <ItVDFEl.h>
#include <Nom.h>

/// \cond DO_NOT_DOCUMENT
class Op_Conv_VDF_Elem_leaves
{ };
/// \endcond

declare_It_VDF_Elem(Eval_Amont_VDF_Elem)
// .DESCRIPTION class Op_Conv_Amont_VDF_Elem
//  Cette classe represente l'operateur de convection associe a une equation de transport d'un scalaire.
//  La discretisation est VDF. Le champ convecte est scalaire. Le schema de convection est du type Amont
//  L'iterateur associe est de type Iterateur_VDF_Elem. L'evaluateur associe est de type Eval_Amont_VDF_Elem
class Op_Conv_Amont_VDF_Elem : public Op_Conv_VDF_base, public Op_Conv_VDF<Op_Conv_Amont_VDF_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Conv_Amont_VDF_Elem);
public:
  Op_Conv_Amont_VDF_Elem();
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { modifier_pour_Cl_elem(matrice,secmem); }
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd,const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_CONV_ELEM,Eval_Amont_VDF_Elem>(zd,zcd,ch); }
  inline void associer_vitesse(const Champ_base& ch_vit) override { associer_vitesse_impl<Eval_Amont_VDF_Elem>(ch_vit); }
  inline Champ_base& vitesse() override { return vitesse_impl<Eval_Amont_VDF_Elem>(); }
  inline const Champ_base& vitesse() const override { return vitesse_impl<Eval_Amont_VDF_Elem>(); }

  inline void dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const override
  {
    const std::string& nom_inco = equation().inconnue().le_nom().getString();
    for (auto &&i_m : mats)
      {
        std::string prefix = nom_inco + "/" ;
        // pour la thermique monolithique: on traite uniquement notre inconnue
        if(Nom(i_m.first).debute_par(prefix)) continue;
        Matrice_Morse mat;
        if (i_m.first == "vitesse") dimensionner_bloc_vitesse_elem(mat);
        else dimensionner_elem(mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
  }

protected:
  // Ce constructeur permet de creer des classes filles (exemple : front_tracking)
  inline Op_Conv_Amont_VDF_Elem(const Iterateur_VDF_base& it) : Op_Conv_VDF_base(it) { }
};

declare_It_VDF_Elem(Eval_Centre_VDF_Elem)
// .DESCRIPTION class Op_Conv_Centre_VDF_Elem
//  Cette classe represente l'operateur de convection associe a une equation de transport d'un scalaire.
//  La discretisation est VDF. Le champ convecte est scalaire. Le schema de convection est du type Centre (sur 2 points)
//  L'iterateur associe est de type Iterateur_VDF_Elem. L'evaluateur associe est de type Eval_Centre_VDF_Elem
class Op_Conv_Centre_VDF_Elem : public Op_Conv_VDF_base, public Op_Conv_VDF<Op_Conv_Centre_VDF_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Conv_Centre_VDF_Elem);
public:
  Op_Conv_Centre_VDF_Elem();
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { modifier_pour_Cl_elem(matrice,secmem); }
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd,const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_CONV_ELEM,Eval_Centre_VDF_Elem>(zd,zcd,ch); }
  inline void associer_vitesse(const Champ_base& ch_vit) override { associer_vitesse_impl<Eval_Centre_VDF_Elem>(ch_vit); }
  inline Champ_base& vitesse() override { return vitesse_impl<Eval_Centre_VDF_Elem>(); }
  inline const Champ_base& vitesse() const override { return vitesse_impl<Eval_Centre_VDF_Elem>(); }
  inline void dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const override
  {
    for (auto &&i_m : mats)
      {
        Matrice_Morse mat;
        if (i_m.first == "vitesse") dimensionner_bloc_vitesse_elem(mat);
        else dimensionner_elem(mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
  }
};

declare_It_VDF_Elem(Eval_Centre4_VDF_Elem)
// .DESCRIPTION class Op_Conv_Centre4_VDF_Elem
//  Cette classe represente l'operateur de convection associe a une equation de transport d'un scalaire.
//  La discretisation est VDF. Le champ convecte est scalaire. Le schema de convection est du type Centre (sur 4 points)
//  L'iterateur associe est de type Iterateur_VDF_Elem. L'evaluateur associe est de type Eval_Centre4_VDF_Elem
class Op_Conv_Centre4_VDF_Elem : public Op_Conv_VDF_base, public Op_Conv_VDF<Op_Conv_Centre4_VDF_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Conv_Centre4_VDF_Elem);
public:
  Op_Conv_Centre4_VDF_Elem();
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd,const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_CONV_ELEM,Eval_Centre4_VDF_Elem>(zd,zcd,ch); }
  inline void associer_vitesse(const Champ_base& ch_vit) override { associer_vitesse_impl<Eval_Centre4_VDF_Elem>(ch_vit); }
  inline Champ_base& vitesse() override { return vitesse_impl<Eval_Centre4_VDF_Elem>(); }
  inline const Champ_base& vitesse() const override { return vitesse_impl<Eval_Centre4_VDF_Elem>(); }
};

declare_It_VDF_Elem(Eval_Quick_VDF_Elem)
// .DESCRIPTION class Op_Conv_Quick_VDF_Elem
//  Cette classe represente l'operateur de convection associe a une equation de transport d'un scalaire.
//  La discretisation est VDF. Le champ convecte est scalaire. Le schema de convection est du type Quick
//  L'iterateur associe est de type Iterateur_VDF_Elem. L'evaluateur associe est de type Eval_Quick_VDF_Elem
class Op_Conv_Quick_VDF_Elem : public Op_Conv_VDF_base, public Op_Conv_VDF<Op_Conv_Quick_VDF_Elem>
{
  Declare_instanciable_sans_constructeur(Op_Conv_Quick_VDF_Elem);
public:
  Op_Conv_Quick_VDF_Elem();
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { modifier_pour_Cl_elem(matrice,secmem); }
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd,const Champ_Inc& ch) override { associer_impl<Type_Operateur::Op_CONV_ELEM,Eval_Quick_VDF_Elem>(zd,zcd,ch); }
  inline void associer_vitesse(const Champ_base& ch_vit) override { associer_vitesse_impl<Eval_Quick_VDF_Elem>(ch_vit); }
  inline Champ_base& vitesse() override { return vitesse_impl<Eval_Quick_VDF_Elem>(); }
  inline const Champ_base& vitesse() const override { return vitesse_impl<Eval_Quick_VDF_Elem>(); }
  inline void dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const override
  {
    const std::string& nom_inco = equation().inconnue().le_nom().getString();
    Matrice_Morse *mat = mats.count(nom_inco) ? mats.at(nom_inco) : NULL, mat2;
    dimensionner_elem(mat2);
    mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
  }

protected:
  // Ce constructeur permet de creer des classes filles (exemple : front_tracking)
  inline Op_Conv_Quick_VDF_Elem(const Iterateur_VDF_base& it) : Op_Conv_VDF_base(it) { }
};

#endif /* Op_Conv_VDF_Elem_leaves_included */
