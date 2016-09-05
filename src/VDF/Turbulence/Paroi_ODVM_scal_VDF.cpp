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
// File:        Paroi_ODVM_scal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_ODVM_scal_VDF.h>
#include <ParoiVDF_TBLE.h>
#include <Paroi_std_hyd_VDF.h>
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
#include <SFichier.h>

Implemente_instanciable_sans_constructeur(Paroi_ODVM_scal_VDF,"loi_ODVM_VDF",Paroi_scal_hyd_base_VDF);

//     printOn()
/////

Sortie& Paroi_ODVM_scal_VDF::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_ODVM_scal_VDF::readOn(Entree& is)
{

  Motcle mot_lu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");

  // Valeurs par defaut
  N= 10;
  facteur = 1;
  gamma        = 1.;
  stats = 0;
  temps_deb_stats = 0.;
  temps_dern_post_inst = -100.;
  compt = 1;
  check = 0;
  // FIN valeurs par defaut

  Motcles les_mots(5);
  {
    les_mots[0]="N";
    les_mots[1]="facteur";
    les_mots[2]="GAMMA";
    les_mots[3]="STATS";
    les_mots[4]="CHECK_FILES";
  }
  Motcle acc_ouverte("{");
  Motcle acc_fermee("}");

  is >> mot_lu;
  assert(mot_lu==accolade_ouverte);
  is >> mot_lu;
  while(mot_lu != acc_fermee)
    {
      int rang=les_mots.search(mot_lu);
      switch(rang)
        {
        case 0 :
          is >> N;
          break;
        case 1 :
          is >> facteur;
          break;
        case 2 :
          is >> gamma;
          break;
        case 3 :
          stats = 1;
          is >> temps_deb_stats;
          is >> dt_impr;
          break;
        case 4 :
          check = 1;
          break;
        default :
          {
            Cerr << mot_lu << " is not a valid keyword for ODVM scalar wall-model" << finl;
            Cerr << "Possible keywords are : " << les_mots << finl;
            exit();
          }
        }
      is >> mot_lu;
    }

  return is;

}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_ODVM_scal_VDF
//
/////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////
// // Initialisation des tableaux
// ////////////////////////////////////////////////

int Paroi_ODVM_scal_VDF::init_lois_paroi()
{

  //tab_d_equiv_.resize(la_zone_VDF->nb_faces_bord());
  tab_u_star.resize(la_zone_VDF->nb_faces_bord());

  // Pour passer a l'echange contact pour imposer la temperature a l'interface.
  Tf0.resize(la_zone_VDF->nb_faces_bord());

  int ndeb,nfin;
  int elem;
  double dist; //distance du premier centre de maille a la paroi
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();

  const Equation_base& eqn_temp = mon_modele_turb_scal->equation();
  const DoubleTab& Temp = eqn_temp.inconnue().valeurs();
  const Zone_Cl_VDF& zone_Cl_VDF_th = ref_cast(Zone_Cl_VDF,eqn_temp.zone_Cl_dis().valeur());

  const double& t0 = eqn_temp.schema_temps().temps_courant();
  double T0=0.;
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Milieu_base& le_milieu_fluide = eqn_hydr.milieu();
  const Champ_Don& ch_lambda = le_milieu_fluide.conductivite();
  const Champ_Don& ch_Cp = le_milieu_fluide.capacite_calorifique();
  const Champ_Don& ch_rho = le_milieu_fluide.masse_volumique();
  const DoubleTab& lambda_f = ch_lambda.valeurs();
  const DoubleTab& Cp_f     = ch_Cp.valeurs();
  const DoubleTab& rho_f    = ch_rho.valeurs();

  int lambda_unif = 0;
  int Cp_unif = 0;
  int rho_unif = 0;
  double lambda_f_loc = -1.;
  double Cp_f_loc = -1;;
  double rho_f_loc = -1.;

  if (sub_type(Champ_Uniforme,ch_lambda.valeur()))
    {
      lambda_unif = 1;
      lambda_f_loc = max(lambda_f(0,0),DMINFLOAT);
    }
  if (sub_type(Champ_Uniforme,ch_Cp.valeur()))
    {
      Cp_unif = 1;
      Cp_f_loc = max(Cp_f(0,0),DMINFLOAT);
    }
  if (sub_type(Champ_Uniforme,ch_rho.valeur()))
    {
      rho_unif = 1;
      rho_f_loc = max(rho_f(0,0),DMINFLOAT);
    }

  if(stats)
    {
      Tmean.resize(N);
      Trms.resize(N);
      Tpm.resize(N);
      qb_mean.resize(N);

      Tm_rms.resize(N);
      Tm_mean.resize(N);
      Tp_rms.resize(N);

      Tau   = 0;
      Tmean = 0;
      Trms  = 0;
      Tpm   = 0;
      qb_mean = 0;

      Tm_rms = 0;
      Tm_mean = 0;
      Tp_rms = 0;
    }


  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      const Cond_lim& la_cl_th = zone_Cl_VDF_th.les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          eq_odvm.dimensionner(zone_VDF.nb_faces_bord());


          //################
          // ISOTHERMAL WALL
          if(sub_type(Echange_externe_impose,la_cl_th.valeur()))
            {
              const Echange_externe_impose& la_cl_ech = ref_cast(Echange_externe_impose,la_cl_th.valeur());
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  if ((elem = face_voisins(num_face,0)) == -1) elem = face_voisins(num_face,1);
                  T0 = la_cl_ech.T_ext(num_face-ndeb);
                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  eq_odvm[num_face].initialiser(N,gamma,dist,T0, Temp(elem), t0, 1., facteur);
                  // Construction du maillage, resize des tableaux
                  //   mise a zero de certains tableaux,
                  //   et profil lineaire de Tm initialement.
                }//Fin boucle sur les faces de bords
            }
          // END OF ISOTHERMAL WALL
          //#######################

          //############################
          // FLUID-STRUCTURE CALCULATION
          else if(sub_type(Echange_contact_VDF,la_cl_th.valeur()))
            {
              const Echange_contact_VDF& la_cl_couplee = ref_cast(Echange_contact_VDF,la_cl_th.valeur());
              const Champ_front_calc& ch_solide = ref_cast(Champ_front_calc, la_cl_couplee.T_autre_pb().valeur());
              const Milieu_base& le_milieu_solide=ch_solide.milieu();
              const DoubleTab& lambda_s = le_milieu_solide.conductivite().valeurs();
              const DoubleTab& Cp_s     = le_milieu_solide.capacite_calorifique().valeurs();
              const DoubleTab& rho_s    = le_milieu_solide.masse_volumique().valeurs();

              double K;
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  if ((elem = face_voisins(num_face,0)) == -1) elem = face_voisins(num_face,1);
                  if (!lambda_unif)
                    lambda_f_loc = lambda_f(elem);
                  if (!Cp_unif)
                    Cp_f_loc = Cp_f(elem);
                  if (!rho_unif)
                    rho_f_loc = rho_f(elem);

                  K = sqrt(lambda_f_loc*Cp_f_loc*rho_f_loc/(lambda_s(0,0)*Cp_s(0,0)*rho_s(0,0)));

                  //Distance a la paroi du 1er centre de maille
                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);

                  eq_odvm[num_face].initialiser(N,gamma,dist,Temp(elem), Temp(elem), t0, K, facteur);
                  // Construction du maillage, resize des tableaux
                  //   mise a zero de certains tableaux,
                  //   et profil constant de Tm initialement.
                }//Fin boucle sur les faces de bords
            }
          // END OF FLUID-STRUCTURE CALCULATION CASE
          //########################################


          // IT'S AN ERROR because it is a BC that is not possible with ODVM
          else
            {
              Cerr << "Problem while initializing ODVM: the boundary condition" << finl;
              Cerr << "can not be used with ODVM wall-model at the moment." << finl;
              exit();
            }

        }//Fin de paroi fixe

    }//Fin boucle sur les bords parietaux


  // We initialize the values of the equivalent distance at the first time step
  //  with the geometrical distance.
  // if (axi)
  //   for (int num_face=0; num_face<zone_VDF.nb_faces_bord(); num_face++)
  //     tab_d_equiv_[num_face] = zone_VDF.dist_norm_bord_axi(num_face);
  // else
  //   for (int num_face=0; num_face<zone_VDF.nb_faces_bord(); num_face++)
  //     tab_d_equiv_[num_face] = zone_VDF.dist_norm_bord(num_face);


  return 1;
}





int Paroi_ODVM_scal_VDF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elems = zone_VDF.nb_elem();

  const Convection_Diffusion_std& eqn_temp = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide   = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin            = le_fluide.viscosite_cinematique();
  const Milieu_base& le_milieu_fluide      = eqn_hydr.milieu();
  const DoubleTab& lambda_f                = le_milieu_fluide.conductivite().valeurs();

  DoubleTab termes_sources;
  termes_sources.resize(nb_elems);
  eqn_temp.sources().calculer(termes_sources); //les termes sources

  // TEMP
  const DoubleTab& xv = zone_VDF.xv();
  // TEMP
  //  GF ne marche pas en // et les tests avec num_face==2402  bof..
  if (0)
    {
      //////////////////////////////////////////////////////////////////////////////
      //Sondes calculs Re_tau =395
      //////////////////////////////////////////////////////////////////////////////
      SFichier fic_sonde_temp_31("sonde_ODVM_yp_3_1.dat",ios::app); // impression sonde 3
      SFichier fic_sonde_temp_61("sonde_ODVM_yp_6_1.dat",ios::app); // impression sonde 6
      SFichier fic_sonde_temp_91("sonde_ODVM_yp_9_1.dat",ios::app); // impression sonde 9

      SFichier fic_sonde_temp_32("sonde_ODVM_yp_3_2.dat",ios::app); // impression sonde 3
      SFichier fic_sonde_temp_62("sonde_ODVM_yp_6_2.dat",ios::app); // impression sonde 6
      SFichier fic_sonde_temp_92("sonde_ODVM_yp_9_2.dat",ios::app); // impression sonde 9

      SFichier fic_sonde_temp_33("sonde_ODVM_yp_3_3.dat",ios::app); // impression sonde 3
      SFichier fic_sonde_temp_63("sonde_ODVM_yp_6_3.dat",ios::app); // impression sonde 6
      SFichier fic_sonde_temp_93("sonde_ODVM_yp_9_3.dat",ios::app); // impression sonde 9

      SFichier fic_sonde_temp_34("sonde_ODVM_yp_3_4.dat",ios::app); // impression sonde 3
      SFichier fic_sonde_temp_64("sonde_ODVM_yp_6_4.dat",ios::app); // impression sonde 6
      SFichier fic_sonde_temp_94("sonde_ODVM_yp_9_4.dat",ios::app); // impression sonde 9

      SFichier fic_sonde_temp_35("sonde_ODVM_yp_3_5.dat",ios::app); // impression sonde 3
      SFichier fic_sonde_temp_65("sonde_ODVM_yp_6_5.dat",ios::app); // impression sonde 6
      SFichier fic_sonde_temp_95("sonde_ODVM_yp_9_5.dat",ios::app); // impression sonde 9
      ////////////////////////////////////////////////////////////////////////////////
    }
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
  // tab_visco+=DMINFLOAT;

  int ndeb=0,nfin=0;
  int elem;
  double diff, visco_cin;


  // On recupere directement u_star par la loi de paroi. Il ne faut surtout pas
  // utiliser la methode calculer_u_star_avec_cisaillement car c'est faux en QC.
  // En effet,on recupere alors u_star*rho^0.5 et non u_star -> influence sur les
  // flux et l'impression du Nusselt.

  const RefObjU& modele_turbulence_hydr = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();
  const DoubleVect& tab_ustar = loi.valeur().tab_u_star();

  int schmidt = 0;
  if (sub_type(Convection_Diffusion_Concentration,eqn_temp)) schmidt = 1;
  const Champ_Don& alpha = (schmidt==1?ref_cast(Convection_Diffusion_Concentration,eqn_temp).constituant().diffusivite_constituant():le_fluide.diffusivite());

  const DoubleVect& Temp = eqn_temp.inconnue().valeurs();
  const Zone_Cl_VDF& zone_Cl_VDF_th = ref_cast(Zone_Cl_VDF,eqn_temp.zone_Cl_dis().valeur());


  const double& tps = eqn_temp.schema_temps().temps_courant();
  const double& dtG = eqn_temp.schema_temps().pas_de_temps();
  double T0=0.;


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

          //################
          // ISOTHERMAL WALL
          if(sub_type(Echange_externe_impose,la_cl_th.valeur()))
            {
              const Echange_externe_impose& la_cl_ech = ref_cast(Echange_externe_impose,la_cl_th.valeur());
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  if ((elem = face_voisins(num_face,0)) == -1) elem = face_voisins(num_face,1);
                  if (l_unif) visco_cin = visco;
                  else visco_cin = tab_visco[elem];
                  if (sub_type(Champ_Uniforme,alpha.valeur())) diff = alpha(0,0);
                  else
                    {
                      if (alpha.nb_comp()==1) diff = alpha(elem);
                      else diff = alpha(elem,0);
                    }
                  T0 = la_cl_ech.T_ext(num_face-ndeb);
                  if(compt) eq_odvm[num_face].set_U_tau(tab_ustar(num_face),1);
                  else eq_odvm[num_face].set_U_tau(tab_ustar(num_face),-1);

                  double ts = termes_sources(elem);
                  eq_odvm[num_face].set_F(ts);
                  eq_odvm[num_face].aller_au_temps(tps,T0,Temp(elem),diff,visco_cin,1);

                  ///////////////////////////////////////////////////////////////////////////
                  //Sondes calcule Re_tau
                  double periode = 1e-2, tn=0.;
                  if((tps-tn) > periode)
                    {
#ifdef bidon
                      if(num_face==2048)
                        {
                          fic_sonde_temp_31 << tps << " " << eq_odvm[num_face].get_Tm(3) << finl;
                          fic_sonde_temp_61 << tps << " " << eq_odvm[num_face].get_Tm(6) << finl;
                          fic_sonde_temp_91 << tps << " " << eq_odvm[num_face].get_Tm(9) << finl;
                        }
                      if(num_face==2049)
                        {
                          fic_sonde_temp_32 << tps << " " << eq_odvm[num_face].get_Tm(3) << finl;
                          fic_sonde_temp_62 << tps << " " << eq_odvm[num_face].get_Tm(6) << finl;
                          fic_sonde_temp_92 << tps << " " << eq_odvm[num_face].get_Tm(9) << finl;
                        }
                      if(num_face==2050)
                        {
                          fic_sonde_temp_33 << tps << " " << eq_odvm[num_face].get_Tm(3) << finl;
                          fic_sonde_temp_63 << tps << " " << eq_odvm[num_face].get_Tm(6) << finl;
                          fic_sonde_temp_93 << tps << " " << eq_odvm[num_face].get_Tm(9) << finl;
                        }
                      if(num_face==2051)
                        {
                          fic_sonde_temp_34 << tps << " " << eq_odvm[num_face].get_Tm(3) << finl;
                          fic_sonde_temp_64 << tps << " " << eq_odvm[num_face].get_Tm(6) << finl;
                          fic_sonde_temp_94 << tps << " " << eq_odvm[num_face].get_Tm(9) << finl;
                        }
                      if(num_face==2052)
                        {
                          fic_sonde_temp_35 << tps << " " << eq_odvm[num_face].get_Tm(3) << finl;
                          fic_sonde_temp_65 << tps << " " << eq_odvm[num_face].get_Tm(6) << finl;
                          fic_sonde_temp_95 << tps << " " << eq_odvm[num_face].get_Tm(9) << finl;
                        }
#endif
                      tn = tps;
                    }
                  //////////////////////////////////////////////////////////////////////////////
                  double Temp_w = la_cl_ech.T_ext(num_face-ndeb);
                  double dy_w   = eq_odvm[num_face].get_dy_w();
                  double Temp_mean       = eq_odvm[num_face].get_Tm(1);
                  double Temp_prim       = eq_odvm[num_face].get_Tp(1);

                  // L'expression de d_equiv ne tient pas compte de alpha_t
                  // Cela dit, c'est normale car c'est lors du calcul du flux que la
                  // turbulence est prise en compte.
                  int global_face=num_face;
                  int local_face=zone_VDF.front_VF(boundary_index).num_local_face(global_face);
                  equivalent_distance_[boundary_index](local_face) = (Temp(elem)-Temp_w)*dy_w/(Temp_mean+Temp_prim - Temp_w);
                }//fin boucle sur les faces de bords
              compt++;
            }
          // END OF ISOTHERMAL WALL
          //#######################

          //############################
          // FLUID-STRUCTURE CALCULATION
          else if(sub_type(Echange_contact_VDF,la_cl_th.valeur()))
            {
              const Echange_contact_VDF& la_cl_couplee = ref_cast(Echange_contact_VDF,la_cl_th.valeur());
              //      const DoubleTab& Temp_wall = la_cl_couplee.T_wall();
              const DoubleTab& h_autre_pb = la_cl_couplee.h_autre_pb();

              // On fait une supposition sur le schema en temps
              // (CLs prises au temps futur)
              const DoubleTab& t_autre=la_cl_couplee.T_autre_pb()->valeurs_au_temps(tps+dtG);
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  if ((elem = face_voisins(num_face,0)) == -1) elem = face_voisins(num_face,1);
                  if (l_unif) visco_cin = visco;
                  else visco_cin = tab_visco[elem];
                  if (sub_type(Champ_Uniforme,alpha.valeur())) diff = alpha(0,0);
                  else
                    {
                      if (alpha.nb_comp()==1) diff = alpha(elem);
                      else diff = alpha(elem,0);
                    }

                  const double& Temp_solid = t_autre(num_face-ndeb,0);

                  /*        double Pr = visco_cin/diff;
                            double Beta = pow(3.85*pow(Pr,1./3.)-1.3,2.)+2.12*log(Pr);
                            double y_plus = eq_odvm[num_face].get_Y(N-1)*tab_u_star(num_face)/visco_cin;
                            double Gamma = (0.01*pow(Pr*y_plus,4.))/(1.+5.*pow(Pr,3.)*y_plus);
                            double T_plus = Pr*y_plus*exp(-Gamma);
                            T_plus += (2.12*log(1.+y_plus) + Beta)*exp(-1./(Gamma+1e-20));
                            double d_equiv = diff*T_plus/tab_u_star(num_face);

                            T0 = (Temp(elem)-Temp_solid)/(d_equiv/lambda_f(0,0) + 1./h_autre_pb(num_face-ndeb,0));
                            T0 = T0*diff/lambda_f(0,0);
                  */
                  T0 = (Tf0(num_face-ndeb)-Temp_solid)*h_autre_pb(num_face-ndeb,0);
                  T0 = T0*diff/lambda_f(0,0);

                  if(compt==1)
                    {
                      eq_odvm[num_face].set_U_tau(tab_ustar(num_face),1);
                      eq_odvm[num_face].init_profile_CHT(Temp_solid,Temp(elem));
                    }
                  else
                    {
                      eq_odvm[num_face].set_U_tau(tab_ustar(num_face),-1);
                      eq_odvm[num_face].aller_au_temps(tps,T0,Temp(elem),diff,visco_cin,2);
                    }

                  double Ttot0  = eq_odvm[num_face].get_Tm(0) + eq_odvm[num_face].get_Tp(0);
                  if(compt!=1) Tf0(num_face-ndeb) = 0.5*Ttot0+0.5*Tf0(num_face-ndeb);
                  else Tf0(num_face-ndeb)=Temp_solid;

                  //        if(compt!=1) tab_d_equiv_[num_face] = lambda_f(0,0)*(Temp(elem)-Ttot0)/(Ttot0-Temp_solid+DMINFLOAT)/h_autre_pb(num_face-ndeb,0);

                  if(check)
                    {
                      double Ttot1 = eq_odvm[num_face].get_Tm(1) + eq_odvm[num_face].get_Tp(1);
                      if((num_face==ndeb)&&(xv(ndeb,1)<1.))
                        {
                          SFichier fic("Suivi_ndeb.dat",ios::app);
                          fic << tps << "   " << Temp_solid
                              << "   " << eq_odvm[num_face].get_Tm(0)
                              << "   " << Ttot0
                              << "   " << Ttot1
                              << "   " << Temp(elem)
                              << "   " << eq_odvm[num_face].get_Tm(N-1) << finl;
                        }
                    }


                  if(check)
                    {
                      if((num_face==ndeb)&&(xv(ndeb,1)<1.))
                        {
                          SFichier fic("q.dat");
                          for(int i=0; i<N; i++)
                            fic << eq_odvm[num_face].get_Y(i) << "   " << sqrt(eq_odvm[num_face].get_qf(i)) << "   " << sqrt(eq_odvm[num_face].get_qb(i)) <<finl;
                          SFichier fic_Q("Q.dat",ios::app);
                          fic_Q << tps << "   " << eq_odvm[num_face].get_Q(0) << finl;
                        }
                    }

                }//fin boucle sur les faces de bords
              compt++;

            }
          // END OF FLUID-STRUCTURE CALCULATION CASE
          //########################################

        }//Fin de paroi fixe

    }//fin boucle sur les bords


  if((stats)&&(tps>temps_deb_stats))
    {
      Tau+=dtG;
      for (int num_face=ndeb; num_face<nfin; num_face++)
        {
          // Calculating RMS and Time Mean values of temperature.
          for(int i=0; i<N; i++)
            {
              Tmean[i] = Tmean[i] + (eq_odvm[num_face].get_Tm(i)+eq_odvm[num_face].get_Tp(i))*dtG/(nfin-ndeb);
              Trms[i]  = Trms[i]  + (eq_odvm[num_face].get_Tm(i)+eq_odvm[num_face].get_Tp(i))*(eq_odvm[num_face].get_Tm(i)+eq_odvm[num_face].get_Tp(i))*dtG/(nfin-ndeb);
              Tpm[i]   = Tpm[i]   + eq_odvm[num_face].get_Tp(i)*dtG/(nfin-ndeb);
              qb_mean[i] = qb_mean[i] + sqrt(eq_odvm[num_face].get_qb(i))*dtG/(nfin-ndeb);

              Tm_mean[i] = Tm_mean[i]+eq_odvm[num_face].get_Tm(i)*dtG/(nfin-ndeb);
              Tm_rms[i] = Tm_rms[i]+eq_odvm[num_face].get_Tm(i)*eq_odvm[num_face].get_Tm(i)*dtG/(nfin-ndeb);
              Tp_rms[i] = Tp_rms[i]+eq_odvm[num_face].get_Tp(i)*eq_odvm[num_face].get_Tp(i)*dtG/(nfin-ndeb);

            }
        }
      if (dabs(tps-temps_dern_post_inst)>=dt_impr)
        {
          Nom nom_fic = "ODVM_fields_";
          Nom temps = Nom(tps);
          nom_fic+= temps;
          nom_fic+=".dat";
          SFichier fic (nom_fic);
          fic << "# Time and Space Averaged ODVM Statistics " << finl;
          fic << "# Y     <T>     Trms     <Tp>     sqrt(q)     Tm_rms     Tp_rms" << finl;
          for(int i=0; i<N; i++)
            fic << eq_odvm[ndeb].get_Y(i) << " " << Tmean[i]/Tau << " " << sqrt((Trms[i]-(Tmean[i]*Tmean[i])/Tau)/Tau) << " " << Tpm[i]/Tau << " " << qb_mean[i]/Tau << " " << sqrt((Tm_rms[i]-(Tm_mean[i]*Tm_mean[i])/Tau)/Tau) << " " << sqrt((Tp_rms[i]-(Tpm[i]*Tpm[i])/Tau)/Tau) << finl;
          temps_dern_post_inst = tps;
        }
    }




  return 1;
}

