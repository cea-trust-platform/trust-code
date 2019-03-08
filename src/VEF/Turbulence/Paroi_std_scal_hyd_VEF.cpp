/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Paroi_std_scal_hyd_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_std_scal_hyd_VEF.h>
#include <Paroi_std_hyd_VEF.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Tabule_P0_VEF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>
#include <SFichier.h>
#include <Param.h>
#include <Paroi_decalee_Robin.h>


Implemente_instanciable_sans_constructeur(Paroi_std_scal_hyd_VEF,"loi_standard_hydr_scalaire_VEF",Paroi_scal_hyd_base_VEF);
Implemente_instanciable(Loi_expert_scalaire_VEF,"Loi_expert_scalaire_VEF",Paroi_std_scal_hyd_VEF);

//     printOn()
/////

Sortie& Paroi_std_scal_hyd_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_std_scal_hyd_VEF::readOn(Entree& s)
{
  return s ;
}

//     printOn()
/////

Sortie& Loi_expert_scalaire_VEF::printOn(Sortie& s) const
{
  return s;
}

//// readOn
//

Entree& Loi_expert_scalaire_VEF::readOn(Entree& s)
{
  Param param(que_suis_je());
  param.ajouter("prdt_sur_kappa",&Prdt_sur_kappa_);
  param.ajouter("calcul_ldp_en_flux_impose",&calcul_ldp_en_flux_impose_);
  param.ajouter_condition("(value_of_calcul_ldp_en_flux_impose_eq_0)_or_(value_of_calcul_ldp_en_flux_impose_eq_1)","calcul_ldp_en_flux_impose must be 0 or 1");
  param.lire_avec_accolades_depuis(s);
  return s ;
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_std_hyd_VEF
//
/////////////////////////////////////////////////////////////////////

int Paroi_std_scal_hyd_VEF::init_lois_paroi()
{
  return (Paroi_scal_hyd_base_VEF::init_lois_paroi() & init_lois_paroi_scalaire());
}

int Paroi_std_scal_hyd_VEF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  DoubleTab& alpha_t = diffusivite_turb.valeurs();
  Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const DoubleVect& volumes_maille = zone_VEF.volumes();
  const DoubleVect& surfaces_face = zone_VEF.face_surfaces();
  int l_unif;

  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      l_unif = 1;
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  else
    l_unif = 0;

  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //    tab_visco+=DMINFLOAT;

  double dist=-1;
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = le_modele.loi_paroi();
  const DoubleVect& tab_u_star = loi.valeur().tab_u_star();
  const Equation_base& eqn = mon_modele_turb_scal->equation();
  // Recuperation de la diffusivite en fonction du type d'equation:
  int schmidt = (sub_type(Convection_Diffusion_Concentration,eqn) ? 1 : 0);
  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn).constituant().diffusivite_constituant():le_fluide.diffusivite());
  int alpha_uniforme = (sub_type(Champ_Uniforme,alpha.valeur()) ? 1 : 0);

  // Verifications (l'algorithme n'est valable que si d_alpha est le meme pour chaque constituant)
  if (schmidt)
    {
      if (alpha_uniforme)
        {
          double d_alpha = alpha(0,0);
          assert(ref_cast(Convection_Diffusion_Concentration,eqn).constituant().nb_constituants()==alpha.valeurs().dimension(1));
          for (int nc=0; nc<alpha.valeurs().dimension(1); nc++)
            {
              if (d_alpha!=alpha(0,nc))
                {
                  Cerr << "Error!" << finl;
                  Cerr << "Law of the wall are not implemented yet for constituants with different diffusion coefficients." << finl;
                  Process::exit();
                }
            }
        }
      else
        {
          for (int elem=0; elem<alpha.valeurs().dimension(0); elem++)
            {
              double d_alpha = alpha(elem,0);
              for (int nc=0; nc<alpha.valeurs().dimension(1); nc++)
                {
                  if (d_alpha!=alpha(elem,nc))
                    {
                      Cerr << "Error!" << finl;
                      Cerr << "Law of the wall are not implemented yet for constituants with different diffusion coefficients." << finl;
                      Process::exit();
                    }
                }
            }
        }
    }

  // Boucle sur les bords:
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()))
           || (sub_type(Symetrie,la_cl.valeur()))
           || (sub_type(Paroi_decalee_Robin,la_cl.valeur())) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces();
          DoubleVect& dist_equiv = equivalent_distance_[n_bord];

          for (int ind_face=0; ind_face<size; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              // We search the element touching the wall on the face "num_face".
              int elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);

              // Calcul de la distance normale de la premiere maille
              if (axi)
                {
                  Cerr<<"Error: the axisymmetric VEF case is not yet implemented"<<finl;
                  Cerr<<"in the scalar wall-function."<<finl;
                  exit();
                }
              else if (sub_type(Paroi_decalee_Robin,la_cl.valeur()))
                {
                  const Paroi_decalee_Robin& Paroi = ref_cast(Paroi_decalee_Robin,la_cl.valeur());
                  double delta = Paroi.get_delta();
                  dist = delta;
                }
              else
                {
                  // We calculate the distance to the wall of the center of gravity of the element.
                  // Expression de dist en fonction  du volume de l element et de l aire de la face
                  dist = volumes_maille(elem)/surfaces_face(num_face);
                }

              // Alex. C. : 11/04/2003
              double u_star = tab_u_star(num_face);
              double d_alpha = (alpha_uniforme ? alpha(0,0) : (alpha.valeurs().nb_dim()==1 ? alpha(elem) : alpha(elem,0)) );
              if (u_star == 0 || d_alpha==0)
                {
                  dist_equiv[ind_face] = dist;
                }
              else
                {
                  // calcul de la viscosite en y+
                  double d_visco = (l_unif ? visco : (tab_visco.nb_dim()==1 ? tab_visco(elem) : tab_visco(elem,0)));
                  double Pr = d_visco/d_alpha;
                  double y_plus = dist*u_star/d_visco;
                  dist_equiv[ind_face] = (d_alpha + alpha_t(elem)) * T_plus(y_plus,Pr) / u_star;
                }
            }
          dist_equiv.echange_espace_virtuel();
        }
    }
  return 1;
}

int Paroi_std_scal_hyd_VEF::init_lois_paroi_scalaire()
{
  return 1;
}

