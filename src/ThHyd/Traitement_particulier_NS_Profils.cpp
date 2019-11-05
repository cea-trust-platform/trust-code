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
// File:        Traitement_particulier_NS_Profils.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_Profils.h>
#include <LecFicDistribueBin.h>
#include <EcrFicCollecteBin.h>
#include <Mod_turb_hyd_base.h>
#include <Navier_Stokes_std.h>

Implemente_base(Traitement_particulier_NS_Profils,"Traitement_particulier_NS_Profils",Traitement_particulier_NS_base);


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
Sortie& Traitement_particulier_NS_Profils::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_Profils::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_Profils::lire(Entree& is)
{
  // Initialisation
  oui_u_inst = 0;
  oui_stat = 0;
  oui_repr = 0;
  oui_repr_nu_t = 0;
  oui_profil_nu_t = 0;
  IntVect Verif(8);
  n_probes = 1;
  Verif=0;

  // END Initialisation
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(8);
      les_mots[0] = "const_profile";
      les_mots[1] = "num_probes";
      les_mots[2] = "dt_output";
      les_mots[3] = "stats";
      les_mots[4] = "profil_nu_t";
      les_mots[5] = "reprise";
      les_mots[6] = "reprise_nu_t";
      les_mots[7] = "homog_dir";
      is >> motlu;
      while(motlu != accfermee)
        {
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                // For const_profile Constant Profile value : X, Y, Z
                is >> motlu;      // Direction of constant profile
                if((motlu=="x")||(motlu=="X")) dir_profil = 0;
                else if((motlu=="y")||(motlu=="Y")) dir_profil = 1;
                else if((motlu=="z")||(motlu=="Z")) dir_profil = 2;
                else
                  {
                    Cerr << "Wrong value for direction of profile !" << finl;
                    exit();
                  }
                Cerr << "Profiles at constant " << motlu << finl;
                break;
              }
            case 1 :
              {
                // Number of probes and reading of the values of the postions
                is >> n_probes;
                if(n_probes<1)
                  {
                    Cerr << "Error in Traitement_particulier_NS_Profils::lire" << finl;
                    Cerr << "Wrong value for number of probes (" << n_probes << ") :" << finl;
                    Cerr << "There must be at least one profile !" << finl;
                    exit();
                  }
                positions.resize(n_probes);
                for(int i=0; i<n_probes; i++)
                  {
                    is >> positions(i);
                    Cerr << "  Probe_" << i+1 << " = " << positions(i) << finl;
                  }
                break;
              }
            case 2 :
              {
                // Pour u_inst
                is >> dt_post_inst;      // intervalle de temps de sortie des moyennes spatiales
                // initialisation pour le calcul des stats spat.
                oui_u_inst = 1;
                Cerr << "Spatial average parameters read..." << finl;
                Cerr << "dt_post_inst : " << dt_post_inst << finl;
                break;
              }
            case 3 :
              {
                // For Time average
                is >> temps_deb;     // Initial time for time average statistics.
                is >> temps_fin;     // Final time for time average statistics.
                is >> dt_post_stat;  // Output intervalfor time average statistics.
                oui_stat=1;          // =1 : on calcule des stats temp.
                Cerr << "Reading time statitics parameters..." << finl;
                Cerr << "Initial time : " << temps_deb << " End time : " << temps_fin << finl;
                Cerr << "Time interval for output : " << dt_post_stat << finl;
                // Verif ensuite pour voir si on a rentrer les valeurs specifiques aux calculs des stats spat.
                break;
              }
            case 4 :
              {
                // For eddy viscosity profile
                oui_profil_nu_t = 1;
                Cerr << "User asked for statistics on eddy-viscosity..." << finl;
                const RefObjU& modele_turbulence = mon_equation.valeur().get_modele(TURBULENCE);
                const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
                if(sub_type(Mod_turb_hyd_base,mod_turb))
                  Cerr << "Statistics for eddy-viscosity : OK !" << finl;
                else
                  {
                    Cerr << "User asked for eddy-viscosity profile," << finl;
                    Cerr << "but is not defined in laminar case !" << finl;
                    Cerr << "Please check the problem type..." << finl;
                    exit();
                  }
                break;
              }
            case 5 :
              {
                oui_repr = 1;
                is  >> fich_repr_u ;  // indication du nom du fichier de reprise des stats
                Cerr << "The time statistics file is : " << fich_repr_u << finl;
                break;
              }
            case 6 :
              {
                // For eddy viscosity profile Reprise
                oui_repr_nu_t = 1;
                is  >> fich_repr_nu_t ;  // indication du nom du fichier de reprise des stats de nut
                Cerr << "Continuing statistics on eddy-viscosity : OK!" << finl;
                break;
              }
            case 7 :
              {
                // For homogeneous direction specification : X, Y, Z
                is >> motlu;      // Direction of homogeneity
                if((motlu=="x")||(motlu=="X")) homo_dir = 0;
                else if((motlu=="y")||(motlu=="Y")) homo_dir = 1;
                else if((motlu=="z")||(motlu=="Z")) homo_dir = 2;
                else
                  {
                    Cerr << "Wrong value for homogeneous direction !" << finl;
                    exit();
                  }
                Cerr << "Homogeneous direction : " << motlu << finl;
                break;
              }
            default :
              {
                lire(motlu,is);
                break;
              }
            }
          is >> motlu;
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Error while reading in Traitement_particulier_NS_Profils::lire" << finl;
          Cerr << "We were expecting a }" << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_NS_Profils" << finl;
      Cerr << "We were expecting a {" << finl;
      exit();
    }

  if ((oui_u_inst!=1)&&(oui_stat!=0))
    {
      Cerr << "User asked for time statistics :" << finl;
      Cerr << "To do so, you also need to specify the parameters for space statistics," << finl;
      Cerr << "since it is used for the time stats.." << finl;
      exit();
    }
  if ((oui_profil_nu_t!=1)&&(oui_repr_nu_t!=0))
    {
      Cerr << "User asked for time statistics on nu_t :" << finl;
      Cerr << "pour cela il faut egalement demander le calcul des stats spatiales," << finl;
      Cerr << "puisque il est necessaire au calcul des stats temp." << finl;
      exit();
    }
  return is;
}

// Entree& Traitement_particulier_NS_canal::lire(const Motcle& motlu, Entree& is)
// {
//   return is;
// }

void Traitement_particulier_NS_Profils::preparer_calcul_particulier()
{
  if ((oui_u_inst != 0)||(oui_profil_nu_t != 0))
    init_calcul_moyenne();
  if (oui_stat != 0)
    init_calcul_stats();
}

void Traitement_particulier_NS_Profils::init_calcul_stats(void)
{
  if (oui_repr!=1) // ce n'est pas une reprise des stats specifiques = pas de lecture dans un fichier
    {
      u_moy_temp_x.resize(n_probes,Nap);
      u_moy_temp_x=0;
      u_moy_temp_y.resize(n_probes,Nap);
      u_moy_temp_y=0;
      u_moy_temp_z.resize(n_probes,Nap);
      u_moy_temp_z=0;

      uv_moy_temp.resize(n_probes,Nap);
      uv_moy_temp=0; //  uv_moy moyenne en temps

      u2_moy_temp.resize(n_probes,Nap);
      u2_moy_temp=0;
      v2_moy_temp.resize(n_probes,Nap);
      v2_moy_temp=0;
      w2_moy_temp.resize(n_probes,Nap);
      w2_moy_temp=0;

      u3_moy_temp.resize(n_probes,Nap);
      u3_moy_temp=0;
      v3_moy_temp.resize(n_probes,Nap);
      v3_moy_temp=0;
      w3_moy_temp.resize(n_probes,Nap);
      w3_moy_temp=0;

      u4_moy_temp.resize(n_probes,Nap);
      u4_moy_temp=0;
      v4_moy_temp.resize(n_probes,Nap);
      v4_moy_temp=0;
      w4_moy_temp.resize(n_probes,Nap);
      w4_moy_temp=0;

      nu_t_temp.resize(n_probes,Nap);
      nu_t_temp=0;
    }
}

void Traitement_particulier_NS_Profils::reprendre_stat()
{
  double tps = mon_equation->inconnue().temps();
  int j,num_prof;
  double ti,ti2;
  Nom temps;
  if (oui_stat == 1)
    {
      ifstream ficu0(fich_repr_u);
      if (ficu0)
        {
          LecFicDistribueBin ficu (fich_repr_u);
          ficu >> temps;
          u_moy_temp_x.resize(n_probes,Nap);
          u_moy_temp_y.resize(n_probes,Nap);
          u_moy_temp_z.resize(n_probes,Nap);

          uv_moy_temp.resize(n_probes,Nap);
          u2_moy_temp.resize(n_probes,Nap);
          v2_moy_temp.resize(n_probes,Nap);
          w2_moy_temp.resize(n_probes,Nap);

          u3_moy_temp.resize(n_probes,Nap);
          v3_moy_temp.resize(n_probes,Nap);
          w3_moy_temp.resize(n_probes,Nap);

          u4_moy_temp.resize(n_probes,Nap);
          v4_moy_temp.resize(n_probes,Nap);
          w4_moy_temp.resize(n_probes,Nap);

          for(num_prof=0; num_prof<n_probes; num_prof++)
            {
              ficu >> Nxy(num_prof) >> Nyy(num_prof) >> Nzy(num_prof) >> Nuv(num_prof);
              for (j=0; j<Nxy(num_prof); j++)
                {
                  ficu >> u_moy_temp_x(num_prof,j) ;
                  ficu >> u2_moy_temp(num_prof,j);
                }
              for (j=0; j<Nyy(num_prof); j++)
                {
                  ficu >> u_moy_temp_y(num_prof,j) ;
                  ficu >> v2_moy_temp(num_prof,j);
                }
              for (j=0; j<Nzy(num_prof); j++)
                {
                  ficu >> u_moy_temp_z(num_prof,j);
                  ficu >> w2_moy_temp(num_prof,j);
                }
              for (j=0; j<Nuv(num_prof); j++)
                ficu >> uv_moy_temp(num_prof,j) ;
            }
          //END reading "n_probes" profiles
          ficu >> ti ;
          Cerr << "ti=" << ti << finl;
          Nom chc_ti = Nom(ti), chc_temps_deb = Nom(temps_deb);
          Cerr << "chc_ti=" << chc_ti << "   chc_temps_deb=" << chc_temps_deb << finl;
          if (chc_ti!=chc_temps_deb)
            {
              if (ti > temps_deb)
                {
                  Cerr << "pb de reprise des stats de la vitesse!!" << finl;
                  Cerr << "Le temps de debut des stats demande " << temps_deb  << finl;
                  Cerr << "est inferieur a celui de debut des stats sauvees !" << ti << finl;
                  exit();
                }
              else
                {
                  if (temps_deb>=tps)
                    {
                      Cerr << "On recommence le calcul des stats de la vitesse a partir de t=" << temps_deb << "s" << finl;
                      u_moy_temp_x = 0.;
                      u_moy_temp_y = 0.;
                      u_moy_temp_z = 0.;
                      uv_moy_temp = 0.;
                      u2_moy_temp = 0.;
                      v2_moy_temp = 0.;
                      w2_moy_temp = 0.;
                      oui_repr = 0;
                    }
                  else
                    {
                      Cerr << "On a deja depasse le temps auquel vous voulez commencer les stats!!" << finl;
                      exit();
                    }
                }
            }
          else
            {
              Cerr << "On continue le calcul des stats de la vitesse, debute a t=" <<  ti << "s" << finl;
              temps_deb = ti;
            }
        }
      else
        {
          if (tps<=temps_deb)
            {
              Cerr << "On n a pas encore debute le calcul des stats" << finl;
              oui_repr = 0;
            }
          else
            {
              Cerr << "Il faut donner le fichier pour reprendre les stats!!" << finl;
              exit();
            }
        }

      if ((oui_profil_nu_t!=0)&&(oui_repr_nu_t!=0))
        {
          ifstream ficu1(fich_repr_nu_t);
          if (ficu1)
            {
              LecFicDistribueBin ficu (fich_repr_nu_t);
              ficu >> temps;
              nu_t_temp.resize(n_probes,Nap);
              for(num_prof=0; num_prof<n_probes; num_prof++)
                {
                  ficu >>  Nuv(num_prof);
                  for (j=0; j<Nuv(num_prof); j++)
                    ficu >> nu_t_temp(num_prof,j) ;
                }
              ficu >> ti2 ;
              if (ti2!=ti)
                {
                  Cerr << "Pbs : les stats de nut et de la vitesse sont decalees!!" << finl;
                  exit();
                }
              Cerr << "ti2=" << ti2 << finl;
              Nom chc_ti2 = Nom(ti2), chc_temps_deb = Nom(temps_deb);
              Cerr << "chc_ti2=" << chc_ti2 << "   chc_temps_deb=" << chc_temps_deb << finl;
              if (chc_ti2!=chc_temps_deb)
                {
                  if (ti2 > temps_deb)
                    {
                      Cerr << "pb de reprise des stats de nu_t!!" << finl;
                      Cerr << "Le temps de debut des stats demande " << temps_deb  << finl;
                      Cerr << "est inferieur a celui de debut des stats sauvees !" << ti << finl;
                      exit();
                    }
                  else
                    {
                      if (temps_deb>=tps)
                        {
                          Cerr << "On recommence le calcul des stats de nu_t a partir de t=" << temps_deb << "s" << finl;
                          nu_t_temp = 0.;
                          oui_repr_nu_t = 0;
                        }
                      else
                        {
                          Cerr << "On a deja depasse le temps auquel vous voulez commencer les stats !" << finl;
                          exit();
                        }
                    }
                }
              else
                {
                  Cerr << "On continue le calcul des stats de nu_t, debute a t=" <<  ti << "s" << finl;
                  temps_deb = ti2;
                }
            }
          else
            {
              if (tps<=temps_deb)
                {
                  Cerr << "On n a pas encore debute le calcul des stats" << finl;
                  oui_repr_nu_t = 0;
                }
              else
                {
                  Cerr << "Il faut donner le fichier pour reprendre les stats !" << finl;
                  exit();
                }
            }
        }
    }
  else
    {
      Cerr << "Pas de Calcul des stats commence !" << finl;
      Cerr << "Pas de reprise de u_moy_temp,u_et_temp et teta_moy_temp" << finl;
    }
}

void Traitement_particulier_NS_Profils::sauver_stat() const
{
  double tps = mon_equation->inconnue().temps();
  if (  (oui_stat == 1)&&(tps>=temps_deb)&&(tps<=temps_fin) )
    {
      Cerr << "Dans Traitement_particulier_NS_canal::sauver_stat!!" << finl;
      // On sauve la somme (sans diviser par dt)
      int j;
      Nom temps = Nom(tps);
      Nom fich_sauv_u ="u_moy_temp_";
      fich_sauv_u+=temps;
      fich_sauv_u+=".sauv";

      // On sauve u_moy!!
      EcrFicCollecteBin ficu (fich_sauv_u);
      //      EcrFicCollecte ficu (fich_sauv_u);
      //      EcrFicCollecteBin ficu ("u_moy_temp.sauv");
      ficu << temps << finl;
      int num_prof;
      for(num_prof=0; num_prof<n_probes; num_prof++)
        {
          ficu << Nxy(num_prof) << Nyy(num_prof) << Nzy(num_prof) << Nuv(num_prof) << finl;
          for (j=0; j<Nxy(num_prof); j++)
            {
              ficu << u_moy_temp_x(num_prof,j) << finl;
              ficu << u2_moy_temp(num_prof,j) << finl;
            }
          for (j=0; j<Nyy(num_prof); j++)
            {
              ficu << u_moy_temp_y(num_prof,j) << finl;
              ficu << v2_moy_temp(num_prof,j) << finl;
            }
          for (j=0; j<Nzy(num_prof); j++)
            {
              ficu << u_moy_temp_z(num_prof,j) << finl;
              ficu << w2_moy_temp(num_prof,j) << finl;
            }
          for (j=0; j<Nuv(num_prof); j++)
            ficu << uv_moy_temp(num_prof,j) << finl;
        }
      ficu << temps_deb << finl;
      Cerr << "Sauvegarde de  u_moy_temp a t=" << tps << finl;
      Cerr << "Debut du calcul des stats a t=" << temps_deb << " a noter pour la reprise des stats !" << finl;

      if (oui_profil_nu_t!=0)
        {
          Nom fich_sauv_nut ="nut_temp_";
          fich_sauv_nut+=temps;
          fich_sauv_nut+=".sauv";
          // On sauve nut!!
          EcrFicCollecteBin ficu2 (fich_sauv_nut);
          ficu2 << temps << finl;
          for(num_prof=0; num_prof<n_probes; num_prof++)
            {
              ficu2 << Nuv(num_prof) << finl;
              for (j=0; j<Nuv(num_prof); j++)
                ficu2 << nu_t_temp(num_prof,j) << finl;
            }
          ficu2 << temps_deb << finl;
          Cerr << "Sauvegarde de  nu_t_temp a t=" << tps << finl;
          Cerr << "Debut du calcul des stats a t=" << temps_deb << " a noter pour la reprise des stats !" << finl;
        }
    }
}

