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
// File:        Paroi_TBLE_scal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_TBLE_scal_VDF.h>
#include <ParoiVDF_TBLE.h>
#include <Paroi_2couches_VDF.h>
#include <Paroi_std_hyd_VDF.h>
#include <Eq_ODVM.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Champ_front_calc.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Concentration.h>
#include <Modele_turbulence_scal_base.h>
#include <Constituant.h>
#include <Fluide_Incompressible.h>
#include <EFichier.h>
#include <Mod_turb_hyd_base.h>
#include <Probleme_base.h>
#include <Echange_contact_VDF.h>
#include <Diffu_totale_scal_base.h>
#include <Ref_DoubleVect.h>
#include <time.h>
#include <verif_cast.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur(Paroi_TBLE_scal_VDF,"Paroi_TBLE_scal_VDF",Paroi_std_scal_hyd_VDF);

//     printOn()
/////

Sortie& Paroi_TBLE_scal_VDF::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_TBLE_scal_VDF::readOn(Entree& is)
{

  Motcle mot_lu;
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");

  // Valeurs par defaut
  Paroi_TBLE_QDM_Scal::init_valeurs_defaut();
  alpha_cv = 0; // termes convectifs dans l'equation TBLE (1=oui/0=non)
  // FIN valeurs par defaut

  Motcles les_mots(1);
  {
    les_mots[0]="alpha_cv";
  }
  is >> mot_lu;
  assert(mot_lu==acc_ouverte);
  is >> mot_lu;
  if(mot_lu == acc_ouverte)
    {
      // on passe l'accolade ouvrante
      is >> mot_lu;
    }
  while(mot_lu != acc_fermee)
    {
      int rang=les_mots.search(mot_lu);
      switch(rang)
        {
        case 0  :
          is >> alpha_cv;
          if (alpha_cv==0)
            Cerr << " Les termes convectifs sont negliges dans TBLE ! "  << finl;
          else if (alpha_cv==1)
            Cerr << " Les termes convectifs sont pris en compte dans TBLE ! "  << finl;
          else
            {
              Cerr << "alpha_cv doit valoir 0 ou 1 !" << finl;
              exit();
            }
          break;
        default :
          {
            Paroi_TBLE_QDM_Scal::lire_donnees(mot_lu, is) ;
          }
        }
      is >> mot_lu;
    }

  return is;

}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_TBLE_scal_VDF
//
/////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int Paroi_TBLE_scal_VDF::init_lois_paroi()
{

  // Pour passer a l'echange contact pour imposer la temperature a l'interface.

  int ndeb,nfin;
  int elem;
  double dist; //distance du premier centre de maille a la paroi
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elems = zone_VDF.nb_elem();
  const DoubleVect& volumes = zone_VDF.volumes();

  const Equation_base& eqn_temp = mon_modele_turb_scal->equation();
  const DoubleTab& Temp = eqn_temp.inconnue().valeurs();
  const Equation_base& eqn_hydr  = mon_modele_turb_scal->equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();

  if (!sub_type(ParoiVDF_TBLE,loi.valeur()))
    {
      Cerr << "Une loi de paroi TBLE en thermique doit etre utilisee obligatoirement avec une loi de paroi TBLE sur la QDM " << finl;
      exit();
    }

  Paroi_std_scal_hyd_VDF::init_lois_paroi();
  Paroi_TBLE_QDM_Scal::init_lois_paroi(zone_VDF, la_zone_Cl_VDF.valeur());

  int compteur_faces_paroi = 0;

  corresp.resize(la_zone_VDF->nb_faces_bord());

  SFichier fic_corresp("corresp.dat",ios::app); // impression de la correspondance


  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              corresp[num_face]=compteur_faces_paroi;
              fic_corresp << num_face << " " << corresp[num_face] << finl;
              compteur_faces_paroi++;
            }
        }
    }

  corresp.resize(compteur_faces_paroi); //Redimensionnement de corresp
  compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi

  DoubleTab termes_sources;
  termes_sources.resize(nb_elems);
  eqn_temp.sources().calculer(termes_sources); //les termes sources

  double T0=0.;// temperature de paroi


  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              if ((elem = face_voisins(num_face,0)) == -1)
                elem = face_voisins(num_face,1);

              //Distance a la paroi du 1er centre de maille
              if (axi)
                dist = zone_VDF.dist_norm_bord_axi(num_face);
              else
                dist = zone_VDF.dist_norm_bord(num_face);

              eq_temp[compteur_faces_paroi].set_y0(0.); //ordonnee de la paroi
              eq_temp[compteur_faces_paroi].set_yn(dist); //ordonnee du 1er centre de maille

              assert(nb_comp==1); // il ne doit pas en etre autrement ici !
              eq_temp[compteur_faces_paroi].initialiser(nb_comp, nb_pts,
                                                        fac, epsilon, max_it, nu_t_dyn); //nbre de pts maillage fin

              double ts = termes_sources(elem)/volumes(elem);

              eq_temp[compteur_faces_paroi].set_F(0, ts);
              eq_temp[compteur_faces_paroi].set_u_y0(0,T0);//Temperature parietale
              eq_temp[compteur_faces_paroi].set_u_yn(0,Temp(elem));//Temperature parietale

              if (reprise_ok==0)
                eq_temp[compteur_faces_paroi].set_Uinit_lin(0, Temp(elem), Temp(elem));

              else
                {
                  for (int itble=0; itble<nb_pts+1; itble++)
                    eq_temp[compteur_faces_paroi].set_Uinit(0,itble,valeurs_reprises(compteur_faces_paroi, itble)) ;
                }

              compteur_faces_paroi ++;
            }//Fin boucle sur les faces de bords





        }//Fin de paroi fixe

    }//Fin boucle sur les bords parietaux


  return 1;
}


int Paroi_TBLE_scal_VDF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const int& nb_elems = zone_VDF.nb_elem();
  const IntVect& orientation = zone_VDF.orientation();
  const DoubleVect& volumes = zone_VDF.volumes();
  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr  = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide   = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin            = le_fluide.viscosite_cinematique();
  const double& tps = eqn_temp.schema_temps().temps_courant();
  const double& dt = eqn_temp.schema_temps().pas_de_temps();
  const double& dt_min = eqn_temp.schema_temps().pas_temps_min();

  DoubleTab termes_sources;
  termes_sources.resize(nb_elems);
  eqn_temp.sources().calculer(termes_sources); //les termes sources

  DoubleTab& alpha_t = diffusivite_turb.valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  int l_unif;
  // double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      l_unif = 1;
      //  visco = max(tab_visco(0,0),DMINFLOAT);
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

  int ndeb=0,nfin=0;
  int elem;
  double dist;
  int ori;

  // int schmidt = 0;
  //if (sub_type(Convection_Diffusion_Concentration,eqn_temp)) schmidt = 1;
  //  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn_temp).constituant().diffusivite_constituant():le_fluide.diffusivite());

  const DoubleVect& Temp = eqn_temp.inconnue().valeurs();
  const Zone_Cl_VDF& zone_Cl_VDF_th = ref_cast(Zone_Cl_VDF,eqn_temp.zone_Cl_dis().valeur());

  double T0=0.;

  int compteur_faces_paroi = 0;

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Cond_lim& la_cl_th = zone_Cl_VDF_th.les_conditions_limites(n_bord);
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
              ori = orientation(num_face);
              if ((elem = face_voisins(num_face,0)) == -1)
                elem = face_voisins(num_face,1);






              if(sub_type(Echange_externe_impose,la_cl_th.valeur()))
                {
                  const Echange_externe_impose& la_cl_ech = ref_cast(Echange_externe_impose,la_cl_th.valeur());
                  //OC 02/2006 : Attention, ce n'est pas encore correctement code !!!
                  //double hs = la_cl_ech.h_imp(num_face-ndeb);
                  //const DoubleTab& h_imp = la_cl_ech.h_imp().valeurs();
                  //double hs = h_imp(num_face-ndeb,0);

                  double Ts = la_cl_ech.T_ext(num_face-ndeb);//Temperature de paroi
                  //double hf = lambda_f(0,0)/tab_d_equiv_(num_face);

                  T0 = Ts;

                  //                       Cerr << "Temp("<<elem<<") = " << Temp(elem) << finl;
                  //                       Cerr << "T0 = " << T0 << finl;
                  //                       Cerr << "Ts = " << Ts << finl;
                  //                       Cerr << "hs = " << hs << finl;
                  //                       Cerr << "hf = " << hf << finl;
                  //                       Cerr << "deq = " << tab_d_equiv_(num_face)  << finl;

                  //Cerr << "T0 = " << T0 << finl;
                }
              //############################
              // FLUID-STRUCTURE CALCULATION
              else if(sub_type(Echange_contact_VDF,la_cl_th.valeur()))
                {
                  const Echange_contact_VDF& la_cl_couplee = ref_cast(Echange_contact_VDF,la_cl_th.valeur());
                  const DoubleTab& T_wall = la_cl_couplee.T_wall();
                  //Verif
                  //                       const double& Temp_solid = la_cl_couplee.T_autre_pb(num_face-ndeb);
                  //                       const DoubleTab& h_autre_pb = la_cl_couplee.h_autre_pb();
                  //Verif


                  T0 = T_wall(num_face-ndeb,0);//Temperature de paroi
                }
              else
                {
                  T0=0.; // En principe si on est ici c est que la frontiere est de type flux_imposee, adiabatique... ou autre
                  // et que le d_equiv calculee par la loi de paroi ne sert a rien => on fait donc n'importe quoi sur TBLE !
                }

              //Distance a la paroi du 1er centre de maille
              if (axi)
                dist = zone_VDF.dist_norm_bord_axi(num_face);
              else
                dist = zone_VDF.dist_norm_bord(num_face);

              if (dt<dt_min)
                eq_temp[compteur_faces_paroi].set_dt(1.e12); // Ca ne devrait pas servir ???
              else
                eq_temp[compteur_faces_paroi].set_dt(dt);
              eq_temp[compteur_faces_paroi].set_y0(0.); //ordonnee de la paroi
              eq_temp[compteur_faces_paroi].set_yn(dist); //ordonnee du 1er centre de maille

              double ts = termes_sources(elem)/volumes(elem);

              eq_temp[compteur_faces_paroi].set_u_y0(0,T0);//Temperature parietale
              eq_temp[compteur_faces_paroi].set_u_yn(0,Temp(elem));//Temperature premiere maille

              ////////////////////////////////////
              // Termes convectifs
              ///////////////////////////////////

              if (alpha_cv==1)        //Si terme de convection : on ajoute terme source et convection dans la methode suivante
                calculer_convection(compteur_faces_paroi, elem, ndeb, nfin, ori, ts);
              else //sinon on ajoute just les termes sources ici :
                {
                  for(int i=1 ; i<nb_pts+1 ; i++)
                    eq_temp[compteur_faces_paroi].set_F(0, i, ts);
                }

              if (statio==0)
                eq_temp[compteur_faces_paroi].aller_au_temps(tps);
              else
                eq_temp[compteur_faces_paroi].aller_jusqu_a_convergence(max_it_statio, eps_statio);


              // L'expression de d_equiv ne tient pas compte de alpha_t comme en VEF
              // Cela dit, c'est normale car c'est lors du calcul du flux que la
              // turbulence est prise en compte.
              // Ne pas uniformiser l'ecriture avec le VEF, car on tombe sur des problemes
              // au niveau des parois contacts entre plusieurs problemes (alpha_t pas recuperable !).
              int global_face=num_face;
              int local_face=zone_VDF.front_VF(boundary_index).num_local_face(global_face);
              if(dabs(eq_temp[compteur_faces_paroi].get_Unp1(0,1) - T0)<1e-10)
                equivalent_distance_[boundary_index][local_face] = 1e10;
              else
                equivalent_distance_[boundary_index][local_face] =  (Temp(elem)-T0)*(eq_temp[compteur_faces_paroi].get_y(1)-eq_temp[compteur_faces_paroi].get_y(0))
                                                                    /(eq_temp[compteur_faces_paroi].get_Unp1(0,1) - T0);
              if(equivalent_distance_[boundary_index][local_face]<1e-10)
                equivalent_distance_[boundary_index][local_face]= dist;

              alpha_t(elem)=eq_temp[compteur_faces_paroi].get_nu_t_yn();

              compteur_faces_paroi++;

            }//fin boucle sur les faces de bords
          //        compt++;

        }//Fin de paroi fixe
    }//fin boucle sur les bords

  if(oui_stats==1)
    calculer_stats();

  //  clock1=clock();
  //  double time = (clock1-clock0)*1.e-6;
  //  if(je_suis_maitre()) {  Cerr << " CPU time TBLE scal : " << Process::mp_max(time) << finl; }

  return 1;
}

//***************************************************************
// Sauvegarde des champs moyennes en temps dans le maillage fin *
//***************************************************************

int Paroi_TBLE_scal_VDF::calculer_stats()
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();

  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const double& tps = eqn_temp.inconnue().temps();
  const double& dt = eqn_temp.schema_temps().pas_de_temps();
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();
  ParoiVDF_TBLE& loi_tble_hyd = ref_cast_non_const(ParoiVDF_TBLE,loi.valeur());



  //////////////////////////////////////
  //Moyennes Temporelles
  //////////////////////////////////////
  if(((tps>tps_deb_stats) && (tps<tps_fin_stats)) && (oui_stats!=0))
    {
      if (dimension==2)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              for(int i=0 ; i<nb_pts+1 ; i++)
                {
                  double T=eq_temp[num_faces_post(j)].get_Unp1(0,i);
                  double u=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);
                  calculer_stat(j,i,u,0,0,T,dt);
                }
            }
        }
      else if (dimension==3)
        {
          for(int j=0; j<nb_post_pts; j++)
            {
              int ori=orientation(num_global_faces_post(j));
              if (ori == 0)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double T=eq_temp[num_faces_post(j)].get_Unp1(0,i);
                      double u=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_v(i);
                      double v=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);
                      double w=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(1,i);
                      calculer_stat(j,i,u,v,w,T,dt);
                    }
                }
              else if (ori == 1)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double T=eq_temp[num_faces_post(j)].get_Unp1(0,i);
                      double u=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(1,i);
                      double v=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_v(i);
                      double w=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);
                      calculer_stat(j,i,u,v,w,T,dt);
                    }
                }
              else if (ori == 2)
                {
                  for(int i=0 ; i<nb_pts+1 ; i++)
                    {
                      double T=eq_temp[num_faces_post(j)].get_Unp1(0,i);
                      double u=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(0,i);
                      double v=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_Unp1(1,i);
                      double w=loi_tble_hyd.get_eq_couch_lim(num_faces_post(j)).get_v(i);
                      calculer_stat(j,i,u,v,w,T,dt);
                    }
                }
            }
        }
    }

  return 1;
}



void Paroi_TBLE_scal_VDF::imprimer_nusselt(Sortie& os) const
{
  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const double& tps = eqn_temp.inconnue().temps();

  Paroi_scal_hyd_base_VDF::imprimer_nusselt(os);

  Paroi_TBLE_QDM_Scal::imprimer_sondes(os, tps, equivalent_distance_);

  Paroi_TBLE_QDM_Scal::imprimer_stat(os, tps);
}

int Paroi_TBLE_scal_VDF::sauvegarder(Sortie& os) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  double tps =  mon_modele_turb_scal->equation().inconnue().temps();
  return Paroi_TBLE_QDM_Scal::sauvegarder(os, zone_VDF, la_zone_Cl_VDF.valeur(), tps);
}


int Paroi_TBLE_scal_VDF::reprendre(Entree& is)
{
  if (la_zone_VDF.non_nul()) // test pour ne pas planter dans "avancer_fichier(...)"
    {
      const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
      double tps_reprise = mon_modele_turb_scal->equation().schema_temps().temps_courant();
      return Paroi_TBLE_QDM_Scal::reprendre(is, zone_VDF, la_zone_Cl_VDF.valeur(), tps_reprise);
    }
  return 1;
}



void Paroi_TBLE_scal_VDF::calculer_convection(int compteur_faces_paroi, int elem, int ndeb, int nfin, int ori, double ts)
{
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();
  ParoiVDF_TBLE& loi_tble_hyd = ref_cast_non_const(ParoiVDF_TBLE,loi.valeur());
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntVect& orientation = zone_VDF.orientation();
  int elem_gauche0, elem_gauche1, elem_droite0, elem_droite1;
  int face_gauche0, face_gauche1, face_droite0, face_droite1;

  // 1er couple de faces perpendiculaires a la paroi
  int face1 = elem_faces(elem,(ori+1));
  int face2 = elem_faces(elem,(ori+4)%6);
  // 2e couple de faces perpendiculaires a la paroi
  int face3 = elem_faces(elem,(ori+2));
  int face4 = elem_faces(elem,(ori+5)%6);

  //On cherche les elements voisins de l'element considere

  elem_gauche0 = face_voisins(face1,0);
  elem_droite0 = face_voisins(face2,1);
  if (elem_gauche0 == elem)
    {
      elem_droite0 = face_voisins(face1,1);
      elem_gauche0 = face_voisins(face2,0);
    }

  elem_gauche1 = face_voisins(face3,0);
  elem_droite1 = face_voisins(face4,1);
  if (elem_gauche1 == elem)
    {
      elem_droite1 = face_voisins(face3,1);
      elem_gauche1 = face_voisins(face4,0);
    }

  //On evite les coins interieurs
  if((elem_droite1!=-1)&&(elem_droite0!=-1)&&(elem_gauche1!=-1)&&(elem_gauche0!=-1))
    {
      //On cherche les faces parietales voisines de la face
      face_gauche0 = elem_faces(elem_gauche0, ori);
      if ((face_voisins(face_gauche0,0) != -1)&&(face_voisins(face_gauche0,1) != -1))
        {
          face_gauche0 = elem_faces(elem_gauche0, ori+3);
        }
      face_droite0 = elem_faces(elem_droite0, ori);
      if ((face_voisins(face_droite0,0) != -1)&&(face_voisins(face_droite0,1) != -1))
        {
          face_droite0 = elem_faces(elem_droite0, ori+3);
        }

      face_gauche1 = elem_faces(elem_gauche1, ori);
      if ((face_voisins(face_gauche1,0) != -1)&&(face_voisins(face_gauche1,1) != -1))
        {
          face_gauche1 = elem_faces(elem_gauche1, ori+3);
        }
      face_droite1 = elem_faces(elem_droite1, ori);
      if ((face_voisins(face_droite1,0) != -1)&&(face_voisins(face_droite1,1) != -1))
        {
          face_droite1 = elem_faces(elem_droite1, ori+3);
        }


      //On evite les coins exterieurs (on evite de calculer des gradients sur des faces non parietales)
      if((face_droite1<nfin)&&(face_droite0<nfin)&&(face_gauche1<nfin)&&(face_gauche0<nfin)&&
          (ndeb<=face_droite1)&&(ndeb<=face_droite0)&&(ndeb<=face_gauche1)&&(ndeb<=face_gauche0))
        {

          //              ///////////////////////////////
          //              //Calcul des termes convectifs
          //              ///////////////////////////////
          double d00d0 = 0., d0vdy= 0., d01d1= 0.;

          eq_temp[compteur_faces_paroi].set_F(0, 0, ts - alpha_cv*(d00d0+d01d1+d0vdy));

          for(int i=1 ; i<nb_pts ; i++)
            {
              d00d0 = (loi_tble_hyd.get_eq_couch_lim(corresp[face_gauche0]).get_Unp1(0,i)
                       *0.5*(eq_temp[corresp[face_gauche0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i))
                       -loi_tble_hyd.get_eq_couch_lim(corresp[face_droite0]).get_Unp1(0,i)
                       *0.5*(eq_temp[corresp[face_droite0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i)))
                      /(zone_VDF.dist_elem_period(elem_gauche0,elem,orientation(face1))
                        +zone_VDF.dist_elem_period(elem,elem_droite0,orientation(face1)));

              d01d1 = (loi_tble_hyd.get_eq_couch_lim(corresp[face_gauche0]).get_Unp1(1,i)
                       *0.5*(eq_temp[corresp[face_gauche0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i))
                       -loi_tble_hyd.get_eq_couch_lim(corresp[face_droite0]).get_Unp1(1,i)
                       *0.5*(eq_temp[corresp[face_droite0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i)))
                      /(zone_VDF.dist_elem_period(elem_gauche1,elem,orientation(face3))
                        +zone_VDF.dist_elem_period(elem,elem_droite1,orientation(face3)));

              d0vdy = (eq_temp[compteur_faces_paroi].get_Unp1(0,i+1)*loi_tble_hyd.get_eq_couch_lim(compteur_faces_paroi).get_v(i+1)
                       -eq_temp[compteur_faces_paroi].get_Unp1(0,i-1)*loi_tble_hyd.get_eq_couch_lim(compteur_faces_paroi).get_v(i-1))
                      /(eq_temp[compteur_faces_paroi].get_y(i+1)-eq_temp[compteur_faces_paroi].get_y(i-1));

              eq_temp[compteur_faces_paroi].set_F(0, i, ts - alpha_cv*(d00d0+d01d1+d0vdy));

            }

          int i=nb_pts;

          d00d0 = (loi_tble_hyd.get_eq_couch_lim(corresp[face_gauche0]).get_Unp1(0,i)
                   *0.5*(eq_temp[corresp[face_gauche0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i))
                   -loi_tble_hyd.get_eq_couch_lim(corresp[face_droite0]).get_Unp1(0,i)
                   *0.5*(eq_temp[corresp[face_droite0]].get_Unp1(0,i)-eq_temp[compteur_faces_paroi].get_Unp1(0,i)))
                  /(zone_VDF.dist_elem_period(elem_gauche0,elem,orientation(face1))
                    +zone_VDF.dist_elem_period(elem,elem_droite0,orientation(face1)));

          d01d1 = (loi_tble_hyd.get_eq_couch_lim(corresp[face_gauche0]).get_Unp1(1,i)
                   *0.5*(eq_temp[corresp[face_gauche0]].get_Unp1(0,i)+eq_temp[compteur_faces_paroi].get_Unp1(0,i))
                   -loi_tble_hyd.get_eq_couch_lim(corresp[face_droite0]).get_Unp1(1,i)
                   *0.5*(eq_temp[corresp[face_droite0]].get_Unp1(0,i)-eq_temp[compteur_faces_paroi].get_Unp1(0,i)))
                  /(zone_VDF.dist_elem_period(elem_gauche1,elem,orientation(face3))
                    +zone_VDF.dist_elem_period(elem,elem_droite1,orientation(face3)));

          d0vdy = (eq_temp[compteur_faces_paroi].get_Unp1(0,i)*loi_tble_hyd.get_eq_couch_lim(compteur_faces_paroi).get_v(i)
                   -eq_temp[compteur_faces_paroi].get_Unp1(0,i-1)*loi_tble_hyd.get_eq_couch_lim(compteur_faces_paroi).get_v(i-1))
                  /(eq_temp[compteur_faces_paroi].get_y(i)-eq_temp[compteur_faces_paroi].get_y(i-1));

          eq_temp[compteur_faces_paroi].set_F(0, i, ts - (d00d0+d01d1+d0vdy));

        }//fin if pas coins interieurs
    }//fin if pas coins exterieurs
  else
    {
      for(int i=1 ; i<nb_pts+1 ; i++)
        {
          eq_temp[compteur_faces_paroi].set_F(0, i, ts);
        }
    }
}


const Probleme_base& Paroi_TBLE_scal_VDF::getPbBase() const
{
  const Probleme_base& pb_base  = mon_modele_turb_scal->equation().probleme();
  return pb_base;
}

Paroi_TBLE_QDM& Paroi_TBLE_scal_VDF::getLoiParoiHydraulique()
{
  const Probleme_base& pb_base  = mon_modele_turb_scal->equation().probleme();
  const Equation_base& eqn_hydr = pb_base.equation(0);
  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi_base& loi = mod_turb_hydr.loi_paroi().valeur();

  if (sub_type(ParoiVDF_TBLE,loi))
    {
      return ref_cast_non_const(ParoiVDF_TBLE,loi);
    }
  else
    {
      Cerr << "Pour utiliser des expressions de Nu et Lambda dependant de T dans TBLE, il faut avoir un probleme de thermohydraulique turbulent avec une loi de paroi de type TBLE et TBLE scalaire" << finl;
      exit();
    }
  return getLoiParoiHydraulique();//n'arrive jamais ici
}

MuLambda_TBLE_base& Paroi_TBLE_scal_VDF::getMuLambda()
{
  return getLoiParoiHydraulique().getMuLambda();
}

Eq_couch_lim& Paroi_TBLE_scal_VDF::get_eq_couch_lim(int i)
{
  return getLoiParoiHydraulique().get_eq_couch_lim(i);
}


// Des restes du developpement de Younes :
/*
   Nom nom_fic ="First_TBLE_cell_temp.dat";
   SFichier fc(nom_fic, ios::app);
   fc << tps << " " << eq_temp[corresp[ndeb]].get_Unp1(0,1) << finl;

   //////////////////////////////////////
   // Moyennes Spatiales
   //////////////////////////////////////


   for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
   {
   const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);

   if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
   {
   const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
   ndeb = le_bord.num_premiere_face();
   nfin = ndeb + le_bord.nb_faces();
   for(int i=0 ; i<nb_pts+1 ; i++)
   {
   if (dimension == 3)
   {
   for(int num_face=ndeb; num_face<nfin; num_face++)
   {
   if(face_voisins(num_face,0)==-1)
   {
   Tmean(i)+=eq_temp[corresp[num_face]].get_Unp1(0,i);
   Tmean_2(i)+=eq_temp[corresp[num_face]].get_Unp1(0,i)*eq_temp[corresp[num_face]].get_Unp1(0,i);
   UTmean(i)+=eq_temp[corresp[num_face]].get_Unp1(0,i)*loi_tble_hyd.get_eq_couch_lim(corresp[num_face]).get_Unp1(1,i);
   VTmean(i)+=eq_temp[corresp[num_face]].get_Unp1(0,i)*loi_tble_hyd.get_eq_couch_lim(corresp[num_face]).get_v(i);
   WTmean(i)+=eq_temp[corresp[num_face]].get_Unp1(0,i)*loi_tble_hyd.get_eq_couch_lim(corresp[num_face]).get_Unp1(0,i);
   //Alpha_mean(i)+=eq_temp[corresp[num_face]].get_nut(i);
   }
   }// for num_face
   Tmean(i) = Tmean(i)/nb_faces_bas;
   //Alpha_mean(i) = Alpha_mean(i)/nb_faces_bas;
   UTmean(i) = UTmean(i)/nb_faces_bas;
   VTmean(i) = VTmean(i)/nb_faces_bas;
   WTmean(i) = WTmean(i)/nb_faces_bas;
   Tmean_2(i) = Tmean_2(i)/nb_faces_bas;
   }// if dim3
   }// for i = ...
   for(int i=0 ; i<nb_pts ; i++)
   {
   if (dimension == 3)
   {
   for(int num_face=ndeb; num_face<nfin; num_face++)
   {
   if(face_voisins(num_face,0)==-1)
   {
   Alpha_mean(i)+=eq_temp[corresp[num_face]].get_nut(i);
   }
   }// for num_face
   Alpha_mean(i) = Alpha_mean(i)/nb_faces_bas;
   }// if dim3
   }// for i = ...
   }// boucle bords
   //Cerr << "Tmean_spat_sum(N) = " << Tmean(nb_pts) << " " << nb_faces_bas << " " << tps << finl;
   }
*/
