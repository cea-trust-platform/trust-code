/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_scal_analytique_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#include <Paroi_scal_analytique_VEF.h>
#include <Paroi_std_scal_hyd_VEF.h>
#include <Paroi_std_hyd_VEF.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Fluide_Incompressible.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>

Implemente_instanciable(Paroi_scal_analytique_VEF,"loi_analytique_scalaire_VEF",Paroi_scal_hyd_base_VEF);

// printOn()
Sortie& Paroi_scal_analytique_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn
Entree& Paroi_scal_analytique_VEF::readOn(Entree& s)
{
  return Paroi_scal_hyd_base_VEF::readOn(s);
}

int Paroi_scal_analytique_VEF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  DoubleTab& alpha_t = diffusivite_turb.valeurs();
  Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const DoubleVect& volumes_maille = zone_VEF.volumes();
  DoubleVect& surfaces_face = ref_cast_non_const(Zone_VEF,zone_VEF).face_surfaces();

  int elem;
  double dist=-1;
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = le_modele.loi_paroi();
  const DoubleVect& tab_u_star = loi.valeur().tab_u_star();
  const Equation_base& eqn = mon_modele_turb_scal->equation();

  int schmidt = 0;
  if (sub_type(Convection_Diffusion_Concentration,eqn)) schmidt = 1;
  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn).constituant().diffusivite_constituant():le_fluide.diffusivite());
  // Boucle sur les bords:
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces_tot();
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              // We search the element touching the wall on the face "num_face".
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);

              // Calcul de la distance normale de la premiere maille
              if (axi)
                {
                  Cerr<<"Attention: the axisymmetric VEF case is not yet implemented"<<finl;
                  Cerr<<"in the thermal wall-function. Trio will now stop."<<finl;
                  exit();
                }
              else
                {
                  // distance to the wall of the center of gravity of the element.
                  dist = volumes_maille(elem)/surfaces_face(num_face);
                }

              double u_star = tab_u_star(num_face);
              if (u_star == 0)
                {
                  equivalent_distance_[n_bord](ind_face) = dist;
                }
              else
                {
                  double d_alpha=0.;
                  if (sub_type(Champ_Uniforme,alpha.valeur()))
                    d_alpha = alpha(0,0);
                  else
                    {
                      if (alpha.nb_comp()==1)
                        d_alpha = alpha(elem);
                      else
                        d_alpha = alpha(elem,0);
                    }

                  //Expression de d_eq formulee pour le cas nu_t imposee analytiquement
                  //Version qui suppose que nu_t et lambda_t on ete modifies pour les mailles pres de la paroi

                  double rap = alpha_t(elem)/d_alpha;
                  equivalent_distance_[n_bord](ind_face) = dist*(1.+((1.+rap)*log(1.+rap)-rap)/(rap+1.e-10));
                  /*
                  //Version qui consiste a ne pas modifier nu_t et lambda_t pres de la paroi
                  //rap *= 4./3.
                  //equivalent_distance_[n_bord](ind_face) = dist*(1.+((1.+rap)*log(1.+rap)-rap)/(rap+1.e-10));
                  //equivalent_distance_[n_bord](ind_face) *= (1.+(3./4.)*rap)/(1.+rap);
                  */

                }

            }
        }
    }
  return 1;
}


