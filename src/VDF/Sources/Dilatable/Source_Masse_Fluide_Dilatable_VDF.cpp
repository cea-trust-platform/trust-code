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
#include <Source_Masse_Fluide_Dilatable_VDF.h>
#include <Fluide_Weakly_Compressible.h>
#include <Champ_front_uniforme.h>
#include <TRUSTTrav.h>
#include <Domaine_VF.h>

Implemente_instanciable(Source_Masse_Fluide_Dilatable_VDF,"Source_Masse_Fluide_Dilatable_VDF",Source_Masse_Fluide_Dilatable_base);

Sortie& Source_Masse_Fluide_Dilatable_VDF::printOn(Sortie& os) const { return os; }
Entree& Source_Masse_Fluide_Dilatable_VDF::readOn(Entree& is) { return Source_Masse_Fluide_Dilatable_base::readOn(is); }
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
 *
 *   VDF :     ////////////////////////
 *            --------------------------
 *              |       |          |
 *              |   x   |     x    |
 *              |       |          |
 *              --------------------
 *
 * - FOR VDF :
 *
 *    Y, P on cell centers. All is then straight forward ...
 *
 *         S = F * surf  (F is in kg / m2 / s)
 *
 *         For projection we code : F * surf / V , where
 *              surf is the surface of the cell touching the boundary and V is the volume of the cell. This gives well the unit Kg / s / m3 ...
 *
 *         For the species equation we code : - Y * F * surf / (rho * V ), where
 *              Y, rho at cell center, same as before for surf and V... This gives well the unit 1 / s, as d(Y)/dt !
 *
 */
void Source_Masse_Fluide_Dilatable_VDF::ajouter_eq_espece(const Convection_Diffusion_Fluide_Dilatable_base& eqn, const Fluide_Dilatable_base& fluide, const bool is_expl, DoubleVect& resu) const
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
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
              int elem = elem1 == -1 ? elem2 : elem1;
              const double surface_elem = zvf.surface(num_face);
              double srcmass = -(Y(elem) * val_flux(num_face - ndeb, 0) * surface_elem) / rho(elem);
              if (is_expl)
                srcmass /= zvf.volumes(elem); // on divise par volume (pas de solveur masse dans l'equation ...)
              resu(elem) += srcmass;

              if (post_src_ch)
                (*post_src_ch)->valeurs()(elem) = srcmass;
            }
        }
    }

  // pour post
  if (post_src_ch)
    (*post_src_ch)->mettre_a_jour(fluide.inco_chaleur()->temps());
}

void Source_Masse_Fluide_Dilatable_VDF::ajouter_projection(const Fluide_Dilatable_base& fluide, DoubleVect& resu) const
{
  assert(sub_type(Fluide_Weakly_Compressible,fluide));
  const Domaine_Cl_dis_base& zclb = domaine_cl_dis_.valeur();
  const Domaine_VF& zvf = ref_cast(Domaine_VF, zclb.domaine_dis());
  const IntTab& face_voisins = zvf.face_voisins();
  const DoubleTab& val_flux0 = ch_front_source_->valeurs();
  DoubleTrav val_flux(zvf.nb_faces(), 1);

  // pour post
  Champ_Don * post_src_ch = fluide.has_source_masse_projection_champ() ? &ref_cast_non_const(Fluide_Dilatable_base, fluide).source_masse_projection() : nullptr;

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

          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
              int elem = elem1 == -1 ? elem2 : elem1;
              const double surf = zvf.surface(num_face);
              const double source_per_dv = val_flux(num_face - ndeb, 0) * surf / zvf.volumes(elem);  // TODO multiple elements!! units [kg.s-1] / zvf.volumes(elem)
              resu(elem) -= source_per_dv;  // in [kg.m-3.s-1]

              if (post_src_ch)
                (*post_src_ch)->valeurs()(elem) = source_per_dv;
            }
        }
    }

  // pour post
  if (post_src_ch)
    (*post_src_ch)->mettre_a_jour(fluide.inco_chaleur()->temps());
}
