/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Traitement_particulier_NS_canal.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/36
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_canal.h>
#include <LecFicDistribue.h>
#include <EcrFicCollecte.h>
#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd_base.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Zone_VF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_Cl_dis_base.h>
#include <Dirichlet_paroi_fixe.h>
#include <Schema_Temps_base.h>
#include <DoubleTrav.h>
#include <communications.h>
#include <Fluide_Incompressible.h>
#include <DoubleVect.h>
#include <Champ_Uniforme.h>

Implemente_base_sans_constructeur(Traitement_particulier_NS_canal,"Traitement_particulier_NS_canal",Traitement_particulier_NS_base);

Traitement_particulier_NS_canal::Traitement_particulier_NS_canal()
{
  oui_profil_nu_t = 0;
  oui_profil_Temp = 0;
  oui_repr = 0;
  oui_pulse = 0;

  dt_impr_moy_spat=1e6;
  dt_impr_moy_temp=1e6;
  temps_deb=1e6;
  temps_fin=1e6;
  debut_phase=1e6;
  ind_phase=0;

  Nval=12;
  Nphase=1;
  Nb_ech_phase.resize(1);
  Nb_ech_phase(0)=1;
}


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
Sortie& Traitement_particulier_NS_canal::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_canal::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_canal::lire(Entree& is)
{
  if(!sub_type(Navier_Stokes_std,mon_equation.valeur()))
    {
      Cerr << " Traitement_particulier_canal has to be called from a Navier_Stokes problem " << finl;
      exit();
    }

  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(7);
      les_mots[0] = "dt_impr_moy_spat";
      les_mots[1] = "dt_impr_moy_temp";
      les_mots[2] = "debut_stat";
      les_mots[3] = "fin_stat";
      les_mots[4] = "reprise";
      les_mots[5] = "pulsation_w";
      les_mots[6] = "nb_points_par_phase";

      is >> motlu;
      while(motlu != accfermee)
        {
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                is >> dt_impr_moy_spat;      // intervalle de temps de sorties des moyennes spatiales
                Cerr << "Spatial averages are printed for : dt_impr_moy_spat = " << dt_impr_moy_spat << finl;
                break;
              }
            case 1 :
              {
                is >> dt_impr_moy_temp;      // intervalle de temps de sorties des moyennes temporelles
                Cerr << "Temporal averages are printed for : dt_impr_moy_temp = " << dt_impr_moy_temp << finl;
                break;
              }
            case 2 :
              {
                is >> temps_deb;             // temps de debut des moyennes temporelles
                Cerr << "Temporal averages start at : temps_deb = " << temps_deb << finl;
                break;
              }
            case 3 :
              {
                is >> temps_fin;             // temps de debut des moyennes temporelles
                Cerr << "Temporal averages finish at : temps_fin = " << temps_fin << finl;
                break;
              }
            case 4 :
              {
                oui_repr = 1;
                is  >> fich_repr ;             // indication du nom du fichier de reprise des stats
                Cerr << "The time statistics file is : " << fich_repr << finl;
                break;
              }
            case 5 :
              {
                oui_pulse = 1;
                is  >> w ;                     // pulsation
                freq = w/(2.*3.141592653); // frequence associee
                break;
              }
            case 6 :
              {
                oui_pulse = 1;
                is  >> Nphase ;            // nombre de points pour decrire une phase
                break;
              }
            default :
              {
                Cerr << motlu << " is not a keyword for Traitement_particulier_NS_canal " << finl;
                Cerr << "Since the TRUST v1.5, the syntax of Canal option has changed." << finl;
                Cerr << "Check the reference manual." << finl;
                exit();
              }
            }
          is >> motlu;
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Error while reading in Traitement_particulier_NS_canal" << finl;;
          Cerr << "We were expecting a }" << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_NS_canal" << finl;
      Cerr << "We were expecting a {" << finl;
      exit();
    }

  return is;
}

void Traitement_particulier_NS_canal::remplir_Tab_recap(IntTab& Tab_rec) const
{
// surchargeee dans VDF
  Cerr << "Traitement_particulier_NS_canal::remplir_Tab_recap ne marche pas pour le VEF" << finl;
//  Process::exit();
}

void Traitement_particulier_NS_canal::preparer_calcul_particulier()
{
  const RefObjU& modele_turbulence = mon_equation.valeur().get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
    {
      oui_profil_nu_t = 1;
      Nval=13;
    }

  if (mon_equation.valeur().probleme().nombre_d_equations()>1)
    try
      {
        Temp = mon_equation.valeur().probleme().equation(1).get_champ("temperature");
        oui_profil_Temp = 1 ;
        Nval=18;
      }
    catch (Champs_compris_erreur)
      {
      }

  remplir_Y(Y,compt,Ny); // renvoie vers Traitement_particulier_NS_canal_VDF ou Traitement_particulier_NS_canal_VEF
  remplir_Tab_recap(Tab_recap);
  remplir_reordonne_Y_tot(Y,Y_tot);

  int NN=Y_tot.size();
  compt_tot.resize(NN);
  val_moy_tot.resize(NN,Nval,1);

  if (oui_repr!=1)
    {
      val_moy_temp.resize(Y_tot.size(),Nval,1);
      val_moy_temp=0.;

      if (oui_pulse==1)
        {
          val_moy_phase.resize(Y_tot.size(),Nval,Nphase);
          val_moy_phase=0.;
          Nb_ech_phase.resize(Nphase);
          Nb_ech_phase=0;
        }
    }
}

void Traitement_particulier_NS_canal::remplir_reordonne_Y_tot(const DoubleVect& tab_Y, DoubleVect& tab_Y_tot) const
{
  DoubleVect Y_p(tab_Y);
  int j_tot=1;

  envoyer(Y_p, Process::me(), 0, Process::me());

  if(je_suis_maitre())
    {
      tab_Y_tot.resize(1);
      tab_Y_tot(0) = tab_Y(0);

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(Y_p,p,0,p);
          for (int k=0; k<Y_p.size(); k++)
            {
              if(!est_egal(Y_p(k),-100.)) // on recherche si Y_p(k) est deja contenu dans Y_tot
                {
                  int ok_new=1;

                  for (int j=0; j<tab_Y_tot.size(); j++)
                    if (est_egal(tab_Y_tot(j),Y_p(k)))
                      ok_new=0;

                  if(ok_new==1)
                    {
                      tab_Y_tot.resize(j_tot+1);
                      tab_Y_tot(j_tot) = Y_p(k);
                      j_tot++;
                    }
                }
            }
        }

      if(est_egal(tab_Y_tot(0),1.e12))
        {
          Cerr << "Probleme a la construction de Y_tot" << finl;
          exit();
        }

      tab_Y_tot.ordonne_array();
      // Envoi de Y_tot aux autres processeurs
      for(int p=1; p<Process::nproc(); p++)
        envoyer(tab_Y_tot, 0, p, 0);
    }
  else
    {
      recevoir(tab_Y_tot,0,Process::me(),0);
    }
}

void Traitement_particulier_NS_canal::reprendre_stat()
{
  if (je_suis_maitre())
    {
      Cerr << "Traitement_particulier_NS_canal::reprendre_stat!!" << flush << finl;

      if (oui_repr==1)
        {
          reprendre_stat_canal(val_moy_temp,fich_repr);
          Nval=val_moy_temp.dimension(1);
        }

      if (oui_repr==1 && oui_pulse==1)
        {
          Nom fich_repr_phase;
          fich_repr_phase  = fich_repr;
          fich_repr_phase +="_phase";

          reprendre_stat_canal(val_moy_phase,fich_repr_phase);
          Nphase=val_moy_phase.dimension(2);
        }
    }
}

void Traitement_particulier_NS_canal::reprendre_stat_canal(DoubleTab& val, const Nom& fichier)
{
  double tps,tps_deb_moy;
  int NN,Nv,Np;
  Nom temps;

  EFichier fic(fichier);
  fic.setf(ios::scientific);

  if(fic.fail())
    {
      Cerr << "Impossible d'ouvrir le fichier " << fichier << finl;
      exit();
    }

  fic >> tps;
  fic >> tps_deb_moy;
  fic >> debut_phase;
  fic >> ind_phase;
  fic >> NN;
  fic >> Nv;
  fic >> Np;

  val.resize(NN,Nv,Np);
  Nb_ech_phase.resize(Np);

  for (int k=0; k<Np; k++)
    fic >> Nb_ech_phase(k);

  for (int k=0; k<Np; k++)
    for (int j=0; j<Nv; j++)
      for (int i=0; i<NN; i++)
        fic >> val(i,j,k);

  if(!est_egal(tps_deb_moy,temps_deb,0.1))
    {
      Cerr << "ERREUR : le temps de debut des stats figurant dans le jeu de donnees differe de beaucoup" << finl;
      Cerr << "du temps defini dans le fichier de reprise : " << fichier << finl;
      exit();
    }

  temps_deb=tps_deb_moy;
}

void Traitement_particulier_NS_canal::sauver_stat() const
{
  double tps = mon_equation->inconnue().temps();

  if (je_suis_maitre() && (tps>=temps_deb) && (tps<=temps_fin) )
    {
      Cerr << "Traitement_particulier_NS_canal::sauver_stat!!" << flush << finl;

      Nom temps = Nom(tps);
      Nom fich_sauv1 ="val_moy_temp_";
      fich_sauv1+=temps;
      fich_sauv1+=".sauv";
      Nom fich_sauv2 = fich_sauv1;
      fich_sauv2+="_phase";

      sauver_stat_canal(val_moy_temp,fich_sauv1);
      if(oui_pulse==1)  sauver_stat_canal(val_moy_phase,fich_sauv2);
    }
}

void Traitement_particulier_NS_canal::sauver_stat_canal(const DoubleTab& val, const Nom& fichier) const
{
  double tps = mon_equation->inconnue().temps();
  int NN,Nv,Np;
  NN=val.dimension(0);
  Nv=val.dimension(1);
  Np=val.dimension(2);

  SFichier fic(fichier);
  fic.setf(ios::scientific);

  fic << tps << finl;
  fic << temps_deb << finl;
  fic << debut_phase << finl;
  fic << ind_phase << finl;
  fic << NN << finl;
  fic << Nv << finl;
  fic << Np << finl;

  for (int k=0; k<Np; k++)
    fic <<Nb_ech_phase(k) << finl;

  for (int k=0; k<Np; k++)
    for (int j=0; j<Nv; j++)
      for (int i=0; i<NN; i++)
        fic << val(i,j,k) << finl;
}

void Traitement_particulier_NS_canal::post_traitement_particulier()
{

  // Calcul des Moyennes spatiales
  //////////////////////////////////////////////////////////

  DoubleTrav val_moy(Ny,Nval);

  val_moy=0.;

  calculer_moyenne_spatiale_vitesse_rho_mu(val_moy);

  if (oui_profil_nu_t != 0)
    calculer_moyenne_spatiale_nut(val_moy);

  if (oui_profil_Temp != 0)
    calculer_moyenne_spatiale_Temp(val_moy);


  // Echange des donnees entre processeurs
  //////////////////////////////////////////////////////////

  DoubleVect Y_p(Y);
  DoubleVect compt_p(compt);
  DoubleTab val_moy_p(val_moy);

  const int NN = Y_tot.size();

  envoyer(Y_p,Process::me(),0,Process::me());
  envoyer(compt_p,Process::me(),0,Process::me());
  envoyer(val_moy_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      compt_tot=0;
      val_moy_tot=0.;
      int j;
      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(Y_p,p,0,p);
          recevoir(compt_p,p,0,p);
          recevoir(val_moy_p,p,0,p);

          int Y_p_size = Y_p.size();
          for (int i=0; i<Y_p_size; i++)
            {
              for (j=0; j<NN; j++)
                if(est_egal(Y_tot[j],Y_p[i])) break;

              compt_tot(j) += compt_p(i);

              for (int k=0; k<Nval; k++)
                val_moy_tot(j,k,0) += val_moy_p(i,k);
            }
        }

      for (int i=0; i<NN; i++)
        for (int k=0; k<Nval; k++)
          val_moy_tot(i,k,0) /= compt_tot[i];
    }


  // Calcul et ecriture des grandeurs parietales
  ///////////////////////////////////////////////////////////////////

  calcul_reynolds_tau();

  // Calcul des Moyennes temporelles
  //////////////////////////////////////////////////////////////////

  double tps = mon_equation->inconnue().temps();

  if(je_suis_maitre())
    {
      if ((tps>=temps_deb)&&(tps<=temps_fin))
        {
          static int init_stat_temps = 0;

          double dt_v = mon_equation->schema_temps().pas_de_temps();
          int NN2 = Y_tot.size();

          if(init_stat_temps==0 && oui_repr!=1) // sinon, les valeurs de val_moy_temp ont ete lues a partir de reprendre_stat()
            {
              temps_deb = tps-dt_v;

              val_moy_temp.resize(NN2,Nval,1);
              val_moy_temp=0.;

              init_stat_temps++;
            }

          for (int j=0; j<Nval; j++)
            for (int i=0; i<NN2; i++)
              {
                if ((j!=3) && (j!=4) && (j!=5) && (j!=6) && (j!=7) && (j!=8) && (j!=14) && (j!=15) && (j!=16) && (j!=17))
                  val_moy_temp(i,j,0)+= dt_v*val_moy_tot(i,j,0);

                if (j==3)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,3,0)-val_moy_tot(i,0,0)*val_moy_tot(i,0,0));
                if (j==4)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,4,0)-val_moy_tot(i,1,0)*val_moy_tot(i,1,0));
                if (j==5)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,5,0)-val_moy_tot(i,2,0)*val_moy_tot(i,2,0));

                if (j==6)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,6,0)-val_moy_tot(i,0,0)*val_moy_tot(i,1,0));
                if (j==7)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,7,0)-val_moy_tot(i,0,0)*val_moy_tot(i,2,0));
                if (j==8)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,8,0)-val_moy_tot(i,1,0)*val_moy_tot(i,2,0));


                if (j==14)
                  //val_moy_temp(i,j,0)+= dt_v*val_moy_tot(i,13,0)*val_moy_tot(i,13,0);
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,14,0)-val_moy_tot(i,13,0)*val_moy_tot(i,13,0));
                if (j==15)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,15,0)-val_moy_tot(i,0,0)*val_moy_tot(i,13,0));
                if (j==16)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,16,0)-val_moy_tot(i,1,0)*val_moy_tot(i,13,0));
                if (j==17)
                  val_moy_temp(i,j,0)+= dt_v*(val_moy_tot(i,17,0)-val_moy_tot(i,2,0)*val_moy_tot(i,13,0));

              }
        }
    }

  // Calcul des Moyennes de phases
  //////////////////////////////////////////////////////////////////

  if(je_suis_maitre() && oui_pulse==1)
    {
      if ((tps>=temps_deb)&&(tps<=temps_fin))
        {
          double dt_v = mon_equation->schema_temps().pas_de_temps();

          if ( (cos(w*tps) > cos(w*(tps+dt_v))) && (cos(w*tps) > cos(w*(tps-dt_v))) ) // debut d'une periode
            {
              debut_phase=tps;
              ind_phase=1;
            }

          if(ind_phase==1)
            {
              for(int k=0; k<Nphase; k++)
                {
                  double tps_k=debut_phase+k/(Nphase*freq);

                  if( (tps > tps_k-0.5*dt_v)  && (tps < tps_k+0.5*dt_v) ) // recherche de la k-phase correspondant
                    {
                      for (int j=0; j<Nval; j++)
                        for (int i=0; i<NN; i++)
                          val_moy_phase(i,j,k) += val_moy_tot(i,j,0);

                      Nb_ech_phase(k)++;

                      if(k==Nphase-1)  ind_phase=2; // marqueur pour ecriture de moyennes de phase (voir plus bas)
                    }
                }
            }
        }
    }

  // Ecriture des Moyennes spatiales et temporelles
  ///////////////////////////////////////////////////////////////

  if(je_suis_maitre())
    {
      double stationnaire_atteint=mon_equation->schema_temps().stationnaire_atteint();
      double tps_passe=mon_equation->schema_temps().temps_courant();
      double nb_pas_dt_max=mon_equation->schema_temps().nb_pas_dt_max();
      double nb_pas_dt=mon_equation->schema_temps().nb_pas_dt();
      double temps_max=mon_equation->schema_temps().temps_max();
      int impr_inst;
      if (dt_impr_moy_spat<=(tps-tps_passe))
        impr_inst=1;
      else
        {
          // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
          double i, j, epsilon = 1.e-8;
          modf(tps/dt_impr_moy_spat + epsilon, &i);
          modf(tps_passe/dt_impr_moy_spat + epsilon, &j);
          impr_inst=(i>j);
        }
      int impr_stat;
      if (dt_impr_moy_temp<=(tps-tps_passe))
        impr_stat=1;
      else
        {
          // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
          double i, j, epsilon = 1.e-8;
          modf(tps/dt_impr_moy_temp + epsilon, &i);
          modf(tps_passe/dt_impr_moy_temp + epsilon, &j);
          impr_stat=(i>j);
        }

      // sauvegarde periodique des moyennes spatiales

      if ((nb_pas_dt+1<=1) || impr_inst || (temps_max <= tps) || (nb_pas_dt_max <= nb_pas_dt+1) || stationnaire_atteint)
        {
          double dt = 1.;

          Nom fichier1 = "Moyennes_spatiales_vitesse_rho_mu_";
          Nom fichier2 = "Moyennes_spatiales_nut_";
          Nom fichier3 = "Moyennes_spatiales_Temp_";
          Nom temps = Nom(tps);

          fichier1 += temps;
          fichier2 += temps;
          fichier3 += temps;

          /*
            ecriture_fichiers_moy_vitesse_rho_mu(val_moy_tot,fichier1,dt,0);
            if (oui_profil_nu_t == 1)   ecriture_fichiers_moy_nut(val_moy_tot,fichier2,dt,0);
            if (oui_profil_Temp == 1)   ecriture_fichiers_moy_Temp(val_moy_tot,fichier3,dt,0);
          */

          ecriture_fichiers_moy_vitesse_rho_mu_old(val_moy_tot,fichier1,dt,0);
          if (oui_profil_nu_t == 1)   ecriture_fichiers_moy_nut(val_moy_tot,fichier2,dt,0);
          if (oui_profil_Temp == 1)   ecriture_fichiers_moy_Temp_old(val_moy_tot,fichier3,dt,0);
        }

      // sauvegarde periodique des moyennes temporelles

      if ((tps>temps_deb) && ((nb_pas_dt+1<=1) || impr_stat || (temps_max <= tps) || (nb_pas_dt_max <= nb_pas_dt+1) || stationnaire_atteint))
        {
          double dt = tps-temps_deb;

          Nom fichier1 = "Moyennes_temporelles_vitesse_rho_mu_";
          Nom fichier2 = "Moyennes_temporelles_nut_";
          Nom fichier3 = "Moyennes__temporelles_Temp_";
          Nom temps = Nom(tps);

          fichier1 += temps;
          fichier2 += temps;
          fichier3 += temps;

          ecriture_fichiers_moy_vitesse_rho_mu(val_moy_temp,fichier1,dt,0);
          if (oui_profil_nu_t == 1)   ecriture_fichiers_moy_nut(val_moy_temp,fichier2,dt,0);
          if (oui_profil_Temp == 1)   ecriture_fichiers_moy_Temp(val_moy_temp,fichier3,dt,0);
        }

      // sauvegarde en continue des moyennes temporelles

      if (tps>temps_deb)
        {
          double dt = tps-temps_deb;

          Nom fichier1 = "Moyennes_temporelles_vitesse_rho_mu";
          Nom fichier2 = "Moyennes_temporelles_nut";
          Nom fichier3 = "Moyennes__temporelles_Temp";

          ecriture_fichiers_moy_vitesse_rho_mu(val_moy_temp,fichier1,dt,0);
          if (oui_profil_nu_t == 1)   ecriture_fichiers_moy_nut(val_moy_temp,fichier2,dt,0);
          if (oui_profil_Temp == 1)   ecriture_fichiers_moy_Temp(val_moy_temp,fichier3,dt,0);
        }

      // sauvegarde en continue des moyennes de phase (en fin de cycle)

      if ( (ind_phase==2) && (tps>temps_deb) )
        {
          ind_phase=0;

          for(int k=0; k<Nphase; k++)
            {
              double dt = Nb_ech_phase(k);

              Nom fichier1 = "Moyennes_de_phase_vitesse_rho_mu_";
              Nom fichier2 = "Moyennes_de_phase_nut_";
              Nom fichier3 = "Moyennes_de_phase_Temp_";
              Nom phase = Nom(k);

              fichier1 += phase;
              fichier2 += phase;
              fichier3 += phase;


              ecriture_fichiers_moy_vitesse_rho_mu_old(val_moy_phase,fichier1,dt,k);
              if (oui_profil_nu_t == 1)   ecriture_fichiers_moy_nut(val_moy_phase,fichier2,dt,k);
              if (oui_profil_Temp == 1)   ecriture_fichiers_moy_Temp_old(val_moy_phase,fichier3,dt,k);
            }
        }
    }
}


void Traitement_particulier_NS_canal::calcul_reynolds_tau()
{
  // Aspects geometriques du maillage
  /////////////////////////////////////////////////////////
  // !!!!!  Hypotheses : maillage symetrique suivant la demi-hauteur et s'etendant de Y=0 a Y=H

  Nom nom_discr=mon_equation.valeur().discretisation().que_suis_je();
  // indice du premier point hors paroi
  int kmin=(nom_discr=="VEFPreP1B" || nom_discr=="VEF") ? 1 : 0;
  int kmax=Y_tot.size()-1-kmin;                         // indice du dernier point hors paroi
  double ymin=Y_tot(kmin);                              // position du premier point
  double ymax=Y_tot(kmax);                              // position du dernier point
  double hs2=0.5*(ymin+ymax);                           // demi-hauteur

  // check that the canal is in the good bounds
  if ( ymin < 0. || ymax < 0.)
    {
      Cerr << "Error Traitement_particulier_NS_canal" << finl;
      Cerr << "Invalid mesh: Y must be between 0 and H>0" << finl;
      Cerr << "In addition, the mesh must be symmetric along the half-height." << finl;
      exit() ;
    }

  // viscosite dynamique
  double mu_bas  = val_moy_tot(kmin,11,0);
  double mu_haut = val_moy_tot(kmax,11,0);

  // masse volumique
  double rho_bas  = val_moy_tot(kmin,10,0);
  double rho_haut = val_moy_tot(kmax,10,0);

  // cisaillement a la paroi
  double tauwb, tauwh, tauwm;
//  int nbfaces_tot=0;

  int nb_cl_diri=0;
  double tauw_diri_m=0.;
  double retau_diri_m=0.;
  double utau_diri_m=0.;

  int nb_cl_robin=0;
  double tauw_robin_m=0.;
  double retau_robin_m=0.;
  double utau_robin_m=0.;

  const RefObjU& modele_turbulence         = mon_equation.valeur().get_modele(TURBULENCE);
  const Equation_base& eqn                 = ref_cast(Equation_base,mon_equation.valeur()) ;
  const Zone_Cl_dis_base& zone_Cl_dis_base = ref_cast(Zone_Cl_dis_base,eqn.zone_Cl_dis().valeur());
  const Conds_lim& les_cl                  = zone_Cl_dis_base.les_conditions_limites();
  const Zone_VF& zone_VF                   = ref_cast(Zone_VF,eqn.zone_dis().valeur());
  double tps                               = mon_equation->inconnue().temps();

  double nb_pas_dt=mon_equation->schema_temps().nb_pas_dt();
  int reprise = mon_equation.valeur().probleme().reprise_effectuee();
  bool new_file = ( nb_pas_dt == 0 && reprise == 0 );
  IOS_OPEN_MODE mode;
  if (new_file == 0)
    mode = (ios::out | ios::app);
  else
    mode = ios::out;

  int nb_cl=les_cl.size();
  for (int num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = les_cl[num_cl];
      if (la_cl.valeur().que_suis_je() == "Paroi_decalee_Robin")
        {
          nb_cl_robin+=1;
        }
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
        {
          nb_cl_diri+=1;
        }
    }

  if (modele_turbulence.non_nul() && !ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur()).loi_paroi().valeur().que_suis_je().debute_par("negligeable"))
    {
      // PQ : 13/07/05 : prise en compte des lois de paroi pour le calcul de u_tau
      // Hypotheses :    1ere condition de Dirichlet = paroi basse
      //                 2eme condition de Dirichlet = paroi haute
      //                 maillage regulier suivant x
      const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,mon_equation.valeur().probleme().equation(0).milieu());
      const Turbulence_paroi& loipar           = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur()).loi_paroi();
      DoubleTab tau_tan;
      tau_tan.ref(loipar->Cisaillement_paroi());

//      int num_cl_rep=0;
//      int nbfaces=0;
      tauwb=0.;
      tauwh=0.;

//    dirichlet
      if ( nb_cl_diri != 0 )
        {
          DoubleVect tauw_diri(nb_cl_diri);
          DoubleVect retau_diri(nb_cl_diri);
          DoubleVect utau_diri(nb_cl_diri);
//            search of cl diri
//            nb cl diri
          double tauw_diri_tmp=0.;
          int numero_bord_diri=0;
          int nbfaces_bord_diri=0;

          double rho = 0.;
          double mu = 0.;

          if ( sub_type(Champ_Uniforme,fluide.masse_volumique()) )
            {
              rho = fluide.masse_volumique().valeurs()(0,0);
            }
          if ( sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
            {
              mu = fluide.viscosite_dynamique().valeurs()(0,0);
            }

//            for each cl, calculation of the mean tauw
//            loop on boundaries
          for (int num_cl=0; num_cl<nb_cl; num_cl++)
            {
              tauw_diri_tmp=0.;
              if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                rho = 0.;
              if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                mu = 0.;
              const Cond_lim& la_cl = les_cl[num_cl];
              if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
                {
                  const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
                  nbfaces_bord_diri = la_front_dis.nb_faces();
                  int ndeb = la_front_dis.num_premiere_face();
                  int nfin = ndeb + nbfaces_bord_diri;
                  if (Objet_U::dimension == 2 )
                    {
//                        loop on faces in 2D
                      for (int fac=ndeb; fac<nfin ; fac++)
                        {
                          tauw_diri_tmp += sqrt(tau_tan(fac,0)*tau_tan(fac,0));

                          int elem = zone_VF.face_voisins(fac,0);
                          if (elem == -1)
                            elem = zone_VF.face_voisins(fac,1);
                          if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                            rho += fluide.masse_volumique().valeurs()[elem];
                          if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                            mu += fluide.viscosite_dynamique().valeurs()[elem];
                        }
                    }
                  else
                    {
//                        loop on faces on 3D
                      for (int fac=ndeb; fac<nfin ; fac++)
                        {
                          tauw_diri_tmp += sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,2)*tau_tan(fac,2));

                          int elem = zone_VF.face_voisins(fac,0);
                          if (elem == -1)
                            elem = zone_VF.face_voisins(fac,1);
                          if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                            rho += fluide.masse_volumique().valeurs()[elem];
                          if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                            mu += fluide.viscosite_dynamique().valeurs()[elem];
                        }
                    }
                  tauw_diri_tmp=mp_sum(tauw_diri_tmp)/mp_sum(nbfaces_bord_diri);
                  if(!(mon_equation.valeur().probleme().is_QC()))
                    tauw_diri_tmp *= rho ;

                  if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                    rho=mp_sum(rho)/mp_sum(nbfaces_bord_diri);
                  if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                    mu=mp_sum(mu)/mp_sum(nbfaces_bord_diri);

                  tauw_diri(numero_bord_diri) = tauw_diri_tmp;
                  utau_diri(numero_bord_diri) = sqrt(tauw_diri_tmp/rho);
                  retau_diri(numero_bord_diri)= rho*utau_diri(numero_bord_diri)*hs2/mu;

                  tauw_diri_m  += tauw_diri(numero_bord_diri);
                  utau_diri_m  += utau_diri(numero_bord_diri);
                  retau_diri_m += retau_diri(numero_bord_diri);

                  numero_bord_diri+=1;
                }
            } // loop on boundaries

          tauw_diri_m  =tauw_diri_m/nb_cl_diri;
          retau_diri_m =retau_diri_m/nb_cl_diri;
          utau_diri_m  =utau_diri_m/nb_cl_diri;

//            prints
          if (je_suis_maitre())
            {
              SFichier fic7("tauw.dat", mode);
              SFichier fic8("reynolds_tau.dat", mode);
              SFichier fic9("u_tau.dat", mode);
              fic7 << tps << "   " << tauw_diri_m   << "   ";
              fic8 << tps << "   " << retau_diri_m  << "   ";
              fic9 << tps << "   " << utau_diri_m   << "   ";
              for ( int num=0 ; num<nb_cl_diri ; num++)
                {
                  fic7 << tauw_diri(num)   << "   ";
                  fic8 << retau_diri(num)  << "   ";
                  fic9 << utau_diri(num)   << "   ";
                }
              fic7 <<  finl;
              fic8 <<  finl;
              fic9 <<  finl;
              fic7.flush();
              fic8.flush();
              fic9.flush();
              fic7.close();
              fic8.close();
              fic9.close();
            }
        } // end dirichlet

//    robin
      if ( nb_cl_robin != 0 )
        {
          DoubleVect tauw_robin(nb_cl_robin);
          DoubleVect retau_robin(nb_cl_robin);
          DoubleVect utau_robin(nb_cl_robin);
//            search of cl robin
//            nb cl robin
          double tauw_robin_tmp=0.;
          int numero_bord_robin=0;
          int nbfaces_bord_robin=0;
          double rho = 0.;
          double mu = 0.;

          if ( sub_type(Champ_Uniforme,fluide.masse_volumique()) )
            {
              rho = fluide.masse_volumique().valeurs()(0,0);
            }
          if ( sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
            {
              mu = fluide.viscosite_dynamique().valeurs()(0,0);
            }

//            for each cl, calculation of the mean tauw
//            loop on boundaries
          for (int num_cl=0; num_cl<nb_cl; num_cl++)
            {
              tauw_robin_tmp=0.;
              if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                rho = 0.;
              if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                mu = 0.;
              const Cond_lim& la_cl = les_cl[num_cl];
              if (la_cl.valeur().que_suis_je() == "Paroi_decalee_Robin")
                {
                  const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
                  nbfaces_bord_robin = la_front_dis.nb_faces();
                  int ndeb = la_front_dis.num_premiere_face();
                  int nfin = ndeb + nbfaces_bord_robin;
                  if (Objet_U::dimension == 2 )
                    {
//                        loop on faces in 2D
                      for (int fac=ndeb; fac<nfin ; fac++)
                        {
                          tauw_robin_tmp+=sqrt(tau_tan(fac,0)*tau_tan(fac,0));

                          int elem = zone_VF.face_voisins(fac,0);
                          if (elem == -1)
                            elem = zone_VF.face_voisins(fac,1);
                          if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                            rho+=fluide.masse_volumique().valeurs()[elem];
                          if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                            mu+=fluide.viscosite_dynamique().valeurs()[elem];
                        }
                    }
                  else
                    {
//                        loop on faces on 3D
                      for (int fac=ndeb; fac<nfin ; fac++)
                        {
                          tauw_robin_tmp+=sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,2)*tau_tan(fac,2));

                          int elem = zone_VF.face_voisins(fac,0);
                          if (elem == -1)
                            elem = zone_VF.face_voisins(fac,1);
                          if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                            rho+=fluide.masse_volumique().valeurs()[elem];
                          if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                            mu+=fluide.viscosite_dynamique().valeurs()[elem];
                        }
                    }
                  tauw_robin_tmp=mp_sum(tauw_robin_tmp)/mp_sum(nbfaces_bord_robin);
                  if(!(mon_equation.valeur().probleme().is_QC()))
                    tauw_robin_tmp *= rho ;

                  if ( !sub_type(Champ_Uniforme,fluide.masse_volumique()) )
                    rho=mp_sum(rho)/mp_sum(nbfaces_bord_robin);
                  if ( !sub_type(Champ_Uniforme,fluide.viscosite_dynamique().valeur()) )
                    mu=mp_sum(mu)/mp_sum(nbfaces_bord_robin);

                  tauw_robin(numero_bord_robin) = tauw_robin_tmp;
                  utau_robin(numero_bord_robin) = sqrt(tauw_robin_tmp/rho);
                  retau_robin(numero_bord_robin)= rho*utau_robin(numero_bord_robin)*hs2/mu;

                  tauw_robin_m  += tauw_robin(numero_bord_robin);
                  utau_robin_m  += utau_robin(numero_bord_robin);
                  retau_robin_m += retau_robin(numero_bord_robin);

                  numero_bord_robin+=1;
                }
            } // loop on boundaries

          tauw_robin_m  = tauw_robin_m/nb_cl_robin;
          retau_robin_m = retau_robin_m/nb_cl_robin;
          utau_robin_m  = utau_robin_m/nb_cl_robin;

//            prints
          if (je_suis_maitre())
            {
              SFichier fic4("tauw_robin.dat", mode);
              SFichier fic5("reynolds_tau_robin.dat", mode);
              SFichier fic6("u_tau_robin.dat", mode);
              fic4 << tps << "   " << tauw_robin_m   << "   ";
              fic5 << tps << "   " << retau_robin_m  << "   ";
              fic6 << tps << "   " << utau_robin_m   << "   ";
              for ( int num=0 ; num<nb_cl_robin ; num++)
                {
                  fic4 << tauw_robin(num)   << "   ";
                  fic5 << retau_robin(num)  << "   ";
                  fic6 << utau_robin(num)   << "   ";
                }
              fic4 <<  finl;
              fic5 <<  finl;
              fic6 <<  finl;
              fic4.flush();
              fic5.flush();
              fic6.flush();
              fic4.close();
              fic5.close();
              fic6.close();
            }
        } // end robin

//// begin old
////    search of cl dirichlet
//      for (int num_cl=0; num_cl<nb_cl; num_cl++)
//        {
//          //Loop on boundaries
//          const Cond_lim& la_cl = les_cl[num_cl];
//          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
//            {
//              const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
//              nbfaces = la_front_dis.nb_faces();
//              int ndeb = la_front_dis.num_premiere_face();
//              int nfin = ndeb + nbfaces;
//              if (Objet_U::dimension == 2 )
//                for (int fac=ndeb; fac<nfin ; fac++)
//                  {
//                    tauwb+=sqrt(tau_tan(fac,0)*tau_tan(fac,0));
//                  }
//              else
//                for (int fac=ndeb; fac<nfin ; fac++)
//                  {
//                    tauwb+=sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,2)*tau_tan(fac,2));
//                  }
//              num_cl_rep=num_cl+1;
//              break;
//            }
//        } //Loop on boundaries
//      nbfaces_tot=mp_sum(nbfaces);
//      if (nbfaces_tot)
//        tauwb=mp_sum(tauwb)/nbfaces_tot;
////    on cherche la deuxieme cl dirichlet
//      nbfaces=0;
//      for (int num_cl=num_cl_rep; num_cl<nb_cl; num_cl++)
//        {
//          //Boucle sur les bords
//          const Cond_lim& la_cl = les_cl[num_cl];
//          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
//            {
//              const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
//              nbfaces = la_front_dis.nb_faces();
//              int ndeb = la_front_dis.num_premiere_face();
//              int nfin = ndeb + nbfaces;
//              if (Objet_U::dimension == 2 )
//                for (int fac=ndeb; fac<nfin ; fac++)
//                  {
//                    tauwh+=sqrt(tau_tan(fac,0)*tau_tan(fac,0));
//                  }
//              else
//                for (int fac=ndeb; fac<nfin ; fac++)
//                  {
//                    tauwh+=sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,2)*tau_tan(fac,2));
//                  }
//              break;
//            }
//        } //Boucle sur les bords
//      nbfaces_tot=mp_sum(nbfaces);
//      if (nbfaces_tot)
//        tauwh=mp_sum(tauwh)/nbfaces_tot;
//      if(!(mon_equation.valeur().probleme().is_QC()))
//        {
//          tauwb*=rho_bas;
//          tauwh*=rho_haut;
//        }
//      if (je_suis_maitre())
//        {
//          // calcul et ecritures des differentes grandeurs parietales
//          //////////////////////////////////////////////////////
//          // vitesse de frottement
//          double utaub=sqrt(tauwb/rho_bas);
//          if(val_moy_tot(kmin,0,0)<=0) utaub*=-1.;
//          double utauh=sqrt(tauwh/rho_haut);
//          if(val_moy_tot(kmax,0,0)<=0) utauh*=-1.;
//          double utaum=0.5*(utauh+utaub);
//          // Reynolds de frottement
//          double retaub=rho_bas*utaub*hs2/mu_bas;
//          double retauh=rho_haut*utauh*hs2/mu_haut;
//          double retaum=0.5*(retauh+retaub);
//          tauwm=0.5*(tauwh+tauwb);
//          SFichier fic1("reynolds_tau_old.dat",ios::app);
//          SFichier fic2("u_tau_old.dat",ios::app);
//          SFichier fic3("tauw_old.dat",ios::app);
//          fic1 << tps << " " << retaum << " " << retaub << " " << retauh << finl;
//          fic2 << tps << " " << utaum << " "  << utaub << " "  << utauh  << finl;
//          fic3 << tps << " " << tauwm << " "  << tauwb << " "  << tauwh  << finl;
//          fic1.close();
//          fic2.close();
//          fic3.close();
//        }
//// end old

    } // end if turbul
  else
    {
      // norme de la vitesse tangente a la paroi
      double utang_bas  = val_moy_tot(kmin,9,0);
      double utang_haut = val_moy_tot(kmax,9,0);
      // calcul du cisaillement a la paroi suivant la vitesse tangentielle
      //////////////////////////////////////////////////////
      // approximation lineaire : Tau_w = mu * ||u_t||(y1) / dy1
      tauwb= mu_bas * utang_bas  / ymin;
      tauwh= mu_haut* utang_haut / ymin;

      if (je_suis_maitre())
        {
          // calcul et ecritures des differentes grandeurs parietales
          //////////////////////////////////////////////////////
          // vitesse de frottement
          double utaub=sqrt(tauwb/rho_bas);
          if(val_moy_tot(kmin,0,0)<=0) utaub*=-1.;
          double utauh=sqrt(tauwh/rho_haut);
          if(val_moy_tot(kmax,0,0)<=0) utauh*=-1.;
          double utaum=0.5*(utauh+utaub);

          // Reynolds de frottement
          double retaub=rho_bas*utaub*hs2/mu_bas;
          double retauh=rho_haut*utauh*hs2/mu_haut;
          double retaum=0.5*(retauh+retaub);
          tauwm=0.5*(tauwh+tauwb);

          SFichier fic1("reynolds_tau.dat", mode);
          SFichier fic2("u_tau.dat", mode);
          SFichier fic3("tauw.dat", mode);
          fic1 << tps << " " << retaum << " " << retaub << " " << retauh << finl;
          fic2 << tps << " " << utaum << " "  << utaub << " "  << utauh  << finl;
          fic3 << tps << " " << tauwm << " "  << tauwb << " "  << tauwh  << finl;
          fic1.close();
          fic2.close();
          fic3.close();
        }

    } // end if not-turbul


}


void Traitement_particulier_NS_canal::ecriture_fichiers_moy_vitesse_rho_mu(const DoubleTab& val_moy, const Nom& fichier, const double& dt, const int& k) const
{
  SFichier fic (fichier);

  double tps = mon_equation->inconnue().temps();
  double u,v,wl,u2,v2,w2,uv,uw,vw,rho,mu;

  fic << "# Temps : " << tps << finl;
  fic << " " << finl;

  fic << "# (0)  : Y " << finl;
  fic << "# (1)  : <u> " << finl;
  fic << "# (2)  : <v> " << finl;
  fic << "# (3)  : <w> " << finl;
  fic << "# (4)  : sqrt(<u.u>-<u>*<u>) " << finl;
  fic << "# (5)  : sqrt(<v.v>-<v>*<v>) " << finl;
  fic << "# (6)  : sqrt(<w.w>-<w>*<w>) " << finl;
  fic << "# (7)  : <u.v>-<u>*<v> " << finl;
  fic << "# (8)  : <u.w>-<u>*<w> " << finl;
  fic << "# (9)  : <v.w>-<v>*<w> " << finl;
  fic << "# (10) : <rho> " << finl;
  fic << "# (11) : <mu> " << finl;

  fic << "#  " << finl;
  fic << "# Nombre minimum de points par plan Y=cste ayant servi au calcul de la moyenne spatiale : " << min_array(compt_tot) << finl;
  fic << "#  " << finl;

  for (int i=0; i<Y_tot.size(); i++)
    {
      u    = val_moy(i,0,k)/dt;
      v    = val_moy(i,1,k)/dt;
      wl    = val_moy(i,2,k)/dt;
      u2   = val_moy(i,3,k)/dt;
      v2   = val_moy(i,4,k)/dt;
      w2   = val_moy(i,5,k)/dt;
      uv   = val_moy(i,6,k)/dt;
      uw   = val_moy(i,7,k)/dt;
      vw   = val_moy(i,8,k)/dt;
      rho  = val_moy(i,10,k)/dt;
      mu   = val_moy(i,11,k)/dt;


      fic << Y_tot(i) << "    "  ;
      fic << u << "    " << v << "    " << wl << "    " ;
      // Pour eviter NAN, on prend le dmax(,0):
      fic << sqrt(dmax(0,u2)) << "    " << sqrt(dmax(v2,0)) << "    " << sqrt(dmax(w2,0)) << "    "  ;
      fic << -uv << "    " << -uw << "    " << -vw  << "    " ;
      fic << rho << "    " << mu << "    " ;
      fic << finl;
    }
  fic.flush();
  fic.close();
}

void Traitement_particulier_NS_canal::ecriture_fichiers_moy_nut(const DoubleTab& val_moy, const Nom& fichier, const double& dt, const int& k) const
{
  SFichier fic (fichier);

  double tps = mon_equation->inconnue().temps();

  fic << "# Temps : " << tps << finl;
  fic << " " << finl;

  fic << "# (0)  : Y " << finl;
  fic << "# (1)  : <nu_t> " << finl;

  fic << "#  " << finl;
  fic << "# Nombre minimum de points par plan Y=cste ayant servi au calcul de la moyenne spatiale : " << min_array(compt_tot) << finl;
  fic << "#  " << finl;

  for (int i=0; i<Y_tot.size(); i++)
    fic << Y_tot(i) << "    " << val_moy(i,12,k)/dt << finl;

  fic.flush();
  fic.close();
}

void Traitement_particulier_NS_canal::ecriture_fichiers_moy_Temp(const DoubleTab& val_moy, const Nom& fichier, const double& dt, const int& k) const
{
  SFichier fic (fichier);

  double tps = mon_equation->inconnue().temps();
  double T,T2,uT,vT,wT;

  fic << "# Temps : " << tps << finl;
  fic << " " << finl;

  fic << "# (0)  : Y " << finl;
  fic << "# (1)  : <T> " << finl;
  fic << "# (2)  : sqrt(<T.T>-<T>*<T>) " << finl;
  fic << "# (3)  : <u.T>-<u>*<T> " << finl;
  fic << "# (4)  : <v.T>-<v>*<T> " << finl;
  fic << "# (5)  : <w.T>-<w>*<T> " << finl;

  fic << "#  " << finl;
  fic << "# Nombre minimum de points par plan Y=cste ayant servi au calcul de la moyenne spatiale : " << min_array(compt_tot) << finl;
  fic << "#  " << finl;

  for (int i=0; i<Y_tot.size(); i++)
    {
      //u    = val_moy(i,0,k)/dt;
      //v    = val_moy(i,1,k)/dt;
      //w    = val_moy(i,2,k)/dt;
      T    = val_moy(i,13,k)/dt;
      T2   = val_moy(i,14,k)/dt;
      uT   = val_moy(i,15,k)/dt;
      vT   = val_moy(i,16,k)/dt;
      wT   = val_moy(i,17,k)/dt;


      fic << Y_tot(i) << "    "  ;
      // Pour eviter NAN, on prend le dmax(,0):
      fic << T << "    "  <<sqrt(dmax(T2,0))<<" ";
      fic << -uT << "    " << -vT  << "    " << -wT  << "    ";
      fic << finl;
    }
  fic.flush();
  fic.close();
}

//Apres correction de l expression de la moyenne temporelle et des methodes d ecriture des moyennes temporelles
//On conserve ces deux methodes d ecriture en version old pour l ecriture des moyennes de phase

void Traitement_particulier_NS_canal::ecriture_fichiers_moy_vitesse_rho_mu_old(const DoubleTab& val_moy, const Nom& fichier, const double& dt, const int& k) const
{
  SFichier fic (fichier);

  double tps = mon_equation->inconnue().temps();
  double u,v,wl,u2,v2,w2,uv,uw,vw,rho,mu;

  fic << "# Temps : " << tps << finl;
  fic << " " << finl;

  fic << "# (0)  : Y " << finl;
  fic << "# (1)  : <u> " << finl;
  fic << "# (2)  : <v> " << finl;
  fic << "# (3)  : <w> " << finl;
  fic << "# (4)  : sqrt(<u.u>-<u>*<u>) " << finl;
  fic << "# (5)  : sqrt(<v.v>-<v>*<v>) " << finl;
  fic << "# (6)  : sqrt(<w.w>-<w>*<w>) " << finl;
  fic << "# (7)  : <u.v>-<u>*<v> " << finl;
  fic << "# (8)  : <u.w>-<u>*<w> " << finl;
  fic << "# (9)  : <v.w>-<v>*<w> " << finl;
  fic << "# (10) : <rho> " << finl;
  fic << "# (11) : <mu> " << finl;

  fic << "#  " << finl;
  fic << "# Nombre minimum de points par plan Y=cste ayant servi au calcul de la moyenne spatiale : " << min_array(compt_tot) << finl;
  fic << "#  " << finl;

  for (int i=0; i<Y_tot.size(); i++)
    {
      u    = val_moy(i,0,k)/dt;
      v    = val_moy(i,1,k)/dt;
      wl    = val_moy(i,2,k)/dt;
      u2   = val_moy(i,3,k)/dt;
      v2   = val_moy(i,4,k)/dt;
      w2   = val_moy(i,5,k)/dt;
      uv   = val_moy(i,6,k)/dt;
      uw   = val_moy(i,7,k)/dt;
      vw   = val_moy(i,8,k)/dt;
      rho  = val_moy(i,10,k)/dt;
      mu   = val_moy(i,11,k)/dt;


      fic << Y_tot(i) << "    "  ;
      fic << u << "    " << v << "    " << wl << "    " ;
      // Pour eviter NAN, on prend le dmax(,0):
      fic << sqrt(dmax(0,u2-u*u)) << "    " << sqrt(dmax(v2-v*v,0)) << "    " << sqrt(dmax(w2-wl*wl,0)) << "    "  ;
      fic << u*v-uv << "    " << u*wl-uw << "    " << v*wl-vw  << "    " ;


      fic << rho << "    " << mu << "    " ;
      fic << finl;
    }
  fic.flush();
  fic.close();
}



void Traitement_particulier_NS_canal::ecriture_fichiers_moy_Temp_old(const DoubleTab& val_moy, const Nom& fichier, const double& dt, const int& k) const
{
  SFichier fic (fichier);
  fic.setf(ios::scientific);
  double tps = mon_equation->inconnue().temps();
  double u,v,wl,T,T2,uT,vT,wT;

  fic << "# Temps : " << tps << finl;
  fic << " " << finl;

  fic << "# (0)  : Y " << finl;
  fic << "# (1)  : <T> " << finl;
  fic << "# (2)  : sqrt(<T.T>-<T>*<T>) " << finl;
  fic << "# (3)  : <u.T>-<u>*<T> " << finl;
  fic << "# (4)  : <v.T>-<v>*<T> " << finl;
  fic << "# (5)  : <w.T>-<w>*<T> " << finl;

  fic << "#  " << finl;
  fic << "# Nombre minimum de points par plan Y=cste ayant servi au calcul de la moyenne spatiale : " << min_array(compt_tot) << finl;
  fic << "#  " << finl;

  for (int i=0; i<Y_tot.size(); i++)
    {
      u    = val_moy(i,0,k)/dt;
      v    = val_moy(i,1,k)/dt;
      wl    = val_moy(i,2,k)/dt;
      T    = val_moy(i,13,k)/dt;
      T2   = val_moy(i,14,k)/dt;
      uT   = val_moy(i,15,k)/dt;
      vT   = val_moy(i,16,k)/dt;
      wT   = val_moy(i,17,k)/dt;


      fic << Y_tot(i) << "    "  ;
      // Pour eviter NAN, on prend le dmax(,0):
      fic << T << "    " << sqrt(dmax(T2-T*T,0)) << "    "  ;
      fic << u*T-uT << "    " << v*T-vT << "    " << wl*T-wT << "    ";

      fic << finl;
    }
  fic.flush();
  fic.close();
}



