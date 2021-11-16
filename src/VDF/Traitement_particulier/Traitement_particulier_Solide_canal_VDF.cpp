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
// File:        Traitement_particulier_Solide_canal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Traitement_particulier
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_Solide_canal_VDF.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Champ_Face.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <LecFicDistribueBin.h>
#include <EcrFicCollecteBin.h>
#include <IntTrav.h>
#include <Conduction.h>
#include <DoubleTrav.h>
#include <communications.h>

Sortie& Traitement_particulier_Solide_canal_VDF::printOn(Sortie& is) const
{
  return is;
}


Entree& Traitement_particulier_Solide_canal_VDF::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_Solide_canal_VDF::lire(Entree& is)
{
  return Traitement_particulier_Solide_canal::lire(is);
}





Implemente_instanciable(Traitement_particulier_Solide_canal_VDF,"Traitement_particulier_Solide_canal_VDF",Traitement_particulier_Solide_canal);



// #################### Postraitement Statistique du Champ de Temperature #########

void Traitement_particulier_Solide_canal_VDF::post_traitement_particulier()
{

  DoubleTrav Tmoy(N);
  Tmoy=0.;
  DoubleTrav Trms(N);
  Trms=0.;

  double tps = mon_equation->inconnue().temps();

  // Moyennes spatiales :
  calculer_moyennes_spatiales_thermo(Tmoy,Trms,Y,corresp,compt);

  static double temps_dern_post_inst = -100.;
  if (fabs(tps-temps_dern_post_inst)>=dt_post_inst)
    {
      ecriture_fichier_moy_spat_thermo(Tmoy,Trms,Y);
      temps_dern_post_inst = tps;
    }


  // Moyennes temporelles :
  if ((tps>=temps_deb)&&(tps<=temps_fin))
    {
      static int init_stat_temps = 0;
      if(init_stat_temps==0)
        {
          double dt_v = mon_equation->schema_temps().pas_de_temps();
          temps_deb = tps-dt_v;
          init_stat_temps++;
        }

      calculer_integrales_temporelles(Tmoy_temp,Tmoy);
      calculer_integrales_temporelles(T2_moy_temp,Trms);

      static double temps_dern_post_stat = -100.;
      if (fabs(tps-temps_dern_post_stat)>=dt_post_stat)
        {
          double dt = tps-temps_deb;
          ecriture_fichier_moy_temp_thermo(Tmoy_temp,T2_moy_temp,Y,dt);
          temps_dern_post_stat = tps;
        }
    }
}





// #################### Calcul Moyennes Spatiales #################################

void Traitement_particulier_Solide_canal_VDF::calculer_moyennes_spatiales_thermo(DoubleVect& tmoy, DoubleVect& trms,const DoubleVect& tabY, IntVect& tab_corresp, IntVect& tab_compt)
{

  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);

  // Le nombre d'elements de la zone VDF.
  int nb_elems = zone_VDF.zone().nb_elem();

  // On veut acceder aux valeurs de la temperature a partir de mon_equation_NRJ.
  const DoubleTab& Temp = mon_equation.valeur().inconnue().valeurs();

  // t2moy correspond
  DoubleTrav t2moy(N);
  t2moy = 0.;


  int num_elem,j;

  // tmoy est la moyenne de T : <T>(y,t)
  tmoy = 0.;
  // trms est l'ecart-type de la temperature : sqrt(<Tp*Tp>)(y,t)=<T*T>-<T*T>
  trms = 0.;

  // On parcourt tous les elements pour faire toutes les moyennes au centre des elements.
  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      // <T>
      tmoy[tab_corresp[num_elem]] += Temp[num_elem];
      // Trms : en fait ici on calcule <T*T> mais ensuite on va soustraire la partie <T*T>
      trms[tab_corresp[num_elem]] += Temp[num_elem]*Temp[num_elem];
    }


  // POUR LE PARALLELE !!
  IntVect compt_p(tab_compt);
  envoyer(compt_p,Process::me(),0,Process::me());

  DoubleVect tmoy_p(tmoy);
  envoyer(tmoy_p,Process::me(),0,Process::me());

  DoubleVect trms_p(trms);
  envoyer(trms_p,Process::me(),0,Process::me());

  if(je_suis_maitre())
    {
      IntVect compt_tot(tab_compt);
      DoubleVect tmoy_tot(tmoy);
      DoubleVect trms_tot(trms);

      compt_tot=0;
      tmoy_tot=0.;
      trms_tot=0.;

      for(int p=0; p<Process::nproc(); p++)
        {
          recevoir(compt_p,p,0,p);
          compt_tot+=compt_p;

          recevoir(tmoy_p,p,0,p);
          tmoy_tot+=tmoy_p;

          recevoir(trms_p,p,0,p);
          trms_tot+=trms_p;
        }

      // compt[j] correspond au nombre d'elements qui ont ete utilises pour calculer une meme valeur de temperature moyenne,
      // ie le nombre d'elements ayant la meme coordonnee Y.
      for (j=0; j<N; j++)
        {
          tmoy[j] = tmoy_tot[j] / compt_tot[j];
          trms[j] = trms_tot[j] / compt_tot[j];

          trms[j] -= tmoy[j]*tmoy[j];
        }
    }

}



// #################### Calcul Integrale Temporelle ###############################

void Traitement_particulier_Solide_canal_VDF::calculer_integrales_temporelles(DoubleVect& moy_temp, const DoubleVect& moy_spat)
{
  double dt_v = mon_equation->schema_temps().pas_de_temps();
  if(je_suis_maitre())
    ////moy_temp.ajoute(dt_v,moy_spat);
    moy_temp.ajoute_sans_ech_esp_virt(dt_v,moy_spat);
}





// #################### Ecriture Moyennes Spatiales dans Fichier ##################

void Traitement_particulier_Solide_canal_VDF::ecriture_fichier_moy_spat_thermo(const DoubleVect& Tmoy, const DoubleVect& Trms,const DoubleVect& tabY)
{
  Nom nom_fic = "Solid_spat_";
  double tps = mon_equation->inconnue().temps();
  Nom temps = Nom(tps);

  nom_fic+= temps;
  nom_fic+=".dat";

  if(je_suis_maitre())
    {
      SFichier fic (nom_fic);
      fic << "# Space Averaged Statistics in the Solid" << finl ;
      fic << "# Y     <T>     Trms    " << finl ;
      fic.precision(6);

      for (int j=0; j<N  ; j++)
        fic << tabY[j] << " " << Tmoy[j] << " " << sqrt(Trms[j]) << finl;

      fic.flush();
      fic.close();
    } // FIN du maitre
}





// #################### Ecriture Moyennes temporelles dans Fichier ################

void Traitement_particulier_Solide_canal_VDF::ecriture_fichier_moy_temp_thermo(const DoubleVect& Tmoy, const DoubleVect& Trms, const DoubleVect& tabY, const double& dt)
{
  Nom nom_fic = "Solid_temp_";
  double tps = mon_equation->inconnue().temps();
  Nom temps = Nom(tps);

  nom_fic+= temps;
  nom_fic+=".dat";

  if(je_suis_maitre())
    {
      SFichier fic (nom_fic);
      fic << "# Time Averaged Statistics in the Solid" << finl ;
      fic << "# Y     <T>     Trms     " << finl ;
      fic.precision(6);

      for (int j=0; j<N  ; j++)
        fic << tabY[j] << " " << Tmoy[j]/dt << " " << sqrt(Trms[j]/dt) << finl;
      fic.flush();
      fic.close();

    } // FIN du maitre
}



void Traitement_particulier_Solide_canal_VDF::init_calcul_stats(void)
{
  Tmoy_temp.resize(N);
  Tmoy_temp=0;
  T2_moy_temp.resize(N);
  T2_moy_temp=0;
}



void Traitement_particulier_Solide_canal_VDF::init_calcul_moyenne(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);

  const DoubleTab& xp = zone_VDF.xp();

  int nb_elems = zone_VDF.zone().nb_elem();

  int num_elem,j,indic,trouve;
  double y;
  j=0;
  indic = 0;

  // dimensionnement aux valeurs rentrees dans le jeu de donnees
  Y.resize(N);
  compt.resize(N);
  corresp.resize(nb_elems);

  Y = -100.;
  compt = 0;
  corresp = -1;

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y = xp(num_elem,1);
      trouve = 0;

      for (j=0; j<indic+1; j++)
        {
          if(y==Y[j])
            {
              corresp[num_elem] = j;
              compt[j] ++;
              j=indic+1;
              trouve = 1;
              break;
            }
        }
      if (trouve==0)
        {
          corresp[num_elem] = indic;
          Y[indic]=y;
          compt[indic] ++;
          indic++;
        }
    }
  N = indic;  // nombre de y pour Temperature

  Y.resize(N);

  compt.resize(N);
}




