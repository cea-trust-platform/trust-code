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
// File:        Op_Diff_VDF_Face_leaves.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_VDF_Face_leaves_included
#define Op_Diff_VDF_Face_leaves_included

#include <Eval_Diff_VDF_Face_leaves.h>
#include <Op_Diff_VDF_Face_Axi_base.h>
#include <Op_Diff_VDF_Face_base.h>
#include <Ref_Champ_Uniforme.h>
#include <Op_Diff_Dift_VDF.h>

/// \cond DO_NOT_DOCUMENT
class Op_Diff_VDF_Face_leaves
{ };
/// \endcond

//////////////// CONST /////////////////

declare_It_VDF_Face(Eval_Diff_VDF_const_Face)
// .DESCRIPTION class Op_Diff_VDF_Face
//  Cette classe represente l'operateur de diffusion associe a une equation de la quantite de mouvement.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite est uniforme
//  L'iterateur associe est de type Iterateur_VDF_Face. L'evaluateur associe est de type Eval_Diff_VDF_const_Face
class Op_Diff_VDF_Face : public Op_Diff_VDF_Face_base, public Op_Diff_Dift_VDF<Op_Diff_VDF_Face>
{
  Declare_instanciable_sans_constructeur(Op_Diff_VDF_Face);
public:
  Op_Diff_VDF_Face();
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd, const Champ_Inc& ch) { associer_impl<Type_Operateur::Op_DIFF_FACE,Eval_Diff_VDF_const_Face>(zd,zcd,ch); }
  inline void associer_diffusivite(const Champ_base& ch) { associer_diffusivite_impl<Eval_Diff_VDF_const_Face>(ch); }
  inline const Champ_base& diffusivite() const { return diffusivite_impl<Eval_Diff_VDF_const_Face>(); }
  inline virtual void mettre_a_jour(double ) { mettre_a_jour_impl<Type_Operateur::Op_DIFF_FACE,Eval_Diff_VDF_const_Face>(); }
};

// .DESCRIPTION class Op_Diff_VDF_Face_Axi
//  Cette classe represente l'operateur de diffusion associe aux equations de quantite de mouvement en coordonnees cylindriques.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite est uniforme
//  Cette classe n'utilise ni iterateur ni evaluateur (il y avait trop de termes supplementaires dus aux coordonnees cylindriques)
class Op_Diff_VDF_Face_Axi : public Op_Diff_VDF_Face_Axi_base
{
  Declare_instanciable(Op_Diff_VDF_Face_Axi);
public:
  inline double nu_(const int i) const { return diffusivite_.valeur()(0,0); }
  inline double nu_mean_2_pts_(const int , const int ) const { return diffusivite_.valeur()(0,0); }
  inline double nu_mean_4_pts_(const int , const int ) const { return diffusivite_.valeur()(0,0); }
  inline void associer_diffusivite(const Champ_base& diffu) { diffusivite_ = ref_cast(Champ_Uniforme, diffu); }
  inline const Champ_base& diffusivite() const { return diffusivite_; }

protected:
  REF(Champ_Uniforme) diffusivite_;
};

//////////////// VAR /////////////////

declare_It_VDF_Face(Eval_Diff_VDF_var_Face)
// .DESCRIPTION class Op_Diff_VDF_var_Face
//  Cette classe represente l'operateur de diffusion associe a une equation de la quantite de mouvement.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite n'est pas uniforme
//  L'iterateur associe est de type Iterateur_VDF_Face. L'evaluateur associe est de type Eval_Diff_VDF_var_Face
class Op_Diff_VDF_var_Face : public Op_Diff_VDF_Face_base, public Op_Diff_Dift_VDF<Op_Diff_VDF_var_Face>
{
  Declare_instanciable_sans_constructeur(Op_Diff_VDF_var_Face);
public:
  Op_Diff_VDF_var_Face();
  inline void associer(const Zone_dis& zd, const Zone_Cl_dis& zcd, const Champ_Inc& ch) { associer_impl<Type_Operateur::Op_DIFF_FACE,Eval_Diff_VDF_var_Face>(zd,zcd,ch); }
  inline void associer_diffusivite(const Champ_base& ch) { associer_diffusivite_impl<Eval_Diff_VDF_var_Face>(ch); }
  inline const Champ_base& diffusivite() const { return diffusivite_impl<Eval_Diff_VDF_var_Face>(); }
  inline virtual void mettre_a_jour(double ) { mettre_a_jour_impl<Type_Operateur::Op_DIFF_FACE,Eval_Diff_VDF_var_Face>(); }
};

// .DESCRIPTION class Op_Diff_VDF_var_Face_Axi
//  Cette classe represente l'operateur de diffusion associe aux equations de quantite de mouvement en coordonnees cylindriques.
//  La discretisation est VDF. Le champ diffuse est un Champ_Face. Le champ de diffusivite n'est pas uniforme
//  Cette classe n'utilise ni iterateur ni evaluateur (il y avait trop de termes supplementaires dus aux coordonnees cylindriques)
class Op_Diff_VDF_var_Face_Axi : public Op_Diff_VDF_Face_Axi_base
{
  Declare_instanciable(Op_Diff_VDF_var_Face_Axi);
public:
  inline double nu_(const int i) const { return diffusivite_->valeurs()(i); }
  inline double nu_mean_2_pts_(const int i, const int j) const { return 0.5*(diffusivite_->valeurs()(i)+diffusivite_->valeurs()(j)); }
  inline double nu_mean_4_pts_(const int , const int ) const;
  inline void associer_diffusivite(const Champ_base& diffu) { diffusivite_ = diffu; }
  inline const Champ_base& diffusivite() const { return diffusivite_; }
protected:
  REF(Champ_base) diffusivite_;
};

inline double Op_Diff_VDF_var_Face_Axi::nu_mean_4_pts_(const int i, const int j) const
{
  double db_diffusivite = 0;
  int element, compteur = 0;

  if ((element=face_voisins(i,0)) != -1)
    {
      db_diffusivite += diffusivite_->valeurs()(element);
      compteur++;
    }
  if ((element=face_voisins(i,1)) != -1)
    {
      db_diffusivite += diffusivite_->valeurs()(element);
      compteur++;
    }
  if ((element=face_voisins(j,0)) != -1)
    {
      db_diffusivite += diffusivite_->valeurs()(element);
      compteur++;
    }
  if ((element=face_voisins(j,1)) != -1)
    {
      db_diffusivite += diffusivite_->valeurs()(element);
      compteur++;
    }
  db_diffusivite /= compteur;
  return db_diffusivite;
}

#endif /* Op_Diff_VDF_Face_leaves_included */
