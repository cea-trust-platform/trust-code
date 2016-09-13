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
// File:        Traitement_particulier_NS_Profils_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_Profils_VDF.h>
#include <Zone_VDF.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <communications.h>
#include <SFichier.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable(Traitement_particulier_NS_Profils_VDF,"Traitement_particulier_NS_Profils_VDF",Traitement_particulier_NS_Profils);


// Description:
//
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Traitement_particulier_NS_Profils_VDF::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Traitement_particulier_NS_Profils_VDF::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_Profils_VDF::lire(Entree& is)
{
  return Traitement_particulier_NS_Profils::lire(is);
}

Entree& Traitement_particulier_NS_Profils_VDF::lire(const Motcle& motlu, Entree& is)
{
  if (motlu=="nb_points")
    {
      is >> Nap;
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_NS_Profils_VDF::readOn" << finl;
      Cerr << "Possible key-words are : nb_points , { and } " << finl;
      Cerr << "User specified :" << motlu << finl;
      exit();
    }
  return is;
}

void Traitement_particulier_NS_Profils_VDF::post_traitement_particulier()
{
  DoubleTab umoy_x_m(n_probes,Nap);
  umoy_x_m=0.;
  DoubleTab umoy_y_m(n_probes,Nap);
  umoy_y_m=0.;
  DoubleTab umoy_z_m(n_probes,Nap);
  umoy_z_m=0.;
  DoubleTab umoy_2_m(n_probes,Nap);
  umoy_2_m=0.;
  DoubleTab vmoy_2_m(n_probes,Nap);
  vmoy_2_m=0.;
  DoubleTab wmoy_2_m(n_probes,Nap);
  wmoy_2_m=0.;
  DoubleTab umoy_3_m(n_probes,Nap);
  umoy_3_m=0.;
  DoubleTab vmoy_3_m(n_probes,Nap);
  vmoy_3_m=0.;
  DoubleTab wmoy_3_m(n_probes,Nap);
  wmoy_3_m=0.;
  DoubleTab umoy_4_m(n_probes,Nap);
  umoy_4_m=0.;
  DoubleTab vmoy_4_m(n_probes,Nap);
  vmoy_4_m=0.;
  DoubleTab wmoy_4_m(n_probes,Nap);
  wmoy_4_m=0.;
  DoubleTab uv_moy_m(n_probes,Nap);
  uv_moy_m=0.;
  DoubleTab nu_t_inst_m(n_probes,Nap);
  nu_t_inst_m=0.;

  DoubleTab umoy_x_p(n_probes,Nap);
  umoy_x_p=0.;
  DoubleTab umoy_y_p(n_probes,Nap);
  umoy_y_p=0.;
  DoubleTab umoy_z_p(n_probes,Nap);
  umoy_z_p=0.;
  DoubleTab umoy_2_p(n_probes,Nap);
  umoy_2_p=0.;
  DoubleTab vmoy_2_p(n_probes,Nap);
  vmoy_2_p=0.;
  DoubleTab wmoy_2_p(n_probes,Nap);
  wmoy_2_p=0.;
  DoubleTab umoy_3_p(n_probes,Nap);
  umoy_3_p=0.;
  DoubleTab vmoy_3_p(n_probes,Nap);
  vmoy_3_p=0.;
  DoubleTab wmoy_3_p(n_probes,Nap);
  wmoy_3_p=0.;
  DoubleTab umoy_4_p(n_probes,Nap);
  umoy_4_p=0.;
  DoubleTab vmoy_4_p(n_probes,Nap);
  vmoy_4_p=0.;
  DoubleTab wmoy_4_p(n_probes,Nap);
  wmoy_4_p=0.;
  DoubleTab uv_moy_p(n_probes,Nap);
  uv_moy_p=0.;
  DoubleTab nu_t_inst_p(n_probes,Nap);
  nu_t_inst_p=0.;

  double tps = mon_equation->inconnue().temps();
  if (oui_u_inst != 0)
    {
      calculer_moyenne_spatiale_vitesse(umoy_x_m,umoy_2_m,umoy_3_m,umoy_4_m,corresp_u_m,compt_x_m,Nxy,0,xUm);
      calculer_moyenne_spatiale_vitesse(umoy_x_p,umoy_2_p,umoy_3_p,umoy_4_p,corresp_u_p,compt_x_p,Nxy,0,xUp);

      calculer_moyenne_spatiale_vitesse(umoy_y_m,vmoy_2_m,vmoy_3_m,vmoy_4_m,corresp_v_m,compt_y_m,Nyy,1,xVm);
      calculer_moyenne_spatiale_vitesse(umoy_y_p,vmoy_2_p,vmoy_3_p,vmoy_4_p,corresp_v_p,compt_y_p,Nyy,1,xVp);

      calculer_moyenne_spatiale_vitesse(umoy_z_m,wmoy_2_m,wmoy_3_m,wmoy_4_m,corresp_w_m,compt_z_m,Nzy,2,xWm);
      calculer_moyenne_spatiale_vitesse(umoy_z_p,wmoy_2_p,wmoy_3_p,wmoy_4_p,corresp_w_p,compt_z_p,Nzy,2,xWp);
      calculer_moyenne_spatiale_uv(uv_moy_m,corresp_uv_m,compt_uv_m,Nuv,xUVm);
      calculer_moyenne_spatiale_uv(uv_moy_p,corresp_uv_p,compt_uv_p,Nuv,xUVp);
      static double temps_dern_post_inst = -100.;

      if (dabs(tps-temps_dern_post_inst)>=dt_post_inst)
        {
          ecriture_fichiers_moy_spat(umoy_x_m,umoy_2_m,umoy_3_m,umoy_4_m,umoy_x_p,umoy_2_p,umoy_3_p,umoy_4_p,delta_Um,delta_Up,Nxy,Yu_m,0);
          ecriture_fichiers_moy_spat(umoy_y_m,vmoy_2_m,vmoy_3_m,vmoy_4_m,umoy_y_p,vmoy_2_p,vmoy_3_p,vmoy_4_p,delta_Vm,delta_Vp,Nyy,Yv_m,1);
          ecriture_fichiers_moy_spat(umoy_z_m,wmoy_2_m,wmoy_3_m,wmoy_4_m,umoy_z_p,wmoy_2_p,wmoy_3_p,wmoy_4_p,delta_Wm,delta_Wp,Nzy,Yw_m,2);
          ecriture_fichiers_moy_spat_1col(uv_moy_m,uv_moy_p,Yuv_m,delta_UVm,delta_UVp,Nuv,3);

          temps_dern_post_inst = tps;
        }
    }

  if (oui_profil_nu_t != 0)
    {
      calculer_moyenne_spatiale_nut(nu_t_inst_m,corresp_uv_m,compt_uv_m,Nuv,xUVm);
      calculer_moyenne_spatiale_nut(nu_t_inst_p,corresp_uv_p,compt_uv_p,Nuv,xUVp);

      static double temps_dern_post_inst_nut = -100.;
      if (dabs(tps-temps_dern_post_inst_nut)>=dt_post_inst)
        {
          ecriture_fichiers_moy_spat_1col(nu_t_inst_m,nu_t_inst_p,Yuv_m,delta_UVm,delta_UVp,Nuv,4);
          temps_dern_post_inst_nut = tps;
        }
    }

  ////// Moyenne temporelle*********************************************//
  if (oui_stat != 0)
    {
      double tpsbis = mon_equation->inconnue().temps();
      if ((tpsbis>=temps_deb)&&(tpsbis<=temps_fin))
        {
          static int init_stat_temps = 0;
          if((init_stat_temps==0)&&( oui_repr != 1))  // si ce n est pas une reprise : sinon valeurs lues dans le fichier de reprise
            {
              double dt_v = mon_equation->schema_temps().pas_de_temps();
              temps_deb = tpsbis-dt_v;
              init_stat_temps++;
            }

          calculer_integrale_temporelle(u_moy_temp_x,umoy_x_m,umoy_x_p,delta_Um,delta_Up);
          calculer_integrale_temporelle(u_moy_temp_y,umoy_y_m,umoy_y_p,delta_Vm,delta_Vp);
          calculer_integrale_temporelle(u_moy_temp_z,umoy_z_m,umoy_z_p,delta_Wm,delta_Wp);

          calculer_integrale_temporelle(uv_moy_temp,uv_moy_m,uv_moy_p,delta_UVm,delta_UVp);

          calculer_integrale_temporelle(u2_moy_temp,umoy_2_m,umoy_2_p,delta_Um,delta_Up);
          calculer_integrale_temporelle(v2_moy_temp,vmoy_2_m,vmoy_2_p,delta_Vm,delta_Vp);
          calculer_integrale_temporelle(w2_moy_temp,wmoy_2_m,wmoy_2_p,delta_Wm,delta_Wp);

          calculer_integrale_temporelle(u3_moy_temp,umoy_3_m,umoy_3_p,delta_Um,delta_Up);
          calculer_integrale_temporelle(v3_moy_temp,vmoy_3_m,vmoy_3_p,delta_Vm,delta_Vp);
          calculer_integrale_temporelle(w3_moy_temp,wmoy_3_m,wmoy_3_p,delta_Wm,delta_Wp);

          calculer_integrale_temporelle(u4_moy_temp,umoy_4_m,umoy_4_p,delta_Um,delta_Up);
          calculer_integrale_temporelle(v4_moy_temp,vmoy_4_m,vmoy_4_p,delta_Vm,delta_Vp);
          calculer_integrale_temporelle(w4_moy_temp,wmoy_4_m,wmoy_4_p,delta_Wm,delta_Wp);


          if (oui_profil_nu_t != 0)
            calculer_integrale_temporelle(nu_t_temp,nu_t_inst_m,nu_t_inst_p,delta_UVm,delta_UVp);

          static double temps_dern_post_stat = -100.;
          if (dabs(tpsbis-temps_dern_post_stat)>=dt_post_stat)
            {
              double dt = tpsbis-temps_deb;

              ecriture_fichiers_moy_temp(u_moy_temp_x,u2_moy_temp,u3_moy_temp,u4_moy_temp,Yu_m,Nxy,0,dt);
              ecriture_fichiers_moy_temp(u_moy_temp_y,v2_moy_temp,v3_moy_temp,v4_moy_temp,Yv_m,Nyy,1,dt);
              ecriture_fichiers_moy_temp(u_moy_temp_z,w2_moy_temp,w3_moy_temp,w4_moy_temp,Yw_m,Nzy,2,dt);
              ecriture_fichiers_moy_temp_1col(uv_moy_temp,Yuv_m,Nuv,3,dt);
              if (oui_profil_nu_t != 0)
                ecriture_fichiers_moy_temp_1col(nu_t_temp,Yuv_m,Nuv,4,dt);
              temps_dern_post_stat = tpsbis;
            }
        }
    }
}







void Traitement_particulier_NS_Profils_VDF::init_calcul_moyenne(void)
{
  // On va initialiser les differents parametres membres de la classe
  // utiles au calcul des differentes moyennes
  // Initialisation de : Yu,Yv,Yw,Yuv + compt_x,compt_y,compt_z,compt_uv
  // + corresp_u,corresp_v,corresp_w,corresp_uv
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);

  const DoubleTab& xv = zone_VDF.xv();
  const DoubleTab& xp = zone_VDF.xp();

  int nb_faces = zone_VDF.nb_faces();
  int nb_elems = zone_VDF.zone().nb_elem();

  const IntVect& orientation = zone_VDF.orientation();

  int num_face,num_elem,ori,indic_m,indicv_m,indicw_m,indicuv_m,indic_p,indicv_p,indicw_p,indicuv_p,trouve;
  int i,j;
  double y;
  j=0;
  indic_m = indic_p = 0;
  indicv_m = indicv_p = 0;
  indicw_m = indicw_p = 0;
  indicuv_m = indicuv_p = 0;

  // initialisation des differentes tailles de tableaux
  Nxy.resize(n_probes);
  Nyy.resize(n_probes);
  Nzy.resize(n_probes);
  Nuv.resize(n_probes);
  Nxy = Nap;
  Nyy = Nap;
  Nzy = Nap;
  Nuv = Nap;

  // dimensionnement aux valeurs rentrees dans le jeu de donnees
  // The points before the value of the probe
  xUm.resize(n_probes);
  xWm.resize(n_probes);
  xVm.resize(n_probes);
  xUVm.resize(n_probes);

  delta_Um.resize(n_probes);
  delta_Wm.resize(n_probes);
  delta_Vm.resize(n_probes);
  delta_UVm.resize(n_probes);

  // The points after the value of the probe
  xUp.resize(n_probes);
  xWp.resize(n_probes);
  xVp.resize(n_probes);
  xUVp.resize(n_probes);

  delta_Up.resize(n_probes);
  delta_Wp.resize(n_probes);
  delta_Vp.resize(n_probes);
  delta_UVp.resize(n_probes);

  // initialisation
  // The points before the value of the probe
  Yu_m = -100.;
  Yv_m = -100.;
  Yw_m = -100.;
  Yuv_m = -100.;

  compt_x_m = 0;
  compt_y_m = 0;
  compt_z_m = 0;
  compt_uv_m = 0;

  corresp_u_m = -1;
  corresp_v_m = -1;
  corresp_w_m = -1;
  corresp_uv_m = -1;
  // The points after the value of the probe
  Yu_p = -100.;
  Yv_p = -100.;
  Yw_p = -100.;
  Yuv_p = -100.;

  compt_x_p = 0;
  compt_y_p = 0;
  compt_z_p = 0;
  compt_uv_p = 0;

  corresp_u_p = -1;
  corresp_v_p = -1;
  corresp_w_p = -1;
  corresp_uv_p = -1;

  delta_Um=delta_Up=delta_Wm=delta_Wp=delta_Vm=delta_Vp=delta_UVm=delta_UVp=10000000.;

  Cerr << (int)system("mkdir Space_Avg") << finl;
  Cerr << (int)system("mkdir Time_Avg") << finl;

  // remplissage des tableaux ci-dessus

  // On cherche tout d'abord les coordonnees des points les plus proches de chaque cote et on les stocke
  for(i=0; i<n_probes; i++)
    {
      //Boucle sur les faces
      for (num_face=0; num_face<nb_faces; num_face++)
        {
          ori = orientation(num_face);

          if((xv(num_face,dir_profil)-positions(i))>=0)
            {
              if(ori==0)
                {
                  if((xv(num_face,dir_profil)-positions(i))<delta_Up(i))
                    {
                      xUp(i)=xv(num_face,dir_profil);
                      delta_Up(i)=xv(num_face,dir_profil)-positions(i);
                    }
                }
              else if(ori==2)
                {
                  if((xv(num_face,dir_profil)-positions(i))<delta_Wp(i))
                    {
                      xWp(i)=xv(num_face,dir_profil);
                      delta_Wp(i)=xv(num_face,dir_profil)-positions(i);
                    }
                }
              else if(ori==1)
                {
                  if((xv(num_face,dir_profil)-positions(i))<delta_Vp(i))
                    {
                      xVp(i)=xv(num_face,dir_profil);
                      delta_Vp(i)=xv(num_face,dir_profil)-positions(i);
                    }
                }
              else
                {
                  Cerr << "Pb dans Traitement_particulier_NS_Profils_VDF::init_calcul_moyenne" << finl;
                  Cerr << "Pb avec la direction de la face!" << finl;
                  exit();
                }
            }
          else
            {
              if(ori==0)
                {
                  if((positions(i)-xv(num_face,dir_profil))<delta_Um(i))
                    {
                      xUm(i)=xv(num_face,dir_profil);
                      delta_Um(i)=positions(i)-xv(num_face,dir_profil);
                    }
                }
              else if(ori==2)
                {
                  if((positions(i)-xv(num_face,dir_profil))<delta_Wm(i))
                    {
                      xWm(i)=xv(num_face,dir_profil);
                      delta_Wm(i)=positions(i)-xv(num_face,dir_profil);
                    }
                }
              else if(ori==1)
                {
                  if((positions(i)-xv(num_face,dir_profil))<delta_Vm(i))
                    {
                      xVm(i)=xv(num_face,dir_profil);
                      delta_Vm(i)=positions(i)-xv(num_face,dir_profil);
                    }
                }

              else
                {
                  Cerr << "Pb dans Traitement_particulier_NS_Profils_VDF::init_calcul_moyenne" << finl;
                  Cerr << "Pb avec la direction de la face!" << finl;
                  exit();
                }
            }
        }


      //Boucle sur les elements
      for (num_elem=0; num_elem<nb_elems; num_elem++)
        {
          if((xp(num_elem,dir_profil)-positions(i))>=0)
            {
              if((xp(num_elem,dir_profil)-positions(i))<delta_UVp(i))
                {
                  xUVp(i)=xp(num_elem,dir_profil);
                  delta_UVp(i)=xp(num_elem,dir_profil)-positions(i);
                }
            }
          else
            {
              if((positions(i)-xp(num_elem,dir_profil))<delta_UVm(i))
                {
                  xUVm(i)=xp(num_elem,dir_profil);
                  delta_UVm(i)=positions(i)-xp(num_elem,dir_profil);
                }
            }
        }//Fin boucle sur les elements

    }

  int numb_del_probes=0;
  //Verification des sondes pour voir si elles sont bien toutes definies
  //dans le domaine de calcul
  for(i=0; i<n_probes; i++)
    {
      if((delta_Um(i)==10000000)||(delta_Up(i)==10000000)||
          (delta_Wm(i)==10000000)||(delta_Wp(i)==10000000)||
          (delta_Vm(i)==10000000)||(delta_Vp(i)==10000000)||
          (delta_UVm(i)==10000000)||(delta_UVp(i)==10000000))
        {
          Cerr << "Probe Number " << i+1+numb_del_probes << " undefined in computational domain." << finl;
          Cerr << "It will be deleted from profile list !" << finl;
          n_probes--;
          numb_del_probes++;
          for(j=i; j<n_probes; j++)
            {
              delta_Um(j)=delta_Um(j+1);
              delta_Up(j)=delta_Up(j+1);
              delta_Wm(j)=delta_Wm(j+1);
              delta_Wp(j)=delta_Wp(j+1);
              delta_Vm(j)=delta_Vm(j+1);
              delta_Vp(j)=delta_Vp(j+1);
              delta_UVm(j)=delta_UVm(j+1);
              delta_UVp(j)=delta_UVp(j+1);
              xUm(j)=xUm(j+1);
              xUp(j)=xUp(j+1);
              xWm(j)=xWm(j+1);
              xWp(j)=xWp(j+1);
              xVm(j)=xVm(j+1);
              xVp(j)=xVp(j+1);
              xUVm(j)=xUVm(j+1);
              xUVp(j)=xUVp(j+1);
              positions(j)=positions(j+1);
            }
          i=i-1;
        }
    }

  //Dimensionnement maintenant que l'on connait le nombre exact de probes
  Yu_m.resize(n_probes,Nap);
  Yv_m.resize(n_probes,Nap);
  Yw_m.resize(n_probes,Nap);
  Yuv_m.resize(n_probes,Nap);

  compt_x_m.resize(n_probes,Nap);
  compt_y_m.resize(n_probes,Nap);
  compt_z_m.resize(n_probes,Nap);
  compt_uv_m.resize(n_probes,Nap);

  corresp_u_m.resize(n_probes,nb_faces);
  corresp_v_m.resize(n_probes,nb_faces);
  corresp_w_m.resize(n_probes,nb_faces);
  corresp_uv_m.resize(n_probes,nb_elems);

  Yu_p.resize(n_probes,Nap);
  Yv_p.resize(n_probes,Nap);
  Yw_p.resize(n_probes,Nap);
  Yuv_p.resize(n_probes,Nap);

  compt_x_p.resize(n_probes,Nap);
  compt_y_p.resize(n_probes,Nap);
  compt_z_p.resize(n_probes,Nap);
  compt_uv_p.resize(n_probes,Nap);

  corresp_u_p.resize(n_probes,nb_faces);
  corresp_v_p.resize(n_probes,nb_faces);
  corresp_w_p.resize(n_probes,nb_faces);
  corresp_uv_p.resize(n_probes,nb_elems);





  // Impression dans le Cerr pour verifications
  Cerr << finl;
  Cerr << "       UPPER/LOWER BOUNDS FOR PROFILES :" << finl;
  for(i=0; i<n_probes; i++)
    {
      Cerr << "Profile " << i+1 << " : X= " << positions(i) << finl;
      Cerr << "xUm= " << xUm(i) << " xUp= " << xUp(i) << finl;
      Cerr << "xElm= " << xUVm(i) << " xElp= " << xUVp(i) << finl;
      Cerr << finl;
    }



  //On remplit les tableaux de correspondances
  for(i=0; i<n_probes; i++)
    {
      indic_m = indic_p = 0;
      indicv_m = indicv_p = 0;
      indicw_m = indicw_p = 0;
      indicuv_m = indicuv_p = 0;
      //Boucle sur les faces pour avoir la correspondance pour les composantes de la vitesse.
      for (num_face=0; num_face<nb_faces; num_face++)
        {
          ori = orientation(num_face);
          switch(ori)
            {
              //Pour la composante de vitesse U
            case 0:
              {
                //Point avant
                if(xv(num_face,dir_profil)==xUm(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indic_m+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yu_m(i,j)))
                          {
                            corresp_u_m(i,num_face) = j;
                            compt_x_m(i,j) ++;
                            j=indic_m+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_u_m(i,num_face) = indic_m;
                        Yu_m(i,indic_m)=y;
                        compt_x_m(i,indic_m) ++;
                        indic_m++;
                      }
                  }
                //Point apres
                else if(xv(num_face,dir_profil)==xUp(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indic_p+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yu_p(i,j)))
                          {
                            corresp_u_p(i,num_face) = j;
                            compt_x_p(i,j) ++;
                            j=indic_p+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_u_p(i,num_face) = indic_p;
                        Yu_p(i,indic_p)=y;
                        compt_x_p(i,indic_p) ++;
                        indic_p++;
                      }
                  }
                break;
              }
              //Pour la composante de vitesse V
            case 1 :
              {
                //Point avant
                if(xv(num_face,dir_profil)==xVm(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indicv_m+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yv_m(i,j)))
                          {
                            corresp_v_m(i,num_face) = j;
                            compt_y_m(i,j) ++;
                            j=indicv_m+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_v_m(i,num_face) = indicv_m;
                        Yv_m(i,indicv_m)=y;
                        compt_y_m(i,indicv_m) ++;
                        indicv_m++;
                      }
                  }
                //Point apres
                else if(xv(num_face,dir_profil)==xVp(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indicv_p+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yv_p(i,j)))
                          {
                            corresp_v_p(i,num_face) = j;
                            compt_y_p(i,j) ++;
                            j=indicv_p+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_v_p(i,num_face) = indicv_p;
                        Yv_p(i,indicv_p)=y;
                        compt_y_p(i,indicv_p) ++;
                        indicv_p++;
                      }
                  }
                break;
              }
              //Pour la composante de vitesse W
            case 2 :
              {
                //Point avant
                if(xv(num_face,dir_profil)==xWm(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indicw_m+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yw_m(i,j)))
                          {
                            corresp_w_m(i,num_face) = j;
                            compt_z_m(i,j) ++;
                            j=indicw_m+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_w_m(i,num_face) = indicw_m;
                        Yw_m(i,indicw_m)=y;
                        compt_z_m(i,indicw_m) ++;
                        indicw_m++;
                      }
                  }
                //Point apres
                else if(xv(num_face,dir_profil)==xWp(i))
                  {
                    y = xv(num_face,3-homo_dir-dir_profil);
                    trouve = 0;
                    for (j=0; j-(indicw_p+1)<0; j++)
                      {
                        if (j==Nap)
                          {
                            Cerr << "Erreur dans la valeur du nombre de points pour le traitement particulier"<<finl;
                            Cerr <<"qui est trop petite dans votre jeu de donnees=" << Nap << finl;
                            exit();
                          }
                        if(est_egal(y,Yw_p(i,j)))
                          {
                            corresp_w_p(i,num_face) = j;
                            compt_z_p(i,j) ++;
                            j=indicw_p+1;
                            trouve = 1;
                          }
                      }
                    if (trouve==0)
                      {
                        corresp_w_p(i,num_face) = indicw_p;
                        Yw_p(i,indicw_p)=y;
                        compt_z_p(i,indicw_p) ++;
                        indicw_p++;
                      }
                  }
                break;
              }
            default :
              {
                Cerr << "Cas de figure impossible..." << finl;
                exit();
                break;
              }
            }//Fin du switch
        }//Fin de boucle sur faces


      //Boucle sur les elements pour table de correspondance pour nu_t et temperature
      for (num_elem=0; num_elem<nb_elems; num_elem++)
        {
          //Point avant
          if(xp(num_elem,dir_profil)==xUVm(i))
            {
              y = xp(num_elem,3-homo_dir-dir_profil);
              trouve = 0;
              for (j=0; j-(indicuv_m+1)<0; j++)
                {
                  if(y==Yuv_m(i,j))
                    {
                      corresp_uv_m(i,num_elem) = j;
                      compt_uv_m(i,j) ++;
                      j=indicuv_m+1;
                      trouve = 1;
                    }
                }
              if (trouve==0)
                {
                  corresp_uv_m(i,num_elem) = indicuv_m;
                  Yuv_m(i,indicuv_m)=y;
                  compt_uv_m(i,indicuv_m) ++;
                  indicuv_m++;
                }
            }
          //Point apres
          if(xp(num_elem,dir_profil)==xUVp(i))
            {
              y = xp(num_elem,3-homo_dir-dir_profil);
              trouve = 0;
              for (j=0; j-(indicuv_p+1)<0; j++)
                {
                  if(y==Yuv_p(i,j))
                    {
                      corresp_uv_p(i,num_elem) = j;
                      compt_uv_p(i,j) ++;
                      j=indicuv_p+1;
                      trouve = 1;
                    }
                }
              if (trouve==0)
                {
                  corresp_uv_p(i,num_elem) = indicuv_p;
                  Yuv_p(i,indicuv_p)=y;
                  compt_uv_p(i,indicuv_p) ++;
                  indicuv_p++;
                }
            }
        }//Fin de boucle sur elements


      //Verification pour voir si avant et apres le profil on a le meme nombre de points...
      //Ce n'est pas le cas si par exemple on a une marche et un profil juste sur l'arete :
      //dans ce cas, on a plus de points du cote du bas de la marche que du cote du haut de la marche.
      if(indic_p!=indic_m)
        {
          Cerr << "indic_m = " << indic_m << " != indic_p = " << indic_p << finl;
          Cerr << "Problem : move profile at position " << positions(i) <<" !" << finl;
          exit();
        }
      if(indicv_p!=indicv_m)
        {
          Cerr << "indicv_m = " << indicv_m << " != indicv_p = " << indicv_p << finl;
          Cerr << "Problem : move profile at position " << positions(i) <<" !" << finl;
          exit();
        }
      if(indicw_p!=indicw_m)
        {
          Cerr << "indicw_m = " << indicw_m << " != indicw_p = " << indicw_p << finl;
          Cerr << "Problem : move profile at position " << positions(i) <<" !" << finl;
          exit();
        }
      if(indicuv_p!=indicuv_m)
        {
          Cerr << "indicuv_m = " << indicuv_m << " != indicuv_p = " << indicuv_p << finl;
          Cerr << "Problem : move profile at position " << positions(i) <<" !" << finl;
          exit();
        }

      Nxy(i) = indic_m;    // nombre de y pour umoy
      Nyy(i) = indicv_m;
      Nzy(i) = indicw_m;
      Nuv(i) = indicuv_m;  // nombre de y pour uv_moy (et pour nu_t!!)

    }//Fin de boucle sur probes
}








//####################################################################################################################

void Traitement_particulier_NS_Profils_VDF::calculer_moyenne_spatiale_nut(DoubleTab& u_moy, const IntTab& corresp, const IntTab& compt, const IntVect& tab_Nuv, const DoubleTab& xUV)
{
  int i,j;
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();

  const RefObjU& modele_turbulence = mon_equation.valeur().get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const DoubleTab& nu_t = mod_turb.viscosite_turbulente()->valeurs();

  int nb_elems = zone_VDF.zone().nb_elem();
  int num_elem;

  u_moy = 0.;
  for(i=0; i<n_probes; i++)
    {
      // Calcul de nut
      for (num_elem=0; num_elem<nb_elems; num_elem++)
        if(xp(num_elem,dir_profil)==xUV(i))
          {
            u_moy(i,corresp(i,num_elem)) += nu_t[num_elem];
          }
    } // on a parcouru tous les profils


  // POUR LE PARALLELE
  IntTab compt_p(compt);
  envoyer(compt_p,Process::me(),0,Process::me());

  DoubleTab u_moy_p(u_moy);
  envoyer(u_moy_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      IntTab compt_tot(compt);
      DoubleTab u_moy_tot(u_moy);

      compt_tot=0;
      u_moy_tot=0.;

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(compt_p,p,0,p);
          compt_tot+=compt_p;

          recevoir(u_moy_p,p,0,p);
          u_moy_tot+=u_moy_p;
        }

      for(i=0; i<n_probes; i++)
        for (j=0; j<tab_Nuv(i); j++)
          u_moy(i,j) = u_moy_tot(i,j)/compt_tot(i,j);

    }
  // FIN DU PARALLELE

}




void Traitement_particulier_NS_Profils_VDF::calculer_moyenne_spatiale_vitesse(DoubleTab& u_moy,DoubleTab& u_moy_2,DoubleTab& u_moy_3,DoubleTab& u_moy_4, const IntTab& corresp, const IntTab& compt, const IntVect& NN, const int& ori, const DoubleTab& xU)
{
  int i,j;
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xv = zone_VDF.xv();

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  int nb_faces = zone_VDF.nb_faces();
  const IntVect& orientation = zone_VDF.orientation();

  int num_face,ori_face;
  double vit;

  u_moy = 0.;
  u_moy_2 = 0.;

  for(i=0; i<n_probes; i++)
    {
      for(num_face=0; num_face<nb_faces; num_face++)
        {
          ori_face = orientation[num_face];
          if ((ori_face==ori)&&(xv(num_face,dir_profil)==xU(i)))
            {
              vit = vitesse[num_face];
              u_moy(i,corresp(i,num_face)) += vit;
              u_moy_2(i,corresp(i,num_face)) += vit*vit;
              u_moy_3(i,corresp(i,num_face)) += vit*vit*vit;
              u_moy_4(i,corresp(i,num_face)) += vit*vit*vit*vit;
            }
        }
    }// On a parcouru tous les profils

  // POUR LE PARALLELE
  IntTab compt_p(compt);
  envoyer(compt_p,Process::me(),0,Process::me());

  DoubleTab u_moy_p(u_moy);
  envoyer(u_moy_p,Process::me(),0,Process::me());

  DoubleTab u_moy_2_p(u_moy_2);
  envoyer(u_moy_2_p,Process::me(),0,Process::me());

  DoubleTab u_moy_3_p(u_moy_3);
  envoyer(u_moy_3_p,Process::me(),0,Process::me());

  DoubleTab u_moy_4_p(u_moy_4);
  envoyer(u_moy_4_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      IntTab compt_tot(compt);
      DoubleTab u_moy_tot(u_moy);
      DoubleTab u_moy_2_tot(u_moy_2);
      DoubleTab u_moy_3_tot(u_moy_3);
      DoubleTab u_moy_4_tot(u_moy_4);

      compt_tot=0;
      u_moy_tot=0.;
      u_moy_2_tot=0.;
      u_moy_3_tot=0.;
      u_moy_4_tot=0.;

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(compt_p,p,0,p);
          compt_tot+=compt_p;

          recevoir(u_moy_p,p,0,p);
          u_moy_tot+=u_moy_p;

          recevoir(u_moy_2_p,p,0,p);
          u_moy_2_tot+=u_moy_2_p;

          recevoir(u_moy_3_p,p,0,p);
          u_moy_3_tot+=u_moy_3_p;

          recevoir(u_moy_4_p,p,0,p);
          u_moy_4_tot+=u_moy_4_p;
        }

      for(i=0; i<n_probes; i++)
        for (j=0; j<NN(i); j++)
          {
            u_moy(i,j) = u_moy_tot(i,j)/compt_tot(i,j);
            u_moy_2(i,j) = u_moy_2_tot(i,j)/compt_tot(i,j);
            u_moy_3(i,j) = u_moy_3_tot(i,j)/compt_tot(i,j);
            u_moy_4(i,j) = u_moy_4_tot(i,j)/compt_tot(i,j);
          }
    }
  // FIN DU PARALLELE

  // commente par guillaume
  //Calcul de uprime2 ou vprime2 ou wprime2
  //  for (j=0;j<NN(i);j++)
  //    u_moy_2(i,j) -= u_moy(i,j)*u_moy(i,j);
}





void Traitement_particulier_NS_Profils_VDF::calculer_moyenne_spatiale_uv(DoubleTab& uv_moy, const IntTab& corresp, const IntTab& compt, const IntVect& NN , const DoubleTab& xUV)
{
  int i,j;
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  int nb_elems = zone_VDF.zone().nb_elem();
  int num_elem;
  int face_u_0,face_u_1,face_v_0,face_v_1;
  double vitu,vitv;

  DoubleTab u_moy_cent(n_probes,Nap);
  u_moy_cent= 0.;
  DoubleTab v_moy_cent(n_probes,Nap);
  v_moy_cent= 0.;
  uv_moy = 0.;

  for(i=0; i<n_probes; i++)
    {
      // Calcul de uv
      for (num_elem=0; num_elem<nb_elems; num_elem++)
        {
          if(xp(num_elem,dir_profil)==xUV(i))
            {
              face_u_0 = elem_faces(num_elem,0);
              face_u_1 = elem_faces(num_elem,dimension);
              vitu = 0.5*(vitesse[face_u_0]+vitesse[face_u_1]);
              face_v_0 = elem_faces(num_elem,1);
              face_v_1 = elem_faces(num_elem,1+dimension);
              vitv = 0.5*(vitesse[face_v_0]+vitesse[face_v_1]);
              uv_moy(i,corresp(i,num_elem)) += vitu*vitv;
              u_moy_cent(i,corresp(i,num_elem)) += vitu;
              v_moy_cent(i,corresp(i,num_elem)) += vitv;
            }
        }
    } // On a parcouru tous les profils

  // POUR LE PARALLELE
  IntTab compt_p(compt);
  envoyer(compt_p,Process::me(),0,Process::me());

  DoubleTab uv_moy_p(uv_moy);
  envoyer(uv_moy_p,Process::me(),0,Process::me());

  DoubleTab u_moy_cent_p(u_moy_cent);
  envoyer(u_moy_cent_p,Process::me(),0,Process::me());

  DoubleTab v_moy_cent_p(v_moy_cent);
  envoyer(v_moy_cent_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      IntTab compt_tot(compt);
      DoubleTab uv_moy_tot(uv_moy);
      DoubleTab u_moy_cent_tot(u_moy_cent);
      DoubleTab v_moy_cent_tot(v_moy_cent);

      compt_tot=0;
      uv_moy_tot=0.;
      u_moy_cent_tot=0.;
      v_moy_cent_tot=0.;

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(compt_p,p,0,p);
          compt_tot+=compt_p;

          recevoir(uv_moy_p,p,0,p);
          uv_moy_tot+=uv_moy_p;

          recevoir(u_moy_cent_p,p,0,p);
          u_moy_cent_tot+=u_moy_cent_p;

          recevoir(v_moy_cent_p,p,0,p);
          v_moy_cent_tot+=v_moy_cent_p;
        }

      for(i=0; i<n_probes; i++)
        {
          for (j=0; j<NN(i); j++)
            {
              uv_moy(i,j) = uv_moy_tot(i,j)/compt_tot(i,j);
              u_moy_cent(i,j) = u_moy_cent_tot(i,j)/compt_tot(i,j);
              v_moy_cent(i,j) = v_moy_cent_tot(i,j)/compt_tot(i,j);
              uv_moy(i,j) *= -1.;
            }
          for (j=0; j<NN(i); j++)
            uv_moy(i,j) += u_moy_cent(i,j)*v_moy_cent(i,j);
        }
    }
  // FIN DU PARALLELE

}





void Traitement_particulier_NS_Profils_VDF::calculer_integrale_temporelle(DoubleTab& u_moy_temp, DoubleTab& umoy_m, DoubleTab& umoy_p, const DoubleVect& delta_m, const DoubleVect& delta_p)
{
  int i,j;
  double dt_v = mon_equation->schema_temps().pas_de_temps();
  DoubleTab umoy(u_moy_temp);

  for(i=0; i<n_probes; i++)
    {
      for(j=0; j<Nap; j++)
        {
          umoy(i,j)=umoy_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          umoy(i,j)+=umoy_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
        }
    }
  if(je_suis_maitre())
    ////u_moy_temp.ajoute(dt_v,umoy);
    u_moy_temp.ajoute_sans_ech_esp_virt(dt_v,umoy);
}




void Traitement_particulier_NS_Profils_VDF::ecriture_fichiers_moy_spat(const DoubleTab& umoy_m,const DoubleTab& umoy_2_m,const DoubleTab& umoy_3_m,const DoubleTab& umoy_4_m, const DoubleTab& umoy_p, const DoubleTab& umoy_2_p, const DoubleTab& umoy_3_p, const DoubleTab& umoy_4_p, const DoubleVect& delta_m, const DoubleVect& delta_p, const IntVect& NN, const DoubleTab& Y, const int& ori)
{
  int i,j;

  for(i=0; i<n_probes; i++)
    {
      Nom nom_fic ="./Space_Avg/Avg_vel_";
      switch(ori)
        {
        case 0:
          {
            nom_fic +="U_";
            break;
          }
        case 1:
          {
            nom_fic +="V_";
            break;
          }
        case 2:
          {
            nom_fic +="W_";
            break;
          }
        }
      switch(dir_profil)
        {
        case 0:
          {
            nom_fic +="X_";
            break;
          }
        case 1:
          {
            nom_fic +="Y_";
            break;
          }
        case 2:
          {
            nom_fic +="Z_";
            break;
          }
        }
      Nom pos  = Nom(positions(i));
      nom_fic += pos;
      nom_fic += "_t_";
      double tps = mon_equation->inconnue().temps();
      Nom temps  = Nom(tps);
      nom_fic   += temps;
      nom_fic   += ".dat";

      //Pour calculer les interpolations lineaires...
      DoubleTab umoy(umoy_m);
      DoubleTab umoy_2(umoy_2_m);
      DoubleTab umoy_3(umoy_3_m);
      DoubleTab umoy_4(umoy_4_m);

      for(j=0; j<NN(i); j++)
        {
          umoy(i,j)=umoy_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          umoy(i,j)+=umoy_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
          umoy_2(i,j)=umoy_2_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          umoy_2(i,j)+=umoy_2_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
          umoy_3(i,j)=umoy_3_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          umoy_3(i,j)+=umoy_3_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
          umoy_4(i,j)=umoy_4_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          umoy_4(i,j)+=umoy_4_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
        }
      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);
          double val_moy, val2,val3,val4;

          fic << "# Space Averaged Statistics" << finl ;
          fic << "# Y       <U>       Urms       S=MOY[(u-umoy)^3]/(MOY[(u-umoy)^2])^3/2       F=MOY[(u-umoy)^4]/(MOY[(u-umoy)^2])^2       " << finl ;

          for (j=0; j<NN(i)  ; j++)
            {
              val_moy = umoy(i,j);
              // Pour eviter NAN, on prend le dmax(,0):
              val2 = sqrt(dmax(umoy_2(i,j)-val_moy*val_moy,0));

              if (val2 ==0 )
                {
                  val3=0.;
                  val4=0.;
                }
              else
                {
                  val3 = umoy_3(i,j)+2*val_moy*val_moy*val_moy-3*umoy_2(i,j)*val_moy;
                  val3 = val3/pow(val2,3);

                  val4 = umoy_4(i,j)-4*umoy_3(i,j)*val_moy+6*umoy_2(i,j)*val_moy*val_moy-3*val_moy*val_moy*val_moy*val_moy;
                  val4 = val4/pow(val2,4);
                }

              if ((val_moy<0.)&&(dabs(val_moy)<1.e-10)) val_moy=0.;
              fic << Y (i,j) << "   " << umoy(i,j) << "   " << val2 << "   " << val3 << "   " << val4 << finl;
            }
          fic.flush();
          fic.close();
        } // Fin du si je suis maitre
    }//FIN boucle sur les probes
}




void Traitement_particulier_NS_Profils_VDF::ecriture_fichiers_moy_spat_1col(const DoubleTab& umoy_m, const DoubleTab& umoy_p, const DoubleTab& Y, const DoubleVect& delta_m, const DoubleVect& delta_p, const IntVect& NN, const int& ori)
{
  int i,j;
  Nom nom_fic;
  for(i=0; i<n_probes; i++)
    {
      switch(ori)
        {
        case 3:
          {
            nom_fic ="./Space_Avg/Avg_vel_uvp_";
            break;
          }
        case 4:
          {
            nom_fic ="./Space_Avg/Avg_nut_";
            break;
          }
        }
      switch(dir_profil)
        {
        case 0:
          {
            nom_fic +="X_";
            break;
          }
        case 1:
          {
            nom_fic +="Y_";
            break;
          }
        case 2:
          {
            nom_fic +="Z_";
            break;
          }
        }
      Nom pos  = Nom(positions(i));
      nom_fic += pos;
      nom_fic += "_t_";
      double tps = mon_equation->inconnue().temps();
      Nom temps = Nom(tps);
      nom_fic+= temps;
      nom_fic+= ".dat";
      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);

          //Pour calculer les interpolations lineaires...
          DoubleTab umoy(umoy_m);
          for(j=0; j<NN(i); j++)
            {
              umoy(i,j)=umoy_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              umoy(i,j)+=umoy_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
            }

          for (j=0; j<NN(i); j++)
            fic << Y (i,j) << " " << umoy(i,j) << finl;
          fic.flush();
          fic.close();
        } // Fin de si je suis maitre
    }//FIN boucle sur les probes
}






void Traitement_particulier_NS_Profils_VDF::ecriture_fichiers_moy_temp(const DoubleTab& umoy, const DoubleTab& umoy_2, const DoubleTab& umoy_3, const DoubleTab& umoy_4, const DoubleTab& Y, const IntVect& NN, const int& ori, const double& dt)
{
  int i,j;

  Nom nom_fic;
  for(i=0; i<n_probes; i++)
    {
      nom_fic ="./Time_Avg/Avg_time_vel_";
      switch(ori)
        {
        case 0:
          {
            nom_fic +="U_";
            break;
          }
        case 1:
          {
            nom_fic +="V_";
            break;
          }
        case 2:
          {
            nom_fic +="W_";
            break;
          }
        }
      switch(dir_profil)
        {
        case 0:
          {
            nom_fic +="X_";
            break;
          }
        case 1:
          {
            nom_fic +="Y_";
            break;
          }
        case 2:
          {
            nom_fic +="Z_";
            break;
          }
        }
      Nom pos  = Nom(positions(i));
      nom_fic += pos;
      nom_fic += "_t_";

      double tps = mon_equation->inconnue().temps();
      Nom temps = Nom(tps);
      nom_fic+= temps;
      nom_fic+= ".dat";
      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);
          double val_moy,val2,val3,val4;

          fic << "# Time Averaged Statistics" << finl ;
          fic << "# Y       <U>       Urms       S=MOY[(u-umoy)^3]/(MOY[(u-umoy)^2])^3/2       F=MOY[(u-umoy)^4]/(MOY[(u-umoy)^2])^2       " << finl ;

          for (j=0; j<NN(i)  ; j++)
            {
              val_moy = umoy(i,j)/dt;
              val2 = umoy_2(i,j)/dt -val_moy*val_moy;
              // Pour eviter NAN, on prend le dmax(,0):
              val2 = sqrt(dmax(val2,0));

              if (val2 ==0 )
                {
                  val3=0.;
                  val4=0.;
                }
              else
                {
                  val3 = umoy_3(i,j)/dt+2*val_moy*val_moy*val_moy-3*umoy_2(i,j)/dt*val_moy;
                  val3 = val3/pow(val2,3);

                  val4 = umoy_4(i,j)/dt-4*umoy_3(i,j)/dt*val_moy+6*umoy_2(i,j)/dt*val_moy*val_moy-3*val_moy*val_moy*val_moy*val_moy;
                  val4 = val4/pow(val2,4);
                }
              if ((val_moy<0.)&&(dabs(val_moy)<1.e-10)) val_moy=0.;
              fic << Y (i,j) << "   " << val_moy << "   "  << val2 <<  "   " << val3 <<  "   " << val4 << finl;
            }
          fic.flush();
          fic.close();
        } // Fin de si je suis maitre
    }//FIN boucle sur les probes
}




void Traitement_particulier_NS_Profils_VDF::ecriture_fichiers_moy_temp_1col(const DoubleTab& umoy, const DoubleTab& Y, const IntVect& NN, const int& ori, const double& dt)
{
  int i,j;
  Nom nom_fic;
  for(i=0; i<n_probes; i++)
    {
      switch(ori)
        {
        case 3:
          {
            nom_fic ="./Time_Avg/Avg_time_vel_uvp_";
            break;
          }
        case 4:
          {
            nom_fic ="./Time_Avg/Avg_time_nut_";
            break;
          }
        }
      switch(dir_profil)
        {
        case 0:
          {
            nom_fic +="X_";
            break;
          }
        case 1:
          {
            nom_fic +="Y_";
            break;
          }
        case 2:
          {
            nom_fic +="Z_";
            break;
          }
        }
      Nom pos  = Nom(positions(i));
      nom_fic += pos;
      nom_fic += "_t_";
      double tps = mon_equation->inconnue().temps();
      Nom temps = Nom(tps);
      nom_fic+= temps;
      nom_fic+= ".dat";
      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);

          for (j=0; j<NN(i)  ; j++)
            fic << Y (i,j) << " " << umoy(i,j)/dt << finl;
          fic.flush();
          fic.close();
        } // Fin de si je suis maitre
    }//FIN boucle sur les probes
}

