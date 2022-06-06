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


#ifndef Op_Diff_VEF_base_included
#define Op_Diff_VEF_base_included


#include <Operateur_Diff_base.h>
#include <Ref_Champ_Inc_base.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Op_VEF_Face.h>
#include <Zone_VEF.h>
class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc_base;
class Sortie;

//
// .DESCRIPTION class Op_Diff_VEF_base
//
// Classe de base des operateurs de convection VEF

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VEF_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VEF_base : public Operateur_Diff_base, public Op_VEF_Face
{

  Declare_base(Op_Diff_VEF_base);

public:

  int impr(Sortie& os) const override;
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& ) override;
  inline double viscA(int face_i, int face_j, int num_elem, double diffu) const;
  // Anisotrope (tensor)
  // diffusivite considered as DoubleTab (case of scalair) or ArrOfDouble (in the case multi-scalar)
  inline double viscA(int face_i, int face_j, int num_elem, DoubleTab& diffu) const;
  inline double viscA(int face_i, int face_j, int num_elem, ArrOfDouble& diffu_ci_cj_elem) const;

  double calculer_dt_stab() const override;
  void calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const override;
  Motcle get_localisation_pour_post(const Nom& option) const override;
  virtual void remplir_nu(DoubleTab&) const;
  int phi_psi_diffuse(const Equation_base& eq) const;

protected:

  REF(Zone_VEF) la_zone_vef;
  REF(Zone_Cl_VEF) la_zcl_vef;
  REF(Champ_Inc_base) inconnue_;
  //DoubleVect porosite_face;
  mutable DoubleTab nu_;

};

// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_nu apres)
// Fonction utile viscA
// nu <Si, Sj> / |K|
inline double Op_Diff_VEF_base::viscA(int i, int j, int num_elem, double diffu) const
{
  const Zone_VEF& zone=la_zone_vef.valeur();
  const IntTab& face_voisins=zone.face_voisins();
  const DoubleTab& face_normales=zone.face_normales();
  const DoubleVect& inverse_volumes=zone.inverse_volumes();
  double pscal = face_normales(i,0)*face_normales(j,0)
                 + face_normales(i,1)*face_normales(j,1);
  if (Objet_U::dimension == 3)
    pscal += face_normales(i,2)*face_normales(j,2);

  // *zone.porosite_elem(num_elem));
  if ( (face_voisins(i,0) == face_voisins(j,0)) ||
       (face_voisins(i,1) == face_voisins(j,1)) )
    return -(pscal*diffu)*inverse_volumes(num_elem);
  else
    return (pscal*diffu)*inverse_volumes(num_elem);
}

// dinhvan 20/08/18 reproduction de la formule par proposition de Piere
// case of scalair
inline double Op_Diff_VEF_base::viscA(int i, int j, int num_elem, DoubleTab& diffu) const
{
  const Zone_VEF& zone=la_zone_vef.valeur();
  const IntTab& face_voisins=zone.face_voisins();
  const DoubleTab& face_normales=zone.face_normales();
  const DoubleVect& inverse_volumes=zone.inverse_volumes();

  double DSiSj=0;
  for (int k=0; k<dimension; k++)
    for (int l=0; l<dimension; l++)
      DSiSj += diffu(num_elem,k*dimension+l)*face_normales(i,k)*face_normales(j,l); // steph

  if ( (face_voisins(i,0) == face_voisins(j,0)) ||
       (face_voisins(i,1) == face_voisins(j,1)) )
    return -DSiSj*inverse_volumes(num_elem);
  else
    return DSiSj*inverse_volumes(num_elem);
}

// dinhvan 14 sept 18: tableau diffu est vu comme bidimensionnel mais pas 1D, ni 3D,
// si scalaire de type [[value]], si tenseur de type [nb_elem x nb_comp]
// case of multi-scalar
inline double Op_Diff_VEF_base::viscA(int i, int j, int num_elem, ArrOfDouble& diffu_ci_cj_elem) const
{
  const Zone_VEF& zone=la_zone_vef.valeur();
  const IntTab& face_voisins=zone.face_voisins();
  const DoubleTab& face_normales=zone.face_normales();
  const DoubleVect& inverse_volumes=zone.inverse_volumes();

  double DSiSj=0;
  for (int k=0; k<dimension; k++)
    for (int l=0; l<dimension; l++)
      DSiSj += diffu_ci_cj_elem[k*dimension+l]*face_normales(i,k)*face_normales(j,l);

  if ( (face_voisins(i,0) == face_voisins(j,0)) ||
       (face_voisins(i,1) == face_voisins(j,1)) )
    return -DSiSj*inverse_volumes(num_elem);
  else
    return DSiSj*inverse_volumes(num_elem);
}

#endif
