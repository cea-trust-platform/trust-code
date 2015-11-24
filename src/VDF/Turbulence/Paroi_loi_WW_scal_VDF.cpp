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
// File:        Paroi_loi_WW_scal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_loi_WW_scal_VDF.h>
#include <Paroi_2couches_VDF.h>
#include <Paroi_std_hyd_VDF.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>

Implemente_instanciable_sans_constructeur(Paroi_loi_WW_scal_VDF,"loi_WW_scalaire_VDF",Paroi_scal_hyd_base_VDF);


//     printOn()
/////

Sortie& Paroi_loi_WW_scal_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_loi_WW_scal_VDF::readOn(Entree& s)
{
  return s ;
}

/////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Paroi_loi_WW_scal_VDF
//
////////////////////////////////////////////////////////////////////////

// Loi analytique avec raccordement des comportements
// asymptotique de la temperature adimensionnee T+
// sous-couche conductrice : T+=Pr y+
// zone logarithmique : T+=2.12*ln(y+)+Beta
double Paroi_loi_WW_scal_VDF::Fthpar(double y_plus,double Pr,double Beta)
{
  static double C_inv = 2.12;
  double Gamma = (0.01*pow(Pr*y_plus,4.))/(1.+5.*pow(Pr,3.)*y_plus);
  double f = Pr*y_plus*exp(-Gamma);

  f += (C_inv*log(1.+y_plus) + Beta)*exp(-1./(Gamma+1e-20));
  return f;
}


int Paroi_loi_WW_scal_VDF::init_lois_paroi()
{
  tab_u_star.resize(la_zone_VDF->nb_faces_bord());
  return Paroi_scal_hyd_base_VDF::init_lois_paroi();
}

int Paroi_loi_WW_scal_VDF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  static double C = 1./2.12;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  DoubleTab& alpha_t = diffusivite_turb.valeurs();
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();

  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
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

  int ndeb,nfin;
  int elem;
  double dist;
  double d_visco;

  // On recupere directement u_star par la loi de paroi. Il ne faut surtout pas
  // utiliser la methode calculer_u_star_avec_cisaillement car c'est faux en QC.
  // En effet,on recupere alors u_star*rho^0.5 et non u_star -> influence sur les
  // flux et l'impression du Nusselt.

  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();
  const DoubleVect& tab_ustar = loi.valeur().tab_u_star();
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  int schmidt = 0;
  if (sub_type(Convection_Diffusion_Concentration,eqn)) schmidt = 1;
  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn).constituant().diffusivite_constituant():le_fluide.diffusivite());

  // Boucle sur les bords:
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // Pour chaque condition limite on regarde son type
      // On applique les lois de paroi thermiques uniquement
      // aux voisinages des parois ou l'on impose la temperature
      // Si l'on est a une paroi adiabatique, le flux a la paroi est connu et nul.
      // Si l'on est a une paroi a flux impose, le flux est connu et il est
      // directement pris a la condition aux limites pour le calcul des flux diffusifs.

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {

          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          //find the associated boundary
          int boundary_index=-1;
          if (zone_VDF.front_VF(n_bord).le_nom() == le_bord.le_nom())
            boundary_index=n_bord;
          assert(boundary_index >= 0);

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);
              if (axi)
                dist = zone_VDF.dist_norm_bord_axi(num_face);
              else
                dist = zone_VDF.dist_norm_bord(num_face);
              if (l_unif)
                d_visco = visco;
              else
                d_visco = tab_visco[elem];

              // ALEX C. 03/12/2002 : Calcul de alpha_t, diffusivite turbulente.
              // La diffusivite turbulente est calculee directement avec nu_t
              // (ou U_tau, c'est pareil) par : alpha_t = nu_t/Pr_t
              // (ou bien avec U_tau : alpha_t = u_tau*C*y)
              // En effet, c'est la valeur qui doit etre donnee dans une couche limite
              // avec nu_t proprement calcule au prealable.

              double u_star = tab_ustar(num_face);
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
              double Pr = d_visco/d_alpha;
              double Beta = pow(3.85*pow(Pr,1./3.)-1.3,2.)+2.12*log(Pr);
              alpha_t(elem) = u_star*dist*C;

              // L'expression de d_equiv ne tient pas compte de alpha_t comme en VEF
              // Cela dit, c'est normale car c'est lors du calcul du flux que la
              // turbulence est prise en compte.
              // Ne pas uniformiser l'ecriture avec le VEF, car on tombe sur des problemes
              // au niveau des parois contacts entre plusieurs problemes (alpha_t pas recuperable !).

              int global_face=num_face;
              int local_face=zone_VDF.front_VF(boundary_index).num_local_face(global_face);
              equivalent_distance_[boundary_index][local_face] = d_alpha*Fthpar(dist*u_star/d_visco,Pr,Beta)/u_star;
            }
        }
    }
  return 1;
}
