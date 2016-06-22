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
// File:        Paroi_TBLE_QDM_Scal.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#include <Paroi_TBLE_QDM_Scal.h>
#include <Paroi_TBLE_QDM.h>
#include <Zone_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Front_VF.h>
#include <Zone_VF.h>
#include <EFichier.h>
#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Diffu_totale_hyd_base.h>
#include <Diffu_totale_scal_base.h>
#include <SFichier.h>

Entree& Paroi_TBLE_QDM_Scal::lire_donnees(const Motcle& mot_lu, Entree& is)
{
  Motcles les_mots(10);
  {
    les_mots[0]="N";
    les_mots[1]="modele_visco";
    les_mots[2]="facteur";
    les_mots[3]="stats";
    les_mots[4]="epsilon";
    les_mots[5]="max_it";
    les_mots[6]="sonde_tble";
    les_mots[7]="restart";
    les_mots[8]="stationnaire";
    les_mots[9]="Prandtl";
  }
  int rang=les_mots.search(mot_lu);
  switch(rang)
    {
    case 0 :
      is >> nb_pts;
      Cerr << "N = " << nb_pts << finl;
      break;
    case 1  :
      is >> modele_visco;
      Cerr << "Le modele de viscosite turbulente dans le maillge fin est "
           << modele_visco  << finl;
      break;
    case 2  :
      is >> fac;
      Cerr << "Raison geometrique du maillage fin : " << fac << finl;
      break;
    case 3  :
      oui_stats=1;
      is >> tps_deb_stats;
      is >> tps_fin_stats;

      Cerr << "tps_deb_stats = " << tps_deb_stats << finl;
      Cerr << "tps_fin_stats = " << tps_fin_stats << finl;

      if((tps_deb_stats!=-1) && tps_fin_stats!=-1)
        Cerr << "Temps stats TBLE : OK" << finl;
      else
        Cerr << "WARNING !!!!! Temps stats TBLE INCORRECTS" << finl;
      break;
    case 4  :
      is >> epsilon;
      Cerr << "Seuil de convergence pour les equations de couche limites : " << epsilon << finl;
      break;
    case 5  :
      is >> max_it;
      Cerr << "Le maximum d'iterations pour la resolution des equations TBLE est de : " << max_it << finl;
      break;
    case 6  :
      is >> nb_post_pts;
      sonde_tble.resize(nb_post_pts, Objet_U::dimension);
      for(int i=0; i<nb_post_pts; i++)
        {
          nom_pts.dimensionner(nb_post_pts);
          is >> nom_pts[i];
          Cerr << "Nom"<<i<<"=" << nom_pts[i] <<finl;
          for(int j=0; j<Objet_U::dimension ; j++)
            {
              is >> sonde_tble(i,j);
              Cerr << "sonde_tble( "<< i << "," << j <<" ) =" << sonde_tble(i,j)  << finl;
            }
        }
      break;
    case 7 :
      restart = 1;
      break;
    case 8 :
      statio = 1;
      is >> max_it_statio >> eps_statio;
      if (max_it_statio<0 || eps_statio<0)
        {
          Cerr << "Vous devez choisir le nombre maximum d'iterations pour attendre la convergence en temps de TBLE" << finl;
          Cerr << "ainsi que la valeur du critere d'arret" << finl;
          Cerr << "Syntaxe : statio max_it_statio eps_statio" << finl;
          Process::exit();
        }
      break;
    case 9:
      is >> Prandtl;
      break;
    default :
      {
        Cerr << mot_lu << " n'est pas un mot compris par Paroi_TBLE_scal" << finl;
        Cerr << "Les mots compris sont : " << les_mots << finl;
        Process::exit();
      }
    }
  return is;
}

void Paroi_TBLE_QDM_Scal::init_valeurs_defaut()
{
  nb_pts=-1; // nb de pts dans le maillage fin
  modele_visco = "Diffu_scal_lm"; //modele de viscosite turbulente
  nb_comp = 1; //nb de composantes dans le maillage fin
  fac=1.; // facteur de raffinement du maillage fin
  oui_stats=0; // stats (1=oui/0=non)
  tps_deb_stats=-1; // debut calcul des moyennes stats dans le maillage fin
  tps_fin_stats=-1; // fin des stats
  epsilon = 1.e-5; // seuil de resolution dans le maillage fin
  max_it = 1000; // max d'iterations dans le maillage fin
  nb_post_pts = 0;
  reprise_ok = 0; // par defaut, reprise des champs TBLE
  restart = 0;

  statio = 0;
  max_it_statio = -1;
  eps_statio = -1.;

  is_init=0;

  Prandtl=1.;
}

int Paroi_TBLE_QDM_Scal::init_lois_paroi(const Zone_VF& zone_dis, const Zone_Cl_dis_base& la_zone_Cl)
{
  is_init=1;
  int compteur_faces_paroi = 0;
  num_faces_post.resize(nb_post_pts);
  num_global_faces_post.resize(nb_post_pts,3);
  for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          compteur_faces_paroi += le_bord.nb_faces();
        }
    }
  eq_temp.dimensionner(compteur_faces_paroi); //Dimensionnement du vecteur eq_couch_lim

  for(int j=0; j<nb_post_pts; j++)
    {
      double d0=1.e9;
      int face=-1,face2=-1;
      int i_bord=-1;
      int ind_face2=-1;
      compteur_faces_paroi=0;
      for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int size=le_bord.nb_faces();
              for (int ind_face=0; ind_face<size; ind_face++)
                {
                  int num_face = le_bord.num_face(ind_face);
                  double d1=0.;
                  for (int d=0; d<Objet_U::dimension; d++)
                    {
                      double x=zone_dis.xv(num_face,d)-sonde_tble(j,d);
                      d1+=x*x;
                    }

                  if (d1<d0)
                    {
                      d0=d1;
                      face=compteur_faces_paroi;
                      ind_face2=ind_face;
                      i_bord=n_bord;
                      face2=le_bord.num_face(ind_face2);
                    }
                  compteur_faces_paroi++;
                }
            }
        }
      num_faces_post(j)=face;
      num_global_faces_post(j,0)=i_bord;
      num_global_faces_post(j,1)=ind_face2;
      num_global_faces_post(j,2)=face2;
    }


  Alpha_mean_sum.resize(nb_post_pts,nb_pts+1);
  Tmean_sum.resize(nb_post_pts,nb_pts+1);
  UTmean_sum.resize(nb_post_pts,nb_pts+1);
  VTmean_sum.resize(nb_post_pts,nb_pts+1);
  WTmean_sum.resize(nb_post_pts,nb_pts+1);
  Tmean_2_sum.resize(nb_post_pts,nb_pts+1);

  // associer les "MuLambda_TBLE" au diffu_scal
  for (int i=0; i<compteur_faces_paroi; i++)
    {
      eq_temp[i].associer_milieu(getPbBase().milieu());
      eq_temp[i].set_diffu(modele_visco); //modele de viscosite turbulente
      Diffu_totale_scal_base& diffu_scal = ref_cast_non_const(Diffu_totale_scal_base, eq_temp[i].get_diffu()); //modele de viscosite turbulente
      Diffu_totale_hyd_base& diffu_hyd = ref_cast_non_const(Diffu_totale_hyd_base, get_eq_couch_lim(i).get_diffu()); //modele de viscosite turbulente
      diffu_scal.set_visco_tot(diffu_hyd);
      diffu_scal.associer_mulambda(getMuLambda());
      // on a de la temperature : on complete les Diffu_hyd
      diffu_hyd.associer_eqn_T(eq_temp[i]);
      diffu_scal.setPrandtl(Prandtl);
    }

  return 1;
}


int Paroi_TBLE_QDM_Scal::reprendre(Entree&, const Zone_dis_base& zone_dis, const Zone_Cl_dis_base& la_zone_Cl, double tps_reprise)
{
  if (restart == 0)    // Si on ne souhaite pas repartir de zero
    {
      Cerr << "Reprise du champ TBLE T au temps = " << tps_reprise<< finl;
      int compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi
      const int ME=Process::me();
      Nom nom_fic="sauvegarde_tble_scal_";
      nom_fic+=Nom(ME);
      nom_fic+=Nom("_");
      nom_fic+=Nom(tps_reprise,getPbBase().reprise_format_temps());
      EFichier fic_sauve(nom_fic);

      if (fic_sauve.fail())
        {
          Cerr << "Erreur a l'ouverture du fichier " << nom_fic << finl;
          Cerr << "Si vous ne souhaitez pas reprendre les quantites TBLE, utilisez le mot-cle 'restart' dans les options TBLE" << finl;
          Process::exit();
        }


      // On compte avant pour dimensionner
      for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )

            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              compteur_faces_paroi+=le_bord.nb_faces();
            }
        }


      valeurs_reprises.resize(compteur_faces_paroi, nb_pts+1);
      compteur_faces_paroi=0;
      assert(nb_comp==1);
      for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);
          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  for (int comp=0; comp<nb_comp; comp++) // en principe nb_comp=1 !!!
                    {
                      for (int itble=0; itble<nb_pts+1; itble++)
                        {
                          fic_sauve >> valeurs_reprises(compteur_faces_paroi,itble);
                        }
                    }
                  compteur_faces_paroi++;
                }
            }
        }
      fic_sauve.close();
      Cerr << "Reprise du champ TBLE T OK" << finl;
      reprise_ok=1;
    }
  return 1;
}




int Paroi_TBLE_QDM_Scal::sauvegarder(Sortie&, const Zone_dis_base& zone_dis, const Zone_Cl_dis_base& la_zone_Cl,double tps) const
{
  Cerr << "Sauvegarde du champ TBLE T au temps = " << tps << finl;
  const int ME=Process::me();
  Nom nom_fic="sauvegarde_tble_scal_";
  nom_fic+=Nom(ME);
  nom_fic+=Nom("_");
  nom_fic+=Nom(tps,"%e");

  SFichier fic_sauve(nom_fic);
  int compteur_faces_paroi=0;
  for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              for (int comp=0; comp<nb_comp; comp++)
                {
                  for (int itble=0; itble<nb_pts+1; itble++)
                    fic_sauve << eq_temp[compteur_faces_paroi].get_Unp1(comp,itble) << " " ;
                }
              fic_sauve << finl;
              compteur_faces_paroi++;
            }
        }
    }
  fic_sauve.close();
  return 0;
}


void Paroi_TBLE_QDM_Scal::imprimer_stat(Sortie&, double tps) const
{
  if (oui_stats==1)
    {
      for(int j=0; j<nb_post_pts; j++)
        {
          double coeff = 1./(tps-tps_deb_stats);
          Nom tmp;
          tmp="tble_T_stats_";
          tmp+=nom_pts[j];
          tmp+=".dat";

          SFichier fic_post(tmp, ios::app);

          if (Objet_U::dimension==2)
            {
              fic_post << "# "<< tps << " T T^2 UT VT "<< finl;
              for(int i=0; i<nb_pts+1; i++)
                fic_post << coeff*Tmean_sum(j,i) << " " << coeff*Tmean_2_sum(j,i) <<  finl;
            }
          else if (Objet_U::dimension==3)
            {
              fic_post << "# "<< tps << " T T^2 UT VT WT "<< finl;
              for(int i=0; i<nb_pts+1; i++)
                {
                  fic_post << coeff*Tmean_sum(j,i) << " " << coeff*Tmean_2_sum(j,i) << " " ;
                  fic_post << coeff*UTmean_sum(j,i) << " " << coeff*VTmean_sum(j,i)<< " " << coeff*WTmean_sum(j,i)<<  finl;
                }
            }
        }
    }
}


void Paroi_TBLE_QDM_Scal::imprimer_sondes(Sortie&, double tps, const VECT(DoubleVect)& distance_equivalente) const
{
  for(int j=0; j<nb_post_pts; j++)
    {
      Nom tmp;
      tmp="T_tble_post_";
      tmp+=nom_pts[j];
      tmp+=".dat";


      SFichier fic_T(tmp, ios::app);

      fic_T << "# " << tps  << " " << "deq= " << distance_equivalente[num_global_faces_post(j,0)](num_global_faces_post(j,1)) <<  finl;
      for(int i=0 ; i<nb_pts+1 ; i++)
        {
          fic_T << eq_temp[num_faces_post(j)].get_y(i)<< " " << eq_temp[num_faces_post(j)].get_Unp1(0,i) << finl;
        }
      fic_T << finl;
    }
}

void Paroi_TBLE_QDM_Scal::imprimer_sondes(Sortie&, double tps, const DoubleVect& tab_d_equiv_) const
{
  for(int j=0; j<nb_post_pts; j++)
    {
      Nom tmp;
      tmp="T_tble_post_";
      tmp+=nom_pts[j];
      tmp+=".dat";


      SFichier fic_T(tmp, ios::app);

      fic_T << "# " << tps  << " " << "deq= " << tab_d_equiv_[num_global_faces_post(j,2)] <<  finl;
      for(int i=0 ; i<nb_pts+1 ; i++)
        {
          fic_T << eq_temp[num_faces_post(j)].get_y(i)<< " " << eq_temp[num_faces_post(j)].get_Unp1(0,i) << finl;
        }
      fic_T << finl;
    }
}

void  Paroi_TBLE_QDM_Scal::calculer_stat(int j, int i, double u, double v, double w, double T, double dt)
{
  Tmean_sum(j,i) += T*dt;
  UTmean_sum(j,i) += u*T*dt;
  VTmean_sum(j,i) += v*T*dt;
  WTmean_sum(j,i) += w*T*dt;
  Tmean_2_sum(j,i) += T*T*dt;
  Alpha_mean_sum(j,i) += eq_temp[num_faces_post(j)].get_nut(i)*dt;
}
