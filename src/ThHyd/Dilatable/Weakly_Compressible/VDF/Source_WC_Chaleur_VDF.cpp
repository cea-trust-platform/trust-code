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

#include <Convection_Diffusion_Chaleur_WC.h>
#include <Source_WC_Chaleur_VDF.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_WC.h>
#include <Probleme_base.h>
#include <Zone_VF.h>

Implemente_instanciable(Source_WC_Chaleur_VDF,"Source_WC_Chaleur_VDF",Source_WC_Chaleur);

Sortie& Source_WC_Chaleur_VDF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_WC_Chaleur_VDF::readOn(Entree& is) { return is; }

void Source_WC_Chaleur_VDF::associer_zones(const Zone_dis& zone,const Zone_Cl_dis& zcl)
{
  associer_volume_porosite_impl(zone,volumes,porosites);
}

void Source_WC_Chaleur_VDF::compute_interpolate_gradP(DoubleTab& UgradP_elem, const DoubleTab& Ptot) const
{
  /*
   * NOTA BENE :
   * On a discretisé P_EOS avec une directive = temperature qui donne en VDF :
   * Ptot => aux elems
   * la_vitesse => faces car vdf
   * grad_Ptot => faces car vdf
   *
   * Donc u*grad(P_tot) => faces
   * L'equation est une equation de scalaire => aux elems car vdf
   * On a d P_tot / d t = del P / del t + u.grad(P_tot)
   *
   * del P / del t => aux elem
   * u.grad(P_tot) !!! faut l'interpoler aux elems
   */

  const Navier_Stokes_WC& eqHyd = ref_cast(Navier_Stokes_WC,mon_equation->probleme().equation(0));
  const DoubleTab& la_vitesse = eqHyd.vitesse().valeurs();
  DoubleTab grad_Ptot(eqHyd.grad_P().valeurs()); // initialise avec grad(P) car face
  const Convection_Diffusion_Chaleur_WC& eq_chal = ref_cast(Convection_Diffusion_Chaleur_WC,mon_equation.valeur());
  const Operateur_Grad& Op_Grad = eq_chal.operateur_gradient_WC();
  Op_Grad.calculer(Ptot,grad_Ptot); // compute grad(P_tot)

  const Zone_dis_base& zone_dis = mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
  assert (zone_dis.que_suis_je() == "Zone_VDF");

  // grad should be zero at boundary
  correct_grad_boundary(zone,grad_Ptot);

  // We compute u*grad(P_tot) on each face
  DoubleTab UgradP(grad_Ptot); // field on faces
  const int n = la_vitesse.dimension_tot(0);
  assert ( n == zone.nb_faces_tot() && la_vitesse.line_size() == 1);
  assert ( Ptot.dimension_tot(0) == zone.nb_elem_tot() );

  for (int i=0 ; i <n ; i++) UgradP(i,0) = la_vitesse(i,0) * grad_Ptot(i,0);
  face_to_elem(zone,UgradP,UgradP_elem);
}

// On peut utiliser les methodes statics de Discretisation_tools... mais faut creer de Champ_base ...
void Source_WC_Chaleur_VDF::face_to_elem(const Zone_VF& zone, const DoubleTab& UgradP,DoubleTab& UgradP_elem) const
{
  const IntTab& elem_faces = zone.elem_faces();
  const int nb_face_elem = elem_faces.line_size(), nb_elem_tot= zone.nb_elem_tot();
  assert (UgradP_elem.dimension_tot(0) == nb_elem_tot && UgradP.dimension_tot(0) == zone.nb_faces_tot());

  UgradP_elem = 0.;
  for (int ele=0; ele<nb_elem_tot; ele++)
    for (int s=0; s<nb_face_elem; s++) UgradP_elem(ele,0) += UgradP(elem_faces(ele,s),0);

  UgradP_elem *= 0.5;
}

// marche bien mais pas bon pour le vef ... P en vef est partout, grad sur les faces mais pas bon pour nous
void Source_WC_Chaleur_VDF::compute_interpolate_gradP_old(DoubleTab& UgradP_elem, const DoubleTab& Ptot) const
{
  // compute the grad
  const Navier_Stokes_WC& eqHyd = ref_cast(Navier_Stokes_WC,mon_equation->probleme().equation(0));
  const DoubleTab& la_vitesse = eqHyd.vitesse().valeurs();
  DoubleTab grad_Ptot(eqHyd.grad_P().valeurs()); // initialise avec grad(P) car face
  const Operateur_Grad& gradient = eqHyd.operateur_gradient(); // recuperer op grad de NS
  gradient.calculer(Ptot,grad_Ptot); // compute grad(P_tot)

  // XXX : very important : sinon we have values * V !
  const Solveur_Masse& solv_mass = eqHyd.solv_masse();
  solv_mass.appliquer(grad_Ptot);

  /*
   * XXX XXX XXX
   * READ CAREFULLY : We use the grad operator from Navier_Stokes_std
   *    => created for the pressure normally
   * There is a special treatement for Neumann bc (Neumann_sortie_libre)
   * because we use explicitly the prescribed pressure.
   * So if we calculate the grad of Ptot or rho for example, we will use
   * P_ext defined in data file which is wrong !
   *
   * Now we do the following : we assume open bd as a wall => null gradient
   */

  // We use that of NS because we test the CL too (attention mon_equation is Chaleur...)
  const Zone_dis_base& zone_dis = eqHyd.inconnue().zone_dis_base();
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
  const Zone_Cl_dis& zone_cl = eqHyd.zone_Cl_dis();
  assert (zone_dis.que_suis_je() == "Zone_VDF");

  for (int n_bord=0; n_bord<zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n_bord);
      // corrige si Neumann_sortie_libre
      if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          // recuperer face et remplace gradient par 0
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++) grad_Ptot(num_face,0) = 0.;
        }
    }

  // We compute u*grad(P_tot) on each face
  DoubleTab UgradP(grad_Ptot); // field on faces
  const int n = la_vitesse.dimension(0);
  assert ( n == zone.nb_faces() );
  assert ( Ptot.dimension(0) == zone.nb_elem() );
  for (int i=0 ; i <n ; i++) UgradP(i,0) = la_vitesse(i,0) * grad_Ptot(i,0);
  face_to_elem(zone,UgradP,UgradP_elem);
}
