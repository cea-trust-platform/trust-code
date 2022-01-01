/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Traitement_particulier_NS_Profils_thermo_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Traitement_particulier
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_Profils_thermo_VDF.h>
#include <Zone_VDF.h>
#include <Probleme_base.h>
#include <LecFicDistribueBin.h>
#include <EcrFicCollecteBin.h>
#include <Navier_Stokes_std.h>
#include <Convection_Diffusion_Temperature.h>
#include <Convection_Diffusion_Chaleur_QC.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur(Traitement_particulier_NS_Profils_thermo_VDF,"Traitement_particulier_NS_Profils_thermo_VDF",Traitement_particulier_NS_Profils_VDF);

Traitement_particulier_NS_Profils_thermo_VDF::Traitement_particulier_NS_Profils_thermo_VDF():oui_repr_stats_thermo(0)
{
}
// Description: Classe permettant de faire les statistiques sur le champs
//              de temperature avec des profils dans un domaine homogene dans une direction.
// Precondition: Il faut traiter un probleme Thermohydraulique
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
Sortie& Traitement_particulier_NS_Profils_thermo_VDF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_Profils_thermo_VDF::readOn(Entree& is)
{
  return is;
}




Entree& Traitement_particulier_NS_Profils_thermo_VDF::lire(Entree& is)
{
  return Traitement_particulier_NS_Profils_VDF::lire(is);
}




Entree& Traitement_particulier_NS_Profils_thermo_VDF::lire(const Motcle& motlu, Entree& is)
{
  // Par defaut on ne fait pas de statistiques sur la temperature.

  if (motlu=="stats_thermo")
    {
      //Statistiques sur le champ de temperature
      oui_stats_thermo = 1;
    }
  else
    {
      if(motlu=="reprise_thermo")
        {
          // Reprise des statistiques pour le champs de temperature
          oui_repr_stats_thermo = 1; // On veut reprendre les statistiques sur la temperature
          is  >> fich_repr_stats_thermo ;  // Indication du nom du fichier de reprise des stats pour le champs de temperature
          Cerr << "On reprend les statistiques sur le champs thermique" << finl;
        }
      else
        {
          if(motlu=="tmin_tmax")
            {
              // On demande les max et min de temperature dans un fichier
              tmin_tmax = 1;
              Cerr << "User asked to monitor min and max of temperature" << finl;
              Cerr << "in channel flow computation." << finl;
            }
          else
            {
              return Traitement_particulier_NS_Profils_VDF::lire(motlu,is);
            }
        }
    }
  return is;
}





void Traitement_particulier_NS_Profils_thermo_VDF::associer_eqn(const Equation_base& eq_ns )
{
  Traitement_particulier_NS_Profils_VDF::associer_eqn(eq_ns);

  const Probleme_base& pb = mon_equation.valeur().probleme();
  int flag=0;
  for(int i=0; i<pb.nombre_d_equations(); i++)
    {
      if(sub_type(Convection_Diffusion_Temperature,pb.equation(i)))
        {
          mon_equation_NRJ = ref_cast(Convection_Diffusion_Temperature,pb.equation(i));
          flag=1;
        }
      else if(sub_type(Convection_Diffusion_Chaleur_QC,pb.equation(i)))
        {
          mon_equation_NRJ = ref_cast(Convection_Diffusion_Chaleur_QC,pb.equation(i));
          flag=1;
        }

    }
  if (flag==0)
    {
      Cerr << "Error : Equation of NRJ was not found..." << finl;
      Cerr << "User can not ask for statistics on temperature" << finl;
      Cerr << "if we do not solve a heat equation." << finl;
      Cerr << "Try to remove the '_thermo' part at key-word 'Profils'" << finl;
      exit();
    }
}





// #################### Postraitement Statistique du Champ de Temperature #########

void Traitement_particulier_NS_Profils_thermo_VDF::post_traitement_particulier()
{
  Traitement_particulier_NS_Profils_VDF::post_traitement_particulier();

  DoubleTab Tmoy_m(n_probes,Nap);
  Tmoy_m=0.;
  DoubleTab Trms_m(n_probes,Nap);
  Trms_m=0.;
  DoubleTab upTp_m(n_probes,Nap);
  upTp_m=0.;
  DoubleTab vpTp_m(n_probes,Nap);
  vpTp_m=0.;
  DoubleTab wpTp_m(n_probes,Nap);
  wpTp_m=0.;

  DoubleTab Tmoy_p(n_probes,Nap);
  Tmoy_p=0.;
  DoubleTab Trms_p(n_probes,Nap);
  Trms_p=0.;
  DoubleTab upTp_p(n_probes,Nap);
  upTp_p=0.;
  DoubleTab vpTp_p(n_probes,Nap);
  vpTp_p=0.;
  DoubleTab wpTp_p(n_probes,Nap);
  wpTp_p=0.;

  double tps = mon_equation->inconnue().temps();


  // On ne fait des statistiques sur le champ thermique que si l'on fait des statistiques sur le champ dynamique en meme temps.
  if ((oui_u_inst != 0)&&(oui_stats_thermo != 0))
    {
      calculer_moyennes_spatiales_thermo(Tmoy_m,Trms_m,upTp_m,vpTp_m,wpTp_m,corresp_uv_m,compt_uv_m,Nuv,xUVm);
      calculer_moyennes_spatiales_thermo(Tmoy_p,Trms_p,upTp_p,vpTp_p,wpTp_p,corresp_uv_p,compt_uv_p,Nuv,xUVp);

      static double temps_dern_post_inst = -100.;
      if (dabs(tps-temps_dern_post_inst)>=dt_post_inst)
        {
          ecriture_fichier_moy_spat_thermo(Tmoy_m,Trms_m,upTp_m,vpTp_m,wpTp_m,Tmoy_p,Trms_p,upTp_p,vpTp_p,wpTp_p,Yuv_m,Nuv,delta_UVm, delta_UVp);

          temps_dern_post_inst = tps;
        }
    }


  // Moyennes temporelles :

  // On ne fait des statistiques sur le champ thermique que si l'on fait des statistiques sur le champ dynamique en meme temps.
  if ((oui_u_inst != 0)&&(oui_stats_thermo != 0)&&(oui_stat != 0))
    {
      double tpsbis = mon_equation->inconnue().temps();
      if ((tpsbis>=temps_deb)&&(tpsbis<=temps_fin))
        {
          static int init_stat_temps = 0;
          if((init_stat_temps==0)&&( oui_repr != 1))  // si ce n est pas une reprise : sinon valeurs lues dans le fichier
            {
              double dt_v = mon_equation->schema_temps().pas_de_temps();
              temps_deb = tpsbis-dt_v;
              init_stat_temps++;
            }

          calculer_integrales_temporelles(Tmoy_temp,Tmoy_m,Tmoy_p,delta_UVm,delta_UVp);
          calculer_integrales_temporelles(Trms_temp,Trms_m,Trms_p,delta_UVm,delta_UVp);
          calculer_integrales_temporelles(upTp_temp,upTp_m,upTp_p,delta_UVm,delta_UVp);
          calculer_integrales_temporelles(vpTp_temp,vpTp_m,vpTp_p,delta_UVm,delta_UVp);
          calculer_integrales_temporelles(wpTp_temp,wpTp_m,wpTp_p,delta_UVm,delta_UVp);

          static double temps_dern_post_stat = -100.;
          if (dabs(tpsbis-temps_dern_post_stat)>=dt_post_stat)
            {
              double dt = tpsbis-temps_deb;
              ecriture_fichier_moy_temp_thermo(Tmoy_temp,Trms_temp,upTp_temp,vpTp_temp,wpTp_temp,Yuv_m,dt,Nuv);
              temps_dern_post_stat = tpsbis;
            }
        }
    }
}





// #################### Calcul Moyennes #################################

void Traitement_particulier_NS_Profils_thermo_VDF::calculer_moyennes_spatiales_thermo(DoubleTab& tmoy, DoubleTab& trms, DoubleTab& uptp, DoubleTab& vptp, DoubleTab& wptp, const IntTab& corresp, const IntTab& compt, const IntVect& NN, const DoubleTab& xUV)
{

  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const IntTab& elem_faces = zone_VDF.elem_faces();

  // Pour trouver les coordonnees des points de temperature max et min :
  const DoubleTab& xp = zone_VDF.xp();

  // Le nombre d'elements de la zone VDF.
  int nb_elems = zone_VDF.zone().nb_elem();

  // On veut acceder aux valeurs des 3 composantes de la vitesse a partir de mon_equation.
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  // On veut acceder aux valeurs de la temperature a partir de mon_equation_NRJ.
  const DoubleTab& Temp = mon_equation_NRJ.valeur().inconnue().valeurs();



  // Ces 3 vecteurs comportent les valeurs des 3 composantes U, V et W moyennees au centre des elements et sur tout l'espace.
  // Ils correspondent a <U>, <V> et <W>.
  DoubleTab u_moy_cent(n_probes,Nap);
  u_moy_cent= 0.;
  DoubleTab v_moy_cent(n_probes,Nap);
  v_moy_cent= 0.;
  DoubleTab w_moy_cent(n_probes,Nap);
  w_moy_cent= 0.;

  // t2moy correspond
  DoubleTab t2moy(n_probes,Nap);
  t2moy     = 0.;

  // vitu, vitv et vitw sont les valeurs des 3 composantes de la vitesse prises au centre de l'element,
  // c'est a dire moyennees.
  double vitu,vitv,vitw;

  // Pour verification des bornes de temperature en canal plan
  double Tmin=1000000000.,Tmax=0.;
  int elem_min=0, elem_max=0;

  // On se sert de face_ui_j pour avoir le numero de la face portant la composante de la vitesse ui
  // et 0 et 1 etant les deux faces qui se font face pour faire une moyenne au centre de l'element.
  // On doit procede comme ca entre autre parceque l'on va parcourir les elements.
  int face_u_0,face_u_1,face_v_0,face_v_1,face_w_0,face_w_1;

  int i,j,deja_fait=0;
  int num_elem;

  // tmoy est la moyenne de T : <T>(y,t)
  tmoy = 0.;
  // trms est l'ecart-type de la temperature : sqrt(<Tp^2>)(y,t)=<T^2>-<T>^2
  trms = 0.;
  // uptp : <U><T>-<UT>
  uptp = 0.;
  // vptp : <V><T>-<VT>
  vptp = 0.;
  // wptp : <W><T>-<WT>
  wptp = 0.;

  for(i=0; i<n_probes; i++)
    {
      // On parcourt tous les elements pour faire toutes les moyennes au centre des elements.
      for (num_elem=0; num_elem<nb_elems; num_elem++)
        {
          // <T>
          if(xp(num_elem,dir_profil)==xUV(i))
            {
              tmoy(i,corresp(i,num_elem)) += Temp[num_elem];
              face_u_0 = elem_faces(num_elem,0);
              face_u_1 = elem_faces(num_elem,dimension);
              vitu = 0.5*(vitesse[face_u_0]+vitesse[face_u_1]);

              face_v_0 = elem_faces(num_elem,1);
              face_v_1 = elem_faces(num_elem,dimension+1);
              vitv = 0.5*(vitesse[face_v_0]+vitesse[face_v_1]);

              face_w_0 = elem_faces(num_elem,2);
              face_w_1 = elem_faces(num_elem,dimension+2);
              vitw = 0.5*(vitesse[face_w_0]+vitesse[face_w_1]);
              // <U>
              u_moy_cent(i,corresp(i,num_elem)) += vitu;
              // <V>
              v_moy_cent(i,corresp(i,num_elem)) += vitv;
              // <W>
              w_moy_cent(i,corresp(i,num_elem)) += vitw;
              // Trms : en fait ici on calcul <T^2> mais ensuite on va soustraire la partie <T>^2
              trms(i,corresp(i,num_elem)) += Temp[num_elem]*Temp[num_elem];
              // On calcule <uT>, <vT>, et <wT>
              uptp(i,corresp(i,num_elem)) += vitu*Temp[num_elem];
              vptp(i,corresp(i,num_elem)) += vitv*Temp[num_elem];
              wptp(i,corresp(i,num_elem)) += vitw*Temp[num_elem];
            }

          // Pour trouver les temperatures max et min dans l'ecoulement.
          if(Temp[num_elem]<Tmin)
            {
              Tmin=Temp[num_elem];
              elem_min=num_elem;
            }
          if(Temp[num_elem]>Tmax)
            {
              Tmax=Temp[num_elem];
              elem_max=num_elem;
            }

        }//FIN boucle sur les elements

      if((tmin_tmax==1)&&(deja_fait==0))
        {
          SFichier fic1("T_min_max.dat",ios::app);
          double tps = mon_equation->inconnue().temps();
          fic1 << tps << " " << Tmin << " " << xp(elem_min,0) << " " << xp(elem_min,1);
          fic1 << " " << Tmax << " " << xp(elem_max,0) << " " << xp(elem_max,1) <<finl;
          fic1.flush();
          fic1.close();
          deja_fait=1;
        }
    } // On a parcouru tous les profils

  // NN correspond au nombre de valeurs differentes pour Y, c'est a dire le nombre de points de la courbe finale apres moyenne.

  // compt[j] correspond au nombre d'elements qui ont ete utilises pour calculer une meme valeur de temperature moyenne,
  // ie le nombre d'elements ayant la meme coordonnee Y.
  // POUR LE PARALLELE !!
  IntTab compt_p(compt);
  envoyer(compt_p,Process::me(),0,Process::me());

  DoubleTab tmoy_p(tmoy);
  envoyer(tmoy_p,Process::me(),0,Process::me());

  DoubleTab trms_p(trms);
  envoyer(trms_p,Process::me(),0,Process::me());

  DoubleTab uptp_p(uptp);
  envoyer(uptp_p,Process::me(),0,Process::me());

  DoubleTab vptp_p(vptp);
  envoyer(vptp_p,Process::me(),0,Process::me());

  DoubleTab wptp_p(wptp);
  envoyer(wptp_p,Process::me(),0,Process::me());

  DoubleTab u_moy_cent_p(u_moy_cent);
  envoyer(u_moy_cent_p,Process::me(),0,Process::me());

  DoubleTab v_moy_cent_p(v_moy_cent);
  envoyer(v_moy_cent_p,Process::me(),0,Process::me());

  DoubleTab w_moy_cent_p(w_moy_cent);
  envoyer(w_moy_cent_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      IntTab compt_tot(compt);
      DoubleTab tmoy_tot(tmoy);
      DoubleTab trms_tot(trms);
      DoubleTab uptp_tot(uptp);
      DoubleTab vptp_tot(vptp);
      DoubleTab wptp_tot(wptp);
      DoubleTab u_moy_cent_tot(u_moy_cent);
      DoubleTab v_moy_cent_tot(v_moy_cent);
      DoubleTab w_moy_cent_tot(w_moy_cent);

      compt_tot=0;
      tmoy_tot=0.;
      trms_tot=0.;
      uptp_tot=0.;
      vptp_tot=0.;
      wptp_tot=0.;
      u_moy_cent_tot=0.;
      v_moy_cent_tot=0.;
      w_moy_cent_tot=0.;

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(compt_p,p,0,p);
          compt_tot+=compt_p;

          recevoir(tmoy_p,p,0,p);
          tmoy_tot+=tmoy_p;

          recevoir(trms_p,p,0,p);
          trms_tot+=trms_p;

          recevoir(uptp_p,p,0,p);
          uptp_tot+=uptp_p;

          recevoir(vptp_p,p,0,p);
          vptp_tot+=vptp_p;

          recevoir(wptp_p,p,0,p);
          wptp_tot+=wptp_p;

          recevoir(u_moy_cent_p,p,0,p);
          u_moy_cent_tot+=u_moy_cent_p;

          recevoir(v_moy_cent_p,p,0,p);
          v_moy_cent_tot+=v_moy_cent_p;

          recevoir(w_moy_cent_p,p,0,p);
          w_moy_cent_tot+=w_moy_cent_p;


        }

      for(i=0; i<n_probes; i++)
        {
          for (j=0; j<NN(i); j++)
            {
              tmoy(i,j) = tmoy_tot(i,j) / compt_tot(i,j);
              trms(i,j) = trms_tot(i,j) / compt_tot(i,j);
              uptp(i,j) = uptp_tot(i,j) / compt_tot(i,j);
              vptp(i,j) = vptp_tot(i,j) / compt_tot(i,j);
              wptp(i,j) = wptp_tot(i,j) / compt_tot(i,j);
              u_moy_cent(i,j) = u_moy_cent_tot(i,j) / compt_tot(i,j);
              v_moy_cent(i,j) = v_moy_cent_tot(i,j) / compt_tot(i,j);
              w_moy_cent(i,j) = w_moy_cent_tot(i,j) / compt_tot(i,j);

              trms(i,j) -= tmoy(i,j)*tmoy(i,j);
              uptp(i,j) -= u_moy_cent(i,j)*tmoy(i,j);
              vptp(i,j) -= v_moy_cent(i,j)*tmoy(i,j);
              wptp(i,j) -= w_moy_cent(i,j)*tmoy(i,j);
            }
        }// Fin boucle sur les profils

    }// FIN Parallele

}



// #################### Calcul Integrale Temporelle ###############################

void Traitement_particulier_NS_Profils_thermo_VDF::calculer_integrales_temporelles(DoubleTab& moy_temp, const DoubleTab& moy_spat_m, const DoubleTab& moy_spat_p, const DoubleVect& delta_m, const DoubleVect& delta_p)
{
  int i,j;
  double dt_v = mon_equation->schema_temps().pas_de_temps();
  DoubleTab moy(moy_temp);
  for(i=0; i<n_probes; i++)
    {
      for(j=0; j<Nap; j++)
        {
          moy(i,j)=moy_spat_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
          moy(i,j)+=moy_spat_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
        }
    }
  if(je_suis_maitre())
    ////moy_temp.ajoute(dt_v,moy);
    moy_temp.ajoute_sans_ech_esp_virt(dt_v,moy);

}





// #################### Ecriture Moyennes Spatiales dans Fichier ##################


void Traitement_particulier_NS_Profils_thermo_VDF::ecriture_fichier_moy_spat_thermo(const DoubleTab& Tmoy_m, const DoubleTab& Trms_m, const DoubleTab& upTp_m, const DoubleTab& vpTp_m, const DoubleTab& wpTp_m, const DoubleTab& Tmoy_p, const DoubleTab& Trms_p, const DoubleTab& upTp_p, const DoubleTab& vpTp_p, const DoubleTab& wpTp_p, const DoubleTab& Y, const IntVect& NN, const DoubleVect& delta_m,  const DoubleVect& delta_p)
{

  int i,j;
  double tps = mon_equation->inconnue().temps();
  Nom temps = Nom(tps);

  for(i=0; i<n_probes; i++)
    {
      Nom nom_fic = "./Space_Avg/Avg_temp_";
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
      nom_fic += temps;
      nom_fic += ".dat";

      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);
          fic << "# Space Averaged Statistics" << finl ;
          fic << "# Y       <T>       Trms       <upTp>       -<vpTp>       -<wpTp>" << finl ;

          DoubleTab Tmoy(Tmoy_m);
          DoubleTab Trms(Trms_m);
          DoubleTab upTp(upTp_m);
          DoubleTab vpTp(vpTp_m);
          DoubleTab wpTp(wpTp_m);

          for(j=0; j<NN(i); j++)
            {
              Tmoy(i,j)=Tmoy_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              Tmoy(i,j)+=Tmoy_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
              Trms(i,j)=Trms_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              Trms(i,j)+=Trms_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
              upTp(i,j)=upTp_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              upTp(i,j)+=upTp_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
              vpTp(i,j)=vpTp_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              vpTp(i,j)+=vpTp_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
              wpTp(i,j)=wpTp_m(i,j)*delta_m(i)/(delta_m(i)+delta_p(i));
              wpTp(i,j)+=wpTp_p(i,j)*delta_p(i)/(delta_m(i)+delta_p(i));
            }

          for (j=0; j<NN(i); j++)
            fic << Y(i,j) << " " << Tmoy(i,j) << " " << sqrt(dmax(Trms(i,j),0)) << " " << upTp(i,j) << " " << -vpTp(i,j) << " " << -wpTp(i,j) << finl;
          fic.flush();
          fic.close();
        } //Fin du si je suis maitre
    }//FIN de boucle sur les probes
}





// #################### Ecriture Moyennes temporelles dans Fichier ################

void Traitement_particulier_NS_Profils_thermo_VDF::ecriture_fichier_moy_temp_thermo(const DoubleTab& Tmoy, const DoubleTab& Trms, const DoubleTab& upTp, const DoubleTab& vpTp, const DoubleTab& wpTp, const DoubleTab& Y, const double& dt, const IntVect& NN)
{

  int i,j;
  for(i=0; i<n_probes; i++)
    {
      Nom nom_fic = "./Time_Avg/Avg_time_temp_";
      double tps = mon_equation->inconnue().temps();

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
      nom_fic +="_t_";

      Nom temps = Nom(tps);
      nom_fic+= temps;

      nom_fic+=".dat";

      if(je_suis_maitre())
        {
          SFichier fic (nom_fic);
          fic << "# Time Averaged Statistics" << finl ;
          fic << "# Y       <T>       Trms       <upTp>       -<vpTp>       -<wpTp>" << finl ;


          for (j=0; j<NN(i)  ; j++)
            fic << Y(i,j) << " " << Tmoy(i,j)/dt << " " << sqrt(dmax(Trms(i,j)/dt,0)) << " " << upTp(i,j)/dt << " " << -vpTp(i,j)/dt << " " << -wpTp(i,j)/dt << finl;
          fic.flush();
          fic.close();
        }
    }//FIN boucle sur les probes
}





// #################### Sauvegarde des statistiques temporelles ###################

void Traitement_particulier_NS_Profils_thermo_VDF::sauver_stat() const
{

  Traitement_particulier_NS_Profils::sauver_stat();
  double tps = mon_equation->inconnue().temps();

  if (  (oui_stat == 1)&&(tps>=temps_deb)&&(tps<=temps_fin) )
    {
      Cerr << "In Traitement_particulier_NS_Profils_thermo_VDF::sauver_stat" << finl;
      // On sauve la somme (sans diviser par dt)
      int i,j;
      Nom temps = Nom(tps);
      Nom fich_sauv_temp ="temperature_field_avg_time_";
      fich_sauv_temp+=temps;
      fich_sauv_temp+=".sauv";

      // On sauve u_moy!!
      EcrFicCollecteBin fict (fich_sauv_temp);
      fict << temps << finl;
      for(i=0; i<n_probes; i++)
        {
          fict << Nuv(i) << finl;
          for (j=0; j<Nuv(i); j++)
            {
              fict << Tmoy_temp(i,j) << " " << finl;
              fict << Trms_temp(i,j) << " " << finl;
              fict << upTp_temp(i,j) << " " << finl;
              fict << vpTp_temp(i,j) << " " << finl;
              fict << wpTp_temp(i,j) << " " << finl;
            }
        }
      fict << temps_deb << finl;
      fict.flush();
      fict.close();
      Cerr << "Temperature statistics saved at t=" << tps << finl;
    }
}





void Traitement_particulier_NS_Profils_thermo_VDF::reprendre_stat()
{
  Traitement_particulier_NS_Profils::reprendre_stat();
  double tps = mon_equation->inconnue().temps();
  int i,j;
  double ti;
  // double ti2;
  Nom temps;
  if (oui_stat == 1)
    {
      ifstream fict(fich_repr_stats_thermo);
      if (fict)
        {
          LecFicDistribueBin fict2 (fich_repr_stats_thermo);
          fict2 >> temps;
          Tmoy_temp.resize(n_probes,Nap);
          Trms_temp.resize(n_probes,Nap);
          upTp_temp.resize(n_probes,Nap);
          vpTp_temp.resize(n_probes,Nap);
          wpTp_temp.resize(n_probes,Nap);

          for(i=0; i<n_probes; i++)
            {
              fict2 >> Nuv(i);
              for (j=0; j<Nuv(i); j++)
                {
                  fict2 >> Tmoy_temp(i,j) ;
                  fict2 >> Trms_temp(i,j);
                  fict2 >> upTp_temp(i,j) ;
                  fict2 >> vpTp_temp(i,j) ;
                  fict2 >> wpTp_temp(i,j) ;
                }
            }
          fict2 >> ti ;
          Cerr << "ti=" << ti << finl;
          Nom chc_ti = Nom(ti), chc_temps_deb = Nom(temps_deb);
          Cerr << "chc_ti=" << chc_ti << "   chc_temps_deb=" << chc_temps_deb << finl;
          if (chc_ti!=chc_temps_deb)
            {
              if (ti > temps_deb)
                {
                  Cerr << "Pb de reprise des stats du champ thermique :" << finl;
                  Cerr << "Le temps de debut des stats demande " << temps_deb  << finl;
                  Cerr << "est inferieur a celui de debut des stats sauvees !" << ti << finl;
                  exit();
                }
              else
                {
                  if (temps_deb>=tps)
                    {
                      Cerr << "On recommence le calcul des stats thermiques a partir de t=" << temps_deb << "s." << finl;
                      Tmoy_temp = 0.;
                      Trms_temp = 0.;
                      upTp_temp = 0.;
                      vpTp_temp = 0.;
                      wpTp_temp = 0.;
                      oui_repr_stats_thermo = 0;
                    }
                  else
                    {
                      Cerr << "On a deja depasse le temps auquel vous voulez commencer les stats thermiques !" << finl;
                      exit();
                    }
                }
            }
          else
            {
              Cerr << "On continue le calcul des stats thermiques, debute a t=" <<  ti << "s." << finl;
              temps_deb = ti;
            }
        }
      else
        {
          if (tps<=temps_deb)
            {
              Cerr << "On n a pas encore debute le calcul des stats thermiques." << finl;
              oui_repr_stats_thermo = 0;
            }
          else
            {
              Cerr << "Il faut donner le fichier pour reprendre les stats thermiques !" << finl;
              exit();
            }
        }
    }
  else
    {
      //      Cerr << "Pas de calcul des stats commence." << finl;
      //      Cerr << "Pas de reprise des statistiques thermiques." << finl;
    }
}





void  Traitement_particulier_NS_Profils_thermo_VDF::init_calcul_moyenne(void)
{
  Traitement_particulier_NS_Profils_VDF::init_calcul_moyenne();
}


void  Traitement_particulier_NS_Profils_thermo_VDF::preparer_calcul_particulier(void)
{
  if ((oui_u_inst != 0)||(oui_profil_nu_t != 0))
    // On fait appel uniquement a la methode dans NS_Profils_VDF pour initialiser
    // le calcul des moyennes puisque on y fait uniquement la creation des tableaux
    // de correspondance et le dimensionnement des grandeurs Yuv et Nuv.

    Traitement_particulier_NS_Profils_VDF::init_calcul_moyenne();

  if (oui_stat != 0)
    // Par contre, pour debuter le calcul des stats il faut faire un petit traitement
    // pour la thermo puisqu'il faut dimensionner les tableaux des moyennes temporelles
    // specifiques a la thermo, ainsi que ceux de l'hydraulique classique dans NS_Profils.

    init_calcul_stats();
}




void Traitement_particulier_NS_Profils_thermo_VDF::init_calcul_stats(void)
{
  Traitement_particulier_NS_Profils::init_calcul_stats();

  if (oui_repr_stats_thermo!=1)
    {
      // L'utilisateur ne demande pas de reprise, c'est a dire qu'il n'y aura pas de lecture
      // dans un fichier et qu'il faut donc dimensionner nos tableaux nous meme pour debuter les statistiques.
      // Ces tableaux sont declares dans le Traitement_particulier_NS_Profils_thermo_VDF.h

      Tmoy_temp.resize(n_probes,Nap);
      Tmoy_temp=0;
      Trms_temp.resize(n_probes,Nap);
      Trms_temp=0;
      upTp_temp.resize(n_probes,Nap);
      upTp_temp=0;
      vpTp_temp.resize(n_probes,Nap);
      vpTp_temp=0;
      wpTp_temp.resize(n_probes,Nap);
      wpTp_temp=0;
    }
}
