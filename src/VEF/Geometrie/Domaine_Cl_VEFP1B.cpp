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

#include <Domaine_Cl_VEFP1B.h>
#include <Conduction.h>
#include <Navier_Stokes_std.h>
#include <Domaine_VEF_PreP1b.h>
#include <Neumann_sortie_libre.h>
#include <Debog.h>

Implemente_instanciable(Domaine_Cl_VEFP1B,"Domaine_Cl_VEFP1B",Domaine_Cl_VEF);

// printOn et readOn

Sortie& Domaine_Cl_VEFP1B::printOn(Sortie& s ) const
{
  return Domaine_Cl_VEF::printOn(s) ;
}

Entree& Domaine_Cl_VEFP1B::readOn(Entree& is )
{
  return Domaine_Cl_VEF::readOn(is)  ;
}

void Domaine_Cl_VEFP1B::imposer_cond_lim(Champ_Inc& ch,double temps)
{
  Domaine_Cl_VEF::imposer_cond_lim(ch,temps);
  // dans le cas Navier stokes et si la condition est forte en pression aux sommets on impose la valeur aux sommets

  if (sub_type(Navier_Stokes_std,ch.valeur().equation()))
    {
      const Domaine_VEF_PreP1b& domaine_vef=ref_cast(Domaine_VEF_PreP1b,equation().domaine_dis().valeur());
      if ((domaine_vef.get_cl_pression_sommet_faible()==0) && (domaine_vef.get_alphaS()))
        {
          int nps=domaine_vef.numero_premier_sommet();
          DoubleTab& pression=ref_cast(Navier_Stokes_std,ch->equation()).pression().valeurs();
          int nbsom_tot = domaine_vef.nb_som_tot();
          ArrOfDouble surf_loc(nbsom_tot);
          int nb_cond_lims = nb_cond_lim();
          // On boucle une premiere fois pour mettre a zero la pression aux sommets
          for(int i=0; i<nb_cond_lims; i++)
            {
              const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
              if (sub_type(Neumann_sortie_libre,la_cl))
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  const IntTab& faces = domaine_vef.face_sommets();
                  int nbsf = faces.dimension(1);
                  int num2 = le_bord.nb_faces_tot();
                  for (int ind_face=0; ind_face<num2; ind_face++)
                    {
                      int face = le_bord.num_face(ind_face);
                      for(int som=0; som<nbsf; som++)
                        {
                          int som_glob = faces(face,som);
                          pression(nps+som_glob) = 0;
                        }
                    }
                }
            }

          // On boucle une deuxieme fois pour ajouter la contribution de chaque face
          for(int i=0; i<nb_cond_lims; i++)
            {
              const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
              if (sub_type(Neumann_sortie_libre,la_cl))
                {
                  const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  const IntTab& faces = domaine_vef.face_sommets();
                  int nbsf = faces.dimension(1);
                  int num2 = le_bord.nb_faces_tot();
                  for (int ind_face=0; ind_face<num2; ind_face++)
                    {
                      int face = le_bord.num_face(ind_face);
                      double P_imp = la_sortie_libre.flux_impose(ind_face);
                      double face_surf = domaine_vef.face_surfaces(face);
                      for(int som=0; som<nbsf; som++)
                        {
                          int som_glob = faces(face,som);
                          pression(nps+som_glob) += P_imp*face_surf;
                          surf_loc[som_glob] += face_surf;
                        }
                    }
                }
            }
          // On boucle une troisieme fois pour diviser par la surface
          for(int i=0; i<nb_cond_lims; i++)
            {
              const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
              if (sub_type(Neumann_sortie_libre,la_cl))
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  const IntTab& faces = domaine_vef.face_sommets();
                  int nbsf = faces.dimension(1);
                  int num2 = le_bord.nb_faces_tot();
                  for (int ind_face=0; ind_face<num2; ind_face++)
                    {
                      int face = le_bord.num_face(ind_face);
                      for(int som=0; som<nbsf; som++)
                        {
                          int som_glob = faces(face,som);
                          double& surf = surf_loc[som_glob];
                          if (surf>0)
                            {
                              pression(nps+som_glob) /= surf;
                              surf = -1;
                            }
                        }
                    }
                }
            }
          pression.echange_espace_virtuel();
          Debog::verifier("pression dans Domaine_Cl_VEFP1B::imposer_cond_lim",pression);
        }
    }
}

void Domaine_Cl_VEFP1B::remplir_volumes_entrelaces_Cl(const Domaine_VEF& le_dom_VEF)
{
  Cerr << "On passe dans Domaine_Cl_VEFP1B::remplir_volumes_entrelaces" << finl;
  // On rappelle la methode de la classe mere pour etendre les volumes de controles sur les faces non standard.
  // Uniquement en conduction pour l'instant ou en P0 seul
  const Domaine_VEF_PreP1b& z=ref_cast(Domaine_VEF_PreP1b, le_dom_VEF);
  if ((z.get_modif_div_face_dirichlet())||sub_type(Conduction, equation()) || (z.get_alphaE() && !z.get_alphaS() && !z.get_alphaA()))
    return Domaine_Cl_VEF::remplir_volumes_entrelaces_Cl(le_dom_VEF);
  else
    {
      const DoubleVect& volumes_entrelaces = le_dom_VEF.volumes_entrelaces();
      // Initialisation du tableau volumes_entrelaces_Cl
      // a priori les faces non standard ne sont pas touchees par les C.L
      for (int i=0; i<le_dom_VEF.nb_faces_non_std(); i++)
        volumes_entrelaces_Cl(i) = volumes_entrelaces(i);
      Cerr << "Fin creation esp. virtuel volumes_entrelaces_Cl_" << finl;
    }
}
