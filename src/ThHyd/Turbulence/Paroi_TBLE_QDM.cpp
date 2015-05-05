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
// File:        Paroi_TBLE_QDM.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////
#include <Paroi_TBLE_QDM.h>
#include <Zone_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Front_VF.h>
#include <Zone_VF.h>
#include <EFichier.h>
#include <Les_Cl.h>
#include <Diffu_totale_hyd_base.h>
#include <Probleme_base.h>
#include <MuLambda_TBLE_Fcts_T.h>
#include <SFichier.h>
#include <Champ_Fonc_Fonction.h>
#include <Fluide_Incompressible.h>
#include <Param.h>

Paroi_TBLE_QDM::Paroi_TBLE_QDM()
{
  nb_pts=-1; // nb de pts dans le maillage fin
  modele_visco = "Diffu_lm"; //modele de viscosite turbulente
  nb_comp = Objet_U::dimension-1; //nb de composantes dans le maillage fin
  fac=1.; // facteur de raffinement du maillage fin
  oui_stats=0; // stats (1=oui/0=non)
  tps_deb_stats=-1; // debut calcul des moyennes stats dans le maillage fin
  tps_fin_stats=-1; // fin des stats
  epsilon = 1.e-5; // seuil de resolution dans le maillage fin
  max_it = 1000; // max d'iterations dans le maillage fin
  nu_t_dyn=0;
  tps_start_stat_nu_t_dyn = -1;//tps de declenchement de la moyenne de nu_t en premiere maille
  tps_nu_t_dyn = -1;//Temps de debut d'utilisation du nu_t en premiere maille dans TBLE
  nb_post_pts = 0;
  reprise_ok = 0;
  restart = 0;

  mu_fonction = 0;
  lambda_fonction = 0;

  statio = 0;
  max_it_statio = -1;
  eps_statio = -1.;

  source_boussinesq=1;
}

void Paroi_TBLE_QDM::set_param(Param& param)
{
  param.ajouter("N",&nb_pts,Param::REQUIRED);
  param.ajouter("modele_visco",&modele_visco);
  param.ajouter("facteur",&fac);
  //param.ajouter_non_std("stats",(this));
  param.ajouter("epsilon",&epsilon);
  param.ajouter("max_it",&max_it);
  /*
    param.ajouter_non_std("nu_t_dyn",(this));
    param.ajouter_non_std("tps_start_stat_nu_t_dyn",(this));
    param.ajouter_non_std("tps_nu_t_dyn",(this));
    param.ajouter_non_std("sonde_tble",(this));
  */
  param.ajouter_flag("restart",&restart);
  /*
    param.ajouter_non_std("stationnaire",(this));
    param.ajouter_non_std("mu",(this));
    param.ajouter_non_std("lambda",(this));
    param.ajouter_non_std("sans_source_boussinesq",(this));
  */
}

int Paroi_TBLE_QDM::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="stats")
    {
      oui_stats=1;
      is >> tps_deb_stats;
      is >> tps_fin_stats;
      if((tps_deb_stats!=-1) && tps_fin_stats!=-1)
        Cerr << "Statistics time TBLE : OK" << finl;
      else
        {
          Cerr << "WARNING !!!!! Statistics time for TBLE not coherent." << finl;
          Process::exit();
        }
      return 1;
    }

  else if (mot=="nu_t_dyn")
    {
      is >> nu_t_dyn;
      Cerr << "nu_t_dyn = " << nu_t_dyn << finl;
      if(nu_t_dyn!=0)
        Cerr << "Dynamic calculation of nu_t for TBLE" << finl;
      return 1;
    }
  else if (mot=="tps_start_stat_nu_t_dyn")
    {
      is >> tps_start_stat_nu_t_dyn;
      Cerr << "tps_start_stat_nu_t_dyn = " << tps_start_stat_nu_t_dyn << finl;
      if((nu_t_dyn!=0)&&(tps_start_stat_nu_t_dyn >= 0))
        Cerr << "nu_t average starts at t = " << tps_start_stat_nu_t_dyn << finl;
      else
        {
          Cerr << "Problem with the tps_start_stat_nu_t_dyn parameter for TBLE." << finl;
          Process::exit();
        }
      return 1;
    }
  else if (mot=="tps_nu_t_dyn")
    {
      is >> tps_nu_t_dyn;
      Cerr << "tps_nu_t_dyn = " << tps_nu_t_dyn << finl;
      if((nu_t_dyn!=0)&&(tps_nu_t_dyn >= 0))
        Cerr << "Time period of nu_t average before use = " << tps_nu_t_dyn << finl;
      else
        {
          Cerr << "Problem with the tps_nu_t_dyn parameter for TBLE." << finl;
          Process::exit();
        }
      return 1;
    }
  else if (mot=="sonde_tble")
    {
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
      return 1;
    }
  else if (mot=="stationnaire")
    {
      statio = 1;
      is >> max_it_statio >> eps_statio;
      if (max_it_statio<0 || eps_statio<0)
        {
          Cerr << "You must select the maximum iterations number to reach the time convergence for TBLE" << finl;
          Cerr << "as well as the stop crterion value." << finl;
          Cerr << "Syntax : statio max_it_statio eps_statio" << finl;
          Process::exit();
        }
      return 1;
    }
  else if (mot=="mu")
    {
      mu_fonction = 1 ;
      is >> mu_chaine;
      return 1;
    }
  else if (mot=="lambda")
    {
      lambda_fonction = 1 ;
      is >> lambda_chaine;
      return 1;
    }
  else if (mot=="sans_source_boussinesq")
    {
      source_boussinesq = 0 ;
      return 1;
    }
  return -1;
}

int Paroi_TBLE_QDM::init_lois_paroi(const Zone_VF& zone_dis, const Zone_Cl_dis_base& la_zone_Cl)
{
  const int& nb_elem = zone_dis.nb_elem();

  num_faces_post.resize(nb_post_pts);
  num_global_faces_post.resize(nb_post_pts);


  int compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi

  for (int n_bord=0; n_bord<zone_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl.les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          compteur_faces_paroi += le_bord.nb_faces();
        }
    }
  eq_vit.dimensionner(compteur_faces_paroi); //Dimensionnement du vecteur eq_couch_lim


  Fxmean_sum.resize(nb_post_pts,nb_pts+1);
  Fymean_sum.resize(nb_post_pts,nb_pts+1);
  Fzmean_sum.resize(nb_post_pts,nb_pts+1);
  Umean_sum.resize(nb_post_pts,nb_pts+1);
  Vmean_sum.resize(nb_post_pts,nb_pts+1);
  Wmean_sum.resize(nb_post_pts,nb_pts+1);
  Umean_2_sum.resize(nb_post_pts,nb_pts+1);
  Vmean_2_sum.resize(nb_post_pts,nb_pts+1);
  Wmean_2_sum.resize(nb_post_pts,nb_pts+1);
  UVmean_sum.resize(nb_post_pts,nb_pts+1);
  WVmean_sum.resize(nb_post_pts,nb_pts+1);
  WUmean_sum.resize(nb_post_pts,nb_pts+1);



  visco_turb_moy.resize(nb_elem);

  for(int j=0; j<nb_post_pts; j++)
    {
      double d0=1.e9;
      int face=-1;
      int face2=-1;
      compteur_faces_paroi=0;
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
                      face2=num_face;
                    }
                  compteur_faces_paroi++;
                }
            }
        }
      num_faces_post(j)=face;
      num_global_faces_post(j)=face2;
    }

  Cerr << "Dimension de eq_vit = " << compteur_faces_paroi << finl;
  Cerr << "nb_faces_bord = " << zone_dis.nb_faces_bord() << finl;

  if (getPbBase().milieu().conductivite().non_nul() && sub_type(Champ_Fonc_Fonction,getPbBase().milieu().conductivite().valeur()))
    {
      lambda_fonction =1;
      lambda_chaine=ref_cast(Champ_Fonc_Fonction,getPbBase().milieu().conductivite().valeur()).table().parser(0).getString().toChar();
    }
  const Champ_base& mu=ref_cast(Fluide_Incompressible,getPbBase().milieu()).viscosite_dynamique().valeur();
  if (sub_type(Champ_Fonc_Fonction,mu))
    {
      mu_fonction =1;
      mu_chaine=ref_cast(Champ_Fonc_Fonction,mu).table().parser(0).getString().toChar();
    }
  // Choix entre MuLambda cte ou MuLambda fonctions de T
  if ((mu_fonction == 0) && (lambda_fonction == 0))
    {
      // typage MuLambda_TBLE_Cte
      mu_lambda.typer("MuLambda_TBLE_Cte");
    }
  else if ((mu_fonction == 1) && (lambda_fonction == 1))
    {
      // typage MuLambda_TBLE_Fonctions_T
      mu_lambda.typer("MuLambda_TBLE_Fcts_T");
      MuLambda_TBLE_Fcts_T& ml_fct = ref_cast(MuLambda_TBLE_Fcts_T, mu_lambda.valeur());
      ml_fct.setFcts(mu_chaine, lambda_chaine);
    }
  else
    {
      Cerr << "Vous devez preciser a la fois mu et lambda dans les options de TBLE" << finl;
      Process::exit();
    }
  mu_lambda.valeur().initialiser(getPbBase().milieu());


  // associer les "Eq_couch_lim" a "MuLambda_TBLE"
  for (int i=0; i<compteur_faces_paroi; i++)
    {
      eq_vit[i].associer_milieu(getPbBase().milieu());
      eq_vit[i].set_diffu(modele_visco); //modele de viscosite turbulente
      Diffu_totale_hyd_base& diffu_hyd = ref_cast_non_const(Diffu_totale_hyd_base, eq_vit[i].get_diffu()); //modele de viscosite turbulente
      diffu_hyd.associer_mulambda(mu_lambda.valeur());
    }



  //Pour l'instant, tout ce qui concerne le terme de Boussinesq dans les equations de TBLE se trouve dans les classe filles : PAroiVDF_TBLE et ParoiVEF_TBLE
  // On verra ce qu'on peut factoriser ici.
  // On verifie la presence ou non d'un terme source de Boussinesq dans le pb hydraulique :
  /*         const Sources& les_sources=eqnNS.sources();
             int nb_sources=les_sources.size();

             for (int j=0; j<nb_sources; j++)
             {
             const Source_base& ts = les_sources(j).valeur();
             if (sub_type(Terme_Boussinesq_temper_VDF_Face,ts))
             {
             Terme_Boussinesq_temper_VDF_Face& terme_boussi = ref_cast(Terme_Boussinesq_temper_VDF_Face,ts);
             T0 = terme_boussi.T0();
             boussi_ok=1;
             }
             }

  */

  return 1;
}


int Paroi_TBLE_QDM::reprendre(Entree&, const Zone_dis_base& zone_dis, const Zone_Cl_dis_base& la_zone_Cl, double tps_reprise)
{
  if (restart == 0)    // Si on souhaite bien reprendre TBLE
    {
      Cerr << "Reprise du champ TBLE au temps " << tps_reprise << finl;
      int compteur_faces_paroi = 0; //Reinitialisation de compteur_faces_paroi
      const int ME=Process::me();
      Nom nom_fic="sauvegarde_tble_";
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

      valeurs_reprises.resize(compteur_faces_paroi, nb_comp, nb_pts+1);
      compteur_faces_paroi=0;

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
                        {
                          fic_sauve >> valeurs_reprises(compteur_faces_paroi,comp,itble);
                        }
                    }
                  compteur_faces_paroi++;
                }
            }
        }
      fic_sauve.close();
      Cerr << "Reprise du champ TBLE OK" << finl;
      reprise_ok=1;
    }
  return 1;
}




int Paroi_TBLE_QDM::sauvegarder(Sortie&, const Zone_dis_base& zone_dis, const Zone_Cl_dis_base& la_zone_Cl,double tps) const
{
  Cerr << "Sauvegarde du champ TBLE  au temps = " << tps<< finl;
  const int ME=Process::me();
  Nom nom_fic="sauvegarde_tble_";
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
                    fic_sauve << eq_vit[compteur_faces_paroi].get_Unp1(comp,itble) << " " ;
                }
              fic_sauve << finl;
              compteur_faces_paroi++;
            }
        }
    }
  fic_sauve.close();
  return 1;
}


void Paroi_TBLE_QDM::imprimer_stat(Sortie&, double tps) const
{
  if (oui_stats==1)
    {
      for(int j=0; j<nb_post_pts; j++)
        {
          int num_face=num_faces_post(j);
          double coeff = 1./(tps-tps_deb_stats);
          Nom tmp;
          tmp="tble_stats_";
          tmp+=nom_pts[j];
          tmp+=".dat";

          SFichier fic_post(tmp, ios::app);

          if (Objet_U::dimension==2)
            {
              fic_post << "# "<< tps << "  U  V  U^2  V^2  UV Fx Fy"<< finl;

              for(int i=0; i<nb_pts+1; i++)
                {
                  fic_post << eq_vit[num_face].get_y(i) <<  " " ;
                  fic_post << coeff*Umean_sum(j,i) <<  " " ;
                  fic_post << coeff*Vmean_sum(j,i) << " " ;
                  fic_post << coeff*Umean_2_sum(j,i) << " " ;
                  fic_post << coeff*Vmean_2_sum(j,i) << " " ;
                  fic_post << coeff*UVmean_sum(j,i) << " " ;
                  fic_post << coeff*Fxmean_sum(j,i) << " " ;
                  fic_post << coeff*Fymean_sum(j,i) << " " << finl;
                }
            }
          else if (Objet_U::dimension==3)
            {
              fic_post << "# "<< tps << "  U  V  W  U^2  V^2  W^2  UV  WV  WU Fx Fy Fz "<< finl;

              for(int i=0; i<nb_pts+1; i++)
                {
                  fic_post << eq_vit[num_face].get_y(i) <<  " " ;
                  fic_post << coeff*Umean_sum(j,i) <<  " " ;
                  fic_post << coeff*Vmean_sum(j,i) << " " ;
                  fic_post << coeff*Wmean_sum(j,i) << " " ;
                  fic_post << coeff*Umean_2_sum(j,i) << " " ;
                  fic_post << coeff*Vmean_2_sum(j,i) << " " ;
                  fic_post << coeff*Wmean_2_sum(j,i) << " " ;
                  fic_post << coeff*UVmean_sum(j,i) << " " ;
                  fic_post << coeff*WVmean_sum(j,i) << " " ;
                  fic_post << coeff*WUmean_sum(j,i) << " " ;
                  fic_post << coeff*Fxmean_sum(j,i) << " " ;
                  fic_post << coeff*Fymean_sum(j,i) << " " ;
                  fic_post << coeff*Fzmean_sum(j,i) << " " ;
                  fic_post << finl;
                }
            }
        }

    }
}


void  Paroi_TBLE_QDM::calculer_stat(int j, int i, double Fx, double Fy, double Fz, double u, double v, double w, double dt)
{
  Fxmean_sum(j,i) += Fx*dt;
  Fymean_sum(j,i) += Fy*dt;
  Fzmean_sum(j,i) += Fz*dt;
  Umean_sum(j,i) += u*dt;
  Umean_2_sum(j,i) += u*u*dt;
  Vmean_sum(j,i) += v*dt;
  Vmean_2_sum(j,i) += v*v*dt;
  Wmean_sum(j,i) += w*dt;
  Wmean_2_sum(j,i) += w*w*dt;
  UVmean_sum(j,i) += u*v*dt;
  WVmean_sum(j,i) += w*v*dt;
  WUmean_sum(j,i) += w*u*dt;
}


