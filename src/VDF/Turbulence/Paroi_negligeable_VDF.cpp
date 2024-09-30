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

#include <Paroi_negligeable_VDF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Modele_turbulence_hyd_base.h>

#include <Champ_Face_VDF.h>
#include <Champ_Uniforme.h>
#include <Domaine_Cl_VDF.h>
#include <Equation_base.h>
#include <distances_VDF.h>
#include <Fluide_base.h>

Implemente_instanciable_sans_constructeur(Paroi_negligeable_VDF, "negligeable_VDF", Paroi_hyd_base_VDF);

Sortie& Paroi_negligeable_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_negligeable_VDF::readOn(Entree& s) { return s; }

int Paroi_negligeable_VDF::init_lois_paroi()
{
  init_lois_paroi_();
  return 1;
}

int Paroi_negligeable_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  if (sub_type(Fluide_base, eqn_hydr.milieu()))
    {
      int ndeb, nfin, elem, ori, l_unif;
      double norm_tau, u_etoile, norm_v = 0, dist, val0, val1, val2, d_visco = 0, visco = 1.;

      const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
      const IntTab& face_voisins = domaine_VDF.face_voisins();
      const IntVect& orientation = domaine_VDF.orientation();
      const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
      const Champ_Don_base& ch_visco_cin = le_fluide.viscosite_cinematique();
      const DoubleTab& tab_visco = ch_visco_cin.valeurs();
      const DoubleTab& vit = eqn_hydr.inconnue().valeurs();

      if (sub_type(Champ_Uniforme, ch_visco_cin))
        {
          visco = tab_visco(0, 0);
          l_unif = 1;
        }
      else
        l_unif = 0;

      for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int num_face = ndeb; num_face < nfin; num_face++)
                {

                  if (face_voisins(num_face, 0) != -1)
                    elem = face_voisins(num_face, 0);
                  else
                    elem = face_voisins(num_face, 1);

                  if (dimension == 2)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_2D_vit(vit, elem, ori, domaine_VDF, val0);
                    }
                  else if (dimension == 3)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_3D_vit(vit, elem, ori, domaine_VDF, val1, val2);
                    }

                  if (axi)
                    dist = domaine_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = domaine_VDF.dist_norm_bord(num_face);
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  norm_tau = d_visco * norm_v / dist;
                  u_etoile = sqrt(norm_tau);
                  tab_u_star_(num_face) = u_etoile;

                } // loop on faces

            } // Fin paroi fixe

        } // Fin boucle sur les bords

    }
  return 1;
}

int Paroi_negligeable_VDF::calculer_hyd(DoubleTab& tab_nu_t, DoubleTab& tab_k)
{
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  if (sub_type(Fluide_base, eqn_hydr.milieu()))
    {
      int ndeb, nfin, elem, ori, l_unif;
      double norm_tau, u_etoile, norm_v = 0, dist, val0, val1, val2, d_visco = 0, visco = 1.;

      const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
      const IntTab& face_voisins = domaine_VDF.face_voisins();
      const IntVect& orientation = domaine_VDF.orientation();
      const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
      const Champ_Don_base& ch_visco_cin = le_fluide.viscosite_cinematique();
      const DoubleTab& tab_visco = ch_visco_cin.valeurs();
      const DoubleTab& vit = eqn_hydr.inconnue().valeurs();

      if (sub_type(Champ_Uniforme, ch_visco_cin))
        {
          visco = tab_visco(0, 0);
          l_unif = 1;
        }
      else
        l_unif = 0;

      for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int num_face = ndeb; num_face < nfin; num_face++)
                {

                  if (face_voisins(num_face, 0) != -1)
                    elem = face_voisins(num_face, 0);
                  else
                    elem = face_voisins(num_face, 1);

                  if (dimension == 2)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_2D_vit(vit, elem, ori, domaine_VDF, val0);
                    }
                  else if (dimension == 3)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_3D_vit(vit, elem, ori, domaine_VDF, val1, val2);
                    }

                  if (axi)
                    dist = domaine_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = domaine_VDF.dist_norm_bord(num_face);
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  norm_tau = d_visco * norm_v / dist;
                  u_etoile = sqrt(norm_tau);
                  tab_u_star_(num_face) = u_etoile;

                } // loop on faces

            } // Fin paroi fixe

        } // Fin boucle sur les bords

    }
  return 1;
}

int Paroi_negligeable_VDF::calculer_hyd_BiK(DoubleTab& tab_k, DoubleTab& tab_eps)
{
  return calculer_hyd(tab_k); // the value in argument is not used anyway
}

bool Paroi_negligeable_VDF::use_shear() const
{
  return false;
}

int Paroi_negligeable_VDF::calculer_scal(Champ_Fonc_base&)
{
  return 1;
}
