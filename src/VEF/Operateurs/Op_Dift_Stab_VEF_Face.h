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
// File:        Op_Dift_Stab_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Dift_Stab_VEF_Face_included
#define Op_Dift_Stab_VEF_Face_included

#include <Ref_Champ_Uniforme.h>
#include <Operateur_Div.h>
#include <Op_Dift_VEF_Face.h>

//
// .DESCRIPTION class Op_Dift_Stab_VEF_Face
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_Stab_VEF_Face : public Op_Dift_VEF_Face
{
  Declare_instanciable(Op_Dift_Stab_VEF_Face);

public:

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;

  void ajouter_cas_scalaire(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const;
  void ajouter_cas_multiscalaire(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const;
  void ajouter_cas_vectoriel(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const;

  void ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice) const;
  void ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const;
  void ajouter_contribution_cl(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;

  void completer() override;

protected :

  void calculer_coefficients(const DoubleTab&, DoubleTab&) const;
  void calculer_coefficients_vectoriel_diag(const DoubleTab& nu, const DoubleTab& nu2, DoubleTab& Aij) const;
  /*   void calculer_coefficients_vectoriel_extradiag(const DoubleTab& nu, DoubleTab& Aij) const ; */
  inline double aij_extradiag(const int elem, const int facei, const int facej, const int dim, const int dim2, const double nu_elem) const;

  void calculer_min(const DoubleTab&, int&, DoubleTab&) const ;
  void calculer_max(const DoubleTab&, int&, DoubleTab&) const ;
  double calculer_gradients(int, const DoubleTab&) const ;

  void modifie_pour_Cl(const DoubleTab&, DoubleTab&) const;
  void ajouter_operateur_centre(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;
  void ajouter_operateur_centre_vectoriel(const DoubleTab& Aij_diag, const DoubleTab& nu, const DoubleTab& inconnue, DoubleTab& resu2) const;
  void ajouter_diffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;
  void ajouter_diffusion_vectoriel(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const;
  void ajouter_antidiffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;
  void ajouter_antidiffusion_vectoriel(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const;

  int standard_;
  IntTab is_dirichlet_faces_;



  REF(Champ_Inc) divergence_U;

  int nu_lu_;
  int nut_lu_;
  int nu_transp_lu_;
  int nut_transp_lu_;
  int new_jacobian_;
};

inline double Op_Dift_Stab_VEF_Face::aij_extradiag(const int elem, const int facei, const int facej, const int dim, const int dim2, const double nu_elem) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& face_voisins=zone_VEF.face_voisins();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volumes=zone_VEF.volumes();

  double volume=0.;
  double signei=1.;
  double signej=1.;
  double aij=0.;

  //Debut de l'algo
  assert(dim<equation().inconnue().valeurs().dimension(1));
  assert(dim2<equation().inconnue().valeurs().dimension(1));
  assert(dim<dim2);

  volume=1./volumes(elem);
  volume*=nu_elem;

  if (face_voisins(facei,0)!=elem) signei=-1.;
  if (face_voisins(facej,0)!=elem) signej=-1.;

  aij=face_normales(facei,dim2)*face_normales(facej,dim);
  aij*=signei*signej;
  aij*=volume;

  return aij;
}

inline void Op_Dift_Stab_VEF_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (!new_jacobian_)
    Op_Dift_VEF_Face::contribuer_a_avec(inco,matrice);
  else
    {
      const Champ_base& inconnue = equation().inconnue().valeur();
      const Nature_du_champ nature_champ = inconnue.nature_du_champ();

      if (nature_champ!=multi_scalaire)
        ajouter_contribution(inco, matrice);
      else
        ajouter_contribution_multi_scalaire(inco, matrice);
    }
}

#endif

