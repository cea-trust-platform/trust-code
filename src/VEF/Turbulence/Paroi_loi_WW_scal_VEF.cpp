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
// File:        Paroi_loi_WW_scal_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_loi_WW_scal_VEF.h>
#include <Paroi_std_hyd_VEF.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Fluide_Quasi_Compressible.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>

Implemente_instanciable_sans_constructeur(Paroi_loi_WW_scal_VEF,"loi_WW_scalaire_VEF",Paroi_std_scal_hyd_VEF);


//     printOn()
/////

Sortie& Paroi_loi_WW_scal_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_loi_WW_scal_VEF::readOn(Entree& s)
{
  return s ;
}

void Paroi_loi_WW_scal_VEF::associer(const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

/////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Paroi_loi_WW_scal_VEF
//
////////////////////////////////////////////////////////////////////////

// Loi analytique avec raccordement des comportements
// asymptotiques de la temperature adimensionnee T+
// sous-couche conductrice : T+=Pr y+
// zone logarithmique : T+=2.12*ln(y+)+Beta

//// POUR PASSAGE A V1.4.4
//// PROBLEME COMPATIBILITE AVEC DEVELOPPEMENT
//// DE LOI PAROI VEF SCAL DE PATRICK
double FthparVEF_WW(double y_plus,double Pr,double Beta)
{
  static double C_inv = 2.12;
  double Gamma = (0.01*pow(Pr*y_plus,4.))/(1.+5.*pow(Pr,3.)*y_plus);
  double f = Pr*y_plus*exp(-Gamma);
  f += (C_inv*log(1.+y_plus) + Beta)*exp(-1./(Gamma+1e-20));
  return f;
}


int Paroi_loi_WW_scal_VEF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  DoubleTab& alpha_t = diffusivite_turb.valeurs();
  Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();

  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  int l_unif;

  if (axi)
    {
      Cerr<<"Attention: the axisymmetric VEF case is not yet implemented"<<finl;
      Cerr<<"in the thermal wall-function. trust will now stop."<<finl;
      exit();
    }

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
  //tab_visco+=DMINFLOAT;

  int elem;
  double dist;
  double d_visco;
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = le_modele.loi_paroi();
  const DoubleVect& tab_u_star = loi.valeur().tab_u_star();
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();

  int schmidt = 0;
  if (sub_type(Convection_Diffusion_Concentration,eqn)) schmidt = 1;
  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn).constituant().diffusivite_constituant():le_fluide.diffusivite());

  // Boucle sur les bords:
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      // Pour chaque condition limite on regarde son type
      // On applique les lois de paroi thermiques uniquement
      // aux voisinages des parois ou l'on impose la temperature
      // Si l'on est a une paroi adiabatique, le flux a la paroi est connu et nul.
      // Si l'on est a une paroi a flux impose, le flux est connu et il est
      // directement pris a la condition aux limites pour le calcul des flux diffusifs.

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {

          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces_tot();
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              const IntTab& face_voisins = zone_VEF.face_voisins();

              // We search the element touching the wall on the face "num_face".
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);

              // We calculate the distance to the wall of the center of gravity of the element.
              if (dimension == 2)
                dist = distance_2D(num_face,elem,zone_VEF)*1.5;
              else
                dist = distance_3D(num_face,elem,zone_VEF)*4./3.;

              if (l_unif)
                d_visco = visco;
              else
                d_visco = tab_visco[elem];
              double u_star = tab_u_star(num_face);
              double (*pf)(double,double,double);
              pf = &FthparVEF_WW;
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

              // Alex. C. : 28/02/2003
              // We modify the value of the eddy diffusivity in the first off-wall element
              // to have the value given by the theoretical mixing length model.

              double y0m=(dist*u_star/d_visco)-0.5;
              double y0p=(dist*u_star/d_visco)+0.5;
              alpha_t(elem)=d_visco/(pf(y0p,Pr,Beta)-pf(y0m,Pr,Beta))-d_alpha;
              if(alpha_t(elem)<0.) alpha_t(elem)=0.; // It means we are in the laminar layer.
              equivalent_distance_[n_bord](ind_face) = (d_alpha+alpha_t(elem))*pf(dist*u_star/d_visco,Pr,Beta)/u_star;
            }
        }
    }
  return 1;
}
