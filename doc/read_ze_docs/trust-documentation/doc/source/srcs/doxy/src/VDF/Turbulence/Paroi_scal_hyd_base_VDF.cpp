/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Modele_turbulence_scal_base.h>
#include <Dirichlet_paroi_defilante.h>
#include <Convection_Diffusion_std.h>
#include <Champ_Fonc_Tabule_P0_VDF.h>
#include <Paroi_scal_hyd_base_VDF.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Dirichlet_paroi_fixe.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Domaine_Cl_VDF.h>
#include <EcrFicPartage.h>
#include <Neumann_paroi.h>
#include <Probleme_base.h>
#include <Fluide_base.h>
#include <Constituant.h>
#include <Debog.h>

Implemente_base(Paroi_scal_hyd_base_VDF, "Paroi_scal_hyd_base_VDF", Turbulence_paroi_scal_base);

Sortie& Paroi_scal_hyd_base_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_scal_hyd_base_VDF::readOn(Entree& s) { return s; }

void Paroi_scal_hyd_base_VDF::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis.valeur());
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis.valeur());
  // On initialise tout de suite la loi de paroi
  Paroi_scal_hyd_base_VDF::init_lois_paroi();
}

int Paroi_scal_hyd_base_VDF::init_lois_paroi()
{
  const Domaine_VDF& zvdf = le_dom_VDF.valeur();

  int nb_front = zvdf.nb_front_Cl();
  equivalent_distance_.dimensionner(nb_front);

  //loop over boundaries (number)
  for (int n_bord = 0; n_bord < nb_front; n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      int n_faces = le_bord.nb_faces();
      equivalent_distance_[n_bord].resize(n_faces);
      //for a given boundary, loop over faces
      for (int ind_face = 0; ind_face < n_faces; ind_face++)
        {
          int num_face = le_bord.num_face(ind_face);
          if (axi)
            equivalent_distance_[n_bord](ind_face) = zvdf.dist_norm_bord_axi(num_face);
          else
            equivalent_distance_[n_bord](ind_face) = zvdf.dist_norm_bord(num_face);
        } //ind_face
    } //n_bord
  return 1;
}

void Paroi_scal_hyd_base_VDF::imprimer_nusselt(Sortie& os) const
{
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const IntTab& face_voisins = domaine_VDF.face_voisins();
  int ndeb, nfin, elem;
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = eqn.probleme().equation(0);
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don& conductivite = le_fluide.conductivite();
  const DoubleTab& temperature = eqn.probleme().equation(1).inconnue().valeurs();

  EcrFicPartage Nusselt;
  ouvrir_fichier_partage(Nusselt, "Nusselt");

  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);

      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Domaine_Cl_VDF& domaine_Cl_VDF_th = ref_cast(Domaine_Cl_VDF, eqn.probleme().equation(1).domaine_Cl_dis().valeur());
          const Cond_lim& la_cl_th = domaine_Cl_VDF_th.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());

          //find the associated boundary
          int boundary_index = -1;
          if (domaine_VDF.front_VF(n_bord).le_nom() == le_bord.le_nom())
            boundary_index = n_bord;
          assert(boundary_index >= 0);

          if ((sub_type(Neumann_paroi, la_cl_th.valeur())))
            {
              const Neumann_paroi& la_cl_neum = ref_cast(Neumann_paroi, la_cl_th.valeur());

              if (je_suis_maitre())
                {
                  Nusselt << finl;
                  Nusselt << "Bord " << le_bord.le_nom() << finl;
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                              << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt
                          << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                          << finl;
                    }
                }
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (int num_face = ndeb; num_face < nfin; num_face++)
                {
                  double x = domaine_VDF.xv(num_face, 0);
                  double y = domaine_VDF.xv(num_face, 1);
                  double dist, lambda;

                  if (axi)
                    dist = domaine_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = domaine_VDF.dist_norm_bord(num_face);
                  elem = face_voisins(num_face, 0);
                  if (elem == -1)
                    elem = face_voisins(num_face, 1);
                  if (sub_type(Champ_Uniforme, conductivite.valeur()))
                    lambda = conductivite(0, 0);
                  else
                    {
                      if (conductivite.nb_comp() == 1)
                        lambda = conductivite(elem);
                      else
                        lambda = conductivite(elem, 0);
                    }

                  if (dimension == 2)
                    Nusselt << x << "\t| " << y;
                  if (dimension == 3)
                    {
                      double z = domaine_VDF.xv(num_face, 2);
                      Nusselt << x << "\t| " << y << "\t| " << z;
                    }

                  double flux = la_cl_neum.flux_impose(num_face - ndeb);
                  int global_face = num_face;
                  int local_face = domaine_VDF.front_VF(boundary_index).num_local_face(global_face);
                  double tparoi = temperature(elem) + flux / lambda * equivalent_distance_[boundary_index](local_face);

                  Nusselt << "\t| " << equivalent_distance_[boundary_index](local_face) << "\t| " << dist / equivalent_distance_[boundary_index](local_face) << "\t| "
                          << lambda / equivalent_distance_[boundary_index](local_face) << "\t| " << temperature(elem) << "\t|" << tparoi << finl;
                }
            }
          else
            {
              if (je_suis_maitre())
                {
                  Nusselt << finl;
                  Nusselt << "Bord " << le_bord.le_nom() << finl;
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)" << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)" << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                              << finl;
                    }
                }

              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (int num_face = ndeb; num_face < nfin; num_face++)
                {
                  double x = domaine_VDF.xv(num_face, 0);
                  double y = domaine_VDF.xv(num_face, 1);
                  double dist, lambda;
                  if (axi)
                    dist = domaine_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = domaine_VDF.dist_norm_bord(num_face);
                  elem = face_voisins(num_face, 0);
                  if (elem == -1)
                    elem = face_voisins(num_face, 1);
                  if (sub_type(Champ_Uniforme, conductivite.valeur()))
                    lambda = conductivite(0, 0);
                  else
                    {
                      if (conductivite.nb_comp() == 1)
                        lambda = conductivite(elem);
                      else
                        lambda = conductivite(elem, 0);
                    }

                  if (dimension == 2)
                    Nusselt << x << "\t| " << y;
                  if (dimension == 3)
                    {
                      double z = domaine_VDF.xv(num_face, 2);
                      Nusselt << x << "\t| " << y << "\t| " << z;
                    }

                  int global_face = num_face;
                  int local_face = domaine_VDF.front_VF(boundary_index).num_local_face(global_face);
                  Nusselt << "\t| " << equivalent_distance_[boundary_index](local_face) << "\t| " << dist / equivalent_distance_[boundary_index](local_face) << "\t| "
                          << lambda / equivalent_distance_[boundary_index](local_face) << "\t| " << temperature(elem) << finl;
                }
            }
          Nusselt.syncfile();
        }
    }
  if (je_suis_maitre())
    Nusselt << finl << finl;
  Nusselt.syncfile();
}

DoubleVect& Paroi_scal_hyd_base_VDF::equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const
{
  int nb_boundaries = le_dom_VDF->domaine().nb_front_Cl();
  for (int n_bord = 0; n_bord < nb_boundaries; n_bord++)
    {
      const Front_VF& fr_vf = le_dom_VDF->front_VF(n_bord);
      int nb_faces = fr_vf.nb_faces();
      if (fr_vf.le_nom() == nom_bord)
        {
          d_eq.resize(fr_vf.nb_faces());
          for (int ind_face = 0; ind_face < nb_faces; ind_face++)
            d_eq(ind_face) = equivalent_distance(n_bord, ind_face);
        }
    }
  return d_eq;
}
