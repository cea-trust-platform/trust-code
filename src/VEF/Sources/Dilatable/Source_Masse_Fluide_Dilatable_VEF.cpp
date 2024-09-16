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
  assert(sub_type(Fluide_Weakly_Compressible,fluide));
  const DoubleTab& Y = eqn.inconnue()->valeurs(),
                   &rho = fluide.masse_volumique()->valeurs(),
                    &val_flux0 = ch_front_source_->valeurs();

  const Domaine_Cl_dis_base& zclb = domaine_cl_dis_.valeur();
  const Domaine_VF& zvf = ref_cast(Domaine_VF, zclb.domaine_dis());
  const IntTab& face_voisins = zvf.face_voisins();
  DoubleTrav val_flux(zvf.nb_faces(), 1);

  // pour post
  Champ_Don * post_src_ch = fluide.has_source_masse_espece_champ() ? &ref_cast_non_const(Fluide_Dilatable_base, fluide).source_masse_espece() : nullptr;

  // Handle uniform case ... such a pain:
  const int is_uniforme = sub_type(Champ_front_uniforme, ch_front_source_.valeur());
  for (int i = 0; i < zvf.nb_faces(); i++)
    for (int ncomp = 0; ncomp < val_flux0.line_size(); ncomp++)
      val_flux(i, 0) += is_uniforme ? val_flux0(0, ncomp) : val_flux0(i, ncomp);

  for (int n_bord = 0; n_bord < domaine_cl_dis_->nb_cond_lim(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

      if (le_bord.le_nom() == nom_bord_)
        {
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          const IntTab& elem_face = zvf.elem_faces();
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
              int elem = elem1 == -1 ? elem2 : elem1;
              const double surface_elem = zvf.surface(num_face);
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
              for (int j = 0; j < elem_face.line_size(); j++)
                YY += Y(elem_face(elem, j));

              YY /= (elem_face.line_size());
              double srcmass = -(YY * val_flux(num_face - ndeb, 0) * surface_elem) / rho(num_face);
              if (is_expl)
                srcmass /= zvf.volumes_entrelaces(num_face); // on divise par volume (pas de solveur masse dans l'equation ...)
              resu(num_face) += srcmass;

              if (post_src_ch)
                (*post_src_ch)->valeurs()(elem) = srcmass;
            }
        }
    }

  // pour post
  if (post_src_ch)
    (*post_src_ch)->mettre_a_jour(fluide.inco_chaleur()->temps());
}

void Source_Masse_Fluide_Dilatable_VEF::ajouter_projection(const Fluide_Dilatable_base& fluide, DoubleVect& resu) const
{
  assert(sub_type(Fluide_Weakly_Compressible,fluide));
  const Domaine_Cl_dis_base& zclb = domaine_cl_dis_.valeur();
  const Domaine_VEF& zp1b = ref_cast(Domaine_VEF, zclb.domaine_dis());
  const DoubleTab& val_flux0 = ch_front_source_->valeurs();
  DoubleTrav val_flux(zp1b.nb_faces(), 1);

  // pour post
  Champ_Don * post_src_ch = fluide.has_source_masse_projection_champ() ? &ref_cast_non_const(Fluide_Dilatable_base, fluide).source_masse_projection() : nullptr;

  // Handle uniform case ... such a pain:
  const int is_uniforme = sub_type(Champ_front_uniforme, ch_front_source_.valeur());
  for (int i = 0; i < zp1b.nb_faces(); i++)
    for (int ncomp = 0; ncomp < val_flux0.line_size(); ncomp++)
      val_flux(i, 0) += is_uniforme ? val_flux0(0, ncomp) : val_flux0(i, ncomp);

  /*
   * Attention : ici resu est comme la Pression => P0 et P1 ... Pa peut etre
   * Le flux est aux faces
   * Donc : passage aux elems et aux sommets
   */
  DoubleTrav tab_flux_faces = fluide.inco_chaleur()->valeurs(); // pour initialiser avec la bonne taille
  tab_flux_faces = 0.;

  const int nb_elem_tot = zp1b.nb_elem_tot(), nb_som_tot = zp1b.domaine().nb_som_tot(), nb_faces_tot = zp1b.nb_faces_tot();
  const IntTab& face_voisins = zp1b.face_voisins(), &elem_faces = zp1b.elem_faces(), &face_sommets = zp1b.face_sommets();
  const DoubleVect& volumes_entrelaces = zp1b.volumes_entrelaces();

  // remplir tab_flux_faces (seulement au bord !)
  for (int n_bord = 0; n_bord < domaine_cl_dis_->nb_cond_lim(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

      if (le_bord.le_nom() == nom_bord_)
        {
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
              int elem = elem1 == -1 ? elem2 : elem1;
              const double surf = zp1b.surface(num_face);
              tab_flux_faces(num_face) = val_flux(num_face - ndeb, 0) * surf / zp1b.volumes(elem); // TODO multiple elements!! units val_flux(num_face-ndeb,0) *surf [kg.s-1] => gives [kg.m-3.s-1]
            }
        }
    }

  DoubleTrav tab_flux_som(nb_som_tot), volume_int_som(nb_som_tot);

  int nfe = zp1b.domaine().nb_faces_elem(), nsf = zp1b.nb_som_face();
  const Domaine& dom = zp1b.domaine();

  // calcul de la somme des volumes entrelacees autour d'un sommet
  volume_int_som = 0.;
  for (int face = 0; face < nb_faces_tot; face++)
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = dom.get_renum_som_perio(face_sommets(face, som));
        volume_int_som(som_glob) += volumes_entrelaces(face);
      }

  // interpolation du flux aux sommets
  tab_flux_som = 0.;
  double pond = 1. / nsf; // version_originale
  for (int face = 0; face < nb_faces_tot; face++)
    for (int som = 0; som < nsf; som++)
      {
        int som_glob = dom.get_renum_som_perio(face_sommets(face, som));
        pond = volumes_entrelaces(face) / volume_int_som(som_glob);
        tab_flux_som(som_glob) += tab_flux_faces(face) * pond;
      }

  // on passe aux elems
  double fll = 0.;
  int decal = 0;
  int p_has_elem = zp1b.get_alphaE();
  int nb_case = nb_elem_tot * p_has_elem;
  for (int elem = 0; elem < nb_case; elem++)
    {
      fll = 0;
      for (int face = 0; face < nfe; face++)
        fll += tab_flux_faces(elem_faces(elem, face));  // divise par nfe ??? sais pas

      resu(elem) -= fll; // in [kg.m-3.s-1]

      if (post_src_ch)
        (*post_src_ch)->valeurs()(elem) = fll;
    }

  decal += nb_case;
  resu.echange_espace_virtuel();
  int p_has_som = zp1b.get_alphaS();
  nb_case = nb_som_tot * p_has_som;

  for (int som = 0; som < nb_case; som++)
    resu(decal + som) -= (tab_flux_som(som)); // in [kg.m-3.s-1]

  resu.echange_espace_virtuel();

  // pour post
  if (post_src_ch)
    (*post_src_ch)->mettre_a_jour(fluide.inco_chaleur()->temps());
}
