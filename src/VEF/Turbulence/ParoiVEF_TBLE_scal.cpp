/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        ParoiVEF_TBLE_scal.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////


#include <ParoiVEF_TBLE_scal.h>
#include <ParoiVEF_TBLE.h>
#include <Paroi_std_hyd_VEF.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Champ_front_calc.h>
#include <Convection_Diffusion_Temperature.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>
#include <Fluide_Incompressible.h>
#include <EFichier.h>
#include <Mod_turb_hyd_base.h>
#include <Probleme_base.h>
#include <Diffu_totale_scal_base.h>
#include <Ref_DoubleVect.h>
#include <time.h>
#include <verif_cast.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur(ParoiVEF_TBLE_scal,"Paroi_TBLE_scal_VEF",Paroi_std_scal_hyd_VEF);

//     printOn()
/////

Sortie& ParoiVEF_TBLE_scal::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& ParoiVEF_TBLE_scal::readOn(Entree& is)
{

  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");
  Motcle mot_lu;

  // Valeurs par defaut
  Paroi_TBLE_QDM_Scal::init_valeurs_defaut();

  is >> mot_lu;
  if(mot_lu == acc_ouverte)
    {
      // on passe l'accolade ouvrante
      is >> mot_lu;
    }

  while(mot_lu != acc_fermee)
    {
      Paroi_TBLE_QDM_Scal::lire_donnees(mot_lu, is) ;
      is >> mot_lu;
    }


  return is;

}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe ParoiVEF_TBLE_scal
//
/////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int ParoiVEF_TBLE_scal::init_lois_paroi()
{
  if(je_suis_maitre())
    {
      Cerr << "ParoiVEF_TBLE_scal::init_lois_paroi()" << finl;
    }

  // Pour passer a l'echange contact pour imposer la temperature a l'interface.

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();

  const Equation_base& eqn_temp = mon_modele_turb_scal->equation();
  const DoubleTab& Temp = eqn_temp.inconnue().valeurs();
  const Equation_base& eq_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eq_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = le_modele.loi_paroi();

  if (!sub_type(ParoiVEF_TBLE,loi.valeur()))
    {
      Cerr << "Une loi de paroi TBLE en thermique doit etre utilisee obligatoirement avec une loi de paroi TBLE sur la QDM " << finl;
      exit();
    }

  Paroi_std_scal_hyd_VEF::init_lois_paroi();
  Paroi_TBLE_QDM_Scal::init_lois_paroi(zone_VEF, la_zone_Cl_VEF.valeur());

  int compteur_faces_paroi = 0;
  int elem;

  //  DoubleTab termes_sources(Temp);
  //  eqn_temp.sources().calculer(termes_sources); //les termes sources

  double surf2;
  double dist; //distance du centre de la maille a la paroi

  IntVect num(nfac);
  DoubleVect n(dimension),t1(dimension),t2(dimension); // vecteurs orthonomes du repere local associe a la face paroi
  double T0;// temperature de paroi
  double T1;// temperature CL haute
  //  double F;// terme de forcage


  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces();
          //Boucle sur les faces des bords parietaux
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              Eq_couch_lim& equation_temp = eq_temp[compteur_faces_paroi];
              int num_face = le_bord.num_face(ind_face);
              elem = face_voisins(num_face,0);

              surf2=0.;

              for(int i=0; i<dimension; i++)
                {
                  surf2 += face_normale(num_face,i)*face_normale(num_face,i);
                }

              if (dimension == 2)
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,2);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                  n[0] = face_normale(num_face,0)/sqrt(surf2);
                  n[1] = face_normale(num_face,1)/sqrt(surf2);

                  t1[0] = -n[1];
                  t1[1] =  n[0];

                  dist = distance_2D(num_face,elem,zone_VEF)*3./2.;

                  T0 =   Temp(num_face);
                  T1 =  (Temp(num[0])+Temp(num[1]))/2.;

                  //                F = (termes_sources(num[0])/volumes_entrelaces(num[0])
                  //                    +termes_sources(num[1])/volumes_entrelaces(num[1]))/2.;
                }
              else
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);
                  num[2]=elem_faces(elem,2);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,3);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                  else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                  n[0]  = face_normale(num_face,0)/sqrt(surf2);
                  n[1]  = face_normale(num_face,1)/sqrt(surf2);
                  n[2]  = face_normale(num_face,2)/sqrt(surf2);

                  if( (est_egal(n[0],0.) && est_egal(n[1],0.)) )
                    {
                      t1[0] =  0.;
                      t1[1] =  1.;
                      t1[2] =  0.;
                    }
                  else
                    {
                      t1[0] = -n[1];
                      t1[1] =  n[0];
                      t1[2] =  0.;
                    }

                  t2[0] =  n[1]*t1[2] - n[2]*t1[1];
                  t2[1] =  n[2]*t1[0] - n[0]*t1[2];
                  t2[2] =  n[0]*t1[1] - n[1]*t1[0];

                  dist = distance_3D(num_face,elem,zone_VEF)*4./3.;

                  T0 =   Temp(num_face);
                  T1 =  (Temp(num[0])+Temp(num[1])+Temp(num[2]))/3.;

                  //                F = (termes_sources(num[0])/volumes_entrelaces(num[0])
                  //                    +termes_sources(num[1])/volumes_entrelaces(num[1])
                  //                    +termes_sources(num[2])/volumes_entrelaces(num[2]))/3.;
                }

              equation_temp.set_y0(0.); //ordonnee de la paroi
              equation_temp.set_yn(dist); //ordonnee du 1er centre de maille

              equation_temp.initialiser(nb_comp, nb_pts, fac, epsilon, max_it, nu_t_dyn); //nbre de pts maillage fin


              //             F = termes_sources(elem)/volumes_entrelaces(elem);
              //             equation_temp.set_F(0, F);

              equation_temp.set_u_y0(0,T0); // Temperature a la paroi
              equation_temp.set_u_yn(0,T1); // Temperature en yn


              //vitesse sur le maillage fin a l'instant initial


              if (reprise_ok==0)
                equation_temp.set_Uinit_lin(0, T0, T1);
              else
                {
                  for (int itble=0; itble<nb_pts+1; itble++)
                    equation_temp.set_Uinit(0,itble,valeurs_reprises(compteur_faces_paroi, itble)) ;
                }
              compteur_faces_paroi++;
            }//Fin boucle sur num_face
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux



  // We initialize the values of the equivalent distance at the first time step
  //  with the geometrical distance.
  // Boucle sur les bords:
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int size=le_bord.nb_faces_tot();
      for (int ind_face=0; ind_face<size; ind_face++)
        {
          int num_face = le_bord.num_face(ind_face);
          elem = face_voisins(num_face,0);
          if (dimension == 2) equivalent_distance_[n_bord](ind_face) = distance_2D(num_face,elem,zone_VEF);
          else                equivalent_distance_[n_bord](ind_face) = distance_3D(num_face,elem,zone_VEF);
        }
    }

  const double& tps = eqn_temp.schema_temps().temps_courant();
  if(oui_stats==1 && tps>tps_deb_stats)
    {
      Cerr << " Reprise des stats non code dans TBLE_scal : .... a faire a l'occasion !!! " << finl;
      exit();
    }

  if(je_suis_maitre())
    {
      Cerr << "fin init_lois_paroi()" << finl;
    }
  return 1;
}


int ParoiVEF_TBLE_scal::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const int& nfac = zone.nb_faces_elem();
  const int& nb_faces = zone_VEF.nb_faces_tot();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();

  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const DoubleVect& Temp = eqn_temp.inconnue().valeurs();
  int elem;

  int itmax=0,itmax_loc;

  DoubleTab termes_sources;
  termes_sources.resize(nb_faces);
  eqn_temp.sources().calculer(termes_sources); //les termes sources


  double surf2;
  // double dist; //distance du centre de la maille a la paroi

  IntVect num(nfac);
  DoubleVect n(dimension),t1(dimension),t2(dimension); // vecteurs orthonomes du repere local associe a la face paroi

  double T0;// temperature de paroi
  double T1;// temperature CL haute
  double F;// terme de forcage


  const double& tps = eqn_temp.schema_temps().temps_courant();
  const double& dt = eqn_temp.schema_temps().pas_de_temps();
  const double& dt_min = eqn_temp.schema_temps().pas_temps_min();

  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_temp.probleme().equation(0).milieu());
  const Champ_Don& alpha = le_fluide.diffusivite();
  DoubleTab alpha_t = diffusivite_turb.valeurs();

  int compteur_faces_paroi = 0;

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int size=le_bord.nb_faces();

          //Boucle sur les faces des bords parietaux
          for (int ind_face=0; ind_face<size; ind_face++)
            {
              Eq_couch_lim& equation_temp = eq_temp[compteur_faces_paroi];
              int num_face = le_bord.num_face(ind_face);
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);

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

              surf2=0.;

              for(int i=0; i<dimension; i++)
                {
                  surf2 += face_normale(num_face,i)*face_normale(num_face,i);
                }

              if (dimension == 2)
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,2);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                  n[0] = face_normale(num_face,0)/sqrt(surf2);
                  n[1] = face_normale(num_face,1)/sqrt(surf2);

                  t1[0] = -n[1];
                  t1[1] =  n[0];

                  //    dist = distance_2D(num_face,elem,zone_VEF)*3./2.;

                  T0 =   Temp(num_face);
                  T1 =  (Temp(num[0])+Temp(num[1]))/2.;

                  F = (termes_sources(num[0])/volumes_entrelaces(num[0])
                       +termes_sources(num[1])/volumes_entrelaces(num[1]))/2.;
                }
              else
                {
                  num[0]=elem_faces(elem,0);
                  num[1]=elem_faces(elem,1);
                  num[2]=elem_faces(elem,2);

                  if         (num[0]==num_face) num[0]=elem_faces(elem,3);
                  else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                  else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                  n[0]  = face_normale(num_face,0)/sqrt(surf2);
                  n[1]  = face_normale(num_face,1)/sqrt(surf2);
                  n[2]  = face_normale(num_face,2)/sqrt(surf2);

                  if( (est_egal(n[0],0.) && est_egal(n[1],0.)) )
                    {
                      t1[0] =  0.;
                      t1[1] =  1.;
                      t1[2] =  0.;
                    }
                  else
                    {
                      t1[0] = -n[1];
                      t1[1] =  n[0];
                      t1[2] =  0.;
                    }

                  t2[0] =  n[1]*t1[2] - n[2]*t1[1];
                  t2[1] =  n[2]*t1[0] - n[0]*t1[2];
                  t2[2] =  n[0]*t1[1] - n[1]*t1[0];

                  //dist = distance_3D(num_face,elem,zone_VEF)*4./3.;

                  T0 =   Temp(num_face);
                  T1 =  (Temp(num[0])+Temp(num[1])+Temp(num[2]))/3.;

                  F = (termes_sources(num[0])/volumes_entrelaces(num[0])
                       +termes_sources(num[1])/volumes_entrelaces(num[1])
                       +termes_sources(num[2])/volumes_entrelaces(num[2]))/3.;
                }

              if(dt<dt_min) equation_temp.set_dt(1.e12);
              else equation_temp.set_dt(dt);


              equation_temp.set_F(0, F);

              equation_temp.set_u_y0(0,T0); // Temperature a la paroi
              equation_temp.set_u_yn(0,T1); // Temperature en yn


              /////////////////////////////////////////////////////
              // On resout les equations aux limites simplifiees //
              //   pendant un pas de temps du maillage grossier  //
              /////////////////////////////////////////////////////

              if (statio==0)
                equation_temp.aller_au_temps(tps);
              else
                equation_temp.aller_jusqu_a_convergence(max_it_statio, eps_statio);

              itmax_loc = equation_temp.get_it();

              if(itmax_loc>max_it) Cerr << "WARNING : TOO MANY ITERATIONS ARE NEEDED IN THE TBLE MESH" << finl;

              if(itmax_loc>itmax) itmax=itmax_loc;


              ///////////////////////////////////////////////
              /////       Determination du tableau     //////
              /////   distance equivalente a la paroi  //////
              ///////////////////////////////////////////////

              if(dabs(equation_temp.get_Unp1(0,1) - T0)<1e-10)
                equivalent_distance_[n_bord](ind_face) = 1e10;
              else
                equivalent_distance_[n_bord](ind_face) =  (d_alpha+alpha_t(elem))/d_alpha*(T1-T0)*(equation_temp.get_y(1)-equation_temp.get_y(0))
                                                          /(equation_temp.get_Unp1(0,1) - T0);

              if(equivalent_distance_[n_bord](ind_face)<1e-10)
                equivalent_distance_[n_bord](ind_face)= equation_temp.get_y(nb_pts)-equation_temp.get_y(0);

              compteur_faces_paroi++;


            }//Fin boucle sur num_face
        }//Fin if Paroi_fixe
    }//Fin boucle sur les bords parietaux


  SFichier fic("iter_scal.dat",ios::app); // ouverture du fichier iter_scal.dat
  fic << tps << " " << itmax << finl;


  if(oui_stats==1)
    calculer_stats();

  return 1;
}


int ParoiVEF_TBLE_scal::calculer_stats()
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleTab& face_normale = zone_VEF.face_normales();

  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const double& tps = eqn_temp.inconnue().temps();
  const double& dt = eqn_temp.schema_temps().pas_de_temps();
  const Equation_base& eq_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eq_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = le_modele.loi_paroi();
  ParoiVEF_TBLE& loi_tble_hyd = ref_cast_non_const(ParoiVEF_TBLE,loi.valeur());

  int num_face;
  double surf;
  double nx,ny,nz=0.; // vecteur normal a la paroi
  double t1x,t1y,t1z=0.; // vecteur tangentiel_1 a la paroi
  double t2x,t2y,t2z=0.; // vecteur tangentiel_2 a la paroi (n^t1)
  double T;
  double Unp1_t1,Unp1_t2=0.;
  double u,v,w=0.;


  //////////////////////////////////////
  //Moyennes Temporelles
  //////////////////////////////////////
  if(((tps>tps_deb_stats) && (tps<tps_fin_stats)) && (oui_stats!=0))
    {
      if (dimension==2)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              num_face=num_faces_post(j);

              surf=sqrt(face_normale(num_face,0)*face_normale(num_face,0)+
                        face_normale(num_face,1)*face_normale(num_face,1));

              nx = face_normale(num_face,0)/surf;
              ny = face_normale(num_face,1)/surf;

              t1x = -ny;
              t1y =  nx;

              for(int i=0 ; i<nb_pts+1 ; i++)
                {
                  T       = eq_temp[num_face].get_Unp1(0,i);
                  Unp1_t1 = loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);;

                  u = t1x*Unp1_t1;
                  v = t1y*Unp1_t1;

                  calculer_stat(j,i,u,v,0,T,dt);
                }
            }
        }
      else if (dimension==3)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              num_face=num_faces_post(j);

              surf=sqrt(face_normale(num_face,0)*face_normale(num_face,0)+
                        face_normale(num_face,1)*face_normale(num_face,1)+
                        face_normale(num_face,2)*face_normale(num_face,2));

              nx = face_normale(num_face,0)/surf;
              ny = face_normale(num_face,1)/surf;
              nz = face_normale(num_face,2)/surf;

              if( (est_egal(nx,0.) && est_egal(ny,0.)) )
                {
                  t1x =  0.;
                  t1y =  1.;
                  t1z =  0.;
                }
              else
                {
                  t1x = -ny/sqrt(nx*nx+ny*ny);
                  t1y =  nx/sqrt(nx*nx+ny*ny);
                  t1z =  0.;
                }

              t2x =  ny*t1z - nz*t1y;
              t2y =  nz*t1x - nx*t1z;
              t2z =  nx*t1y - ny*t1x;

              for(int i=0 ; i<nb_pts+1 ; i++)
                {

                  T       = eq_temp[num_face].get_Unp1(0,i);
                  Unp1_t1 = loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);;
                  Unp1_t2 = loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(1,i);;

                  u = t1x*Unp1_t1 + t2x*Unp1_t2 ;
                  v = t1y*Unp1_t1 + t2y*Unp1_t2 ;
                  w = t1z*Unp1_t1 + t2z*Unp1_t2 ;

                  calculer_stat(j,i,u,v,w,T,dt);
                }
            }
        }
    }

  return 1;
}

void ParoiVEF_TBLE_scal::imprimer_nusselt(Sortie& os) const
{
  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const double& tps = eqn_temp.inconnue().temps();
  Paroi_scal_hyd_base_VEF::imprimer_nusselt(os);
  Paroi_TBLE_QDM_Scal::imprimer_sondes(os, tps, equivalent_distance_);
  Paroi_TBLE_QDM_Scal::imprimer_stat(os, tps);
}

int ParoiVEF_TBLE_scal::sauvegarder(Sortie& os) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double tps =  mon_modele_turb_scal->equation().inconnue().temps();
  return Paroi_TBLE_QDM_Scal::sauvegarder(os, zone_VEF, la_zone_Cl_VEF.valeur(), tps);
}


int ParoiVEF_TBLE_scal::reprendre(Entree& is)
{
  if (la_zone_VEF.non_nul()) // test pour ne pas planter dans "avancer_fichier(...)"
    {
      const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
      double tps_reprise = mon_modele_turb_scal->equation().schema_temps().temps_courant();
      return Paroi_TBLE_QDM_Scal::reprendre(is, zone_VEF, la_zone_Cl_VEF.valeur(), tps_reprise);
    }
  return 1;
}

const Probleme_base& ParoiVEF_TBLE_scal::getPbBase() const
{
  const Probleme_base& pb_base  = mon_modele_turb_scal->equation().probleme();
  return pb_base;
}

Paroi_TBLE_QDM& ParoiVEF_TBLE_scal::getLoiParoiHydraulique()
{
  const Probleme_base& pb_base  = mon_modele_turb_scal->equation().probleme();
  const Equation_base& eqn_hydr = pb_base.equation(0);
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& le_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi_base& loi = le_modele.loi_paroi().valeur();

  if (sub_type(ParoiVEF_TBLE,loi))
    {
      return ref_cast_non_const(ParoiVEF_TBLE,loi);
    }
  else
    {
      Cerr << "Pour utiliser des expressions de Nu et Lambda dependant de T dans TBLE, il faut avoir un probleme de thermohydraulique turbulent avec une loi de paroi de type TBLE et TBLE scalaire" << finl;
      exit();
    }
  return getLoiParoiHydraulique();//n'arrive jamais ici
}

MuLambda_TBLE_base& ParoiVEF_TBLE_scal::getMuLambda()
{
  return getLoiParoiHydraulique().getMuLambda();
}

Eq_couch_lim& ParoiVEF_TBLE_scal::get_eq_couch_lim(int i)
{
  return getLoiParoiHydraulique().get_eq_couch_lim(i);
}
