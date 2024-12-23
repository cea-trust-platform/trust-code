/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Convection_Diffusion_Fluide_Dilatable_base.h>
#include <Source_Masse_Fluide_Dilatable_VEF.h>
#include <Fluide_Weakly_Compressible.h>
#include <Champ_front_uniforme.h>
#include <Domaine_VEF.h>
#include <TRUSTTrav.h>
#include <Domaine.h>
#include <kokkos++.h>

Implemente_instanciable(Source_Masse_Fluide_Dilatable_VEF,"Source_Masse_Fluide_Dilatable_VEF",Source_Masse_Fluide_Dilatable_base);

Sortie& Source_Masse_Fluide_Dilatable_VEF::printOn(Sortie& os) const { return os; }
Entree& Source_Masse_Fluide_Dilatable_VEF::readOn(Entree& is) { return Source_Masse_Fluide_Dilatable_base::readOn(is); }
/*
 * Elie Saikali : Implementation notes
 *
 * - The classical system of binary mixture LMN equations (iso-thermal) is as follows
 *
 *      Mass : d rho / dt + div (rho.u) = 0
 *      Momentum : ....
 *      Species :  d(rho Y)/dt + div( rho*u*Y ) = div( rho*D*grad(Y) ), which using the mass equation
 *                 is written in a non-conservative formulation as : rho d(Y)/dt + rho*u grad(Y) = div( rho*D*grad(Y) )
 *                 and when divided by rho as : d(Y)/dt = div( rho*D*grad(Y) ) / rho - u grad(Y)
 *      EOS : ...
 *
 * - With the mass source term, the Mass equation becomes
 *
 *      Mass : d rho / dt + div (rho.u) = S ( S in Kg / s / m3)
 *
 *      This will induce an additional term in the Species equation that becomes
 *
 *      Species :  rho d(Y)/dt + rho*u grad(Y) = div( rho*D*grad(Y) ) - Y.S
 *                 and when divided by rho as : d(Y)/dt = div( rho*D*grad(Y) ) / rho - u grad(Y) - Y.S / rho
 *
 *      Other equations are not touched ...
 *
 *
 * - Note that the mass equation is never explicitly solved. It is only used in the projection algorithm to correct the velocity using
 *
 *      div (rho.u) = - d rho / dt + S
 *
 *      Note that div (rho.u) is located as the pressure : on cell centers in VEF, cell centers + nodes in VEF
 *
 * - To code this source term, we have 2 situations (at present) : VDF and VEF. The source term is considered as a volumic one
 *   and is imposed on the first cell near the boundary where the mass is removed ...
 *
 *   VEF :    ////////////////////////
 *           --------------------------
 *            \  x /\  x /\  x /
 *             \  /  \  /  \  /
 *              \/    \/    \/
 *              --------------
 *
 * - FOR VEF : PAY ATTENTION
 *
 *   Y on faces, P on cell centers + nodes. We need to apply the term source on the first cell touching the boundary !
 *
 *         S = F * surf  (F is in kg / m2 / s)
 *
 *         For projection we code at the element center : F * surf / V , where
 *              surf and V are the surface of the cell touching the boundary and V is the volume of the tetra/triangle. As in VDF, this gives well the unit Kg / s / m3 ...
 *
 *              After, we interpolate the elem values on the nodes of the tetra/triangle touching the boundary.
 *
 *
 *         For the species equation we code : - Y * F * surf / (rho * V ), where
 *              Y is the value interpolated on the elem, rho at boundary face, and V is teh volume_entrelaces for the bd face cell... This gives well the unit 1 / s, as d(Y)/dt !
 *
 */
void Source_Masse_Fluide_Dilatable_VEF::ajouter_eq_espece(const Convection_Diffusion_Fluide_Dilatable_base& eqn, const Fluide_Dilatable_base& fluide, const bool is_expl, DoubleVect& resu) const
{
  ToDo_Kokkos("critical");
  assert(sub_type(Fluide_Weakly_Compressible,fluide));
  const DoubleTab& Y = eqn.inconnue().valeurs(),
                   &rho = fluide.masse_volumique().valeurs(),
                    &val_flux0 = ch_front_source_->valeurs();

  const Domaine_Cl_dis_base& zclb = domaine_cl_dis_.valeur();
  const Domaine_VF& zvf = ref_cast(Domaine_VF, zclb.domaine_dis());
  const int nb_faces = zvf.nb_faces();
  const IntTab& face_voisins = zvf.face_voisins();
  DoubleTrav val_flux(nb_faces, 1);

  CDoubleTabView view_val_flux0 = val_flux0.view_ro();
  DoubleTabView view_val_flux = val_flux.view_rw();
  const int val_flux0_line_sz = val_flux0.line_size();
  CIntTabView view_face_voisins = face_voisins.view_ro();
  const DoubleVect& face_surfaces = zvf.face_surfaces();
  CDoubleArrView view_face_surfaces = face_surfaces.view_ro();
  CDoubleArrView view_rho = static_cast<const DoubleVect&>(rho).view_ro();

  const DoubleVect& volumes_entrelaces = zvf.volumes_entrelaces();
  CDoubleArrView view_volumes_entrelaces = volumes_entrelaces.view_ro();
  DoubleArrView view_resu = resu.view_wo();

  // pour post
  Champ_Don_base * post_src_ch = fluide.has_source_masse_espece_champ() ? &ref_cast_non_const(Fluide_Dilatable_base, fluide).source_masse_espece() : nullptr;

  bool ok_post_src_ch = post_src_ch ? true:false;
  DoubleArrView view_valeurs;
  if (ok_post_src_ch) view_valeurs = static_cast<DoubleVect&>((*post_src_ch).valeurs()).view_wo();

  // Handle uniform case ... such a pain:
  const int is_uniforme = sub_type(Champ_front_uniforme, ch_front_source_.valeur());
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces, KOKKOS_LAMBDA(const int i_face)
  {
    for (int ncomp = 0; ncomp < val_flux0_line_sz; ncomp++)
      view_val_flux(i_face, 0) += is_uniforme ? view_val_flux0(0, ncomp) : view_val_flux0(i_face, ncomp);
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);


  for (int n_bord = 0; n_bord < domaine_cl_dis_->nb_cond_lim(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

      if (le_bord.le_nom() == nom_bord_)
        {
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          const IntTab& elem_faces = zvf.elem_faces();
          CIntTabView view_elem_faces = elem_faces.view_ro();
          int elem_faces_line_size = elem_faces.line_size();

          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int num_face)
          {
            const int elem1 = view_face_voisins(num_face, 0), elem2 = view_face_voisins(num_face, 1);
            int elem = elem1 == -1 ? elem2 : elem1;
            const double surface_elem = view_face_surfaces(num_face);
            /*
             * NOTA BENE : on cherche un facteur de correction car Y est aux faces
             * Terme source surfacique, on utilise Y face => volume entrelaces
             * Or P aux elems et sommets => on a pas le meme volume
             * On interpole Y aux elem, le facteur = Yelem / Yface
             *
             * Conclusion : pour Y on utilise les valeurs aux elems pas faaces !
             * Attention, on divise par rho(face) car c'est pas dans la formulation de terme source !
             */
            double YY = 0.;
            for (int j = 0; j < elem_faces_line_size; j++)
              YY += Y(view_elem_faces(elem, j));

            YY /= elem_faces_line_size;
            double srcmass = -(YY * view_val_flux(num_face - ndeb, 0) * surface_elem) / view_rho(num_face);
            if (is_expl)
              srcmass /= view_volumes_entrelaces(num_face); // on divise par volume (pas de solveur masse dans l'equation ...)
            view_resu(num_face) += srcmass;

            // DOUBT_HARI: Could give a different result according to order of execution
            if (ok_post_src_ch)
              view_valeurs(elem) = srcmass;
          });
          end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
        }
    }

  // pour post
  if (post_src_ch)
    (*post_src_ch).mettre_a_jour(fluide.inco_chaleur().temps());
}

void Source_Masse_Fluide_Dilatable_VEF::ajouter_projection(const Fluide_Dilatable_base& fluide, DoubleVect& resu) const
{
  assert(sub_type(Fluide_Weakly_Compressible,fluide));
  const Domaine_Cl_dis_base& zclb = domaine_cl_dis_.valeur();
  const Domaine_VEF& zp1b = ref_cast(Domaine_VEF, zclb.domaine_dis());
  const DoubleTab& val_flux0 = ch_front_source_->valeurs();
  // pour post
  Champ_Don_base * post_src_ch = fluide.has_source_masse_projection_champ() ? &ref_cast_non_const(Fluide_Dilatable_base, fluide).source_masse_projection() : nullptr;

  const int nb_faces = zp1b.nb_faces();
  const int val_flux0_line_sz = val_flux0.line_size();
  CDoubleTabView view_val_flux0 = val_flux0.view_ro();
  DoubleTrav val_flux(zp1b.nb_faces(), 1);
  DoubleTabView view_val_flux = val_flux.view_rw();

  // Handle uniform case ... such a pain:
  const int is_uniforme = sub_type(Champ_front_uniforme, ch_front_source_.valeur());
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces, KOKKOS_LAMBDA(const int i_face)
  {
    for (int ncomp = 0; ncomp < val_flux0_line_sz; ncomp++)
      view_val_flux(i_face, 0) += is_uniforme ? view_val_flux0(0, ncomp) : view_val_flux0(i_face, ncomp);
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
  /*
   * Attention : ici resu est comme la Pression => P0 et P1 ... Pa peut etre
   * Le flux est aux faces
   * Donc : passage aux elems et aux sommets
   */
  const DoubleVect& face_surfaces = zp1b.face_surfaces();
  CDoubleArrView view_face_surfaces = face_surfaces.view_ro();
  const DoubleVect& volumes = zp1b.volumes();
  CDoubleArrView view_volumes = volumes.view_ro();
  DoubleTrav tab_flux_faces = fluide.inco_chaleur().valeurs(); // pour initialiser avec la bonne taille
  tab_flux_faces = 0.;
  DoubleArrView view_flux_faces = static_cast<DoubleVect&>(tab_flux_faces).view_rw();

  const int nb_elem_tot = zp1b.nb_elem_tot(), nb_som_tot = zp1b.domaine().nb_som_tot(), nb_faces_tot = zp1b.nb_faces_tot();
  const IntTab& face_voisins = zp1b.face_voisins(), &elem_faces = zp1b.elem_faces(), &face_sommets = zp1b.face_sommets();
  const DoubleVect& volumes_entrelaces = zp1b.volumes_entrelaces();
  CDoubleArrView view_volumes_entrelaces = volumes_entrelaces.view_ro();
  CIntTabView view_face_voisins = face_voisins.view_ro();
  CIntTabView view_face_sommets = face_sommets.view_ro();
  // remplir flux_faces (seulement au bord !)
  for (int n_bord = 0; n_bord < domaine_cl_dis_->nb_cond_lim(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

      if (le_bord.le_nom() == nom_bord_)
        {
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(ndeb, nfin), KOKKOS_LAMBDA (const int num_face)
          {
            const int elem1 = view_face_voisins(num_face, 0), elem2 = view_face_voisins(num_face, 1);
            int elem = elem1 == -1 ? elem2 : elem1;
            const double surf = view_face_surfaces(num_face);
            view_flux_faces(num_face) = view_val_flux(num_face - ndeb, 0) * surf / view_volumes(elem); // TODO multiple elements!! units val_flux(num_face-ndeb,0) *surf [kg.s-1] => gives [kg.m-3.s-1]
          });
          end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
        }
    }

  DoubleTrav tab_flux_som(nb_som_tot), volume_int_som(nb_som_tot);

  const int nfe = zp1b.domaine().nb_faces_elem(), nsf = zp1b.nb_som_face();
  const Domaine& dom = zp1b.domaine();
  CIntArrView view_renum_som_perio = dom.get_renum_som_perio().view_ro();
  // calcul de la somme des volumes entrelacees autour d'un sommet
  volume_int_som = 0.;
  DoubleArrView view_volume_int_som = static_cast<DoubleVect&>(volume_int_som).view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces_tot, KOKKOS_LAMBDA (const int face)
  {
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = view_renum_som_perio(view_face_sommets(face, som));
        Kokkos::atomic_add(&view_volume_int_som(som_glob), view_volumes_entrelaces(face));
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  // interpolation du flux aux sommets
  tab_flux_som = 0.;
  DoubleArrView view_flux_som = static_cast<DoubleVect&>(tab_flux_som).view_rw();
  // double pond = 1. / nsf; // version_originale
  // DOUBT_HARI: This value is never used and treated as const inside the functor

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_faces_tot, KOKKOS_LAMBDA (const int face)
  {
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = view_renum_som_perio(view_face_sommets(face, som));
        double pond = view_volumes_entrelaces(face) / view_volume_int_som(som_glob); // DOUBT_HARI: pond could be
        // declared here.
        Kokkos::atomic_add(&view_flux_som(som_glob), view_flux_faces(face) * pond);
      }
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
  // on passe aux elems
  DoubleArrView view_resu = resu.view_wo();
  CIntTabView view_elem_faces = elem_faces.view_ro();
  bool ok_post_src_ch = post_src_ch ? true:false;
  DoubleArrView view_valeurs;
  if (ok_post_src_ch) view_valeurs = static_cast<DoubleVect&>((*post_src_ch).valeurs()).view_wo();
  int decal = 0;
  int p_has_elem = zp1b.get_alphaE();
  int nb_case = nb_elem_tot * p_has_elem;
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_case, KOKKOS_LAMBDA (const int elem)
  {
    double fll = 0.;
    for (int face = 0; face < nfe; face++)
      fll += view_flux_faces(view_elem_faces(elem, face));  // divise par nfe ??? sais pas

    view_resu(elem) -= fll; // in [kg.m-3.s-1]

    if (ok_post_src_ch) view_valeurs(elem) = fll;
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  decal += nb_case;
  resu.echange_espace_virtuel(); // DOUBT_HARI: How will this affect view_resu?
  int p_has_som = zp1b.get_alphaS();
  nb_case = nb_som_tot * p_has_som;

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_case, KOKKOS_LAMBDA (const int som)
  {
    view_resu(decal + som) -= view_flux_som(som); // in [kg.m-3.s-1]
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  resu.echange_espace_virtuel();

  // pour post
  if (post_src_ch)
    (*post_src_ch).mettre_a_jour(fluide.inco_chaleur().temps());
}
