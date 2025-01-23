/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Modifier_pour_fluide_dilatable.h>
#include <Modele_turbulence_scal_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Dirichlet_paroi_defilante.h>
#include <Convection_Diffusion_std.h>
#include <Paroi_scal_hyd_base_EF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Paroi_decalee_Robin.h>
#include <Champ_Uniforme.h>
#include <EcrFicPartage.h>
#include <Neumann_paroi.h>
#include <Probleme_base.h>
#include <Domaine_Cl_EF.h>
#include <Fluide_base.h>
#include <Domaine_EF.h>
#include <SFichier.h>

Implemente_base(Paroi_scal_hyd_base_EF, "Paroi_scal_hyd_base_EF", Turbulence_paroi_scal_base);

Sortie& Paroi_scal_hyd_base_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_scal_hyd_base_EF::readOn(Entree& s) { return s; }

void Paroi_scal_hyd_base_EF::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_dis_ = ref_cast(Domaine_VF, domaine_dis);
  le_dom_Cl_dis_ = domaine_Cl_dis;
  // On initialise tout de suite la loi de paroi
  Paroi_scal_hyd_base_EF::init_lois_paroi();
}

DoubleVect& Paroi_scal_hyd_base_EF::equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const
{
  int nb_boundaries = le_dom_dis_->domaine().nb_front_Cl();
  for (int n_bord = 0; n_bord < nb_boundaries; n_bord++)
    {
      const Front_VF& fr_vf = le_dom_dis_->front_VF(n_bord);
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

int Paroi_scal_hyd_base_EF::init_lois_paroi()
{
  int nb_faces_bord_reelles = le_dom_dis_->nb_faces_bord();
  tab_d_reel_.resize(nb_faces_bord_reelles);
  tab_.resize(nb_faces_bord_reelles, nb_fields_);

  // Initialisations de equivalent_distance_, tab_d_reel, positions_Pf, elems_plus
  // On initialise les distances equivalentes avec les distances geometriques
  const IntTab& face_voisins = le_dom_dis_->face_voisins();
  const DoubleVect& volumes_maille = le_dom_dis_->volumes();
  const DoubleVect& surfaces_face = le_dom_dis_->face_surfaces();

  if (axi)
    {
      Cerr << "Attention, rien n'est fait en Axi pour le EF" << finl;
      exit();
    }
  else
    {
      int nb_front = le_dom_dis_->nb_front_Cl();
      equivalent_distance_.dimensionner(nb_front);
      for (int n_bord = 0; n_bord < nb_front; n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_dis_->les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

          int size = le_bord.nb_faces();
          DoubleVect& dist_equiv = equivalent_distance_[n_bord];
          // Note B.M.: on passe ici deux fois: une fois au readOn (par Paroi_scal_hyd_base_EF::associer())
          //  et une fois par Modele_turbulence_scal_base::preparer_calcul())
          // donc tester si pas deja fait:
          if (!dist_equiv.get_md_vector().non_nul())
            le_bord.frontiere().creer_tableau_faces(dist_equiv, RESIZE_OPTIONS::NOCOPY_NOINIT);
          //assert(dist_equiv.get_md_vector() == le_bord.frontiere().md_vector_faces());

          for (int ind_face = 0; ind_face < size; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int elem;
              elem = face_voisins(num_face, 0);
              if (elem == -1)
                elem = face_voisins(num_face, 1);

              double distance = volumes_maille(elem) / surfaces_face(num_face);

              tab_d_reel_[num_face] = distance;

              dist_equiv(ind_face) = distance;
            }

          dist_equiv.echange_espace_virtuel();
        }
    }

  return 1;
}

void Paroi_scal_hyd_base_EF::imprimer_nusselt(Sortie& os) const
{
  const IntTab& face_voisins = le_dom_dis_->face_voisins();
  int ndeb, nfin, elem;
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = eqn.probleme().equation(0);
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don_base& conductivite = le_fluide.conductivite();
  const DoubleTab& temperature = eqn.probleme().equation(1).inconnue().valeurs();

  const DoubleTab& conductivite_turbulente = mon_modele_turb_scal->conductivite_turbulente().valeurs();

  const IntTab& elems = le_dom_dis_->domaine().les_elems();
  int nsom = le_dom_dis_->nb_som_face();
  int nsom_elem = le_dom_dis_->domaine().nb_som_elem();
  ArrOfInt nodes_face(nsom);
  int nb_nodes_free = nsom_elem - nsom;

  EcrFicPartage Nusselt;
  ouvrir_fichier_partage(Nusselt, "Nusselt");

  for (int n_bord = 0; n_bord < le_dom_dis_->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_dis_->les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())) || (sub_type(Paroi_decalee_Robin, la_cl.valeur())))
        {
          const Domaine_Cl_EF& domaine_Cl_EF_th = ref_cast(Domaine_Cl_EF, eqn.probleme().equation(1).domaine_Cl_dis());
          const Cond_lim& la_cl_th = domaine_Cl_EF_th.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

          if (je_suis_maitre())
            {
              Nusselt << finl;
              Nusselt << "Bord " << le_bord.le_nom() << finl;
              if ((sub_type(Neumann_paroi, la_cl_th.valeur())))
                {
                  if (dimension == 2)
                    {
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| T face de bord (K)\t| Tparoi equiv.(K) " << finl;
                      Nusselt
                          << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                          << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt
                          << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
                          << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| T face de bord (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt
                          << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                          << finl;
                    }
                }
              else
                {
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K) " << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------"
                              << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K) " << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------"
                              << finl;
                    }
                }
            }
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              double x = le_dom_dis_->xv(num_face, 0);
              double y = le_dom_dis_->xv(num_face, 1);
              double lambda, lambda_t;
              elem = face_voisins(num_face, 0);
              if (elem == -1)
                elem = face_voisins(num_face, 1);
              if (sub_type(Champ_Uniforme, conductivite))
                lambda = conductivite.valeurs()(0, 0);
              else
                {
                  if (conductivite.nb_comp() == 1)
                    lambda = conductivite.valeurs()(elem);
                  else
                    lambda = conductivite.valeurs()(elem, 0);
                }

              lambda_t = conductivite_turbulente(elem);
              if (dimension == 2)
                Nusselt << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z = le_dom_dis_->xv(num_face, 2);
                  Nusselt << x << "\t| " << y << "\t| " << z;
                }

              // temperature tparoi face CL
              double tparoi = 0.;
              nodes_face = 0;
              for (int jsom = 0; jsom < nsom; jsom++)
                {
                  int num_som = le_dom_dis_->face_sommets(num_face, jsom);
                  nodes_face[jsom] = num_som;
                  tparoi += temperature(num_som) / nsom;
                }

              // on doit calculer Tfluide premiere maille sans prendre en compte Tparoi
              double tfluide = 0.;
              for (int i = 0; i < nsom_elem; i++)
                {
                  int node = elems(elem, i);
                  int IOK = 1;
                  for (int jsom = 0; jsom < nsom; jsom++)
                    if (nodes_face[jsom] == node)
                      IOK = 0;
                  // Le noeud contribue
                  if (IOK)
                    tfluide += temperature(node) / nb_nodes_free;
                }

              double d_equiv = equivalent_distance_[n_bord](num_face - ndeb);

              tab_(num_face, 0) = d_equiv;
              tab_(num_face, 1) = (lambda + lambda_t) / lambda * tab_d_reel_[num_face] / d_equiv;
              tab_(num_face, 2) = (lambda + lambda_t) / d_equiv;
              tab_(num_face, 3) = tfluide;
              tab_(num_face, 4) = tparoi;
              if (sub_type(Neumann_paroi, la_cl_th.valeur()))
                {
                  // dans ce cas on va imprimer Tfluide (moyenne premiere maille), Tface et on Tparoi recalcule avec d_equiv
                  const Neumann_paroi& la_cl_neum = ref_cast(Neumann_paroi, la_cl_th.valeur());
                  double flux = la_cl_neum.flux_impose(num_face - ndeb);
                  double tparoi_equiv = tfluide + flux / (lambda + lambda_t) * d_equiv;
                  tab_(num_face, 5) = tparoi_equiv;
                  for (int i=0; i<nb_fields_; i++)
                    Nusselt << "\t| " << tab_(num_face, i);
                  Nusselt << finl;
                }
              else
                {
                  tab_(num_face, 5) = 0.;
                  // on imprime Tfluide seulement car normalement Tface=Tparoi est connu
                  for (int i=0; i<4; i++)
                    Nusselt << "\t| " << tab_(num_face, i);
                  Nusselt << finl;
                }
            }
          Nusselt.syncfile();
        }
    }
  if (je_suis_maitre())
    Nusselt << finl << finl;
  Nusselt.syncfile();
}
