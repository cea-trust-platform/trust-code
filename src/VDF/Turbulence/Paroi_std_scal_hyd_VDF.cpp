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
// File:        Paroi_std_scal_hyd_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/35
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_std_scal_hyd_VDF.h>
#include <Paroi_2couches_VDF.h>
#include <Paroi_std_hyd_VDF.h>
#include <Champ_Uniforme.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Tabule_P0_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>
#include <Debog.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Paroi_std_scal_hyd_VDF,"loi_standard_hydr_scalaire_VDF",Paroi_scal_hyd_base_VDF);
Implemente_instanciable(Loi_expert_scalaire_VDF,"Loi_expert_scalaire_VDF",Paroi_std_scal_hyd_VDF);

//     printOn()
/////

Sortie& Paroi_std_scal_hyd_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_std_scal_hyd_VDF::readOn(Entree& s)
{
  return s ;
}

//     printOn()
/////

Sortie& Loi_expert_scalaire_VDF::printOn(Sortie& s) const
{
  return s;
}

//// readOn
//

Entree& Loi_expert_scalaire_VDF::readOn(Entree& s)
{
  Param param(que_suis_je());
  param.ajouter("prdt_sur_kappa",&Prdt_sur_kappa_);
  param.lire_avec_accolades_depuis(s);
  return s ;
}


/////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Paroi_std_scal_hyd_VDF
//
////////////////////////////////////////////////////////////////////////

int Paroi_std_scal_hyd_VDF::init_lois_paroi()
{
  return (Paroi_scal_hyd_base_VDF::init_lois_paroi() & init_lois_paroi_scalaire());
}

int  Paroi_std_scal_hyd_VDF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  Process::Journal()<<"steph [Paroi_std_scal_hyd_VDF::calculer_scal] is called "<<finl;
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
    } //    tab_visco+=DMINFLOAT;

  double dist;

  // On recupere directement u_star par la loi de paroi. Il ne faut surtout pas
  // utiliser la methode calculer_u_star_avec_cisaillement car c'est faux en QC.
  // En effet,on recupere alors u_star*rho^0.5 et non u_star -> influence sur les
  // flux et l'impression du Nusselt.

  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();
  const DoubleVect& tab_u_star = loi.valeur().tab_u_star();
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const IntVect& orientation = zone_VDF.orientation();

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
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();

          //find the associated boundary
          int boundary_index=-1;
          if (zone_VDF.front_VF(n_bord).le_nom() == le_bord.le_nom())
            boundary_index=n_bord;
          assert(boundary_index >= 0);

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              int elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);
              if (axi)
                dist = zone_VDF.dist_norm_bord_axi(num_face);
              else
                dist = zone_VDF.dist_norm_bord(num_face);

              double u_star = tab_u_star(num_face);
              double d_alpha = (alpha_uniforme ? alpha(0,0) : (alpha.valeurs().nb_dim()==1 ? alpha(elem) : alpha(elem,0)) );

              int global_face=num_face;
              int local_face=zone_VDF.front_VF(boundary_index).num_local_face(global_face);

              if (u_star == 0 || d_alpha == 0)
                {
                  equivalent_distance_[boundary_index](local_face)=dist;
                  alpha_t(elem) = 0;
                }
              else
                {
                  // calcul de la viscosite en y+
                  double d_visco = (l_unif ? visco : (tab_visco.nb_dim()==1 ? tab_visco(elem) : tab_visco(elem,0)));
                  double Pr = d_visco/d_alpha;
                  double y_plus = dist*u_star/d_visco;
                  // L'expression de d_equiv ne tient pas compte de alpha_t comme en VEF
                  // Cela dit, c'est normale car c'est lors du calcul du flux que la
                  // turbulence est prise en compte.
                  // Ne pas uniformiser l'ecriture avec le VEF, car on tombe sur des problemes
                  // au niveau des parois contacts entre plusieurs problemes (alpha_t pas recuperable !).
                  equivalent_distance_[boundary_index](local_face)=d_alpha * T_plus(y_plus,Pr) / u_star;

                  // Alex. C. : 19/02/2003
                  // We modify the value of the eddy diffusivity in the first off-wall element
                  // to have the value given by the theoretical mixing length model.
                  int ori = orientation(num_face);
                  double y0=0.5*zone_VDF.dim_elem(elem,ori)*u_star/d_visco;
                  if (y0<0.5)
                    alpha_t(elem)=0.; // It means we are in the laminar layer.
                  else
                    {
                      double y0m=y0-0.5;
                      double y0p=y0+0.5;
                      alpha_t(elem)=d_visco/(T_plus(y0p,Pr)-T_plus(y0m,Pr))-d_alpha;
                    }
                }
            }
        }
    }
  return 1;
}

int Paroi_std_scal_hyd_VDF::init_lois_paroi_scalaire()
{
  /*
  // Initialisations de tab_d_equiv
  // On initialise les distances equivalentes avec les distances geometriques
  const Zone_VDF& zvdf = la_zone_VDF.valeur();

  if (axi)
  for (int num_face=0; num_face<zvdf.nb_faces_bord(); num_face++)
  tab_d_equiv_[num_face] = zvdf.dist_norm_bord_axi(num_face);
  else
  for (int num_face=0; num_face<zvdf.nb_faces_bord(); num_face++)
  tab_d_equiv_[num_face] = zvdf.dist_norm_bord(num_face);     */
  return 1;

}
