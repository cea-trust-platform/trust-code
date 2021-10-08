/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_WC_Chaleur_VEF.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/VEF
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Chaleur_WC.h>
#include <Source_WC_Chaleur_VEF.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_WC.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Zone_VF.h>

Implemente_instanciable(Source_WC_Chaleur_VEF,"Source_WC_Chaleur_VEF",Source_WC_Chaleur);

Sortie& Source_WC_Chaleur_VEF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_WC_Chaleur_VEF::readOn(Entree& is) { return is; }

void Source_WC_Chaleur_VEF::associer_zones(const Zone_dis& zone,const Zone_Cl_dis& )
{
  associer_volume_porosite_impl(zone,volumes,porosites);
}

void Source_WC_Chaleur_VEF::compute_interpolate_gradP(DoubleTab& UgradP_face, const DoubleTab& Ptot) const
{
  /*
   * NOTA BENE :
   * On a discretisé P_EOS avec une directive = temperature qui donne en VEF :
   * Ptot => aux faces
   * la_vitesse => faces car vef
   * grad_Ptot => elem
   *
   * L'equation est une equation de scalaire => aux faces car vef
   * On a d P_tot / d t = del P / del t + u.grad(P_tot)
   *
   * del P / del t => aux faces
   * u.grad(P_tot) !!! faut interpoler grad(P_tot) aux faces
   */

  const Navier_Stokes_WC& eqHyd = ref_cast(Navier_Stokes_WC,mon_equation->probleme().equation(0));
  const DoubleTab& la_vitesse = eqHyd.vitesse().valeurs();

  // On sait que grad_Ptot est un champ elem => on pense à la conductivite (elem en vef !! )
  DoubleTab grad_Ptot;
  const DoubleTab& lambda = eqHyd.milieu().conductivite()->valeurs();
  const int nb = lambda.size_totale(), nbcomp = la_vitesse.line_size();
  assert (nb == lambda.dimension_tot(0) && lambda.line_size() == 1);
  grad_Ptot.resize(nb,nbcomp);

  const Convection_Diffusion_Chaleur_WC& eq_chal = ref_cast(Convection_Diffusion_Chaleur_WC,mon_equation.valeur());
  const Operateur_Grad& Op_Grad = eq_chal.operateur_gradient_WC();
  Op_Grad.calculer(Ptot,grad_Ptot); // compute grad(P_tot)

  const Zone_dis_base& zone_dis = mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
  assert (zone_dis.que_suis_je() == "Zone_VEFPreP1b" || zone_dis.que_suis_je() == "Zone_VEF");

  // grad should be zero at boundary
  correct_grad_boundary(zone,grad_Ptot);

  // We compute u*grad(P_tot) on each face
  DoubleTab grad_Ptot_face(la_vitesse); // champs sur les faces

  // get grad_Ptot on faces
  elem_to_face(zone,grad_Ptot,grad_Ptot_face);

  const int n = la_vitesse.dimension_tot(0);
  assert (UgradP_face.dimension_tot(0) == n && grad_Ptot_face.dimension_tot(0) == n);
  assert (UgradP_face.line_size() == 1 && n == zone.nb_faces_tot());
  for (int i=0 ; i <n ; i++)
    {
      UgradP_face(i,0) = 0.;
      for (int j=0 ; j <nbcomp ; j++) UgradP_face(i,0) += la_vitesse(i,j) * grad_Ptot_face(i,j);
    }
}

// On peut utiliser les methodes statics de Discretisation_tools... mais faut creer de Champ_base ...
void Source_WC_Chaleur_VEF::elem_to_face(const Zone_VF& zone, const DoubleTab& grad_Ptot,DoubleTab& grad_Ptot_face) const
{
  const DoubleVect& vol = zone.volumes();
  const IntTab& elem_faces = zone.elem_faces();
  const int nb_face_elem = elem_faces.line_size(), nb_elem_tot = zone.nb_elem_tot(), nb_comp = grad_Ptot_face.line_size();
  assert (grad_Ptot.dimension_tot(0) == nb_elem_tot && grad_Ptot_face.dimension_tot(0) == zone.nb_faces_tot());
  assert (grad_Ptot.line_size() == nb_comp);

  grad_Ptot_face = 0.;
  for (int ele = 0; ele < nb_elem_tot; ele++)
    for (int s = 0; s < nb_face_elem; s++)
      {
        const int face = elem_faces(ele,s);
        for (int comp = 0; comp < nb_comp; comp++) grad_Ptot_face(face,comp) += grad_Ptot(ele,comp)*vol(ele);
      }

  for (int f=0; f<zone.nb_faces_tot(); f++)
    for (int comp=0; comp<nb_comp; comp++) grad_Ptot_face(f,comp) /= volumes(f)*nb_face_elem;
}
