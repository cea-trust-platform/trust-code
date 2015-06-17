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
// File:        Traitement_particulier_NS_THI_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_THI_VEF.h>
#include <calcul_spectres.h>
#include <Zone_VEF.h>
#include <Navier_Stokes_std.h>
#include <Champ_P1NC.h>
#include <Domaine.h>
#include <Periodique.h>
#include <DoubleTrav.h>
#include <Pave.h>
#include <Pb_Hydraulique_Turbulent.h>
#include <Noms.h>
#include <Nom.h>
#include <distances_VEF.h>
#include <SFichier.h>
#include <DoubleVect.h>
#include <MD_Vector_tools.h>

// appels librairies fonctions calcul FFT (JMFFT)
extern "C" void fftfax(int ,int* ,double* );
extern "C" void cftfax(int ,int* ,double* );
extern "C" void rfftmlt(double*, double*, double*, int*,  int, int, int, int, int);



// Avec cette classe le calcul de spectres en VEF ne depend pas de la
// discretisation. On peut obtenir :
// * des spectres 3D en construisant des grilles cartesiennes (les grilles construites dependent de la discretisation),
// * des spectres 1D, en suivant des lignes et non plus des grilles cartesiennes.


Implemente_instanciable(Traitement_particulier_NS_THI_VEF,"Traitement_particulier_NS_THI_VEF",Traitement_particulier_NS_THI);


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
Sortie& Traitement_particulier_NS_THI_VEF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_THI_VEF::readOn(Entree& is)
{
  return is;
}
Entree& Traitement_particulier_NS_THI_VEF::lire(Entree& is)
{
  // valeurs par defaut
  init = 0;
  fac_init = 0;
  oui_transf = 0;
  oui_calc_spectre = 0;
  oui_conservation_Ec = 0;
  calc_sp_3D = 0;
  calc_sp_1D = 0;
  calc_correlations = 0;
  cle_suppr_vit_moy = 0;
  nb_champs_scalaires=0;
  periode_calc_spectre=0;
  L_BOITE=-1;
  Ec_init=-1;
  // FIN valeurs par defaut

  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle valec="val_Ec";
  Motcle facon="facon_init";
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(14);
      {
        les_mots[0] = "init_Ec";
        les_mots[1] = "periode_calc_spectre";
        les_mots[2] = "calc_spectre";
        les_mots[3] = "conservation_Ec";
        les_mots[4] = "longueur_boite";
        les_mots[6] = "3D";
        les_mots[7] = "1D";
        les_mots[8] = "suppr_vit_moy";
        les_mots[12] = "correlations";
        les_mots[13] = "champs_scalaires";
      }
      {
        is >> motlu;
        while(motlu != accfermee)
          {
            int rang=les_mots.search(motlu);
            switch(rang)
              {
              case 0 :
                {
                  is >> init;
                  if (init!=0)
                    {
                      is >> motlu;
                      if (motlu==valec)
                        {
                          is >> Ec_init;
                          if (motlu==facon)
                            is >> fac_init; // Sur quelle valeur se base l initialisation??
                          if(je_suis_maitre())
                            {
                              Cerr << "Avec initialisation de l'energie cinetique sur la valeur Ec_init=" << Ec_init << finl;
                              Cerr << "par la methode (fac_init=" << fac_init << ") :";
                              switch(fac_init)
                                {
                                case 0 :
                                  {
                                    Cerr << " en se basant sur Ec_spatial" << finl;
                                    break ;
                                  }
                                case 1 :
                                  {
                                    Cerr << " en se basant sur Ec_spectral_1D" << finl;
                                    break ;
                                  }
                                case 3 :
                                  {
                                    Cerr << " en se basant sur Ec_spectral_3D" << finl;
                                    break ;
                                  }
                                }
                            }
                        }
                      else
                        {
                          if(je_suis_maitre())
                            {
                              Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI" << finl;
                              Cerr << "Le seul mot cle possible ici est : " << valec << finl;
                              Cerr << "Vous avez lu :" << motlu << finl;
                              Process::exit();
                            }
                        }
                    }
                  break;
                }
              case 1 :
                {
                  is >> periode_calc_spectre;
                  double temps_courant = mon_equation->inconnue().temps();
                  compteur_perio_spectre = int(temps_courant / periode_calc_spectre) + 1;
                  if (calc_sp_3D)
                    {
                      if(je_suis_maitre())
                        Cerr << "Calcul des spectres 3D tous les : " << periode_calc_spectre << finl;
                    }
                  if (calc_sp_1D)
                    {
                      if(je_suis_maitre())
                        Cerr << "Calcul des spectres 1D tous les : " << periode_calc_spectre << finl;
                    }
                  break;
                }
              case 2 :
                {
                  is >> oui_calc_spectre;
                  break;
                }
              case 3 :
                {
                  oui_conservation_Ec=1;
                  if(je_suis_maitre())
                    Cerr << "A chaque pas de temps, on multiplie le champ de vitesse par le coefficient permettant d'assurer Ec(n+1)=Ec(n)" << finl;
                  break;
                }
              case 4 :
                {
                  is >> L_BOITE;
                  break;
                }
              case 6 :
                {
                  is >> calc_sp_3D;
                  break;
                }
              case 7 :
                {
                  is >> calc_sp_1D;
                  break;
                }
              case 8 :
                {
                  is >> cle_suppr_vit_moy;
                  break;
                }
              case 12 :
                {
                  is >> calc_correlations;
                  break;
                }
              case 13 :
                {
                  is >> noms_champs_scalaires;
                  nb_champs_scalaires = noms_champs_scalaires.size();
                  break;
                }
              default :
                {
                  if(je_suis_maitre())
                    {
                      Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI" << finl;
                      Cerr << "Les mots cles possibles sont :" << finl;
                      Cerr << les_mots;
                      Cerr << "Vous avez lu :" << motlu << finl;
                      Process::exit();
                    }
                  break;
                }
              }
            is >> motlu;
          }
        is >> motlu;
        if (motlu != accfermee)
          {
            if(je_suis_maitre())
              {
                Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI" << finl;
                Cerr << "On attendait une }" << finl;
                Cerr << "Vous avez lu :" << motlu << finl;
                Process::exit();
              }
          }
      }
    }
  else
    {
      if(je_suis_maitre())
        {
          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_THI" << finl;
          Cerr << "On attendait une {" << finl;
          Cerr << "Vous avez lu :" << motlu << finl;
          Process::exit();
        }
    }

  if (L_BOITE <0)
    {
      Cerr << " Vous devez entrer la longueur de la boite (cubique) de calcul avec le mot cle longueur_boite" << finl;
      Process::exit();
    }
  return is;
}


void Traitement_particulier_NS_THI_VEF::init_calc_spectre(void)
// Appelle les fonctions qui a priori ne doivent etre lancees qu'une fois
// (sauf si le domaine de calcul change, raffinement, ...)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const int nb_faces = zone_VEF.nb_faces();
  IntTab tab_zone;
  DoubleTab coord_zone(nb_faces,dimension);
  IntVect nb_coord_zone(dimension);

  if (cle_suppr_vit_moy)
    suppression_vitesse_moyenne();

  if ( (calc_sp_3D || calc_sp_1D) && nproc()==1)
    determine_tab_fft_VEF_3D(tab_zone, coord_zone, nb_coord_zone, nb_spectres_3D, nb_points_3D);

  if (calc_sp_1D && nproc()==1)
    determine_tab_fft_VEF_1D(tab_zone, coord_zone, nb_coord_zone, nb_spectres_1D, nb_points_1D);

  cle_premier_pas_dt=0;
  Ec_tot_old = 0;
  temps_old = 0;



  const int nb_elem = zone_VEF.nb_elem();

  DoubleTab coord_zone_elem(nb_elem,dimension);
  IntVect nb_coord_zone_elem(dimension);

  if ( (calc_sp_3D || calc_sp_1D) && nproc()==1)
    determine_tab_fft_VEF_3D(tab_zone, coord_zone_elem, nb_coord_zone_elem, nb_spectres_3D_elem, nb_points_3D_elem);



}


void Traitement_particulier_NS_THI_VEF::renorm_Ec(void)
{
  // Ajuste les valeurs du champ de vitesse pour retrouver une energie definie.
  // La renormalisation peut se faire sur l'energie de l'espace physique ou
  // sur l'energie de l'espace spectral (3D ou 1D).
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double Ec;

  switch (fac_init)
    {
    case 0 :
      {
        // renormalisation par rapport a la valeur de l'energie "physique"
        Ec = calcul_Ec_spatial(vitesse, "init");
        if(je_suis_maitre())
          Cerr << "Ec (espace physique) = " << Ec << finl;
        break;
      }
    case 1 :
      {
        // renormalisation par rapport a la valeur de l'energie "spectrale" 1D
        calcul_spectre_1D(vitesse, "init", Ec);
        if(je_suis_maitre())
          Cerr << "Ec (espace spectral 1D) = " << Ec << finl;
        break;
      }
      //   case 2 :
      //     {
      //       // multiplication de tout le champ vitesse par une valeur
      //       multiplier_vitesse(facteur_init);
      //       return;
      //     }
    case 3 :
      {
        // renormalisation par rapport a la valeur de l'energie "spectrale" 3D
        calcul_spectre_3D(vitesse, "init", Ec);
        if(je_suis_maitre())
          Cerr << "Ec (espace spectral 3D) = " << Ec << finl;
        break;
      }
    default :
      {
        if(je_suis_maitre())
          {
            Cerr << "Traitement_particulier_NS_THI_VEF::renorm_Ec" << finl;
            Cerr << "Impossible de trouver la methode demandee pour initialiser l'energie cinetique : fac_init = " << fac_init << finl;
            assert( 0 );
            Process::exit();
          }
      }
    }

  double nEc=pow(Ec/Ec_init,0.5);
  if(je_suis_maitre())
    Cerr << "Renormalisation pour Ec_init = " << Ec_init << finl;
  vitesse/=nEc;

}






void Traitement_particulier_NS_THI_VEF::calcul_spectre(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const int nb_faces = zone_VEF.nb_faces();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs(); // vitesse aux faces
  //  const Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());
  Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());


  DoubleTab vitesse_bar(vitesse);
  champ_vitesse.filtrer_L2(vitesse_bar);

  DoubleTab vitesse_prm(nb_faces, dimension);
  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<dimension; dim++)
        {
          vitesse_prm(num_face, dim) = vitesse(num_face, dim) - vitesse_bar(num_face, dim);
        }
    }

  Champ_P1NC champ_vitesse_bar(champ_vitesse);
  champ_vitesse_bar.valeurs().ref(vitesse_bar);
  Champ_P1NC champ_vitesse_prm(champ_vitesse);
  champ_vitesse_prm.valeurs().ref(vitesse_prm);



  double temps = mon_equation->inconnue().temps();

  if(je_suis_maitre())
    Cerr << finl << "temps = " << temps << finl << finl;
  temps /= temps_retournement;
  if(je_suis_maitre())
    Cerr << finl << "temps (adimensionne) = " << temps << finl << finl;

  calcul_vitesse_moyenne(vitesse, moyenne_vitesse);


  // on traite les champs scalaires
  for (int i=0; i<nb_champs_scalaires; i++)
    {
      calcul_moyenne(mon_equation->probleme().get_champ(noms_champs_scalaires[i]).valeurs(), moyennes_scal(i));
    }

  impression_moyenne();

  double Ec;

  if (calc_sp_3D && nproc()==1)
    {
      calcul_spectre_3D(vitesse, "tot", Ec);
      calcul_spectre_3D(vitesse_bar, "bar", Ec);
      calcul_spectre_3D(vitesse_prm, "prm", Ec);

      // on traite les champs scalaires
      for (int i=0; i<nb_champs_scalaires; i++)
        {
          const DoubleTab& scal = mon_equation->probleme().get_champ(noms_champs_scalaires[i]).valeurs();
          DoubleTab temp;
          temp.resize(vitesse.dimension(0),dimension);
          for (int j=0; j<temp.dimension(0); j++)
            {
              for (int d=0; d<dimension; d++)
                temp(j,d) = scal(j);
            }
          calcul_spectre_3D(temp, noms_champs_scalaires[i]+"_tot", Ec);
        }

      if(je_suis_maitre())
        Cerr << finl;
    }



  if (calc_sp_1D && nproc()==1)
    {
      calcul_spectre_1D(vitesse, "tot", Ec);
      calcul_spectre_1D(vitesse_bar, "bar", Ec);
      calcul_spectre_1D(vitesse_prm, "prm", Ec);

      // Pas de spectres 1d pour les scalaires
    }


}




void Traitement_particulier_NS_THI_VEF::sorties_globales(void)
{
  // Pour les traitements effectues a chaque pas de temps

  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const int nb_faces = zone_VEF.nb_faces();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs(); // vitesse aux faces
  const Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());
  double temps = mon_equation->inconnue().temps();


  DoubleTab vitesse_bar(vitesse);
  champ_vitesse.filtrer_L2(vitesse_bar);

  DoubleTab vitesse_prm(nb_faces, dimension);
  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<dimension; dim++)
        {
          vitesse_prm(num_face, dim) = vitesse(num_face, dim) - vitesse_bar(num_face, dim);
        }
    }

  double Ec_tot;
  Ec_tot = calcul_Ec_spatial(vitesse, "tot");
  if (je_suis_maitre() && cle_premier_pas_dt)
    {
      Nom fichier = Nom("Sorties_diff_tot");
      SFichier fic (fichier,ios::app);
      Nom fichier_eps = Nom("Sorties_epsilon_-2_3");
      SFichier fic_eps (fichier_eps,ios::app);
      Nom fichier_eps_ret = Nom("Sorties_epsilon_-2_3_ret");
      SFichier fic_eps_ret (fichier_eps_ret,ios::app);
      fic.precision(8);
      double epsilon = (Ec_tot_old - Ec_tot) / (temps - temps_old);
      fic << temps_old << " " << epsilon << finl;
      double temps_old_ret = temps_old/temps_retournement;
      if ( epsilon > 1.e-12 )
        {
          fic_eps << temps_old << " " << pow(epsilon,-2./3.) << finl;
          fic_eps_ret << temps_old_ret << " " << pow(epsilon,-2./3.) << finl;
        }
      else
        {
          fic_eps << temps_old << " " << 0 << finl;
          fic_eps_ret << temps_old_ret << " " << 0 << finl;
        }

    }
  Ec_tot_old = Ec_tot;
  temps_old = temps;

  calcul_Ec_spatial(vitesse_bar, "bar");
  calcul_Ec_spatial(vitesse_prm, "prm");

  if(je_suis_maitre())
    Cerr << finl << "temps = " << temps << finl << finl;
  temps /= temps_retournement;
  if(je_suis_maitre())
    Cerr << finl << "temps (adimensionne) = " << temps << finl << finl;

  double Df;
  calcul_Df_spatial(Df);

  DoubleTab Sk(dimension);
  calcul_Sk(Sk);

  calcul_nu_t();

  if ( calc_correlations==1 ) calcul_correlations(vitesse);


  // Les scalaires :
  // Pour les traitements effectues a chaque pas de temps

  for (int iscal=0; iscal<nb_champs_scalaires; iscal++)
    {
      const DoubleTab& scal = mon_equation->probleme().get_champ(noms_champs_scalaires[iscal]).valeurs();
      DoubleTab temp;
      temp.resize(scal.dimension(0),1);
      for (int j=0; j<temp.dimension(0); j++)
        temp(j,0) = scal(j);
      calcul_Ec_spatial(temp, noms_champs_scalaires[iscal]+"_tot");
    }



}




void Traitement_particulier_NS_THI_VEF::determine_tab_fft_VEF_3D(IntTab& tab_zone, DoubleTab& coord_zone, IntVect& nb_coord_zone, int& nb_spectres, int& nb_points)
// Remplit le tableau tab_calc_fft_3D contenant les centres de faces
// reparties sur tout le domaine, ordonnes pour le calcul de spectre par FFT.
// On ne peut pas utiliser tous les points pour un seul spectre (ils ne
// forment pas un maillage cartesien, generalement), mais on peut calculer
// plusieurs spectres (moins bons a priori qu'un spectre unique) a partir de
// plusieurs sous-ensembles de points et en faire la moyenne.
// A faire tourner une fois au debut, et a chaque fois que le domaine change.

// Preconditions : le domaine s'etend de 0 a 2*Pi dans chaque direction

// Sorties : int nb_points; nombre de points dans une direction pour chaque sous-ensemble de points
//           int nb_spectres; nombre de sous-ensemble de points

// Effet de bord : remplit le tableau tab_calc_fft_3D

{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const int nb_faces = zone_VEF.nb_faces();
  const DoubleTab& coord_centre = zone_VEF.xv();
  const double epsilon = 1.e-8;
  // const double Pi = 3.14159265358979;



  if(je_suis_maitre())
    {
      Cerr << "Preparation pour le calcul en FFT." << finl;
      Cerr << "  (table de correspondance : coordonnees de centre de la face / index de face). " ;
    }




  int nb_coord_zone_max = 0;

  // recuperer les differentes coordonnees trouvees dans la zone
  for (int dim=0; dim<dimension; dim++)
    {

      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          double coord = coord_centre(num_face,dim);
          int trouve_coord = 0; // booleen

          for (int jj=0; jj<nb_coord_zone(dim); jj++)
            {
              if (dabs(coord - coord_zone(jj,dim)) < epsilon)
                {
                  // cette valeur de "coord" a deja ete trouvee dans la zone
                  trouve_coord = 1;
                }
            }

          if ( !trouve_coord )
            {
              // ajouter dans le tableau coord_zone
              int jj=nb_coord_zone(dim)-1;

              while ( (jj > -1) && (coord_zone(jj,dim) > coord) )
                {
                  // decalage vers la droite des valeurs plus grandes
                  coord_zone(jj+1,dim) = coord_zone(jj,dim);
                  jj--;
                }

              // insertion a la position voulue
              coord_zone(jj+1,dim) = coord;
              nb_coord_zone(dim)++;

            } // endif ( !trouve_coord )

        } // fin boucle sur les faces

      if ( nb_coord_zone(dim) > nb_coord_zone_max )
        {
          nb_coord_zone_max = nb_coord_zone(dim) ;
        }

    } // fin boucle sur dim

  coord_zone.resize(nb_coord_zone_max,dimension);


  // remplir un tableau pour toutes les faces de la zone ordonnees selon leurs coordonnees, avec des vides a -1
  tab_zone.resize(nb_coord_zone(0),nb_coord_zone(1),nb_coord_zone(2));
  tab_zone = -1;
  IntVect index(3);
  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      index = -1;
      for (int dim=0; dim<dimension; dim++)
        {
          for (int ii=0; ii<nb_coord_zone(dim); ii++)
            {
              if ( dabs(coord_centre(num_face,dim) - coord_zone(ii,dim)) < epsilon )
                {
                  index(dim) = ii;
                  break ;
                }
            }
        }

      tab_zone(index(0),index(1),index(2)) = num_face;
    }

  if(je_suis_maitre())
    {
      Cerr << "OK." << finl;

      Cerr << "Remplissage de la table pour le calcul en FFT, spectres 3D." << finl;
      Cerr << "  (faces ordonnees en grilles cartesiennes, selon les coordonnees de leur centre). " ;
    }



  // rechercher dans chaque direction l'ecart minimum, et l'ecart minimum global
  DoubleVect ecart_min_dir(dimension); // ecart minimum dans chaque direction
  ecart_min_dir = 100;
  double ecart_min = 100; // ecart minimum global

  for (int dim=0; dim<dimension; dim++)
    {
      for (int ii=0; ii<nb_coord_zone(dim)-1; ii++)
        {
          double ecart = coord_zone(ii+1,dim) - coord_zone(ii,dim);
          if ( ecart < ecart_min_dir(dim) )
            {
              ecart_min_dir(dim) = ecart ;
            }
        }
      if ( ecart_min_dir(dim) < ecart_min )
        {
          ecart_min = ecart_min_dir(dim) ;
        }
    }

  IntVect ecart_min_dir_int(3);

  for (int dim=0; dim<dimension; dim++)
    {
      ecart_min_dir_int(dim) = int( ecart_min_dir(dim) / ecart_min + 0.5 );
    }
  // on a besoin du plus petit commun multiple des 3 ints pour avoir une chance de construire une grille cartesienne
  int ecart_base_int = ppcm(ecart_min_dir_int(0),
                            ppcm(ecart_min_dir_int(1),
                                 ecart_min_dir_int(2)));


  double pas_base = ecart_base_int * ecart_min;
  double pas = pas_base;



  IntVect hexa(nb_faces); // correspondance indice des faces dans zone - dans hexa
  IntTab rempli(3);
  int fini = 0; // booleen
  nb_spectres = 0; // nombre de spectres qui vont etre calcules et donc nombre de sous-ensembles de points qui sont ici releves
  int num_spectre = 0;

  while ( !fini && ( pas < L_BOITE/2. ) )
    {
      // prendre un hexa-reference, de cote "pas", a l'origine
      int nb_dans_hexa = 0;
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          int dans_hexa = 0;
          for (int dim=0; dim<dimension; dim++)
            {
              if ( coord_centre(num_face,dim) + epsilon < pas )
                {
                  dans_hexa++ ;
                }
            }
          if ( dans_hexa == dimension )
            {
              hexa(nb_dans_hexa) = num_face;
              nb_dans_hexa++;
            }
        }

      nb_points = int(L_BOITE/pas + 0.5);

      tab_calc_fft_3D.resize(nb_dans_hexa, nb_points+1,nb_points+1,nb_points+1); // tableau avec les centres des faces ordonnees pour le calcul de spectre par FFT
      tab_calc_fft_3D = -1;

      IntTab coord_zone_ok(nb_coord_zone_max, dimension);

      // parcourir les faces contenues dans l'hexa-reference et tenter de construire des grilles cartesiennes
      for (int num_face=0; num_face<nb_dans_hexa; num_face++)
        {
          // chercher les coordonnees qui vont bien avec la face de l'hexa-reference pour construire une grille cartesienne avec le pas, en evitant les dernieres coordonnees, qui sont sur des faces periodiques

          coord_zone_ok = 0;

          for (int dim=0; dim<dimension; dim++)
            {
              for (int ii=0; ii<nb_coord_zone(dim)-1; ii++)
                {
                  double ratio_double =
                    ( coord_zone(ii,dim)-coord_centre(hexa(num_face),dim) ) / pas;
                  int ratio_int = int(ratio_double + 0.5);
                  if ( dabs(ratio_double - ratio_int) < epsilon )
                    {
                      coord_zone_ok(ii,dim) = 1;
                    }
                }

            } // endfor coordonnees qui vont bien

          int valide = 1; // booleen

          // prendre les faces qui ont les 3 coordonnees qui vont bien pour construire la grille, en evitant les dernieres coordonnees, qui sont sur des faces periodiques (ainsi chaque grille de tab_calc_fft_3D est de taille nb_points*nb_points*nb_points exactement)
          // (et non pas nb_points*nb_points*nb_points pour une grille et nb_points+1*nb_points*nb_points pour une autre grille, et pour d'autres grilles nb_points*nb_points+1*nb_points ou nb_points+1*nb_points+1*nb_points ou ... )

          int ix = 0;
          for (int ii=0; ii<nb_coord_zone(0)-1; ii++)
            {
              rempli(0) = 0;
              int iy = 0;
              for (int jj=0; jj<nb_coord_zone(1)-1; jj++)
                {
                  rempli(1) = 0;
                  int iz = 0;
                  for (int kk=0; kk<nb_coord_zone(2)-1; kk++)
                    {
                      rempli(2) = 0;

                      if ( coord_zone_ok(ii,0) == 1
                           && coord_zone_ok(jj,1) == 1
                           && coord_zone_ok(kk,2) == 1 )
                        {
                          // a cette position devrait se trouver une face
                          if ( tab_zone(ii,jj,kk) != -1 )
                            // la face "tab_zone(ii,jj,kk)" existe et est sur la grille qui s'appuie sur la face "hexa(num_face)" contenue dans l'hexa-reference.
                            {
                              tab_calc_fft_3D(num_spectre,ix,iy,iz) = tab_zone(ii,jj,kk);
                              rempli = 1;
                            }
                          else
                            // on ne trouve pas de face a la position requise : la grille en construction n'est pas valide
                            {
                              valide = 0;
                              break;
                            }

                        } // endif bonnes coordonnees

                      if ( !valide ) break ;
                      if ( rempli(2) ) iz++;

                    } // endfor kk

                  if ( !valide ) break ;
                  if ( rempli(1) ) iy++;

                } // endfor jj

              if ( !valide ) break ;
              if ( rempli(0) ) ix++;

            } // endfor ii

          if ( valide )
            {
              // on a reussi a construire une grille complete
              assert( ix == nb_points );

              // On passe de nb_points a nb_points+1 dans chaque direction, en creant par periodicite les points de coordonees superieure a 2*Pi (il est necessaire, pour le bon fonctionnement des routines de FFT utilisees, d'avoir le premier et le dernier point a la meme valeur).

              for (int jj=0; jj<nb_points; jj++)
                for (int kk=0; kk<nb_points; kk++)
                  {
                    tab_calc_fft_3D(num_spectre, nb_points, jj, kk) =
                      tab_calc_fft_3D(num_spectre, 0        , jj, kk);
                  }

              for (int ii=0; ii<nb_points+1; ii++)
                for (int kk=0; kk<nb_points; kk++)
                  {
                    tab_calc_fft_3D(num_spectre, ii, nb_points, kk) =
                      tab_calc_fft_3D(num_spectre, ii, 0        , kk);
                  }

              for (int ii=0; ii<nb_points+1; ii++)
                for (int jj=0; jj<nb_points+1; jj++)
                  {
                    tab_calc_fft_3D(num_spectre, ii, jj, nb_points) =
                      tab_calc_fft_3D(num_spectre, ii, jj, 0        );
                  }

              num_spectre++;
            }

        } // endfor essai de toutes les faces de l'hexa-reference

      if ( num_spectre )
        {
          // on a au moins une grille valable
          nb_spectres = num_spectre;
          fini = 1;
        }
      else
        {
          pas += pas_base;  // et on essaie avec un autre pas plus grand et un nouvel hexa-reference
        }

    } // endwhile

  if(je_suis_maitre())
    if ( !nb_spectres )
      {
        Cerr << finl << "Erreur dans Traitement_particulier_NS_THI_VEF::determine_tab_fft_VEF_3D" << finl;
        Cerr << "La fonction n'est pas capable de trouver une ou des grilles cartesiennes 3D" << finl;
        Cerr << "pour le calcul de spectres en FFT." << finl;
        assert(0);
        Process::exit();
      }






  if(je_suis_maitre())
    {

      Cerr << "OK." << finl;

      Cerr << nb_spectres << " grilles de " << nb_points << " points par cote seront utilisees pour calculer les spectres 3D." << finl << finl;

    }



}


void Traitement_particulier_NS_THI_VEF::determine_tab_fft_VEF_1D(const IntTab& tab_zone, const DoubleTab& coord_zone, const IntVect& nb_coord_zone, IntVect& nb_spectres, IntVect& nb_points)

// limitation : en cherchant les lignes les plus longues pour porter les spectres,
//              la methode ne verifie pas si les points sont regulierement espaces
//              (ce qui est necessaire pour une FFT mais pas pour une DFT).


{

  if(je_suis_maitre())
    {
      Cerr << "Remplissage de la table pour le calcul en FFT, spectres 1D." << finl;
      Cerr << "  (faces ordonnees en lignes droites, selon les coordonnees de leur centre). " ;
    }


  IntVect nb_coord_zone_tri(nb_coord_zone) ;
  // tri
  for (int ii=dimension ; ii>0; ii--)
    {
      for (int jj=1; jj<ii; jj++)
        {
          if ( nb_coord_zone_tri(jj-1) > nb_coord_zone_tri(jj) )
            {
              int temp = nb_coord_zone_tri(jj-1);
              nb_coord_zone_tri(jj-1) = nb_coord_zone_tri(jj);
              nb_coord_zone_tri(jj) = temp;
            }
        }
    }


  int nb_coord_zone_max = nb_coord_zone_tri(2) ;
  int nb_spectres_max = nb_coord_zone_tri(2) * nb_coord_zone_tri(1) ;


  tab_calc_fft_1D.resize(dimension, nb_spectres_max, nb_coord_zone_max) ;
  tab_calc_fft_1D = -1 ;

  tab_coord_1D.resize(dimension, nb_spectres_max, nb_coord_zone_max) ;

  IntVect num_spectre(dimension);
  nb_spectres.resize(dimension);
  nb_points.resize(dimension);
  nb_spectres = 0 ;
  num_spectre = 0 ;
  nb_points = 0 ;

  // pour chaque point, on cherche les points sur la meme ligne selon x
  for (int jj=0; jj<nb_coord_zone(1)-1; jj++)
    for (int kk=0; kk<nb_coord_zone(2)-1; kk++)
      {
        int ix = 0 ;
        for (int ii=0; ii<nb_coord_zone(0)-1; ii++)
          {
            int num_face = tab_zone(ii, jj, kk);
            if ( num_face != -1 )
              {
                tab_calc_fft_1D(0, num_spectre(0), ix) = num_face;
                tab_coord_1D   (0, num_spectre(0), ix) = coord_zone(ii, 0);
                ix++;
              }
          }
        if ( ix == nb_points(0) )
          {
            num_spectre(0)++;
          }
        else if ( ix > nb_points(0) )
          {
            // on ne garde que les lignes les plus longues
            nb_points(0) = ix ;
            for (int index=0; index<ix; index++)
              {
                tab_calc_fft_1D(0, 0             , index) =
                  tab_calc_fft_1D(0, num_spectre(0), index) ;
                tab_coord_1D   (0, 0             , index) =
                  tab_coord_1D   (0, num_spectre(0), index) ;
              }
            num_spectre(0) = 1 ;
          }
        nb_spectres(0) = num_spectre(0);
      }

  // pour chaque point, on cherche les points sur la meme ligne selon y
  for (int ii=0; ii<nb_coord_zone(0)-1; ii++)
    for (int kk=0; kk<nb_coord_zone(2)-1; kk++)
      {
        int iy = 0 ;
        for (int jj=0; jj<nb_coord_zone(1)-1; jj++)
          {
            int num_face = tab_zone(ii, jj, kk);
            if ( num_face != -1 )
              {
                tab_calc_fft_1D(1, num_spectre(1), iy) = num_face;
                tab_coord_1D   (1, num_spectre(1), iy) = coord_zone(jj, 1);
                iy++;
              }
          }
        if ( iy == nb_points(1) )
          {
            num_spectre(1)++;
          }
        else if ( iy > nb_points(1) )
          {
            // on ne garde que les lignes les plus longues
            nb_points(1) = iy ;
            for (int index=0; index<iy; index++)
              {
                tab_calc_fft_1D(1, 0             , index) =
                  tab_calc_fft_1D(1, num_spectre(1), index) ;
                tab_coord_1D   (1, 0             , index) =
                  tab_coord_1D   (1, num_spectre(1), index) ;
              }
            num_spectre(1) = 1 ;
          }
        nb_spectres(1) = num_spectre(1);
      }

  // pour chaque point, on cherche les points sur la meme ligne selon z
  for (int jj=0; jj<nb_coord_zone(1)-1; jj++)
    for (int ii=0; ii<nb_coord_zone(0)-1; ii++)
      {
        int iz = 0 ;
        for (int kk=0; kk<nb_coord_zone(2)-1; kk++)
          {
            int num_face = tab_zone(ii, jj, kk);
            if ( num_face != -1 )
              {
                tab_calc_fft_1D(2, num_spectre(2), iz) = num_face;
                tab_coord_1D   (2, num_spectre(2), iz) = coord_zone(kk, 2);
                iz++;
              }
          }
        if ( iz == nb_points(2) )
          {
            num_spectre(2)++;
          }
        else if ( iz > nb_points(2) )
          {
            // on ne garde que les lignes les plus longues
            nb_points(2) = iz ;
            for (int index=0; index<iz; index++)
              {
                tab_calc_fft_1D(2, 0             , index) =
                  tab_calc_fft_1D(2, num_spectre(2), index) ;
                tab_coord_1D   (2, 0             , index) =
                  tab_coord_1D   (2, num_spectre(2), index) ;
              }
            num_spectre(2) = 1 ;
          }
        nb_spectres(2) = num_spectre(2);
      }

  if(je_suis_maitre())
    {
      Cerr << "OK." << finl;

      Cerr << "Seront utilisees pour calculer les spectres 1D :" << finl;
      Cerr << "selon x, " << nb_spectres(0) << " lignes de " << nb_points(0) << " points," << finl;
      Cerr << "selon y, " << nb_spectres(1) << " lignes de " << nb_points(1) << " points," << finl;
      Cerr << "selon z, " << nb_spectres(2) << " lignes de " << nb_points(2) << " points." << finl << finl;


    }

}





void Traitement_particulier_NS_THI_VEF::ch_pour_fft_VEF_3D(const DoubleTab& tab_global, DoubleTab& tab_u, DoubleTab& tab_v, DoubleTab& tab_w, int num_spectre) const
// Remplit les tableaux (tab_u, tab_v et tab_w) avec les valeurs a envoyer
// dans les routines FFT, en utilisant les valeurs du tableau tab_global
// et en tenant compte du tableau de relation tab_calc_fft_3D.
// A faire tourner a chaque calcul de spectre 3D.
{
  //  Cerr  << "Remplissage des tableaux de vitesse (calcul en FFT). " ;

  // Remplissage des tableaux
  for (int ii=0; ii<nb_points_3D+1; ii++)
    for (int jj=0; jj<nb_points_3D+1; jj++)
      for (int kk=0; kk<nb_points_3D+1; kk++)
        {
          int num_face = tab_calc_fft_3D(num_spectre, ii, jj, kk);
          tab_u(ii, jj, kk) = tab_global(num_face, 0);
          tab_v(ii, jj, kk) = tab_global(num_face, 1);
          tab_w(ii, jj, kk) = tab_global(num_face, 2);
        }

}






void Traitement_particulier_NS_THI_VEF::ch_pour_fft_VEF_1D(const DoubleTab& tab_global, DoubleVect& vect_u, DoubleVect& vect_v, DoubleVect& vect_w, int dir, int num_spectre ) const
// Remplit les vecteurs (vect_u, vect_v et vect_w) avec les valeurs a envoyer
// dans les routines FFT, en utilisant les valeurs du tableau tab_zone
// et en tenant compte du tableau de relation tab_calc_fft_1D.
// A faire tourner a chaque calcul de spectre 1D.
{
  // Remplissage des vecteurs
  for (int ii=0; ii<nb_points_1D(dir); ii++)
    {
      int num_face = tab_calc_fft_1D(dir, num_spectre, ii);
      vect_u(ii) = tab_global( num_face, 0 );
      vect_v(ii) = tab_global( num_face, 1 );
      vect_w(ii) = tab_global( num_face, 2 );
    }

}



void Traitement_particulier_NS_THI_VEF::calcul_spectre_3D(const DoubleTab& vitesse, Nom ext, double& Ec_spectre)
{
  double temps = mon_equation->inconnue().temps();
  temps /= temps_retournement;

  //    Cerr << "Spectre d'energie en 3D." << finl; ;

  DoubleVect Ec(nb_points_3D); // energie par bande spectrale
  DoubleVect Ec_min(nb_points_3D);
  DoubleVect Ec_max(nb_points_3D);
  DoubleVect Ec_moy(nb_points_3D);
  Ec_min = 1.e+20;
  Ec_max = 0;
  Nom fichier = Nom("spectre_3D_") + ext + Nom("_") + Nom(temps);
  Nom fichier_k = Nom("spectre_k_3D_") + ext + Nom("_") + Nom(temps);

  for (int num_spectre=0; num_spectre<nb_spectres_3D; num_spectre++)
    {
      //      Cerr << "spectre #" << num_spectre+1 << "/" << nb_spectres_3D << finl;

      DoubleTab vit_u(nb_points_3D+1,nb_points_3D+1,nb_points_3D+1),
                vit_v(nb_points_3D+1,nb_points_3D+1,nb_points_3D+1),
                vit_w(nb_points_3D+1,nb_points_3D+1,nb_points_3D+1);
      // tableaux de vitesse a envoyer dans les routines FFT 3D

      ch_pour_fft_VEF_3D(vitesse, vit_u, vit_v, vit_w, num_spectre);

      calc_sp_operateur(vit_u, vit_v, vit_w, nb_points_3D, temps, 0,0, Ec);

      for (int ii=0; ii<nb_points_3D; ii++)
        {
          if ( Ec(ii) < Ec_min(ii) )
            {
              Ec_min(ii) = Ec(ii);
            }
          if ( Ec(ii) > Ec_max(ii) )
            {
              Ec_max(ii) = Ec(ii);
            }
          Ec_moy(ii) += Ec(ii)/nb_spectres_3D;
        }
    }

  int kc = nb_points_3D/2 ; // nombre d'onde de coupure

  // ecriture des spectres moy, max et min dans un fichier
  SFichier fic_spectre (fichier);
  SFichier fic_spectre_k (fichier_k);
  for (int ii=1; ii<kc; ii++)
    {
      fic_spectre << ii << " " << Ec_moy(ii-1) << " " << Ec_max(ii-1) << " " << Ec_min(ii-1) << finl;
      fic_spectre_k << ii << " " << Ec_moy(ii-1) * pow(ii,5./3.) << " " << Ec_max(ii-1) * pow(ii,5./3.) << " " << Ec_min(ii-1) * pow(ii,5./3.) << finl;
    }

  // mise en evidence de la frequence de coupure
  fic_spectre << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
  fic_spectre << kc << " " << 0 << " " << 0 << " " << 0 << finl;
  fic_spectre_k << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
  fic_spectre_k << kc << " " << 0 << " " << 0 << " " << 0 << finl;

  for (int ii=kc; ii<nb_points_3D+1; ii++)
    {
      fic_spectre << ii << " " << Ec_moy(ii-1) << " " << Ec_max(ii-1) << " " << Ec_min(ii-1) << finl;
      fic_spectre_k << ii << " " << Ec_moy(ii-1) * pow(ii,5./3.) << " " << Ec_max(ii-1) * pow(ii,5./3.) << " " << Ec_min(ii-1) * pow(ii,5./3.) << finl;
    }

  // calcul de l'energie cinetique par integration sur le spectre jusqu a kc
  calc_Ec(Ec_moy, kc, Ec_spectre);

  // calcul de l'enstrophie par integration sur le spectre jusqu a kc
  DoubleVect Df(nb_points_3D); // enstrophie par bande spectrale
  for (int ii=0; ii<nb_points_3D; ii++)
    {
      Df(ii) = (ii+1)*(ii+1)*Ec_moy(ii);
    }
  double Df_spectre;
  calc_Ec(Df, kc, Df_spectre);

  // ecriture de l'energie (en spectral) dans un fichier
  Nom fichier_Ec_spectral = Nom("Sorties_Ec_") + ext + Nom("_spectral_3D") ;
  SFichier fic_Ec_spectral (fichier_Ec_spectral, ios::app);
  fic_Ec_spectral << temps << " " << Ec_spectre << finl;

  // ecriture de l'enstrophie (en spectral) dans un fichier
  Nom fichier_Df_spectral = Nom("Sorties_Df_") + ext + Nom("_spectral_3D") ;
  SFichier fic_Df_spectral (fichier_Df_spectral, ios::app);
  fic_Df_spectral << temps << " " << Df_spectre << finl;



}







void Traitement_particulier_NS_THI_VEF::calcul_spectre_1D(const DoubleTab& vitesse, Nom ext, double& Ec_spectre)
{

  double temps = mon_equation->inconnue().temps();
  temps /= temps_retournement;
  //  Nom fichier_data = Nom("data_spectre_1D_") + ext + Nom("_") + Nom(temps) + Nom("_");
  Nom fichier_spectre = Nom("spectre_1D_") + ext + Nom("_") + Nom(temps) + Nom("_");
  Nom fichier_k_spectre = Nom("spectre_k_1D_") + ext + Nom("_") + Nom(temps) + Nom("_");

  Ec_spectre = 0;
  double Df_spectre = 0;

  int nb_points_1D_max = local_max_vect(nb_points_1D);

  DoubleVect Ec_moy(nb_points_1D_max),
             Ec_moy_u(nb_points_1D_max),
             Ec_moy_v(nb_points_1D_max),
             Ec_moy_w(nb_points_1D_max);

  for (int dir=0; dir<dimension; dir++)
    {
      Nom direction;
      switch(dir)
        {
        case 0:
          {
            direction = Nom("x");
            break;
          }
        case 1:
          {
            direction = Nom("y");
            break;
          }
        case 2:
          {
            direction = Nom("z");
            break;
          }
        }
      //          Nom fichier_data_direction = fichier_data + direction + Nom("_");
      Nom fichier_spectre_direction = fichier_spectre + direction + Nom("_");
      Nom fichier_k_spectre_direction = fichier_k_spectre + direction + Nom("_");

      //      SFichier fic_data_direction (fichier_data_direction);
      //      fic_data_direction << "# coordonnee selon la direction" << dim << " ; vit_u ; vit_v ; vit_w" << finl;


      DoubleTab Ec_comp(nb_points_1D(dir), dimension); // energie par composante (u,v,w) par bande spectrale
      DoubleTab Ec_comp_min(nb_points_1D(dir), dimension);
      DoubleTab Ec_comp_max(nb_points_1D(dir), dimension);
      DoubleTab Ec_comp_moy(nb_points_1D(dir), dimension);

      Ec_comp_min = 1.e+20;
      Ec_comp_max = 0;

      int n = nb_points_1D(dir);
      DoubleVect trig(2*n);
      IntVect ifax(19);

      // preparer JMFFT
      cftfax(n,ifax.addr(),trig.addr());

      int lot=1;
      DoubleVect work(2*n*lot);
      int inc=1;
      int jump=inc*(n+2);
      int isign=-1; // -1 pour FFT, +1 pour FFT inverse

      int kc = nb_points_1D(dir)/2 ; // nombre d'onde de coupure

      for (int num_spectre=0; num_spectre<nb_spectres_1D(dir); num_spectre++)
        {
          //          Cerr << "spectres #" << num_spectre+1 << "/" << nb_spectres_1D(dim) << " selon direction " << dim << finl;

          DoubleVect vect_u(nb_points_1D(dir)+2),
                     vect_v(nb_points_1D(dir)+2),
                     vect_w(nb_points_1D(dir)+2);
          // vecteurs de vitesse a envoyer dans les routines FFT 1D

          ch_pour_fft_VEF_1D(vitesse, vect_u, vect_v, vect_w, dir, num_spectre);



          // calcul des spectres 1D
          // calcul du spectre de la composante u selon la direction (dir)
          rfftmlt(vect_u.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,n,lot,isign);
          // maintenant vect_u contient le resultat de la FFT

          for (int ii=0; ii<kc; ii++)
            {
              double Ec_temp = vect_u(2*ii)*vect_u(2*ii)+vect_u(2*ii+1)*vect_u(2*ii+1);

              Ec_temp *= 4;

              if ( Ec_temp < Ec_comp_min(ii,0) )
                {
                  Ec_comp_min(ii,0) = Ec_temp;
                }
              if ( Ec_temp > Ec_comp_max(ii,0) )
                {
                  Ec_comp_max(ii,0) = Ec_temp;
                }
              Ec_comp_moy(ii,0) += Ec_temp/nb_spectres_1D(dir);
            }

          // calcul du spectre de la composante v selon la direction (dir)
          rfftmlt(vect_v.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,n,lot,isign);
          // maintenant vect_v contient le resultat de la FFT
          for (int ii=0; ii<kc; ii++)
            {
              double Ec_temp = vect_v(2*ii)*vect_v(2*ii)+vect_v(2*ii+1)*vect_v(2*ii+1);

              Ec_temp *= 4;

              if ( Ec_temp < Ec_comp_min(ii,1) )
                {
                  Ec_comp_min(ii,1) = Ec_temp;
                }
              if ( Ec_temp > Ec_comp_max(ii,1) )
                {
                  Ec_comp_max(ii,1) = Ec_temp;
                }
              Ec_comp_moy(ii,1) += Ec_temp/nb_spectres_1D(dir);
            }

          // calcul du spectre de la composante w selon la direction (dir)
          rfftmlt(vect_w.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,n,lot,isign);
          // maintenant vect_w contient le resultat de la FFT
          for (int ii=0; ii<kc; ii++)
            {
              double Ec_temp = vect_w(2*ii)*vect_w(2*ii)+vect_w(2*ii+1)*vect_w(2*ii+1);

              Ec_temp *= 4;

              if ( Ec_temp < Ec_comp_min(ii,2) )
                {
                  Ec_comp_min(ii,2) = Ec_temp;
                }
              if ( Ec_temp > Ec_comp_max(ii,2) )
                {
                  Ec_comp_max(ii,2) = Ec_temp;
                }
              Ec_comp_moy(ii,2) += Ec_temp/nb_spectres_1D(dir);
            }

        }



      // ecriture des spectres moy, max et min dans 1 direction dans un fichier
      SFichier fic_spectre_u (fichier_spectre_direction + Nom("u") );
      SFichier fic_spectre_k_u (fichier_k_spectre_direction + Nom("u") );
      SFichier fic_spectre_v (fichier_spectre_direction + Nom("v") );
      SFichier fic_spectre_k_v (fichier_k_spectre_direction + Nom("v") );
      SFichier fic_spectre_w (fichier_spectre_direction + Nom("w") );
      SFichier fic_spectre_k_w (fichier_k_spectre_direction + Nom("w") );
      SFichier fic_spectre_e (fichier_spectre_direction + Nom("e") );
      SFichier fic_spectre_k_e (fichier_k_spectre_direction + Nom("e") );

      DoubleVect Ec_comp_moy_e(nb_points_1D(dir));

      for (int ii=1; ii<kc; ii++)
        {
          Ec_comp_moy_e(ii) = Ec_comp_moy(ii,0) + Ec_comp_moy(ii,1) + Ec_comp_moy(ii,2);

          fic_spectre_u << ii << " " << Ec_comp_moy(ii,0) << " " << Ec_comp_max(ii,0) << " " << Ec_comp_min(ii,0) << finl;
          fic_spectre_k_u << ii << " " << Ec_comp_moy(ii,0) * pow(ii,5./3.) << " " << Ec_comp_max(ii,0) * pow(ii,5./3.) << " " << Ec_comp_min(ii,0) * pow(ii,5./3.) << finl;
          fic_spectre_v << ii << " " << Ec_comp_moy(ii,1) << " " << Ec_comp_max(ii,1) << " " << Ec_comp_min(ii,1) << finl;
          fic_spectre_k_v << ii << " " << Ec_comp_moy(ii,1) * pow(ii,5./3.) << " " << Ec_comp_max(ii,1) * pow(ii,5./3.) << " " << Ec_comp_min(ii,1) * pow(ii,5./3.) << finl;
          fic_spectre_w << ii << " " << Ec_comp_moy(ii,2) << " " << Ec_comp_max(ii,2) << " " << Ec_comp_min(ii,2) << finl;
          fic_spectre_k_w << ii << " " << Ec_comp_moy(ii,2) * pow(ii,5./3.) << " " << Ec_comp_max(ii,2) * pow(ii,5./3.) << " " << Ec_comp_min(ii,2) * pow(ii,5./3.) << finl;
          fic_spectre_e << ii << " " << Ec_comp_moy_e(ii) << finl;
          fic_spectre_k_e << ii << " " << Ec_comp_moy_e(ii) * pow(ii,5./3.) << finl;
        }

      // mise en evidence de la frequence de coupure
      fic_spectre_u << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
      fic_spectre_k_u << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
      fic_spectre_v << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
      fic_spectre_k_v << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
      fic_spectre_w << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;
      fic_spectre_k_w << kc-1 << " " << 0 << " " << 0 << " " << 0 << finl;


      // calcul de l'energie et l'enstrophie par integration sur le spectre jusqu a kc
      DoubleVect Ec_comp_moy_u(nb_points_1D(dir));
      DoubleVect Ec_comp_moy_v(nb_points_1D(dir));
      DoubleVect Ec_comp_moy_w(nb_points_1D(dir));
      DoubleVect Df_comp_moy_u(nb_points_1D(dir));
      DoubleVect Df_comp_moy_v(nb_points_1D(dir));
      DoubleVect Df_comp_moy_w(nb_points_1D(dir));
      for (int ii=0; ii<kc; ii++)
        {
          Ec_comp_moy_u(ii) = Ec_comp_moy(ii+1,0);
          Ec_comp_moy_v(ii) = Ec_comp_moy(ii+1,1);
          Ec_comp_moy_w(ii) = Ec_comp_moy(ii+1,2);
          Df_comp_moy_u(ii) = (ii+1)*(ii+1)*Ec_comp_moy(ii+1,0);
          Df_comp_moy_v(ii) = (ii+1)*(ii+1)*Ec_comp_moy(ii+1,1);
          Df_comp_moy_w(ii) = (ii+1)*(ii+1)*Ec_comp_moy(ii+1,2);
        }
      double Ec_spectre_u, Ec_spectre_v, Ec_spectre_w, Df_spectre_u, Df_spectre_v, Df_spectre_w;
      calc_Ec(Ec_comp_moy_u, kc, Ec_spectre_u);
      calc_Ec(Ec_comp_moy_v, kc, Ec_spectre_v);
      calc_Ec(Ec_comp_moy_w, kc, Ec_spectre_w);
      calc_Ec(Df_comp_moy_u, kc, Df_spectre_u);
      calc_Ec(Df_comp_moy_v, kc, Df_spectre_v);
      calc_Ec(Df_comp_moy_w, kc, Df_spectre_w);
      double Ec_spectre_e = Ec_spectre_u + Ec_spectre_v + Ec_spectre_w;
      double Df_spectre_e = Df_spectre_u + Df_spectre_v + Df_spectre_w;

      // ecriture de l'energie (en spectral) dans un fichier
      Nom fichier_Ec_spectral_direction = Nom("Sorties_Ec_") + ext + Nom("_spectral_1D_") + direction ;
      SFichier fic_Ec_spectral_u (fichier_Ec_spectral_direction + Nom("_u"), ios::app );
      SFichier fic_Ec_spectral_v (fichier_Ec_spectral_direction + Nom("_v"), ios::app );
      SFichier fic_Ec_spectral_w (fichier_Ec_spectral_direction + Nom("_w"), ios::app );
      SFichier fic_Ec_spectral_e (fichier_Ec_spectral_direction + Nom("_e"), ios::app );
      fic_Ec_spectral_u << temps << " " << Ec_spectre_u << finl;
      fic_Ec_spectral_v << temps << " " << Ec_spectre_v << finl;
      fic_Ec_spectral_w << temps << " " << Ec_spectre_w << finl;
      fic_Ec_spectral_e << temps << " " << Ec_spectre_e << finl;

      // ecriture de l'enstrophie (en spectral) dans un fichier
      Nom fichier_Df_spectral_direction = Nom("Sorties_Df_") + ext + Nom("_spectral_1D_") + direction ;
      SFichier fic_Df_spectral_u (fichier_Df_spectral_direction + Nom("_u"), ios::app );
      SFichier fic_Df_spectral_v (fichier_Df_spectral_direction + Nom("_v"), ios::app );
      SFichier fic_Df_spectral_w (fichier_Df_spectral_direction + Nom("_w"), ios::app );
      SFichier fic_Df_spectral_e (fichier_Df_spectral_direction + Nom("_e"), ios::app );
      fic_Df_spectral_u << temps << " " << Df_spectre_u << finl;
      fic_Df_spectral_v << temps << " " << Df_spectre_v << finl;
      fic_Df_spectral_w << temps << " " << Df_spectre_w << finl;
      fic_Df_spectral_e << temps << " " << Df_spectre_e << finl;


      Ec_spectre += Ec_spectre_e ;
      Df_spectre += Df_spectre_e ;

    }


  //    Cerr << "Spectre d'energie en 1D. Fin des calculs" << finl; ;

  Nom fichier_Ec_spectral = Nom("Sorties_Ec_") + ext + Nom("_spectral_1D") ;
  SFichier fic_Ec_spectral (fichier_Ec_spectral, ios::app );
  fic_Ec_spectral << temps << " " << Ec_spectre << finl;

  Nom fichier_Df_spectral = Nom("Sorties_Df_") + ext + Nom("_spectral_1D") ;
  SFichier fic_Df_spectral (fichier_Df_spectral, ios::app );
  fic_Df_spectral << temps << " " << Df_spectre << finl;

  //     Cerr << "Ec_" << ext << "_spectre_1D=" << Ec_spectre << " Df_" << ext << "_spectre_1D=" << Df_spectre << finl;

}





void Traitement_particulier_NS_THI_VEF::calcul_correlations(const DoubleTab& vitesse)

{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const int nb_faces = zone_VEF.nb_faces();
  const Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());

  DoubleTab vitesse_bar(vitesse);
  champ_vitesse.filtrer_L2(vitesse_bar);

  DoubleTab vitesse_prm(nb_faces, dimension);
  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<dimension; dim++)
        {
          vitesse_prm(num_face, dim) = vitesse(num_face, dim) - vitesse_bar(num_face, dim);
        }
    }

  if (nproc()==1)
    {
      isotropie(vitesse, "tot");
      isotropie(vitesse_bar, "bar");
      isotropie(vitesse_prm, "prm");
    }
}




void Traitement_particulier_NS_THI_VEF::isotropie(const DoubleTab& vitesse, Nom ext)

{
  //    Cerr << "Isotropie." << finl; ;

  if (nproc()==1)
    {
      Nom fichier = "isotropie_";
      fichier += ext;

      DoubleTab isotrop(6), isotrop_moy(6), isotrop_max(6), isotrop_min(6);
      isotrop_moy = 0 ;
      isotrop_max = 0 ;
      isotrop_min = 1e+20 ;

      for (int num_spectre=0; num_spectre<nb_spectres_3D; num_spectre++)
        {
          //      Cerr << "spectre #" << num_spectre+1 << "/" << nb_spectres_3D << finl;

          // Etude de l'isotropie on calcule la moyenne de uu,vv,ww,uv,uw et vw
          isotrop = 0 ;

          for(int ii=0; ii<nb_points_3D; ii++)
            for(int jj=0; jj<nb_points_3D; jj++)
              for(int kk=0; kk<nb_points_3D; kk++)
                {
                  int num_face = tab_calc_fft_3D(num_spectre,ii,jj,kk);
                  double u = vitesse(num_face,0) - moyenne_vitesse(0);
                  double v = vitesse(num_face,1) - moyenne_vitesse(1);
                  double w = vitesse(num_face,2) - moyenne_vitesse(2);

                  isotrop(0) += u * u ;
                  isotrop(1) += v * v ;
                  isotrop(2) += w * w ;
                  isotrop(3) += u * v ;
                  isotrop(4) += u * w ;
                  isotrop(5) += v * w ;
                }
          int nb_points = nb_points_3D * nb_points_3D * nb_points_3D ;
          isotrop /= nb_points;

          for (int ii=0; ii<6; ii++)
            {
              if ( isotrop(ii) < isotrop_min(ii) )
                {
                  isotrop_min(ii) = isotrop(ii);
                }
              if ( isotrop(ii) > isotrop_max(ii) )
                {
                  isotrop_max(ii) = isotrop(ii);
                }
              isotrop_moy(ii) += isotrop(ii)/nb_spectres_3D;
            }
        }

      // ecriture des donnees moy, max et min dans un fichier

      if (je_suis_maitre())
        {
          double temps = mon_equation->inconnue().temps();
          temps /= temps_retournement;
          Nom fichier_uu = fichier + Nom("_uu");
          SFichier fic_uu (fichier_uu, ios::app);
          fic_uu << temps << " " << isotrop_moy(0) << " " << isotrop_max(0) << " " << isotrop_min(0) << finl ;

          Nom fichier_vv = fichier + Nom("_vv");
          SFichier fic_vv (fichier_vv, ios::app);
          fic_vv << temps << " " << isotrop_moy(1) << " " << isotrop_max(1) << " " << isotrop_min(1) << finl ;

          Nom fichier_ww = fichier + Nom("_ww");
          SFichier fic_ww (fichier_ww, ios::app);
          fic_ww << temps << " " << isotrop_moy(2) << " " << isotrop_max(2) << " " << isotrop_min(2) << finl ;

          Nom fichier_uv = fichier + Nom("_uv");
          SFichier fic_uv (fichier_uv, ios::app);
          fic_uv << temps << " " << isotrop_moy(3) << " " << isotrop_max(3) << " " << isotrop_min(3) << finl ;

          Nom fichier_uw = fichier + Nom("_uw");
          SFichier fic_uw (fichier_uw, ios::app);
          fic_uw << temps << " " << isotrop_moy(4) << " " << isotrop_max(4) << " " << isotrop_min(4) << finl ;

          Nom fichier_vw = fichier + Nom("_vw");
          SFichier fic_vw (fichier_vw, ios::app);
          fic_vw << temps << " " << isotrop_moy(5) << " " << isotrop_max(5) << " " << isotrop_min(5) << finl ;

        }
    }
  else
    {
      if(je_suis_maitre())
        Cerr << "Traitement_particulier_NS_THI_VEF::isotropie n'est pas parallelise" << finl;
    }

}



double Traitement_particulier_NS_THI_VEF::calcul_Ec_spatial(const DoubleTab& vitesse, Nom ext)
{
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur());
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const int nb_faces = zone_VEF.nb_faces();
  //  const int nb_faces_tot = zone_VF.nb_faces_tot();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();
  int nb_comp=vitesse.dimension(1);
  double Ec = 0;
  double Ec_min = 1e+20;
  double Ec_max = 0;

  DoubleVect ec_face_vect;
  zone_VEF.creer_tableau_faces(ec_face_vect);

  // calcul de la moyenne, recherche du minimum et du maximum
  for (int num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord=le_bord.nb_faces();
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + nb_faces_bord;

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          IntVect fait(nb_faces_bord);
          fait = 0;
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int face_associee=la_cl_perio.face_associee(num_face-num1);
              if (fait(num_face-num1) == 0)
                {
                  ec_face_vect(num_face)=0;
                  for (int dim=0; dim<nb_comp; dim++)
                    {
                      ec_face_vect(num_face) += vitesse(num_face,dim) * vitesse(num_face,dim);
                    }
                  //                   Ec += ec_face * volumes_entrelaces(num_face);

                  fait(num_face-num1) = 1;
                  fait(face_associee) = 1;


                }
            }
        } // fin du cas periodique

      else // pour les autres conditions aux limites
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              for (int dim=0; dim<nb_comp; dim++)
                {
                  ec_face_vect(num_face) += vitesse(num_face,dim) * vitesse(num_face,dim);
                }

            }
        }
    } // fin du traitement des conditions limites

  //  for (int num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
  for (int num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<nb_comp; dim++)
        {
          ec_face_vect(num_face) += vitesse(num_face,dim) * vitesse(num_face,dim);
        }


    } // fin de la boucle pour les faces internes, incluant les zones de joints

  Ec_min = ec_face_vect.mp_min_vect();
  Ec_max = ec_face_vect.mp_max_vect();

  for (int i=0; i<nb_faces; i++)
    ec_face_vect(i) *= volumes_entrelaces(i);
  Ec = mp_somme_vect(ec_face_vect);

  Ec /= 2*volume_total;

  Ec_min /= 2.;

  Ec_max /= 2.;


  if (je_suis_maitre())
    {
      Nom fichier = Nom("Sorties_Ec_") + ext + Nom("_spatial");
      SFichier fic (fichier,ios::app);
      fic.precision(8);
      double temps = mon_equation->inconnue().temps();
      temps /= temps_retournement;
      fic << temps << " " << Ec << " " << Ec_max << " " << Ec_min  << finl;

    }

  return Ec;

}


void Traitement_particulier_NS_THI_VEF::calcul_Df_spatial(double& Df)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes = zone_VEF.volumes();
  const int nb_elem = zone_VEF.nb_elem();
  const Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());

  DoubleTab vorticite(nb_elem,dimension);
  champ_vitesse.cal_rot_ordre1(vorticite);

  Df = 0;
  double Df_min = 1e+20;
  double Df_max = 0;

  // calcul de la moyenne, recherche du minimum et du maximum
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {

      double df_elem = 0;

      for (int dim=0; dim<dimension; dim++)
        {
          df_elem += vorticite(num_elem,dim) * vorticite(num_elem,dim);
        }

      Df += df_elem * volumes(num_elem);

      if ( df_elem < Df_min )
        {
          Df_min = df_elem ;
        }
      if ( df_elem > Df_max )
        {
          Df_max = df_elem ;
        }

    }

  Df = mp_sum(Df);
  Df /= 2*volume_total;

  Df_min = mp_min(Df_min);
  Df_min /= 2.;

  Df_max = mp_max(Df_max);
  Df_max /= 2.;

  // calcul de l'ecart-type
  double Df_ecart_type = 0;
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      double df_elem = 0;
      for (int dim=0; dim<dimension; dim++)
        {
          df_elem += vorticite(num_elem,dim) * vorticite(num_elem,dim);
        }
      Df_ecart_type += ( df_elem - Df ) * ( df_elem - Df );
    }
  Df_ecart_type /= nb_elem;
  Df_ecart_type = sqrt(Df_ecart_type);

  if (je_suis_maitre())
    {
      Nom fichier = Nom("Sorties_Df_tot_spatial");
      SFichier fic (fichier,ios::app);
      fic.precision(8);
      double temps = mon_equation->inconnue().temps();
      temps /= temps_retournement;
      fic << temps << " " << Df << " " << Df_max << " " << Df_min << " " << Df+Df_ecart_type <<  " " << Df-Df_ecart_type << finl;

      //       Cerr << "Df_tot=" << Df << finl;
      //       Cerr << "Df_tot_max=" << Df_max << " Df_tot_min=" << Df_min << finl;
      //       Cerr << "Df_tot_ecart_type=" << Df_ecart_type << finl << finl;
    }
}


void Traitement_particulier_NS_THI_VEF::calcul_Sk(DoubleTab& Sk)
{
  // calcul du facteur de dissymetrie (skewness)
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes = zone_VEF.volumes();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_elem = zone_VEF.nb_elem();
  const Champ_P1NC& champ_vitesse = ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());

  DoubleTab gradient(nb_elem_tot,dimension,dimension);
  gradient=0.;
  champ_vitesse.gradient(gradient);

  Sk = 0;

  DoubleTab grad_carre(dimension);
  grad_carre = 0;

  DoubleTab grad_cube(dimension);
  grad_cube = 0;

  for (int dim=0; dim<dimension; dim++)
    {
      // calcul du numerateur et du denominateur
      for(int num_elem=0; num_elem<nb_elem; num_elem++)
        {
          grad_carre(dim) += gradient(num_elem,dim,dim) * gradient(num_elem,dim,dim) * volumes(num_elem);
          grad_cube(dim) += gradient(num_elem,dim,dim) * gradient(num_elem,dim,dim) * gradient(num_elem,dim,dim) * volumes(num_elem);
        }
      grad_carre(dim) = mp_sum(grad_carre(dim));
      grad_carre(dim) /= volume_total;

      grad_cube(dim) = mp_sum(grad_cube(dim));
      grad_cube(dim) /= volume_total;

      if (grad_carre(dim) < 1.e-20 )
        {
          if (grad_cube(dim) < 1.e-20 )
            {
              Sk = 0;
            }
          else
            {
              Sk = 1e20;
            }
        }
      else
        {
          Sk(dim) = - grad_cube(dim) / pow(grad_carre(dim),1.5);
        }

    }

  if (je_suis_maitre())
    {
      Nom fichier = Nom("Sorties_Sk_tot_spatial");
      SFichier fic (fichier,ios::app);
      fic.precision(8);
      double temps = mon_equation->inconnue().temps();
      temps /= temps_retournement;
      fic << temps ;
      for (int dim=0; dim<dimension; dim++)
        {
          fic << " " << Sk(dim) ;
        }
      fic << finl;
    }
}



void Traitement_particulier_NS_THI_VEF::calcul_nu_t(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes = zone_VEF.volumes();
  const int nb_elem = zone_VEF.nb_elem();
  const Navier_Stokes_Turbulent& N_S_Turb  = ref_cast(Navier_Stokes_Turbulent,mon_equation.valeur());
  const DoubleTab& visc_turb = N_S_Turb.viscosite_turbulente().valeurs();

  double nu_t = 0;
  double nu_t_min = 1e+20;
  double nu_t_max = 0;

  // calcul de la moyenne, recherche du minimum et du maximum
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {

      double nu_t_elem = visc_turb(num_elem);

      nu_t += nu_t_elem * volumes(num_elem);

      if ( nu_t_elem < nu_t_min )
        {
          nu_t_min = nu_t_elem ;
        }
      if ( nu_t_elem > nu_t_max )
        {
          nu_t_max = nu_t_elem ;
        }

    }

  nu_t = mp_sum(nu_t);
  nu_t /= volume_total;

  nu_t_min = mp_min(nu_t_min);
  nu_t_max = mp_max(nu_t_max);

  // calcul de l'ecart-type
  double nu_t_ecart_type = 0;
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      double nu_t_elem = visc_turb(num_elem);

      nu_t_ecart_type += (nu_t_elem - nu_t) * (nu_t_elem - nu_t) ;
    }
  nu_t_ecart_type /= nb_elem;
  nu_t_ecart_type = sqrt(nu_t_ecart_type);

  if (je_suis_maitre())
    {
      Nom fichier = Nom("Sorties_nu_t") ;
      SFichier fic (fichier,ios::app);
      fic.precision(8);
      double temps = mon_equation->inconnue().temps();
      temps /= temps_retournement;
      fic << temps << " " << nu_t << " " << nu_t_max << " " << nu_t_min << " " << nu_t+nu_t_ecart_type << " " << nu_t-nu_t_ecart_type << finl;

    }

}




void Traitement_particulier_NS_THI_VEF::calcul_vitesse_moyenne(const DoubleTab& tab_global , DoubleVect& moyenne)
{
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur());
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  //  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VEF.nb_faces();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();

  ArrOfInt flags;
  MD_Vector_tools::get_sequential_items_flags(tab_global.get_md_vector(), flags);

  moyenne = 0;
  int nb_comp = tab_global.dimension(1);

  // calcul de la moyenne, recherche du minimum et du maximum
  for (int num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord=le_bord.nb_faces();
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + nb_faces_bord;

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          IntVect fait(nb_faces_bord);
          fait = 0;
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int face_associee=la_cl_perio.face_associee(num_face-num1);
              if (fait(num_face-num1) == 0)
                {
                  if (flags[num_face])
                    {
                      for (int dim=0; dim<nb_comp; dim++)
                        moyenne(dim) += tab_global(num_face,dim) * volumes_entrelaces(num_face);
                    }
                  fait(num_face-num1) = 1;
                  fait(face_associee) = 1;
                }
            }
        } // fin du cas periodique

      else // pour les autres conditions aux limites
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              if (flags[num_face])
                {
                  for (int dim=0; dim<nb_comp; dim++)
                    {
                      moyenne(dim) += tab_global(num_face,dim) * volumes_entrelaces(num_face);
                    }
                }
            }
        }
    } // fin du traitement des conditions limites

  for (int num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      if (flags[num_face])
        {
          for (int dim=0; dim<nb_comp; dim++)
            moyenne(dim) += tab_global(num_face,dim) * volumes_entrelaces(num_face);
        }
    } // fin de la boucle pour les faces internes


  for (int dim=0; dim<nb_comp; dim++)
    moyenne(dim) = Process::mp_sum(moyenne(dim));

  moyenne /= volume_total;

}


void Traitement_particulier_NS_THI_VEF::calcul_moyenne(const DoubleTab& tab_global , double& moyenne)
{
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur());
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  //  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VEF.nb_faces();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();

  DoubleVect moyenne_vect;
  zone_VEF.creer_tableau_faces(moyenne_vect);
  moyenne = 0;

  // calcul de la moyenne, recherche du minimum et du maximum
  for (int num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord=le_bord.nb_faces();
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + nb_faces_bord;

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          IntVect fait(nb_faces_bord);
          fait = 0;
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int face_associee=la_cl_perio.face_associee(num_face-num1);
              if (fait(num_face-num1) == 0)
                {
                  moyenne_vect(num_face) = tab_global(num_face) * volumes_entrelaces(num_face);

                  fait(num_face-num1) = 1;
                  fait(face_associee) = 1;
                }
            }
        } // fin du cas periodique

      else // pour les autres conditions aux limites
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              moyenne_vect(num_face) = tab_global(num_face) * volumes_entrelaces(num_face);
            }
        }
    } // fin du traitement des conditions limites

  for (int num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      moyenne_vect(num_face) = tab_global(num_face) * volumes_entrelaces(num_face);

    } // fin de la boucle pour les faces internes



  moyenne=mp_somme_vect(moyenne_vect);



  moyenne /= volume_total;

}

void Traitement_particulier_NS_THI_VEF::impression_moyenne(void)
{
  if (je_suis_maitre())
    {
      Nom fichier = Nom("Sorties_vitesse_moyenne") ;
      SFichier fic (fichier,ios::app);
      fic.precision(8);
      double temps = mon_equation->inconnue().temps();
      temps /= temps_retournement;
      fic << temps;
      for (int dim=0; dim<dimension; dim ++)
        fic << " " << moyenne_vitesse(dim);
      fic << finl;

      // Les champs scalaires
      for (int i=0; i<nb_champs_scalaires; i++)
        {
          Nom fichier2 = Nom("Sorties_");
          fichier2+=noms_champs_scalaires[i];
          fichier2+="_moyenne";
          SFichier fic2 (fichier2,ios::app);
          fic2.precision(8);
          fic2 << temps;
          fic2 << " " << moyennes_scal(i);
          fic2 << finl;
        }
    }

}


void Traitement_particulier_NS_THI_VEF::suppression_vitesse_moyenne(void)
// Supprime la composante moyenne de la vitesse dans chaque direction,
// travaille avec les vitesses aux faces
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone_VF=ref_cast(Zone_VF, zdisbase);
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VF.nb_faces();


  if (je_suis_maitre())
    Cerr << "Suppression de la vitesse moyenne. " ;

  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<dimension; dim ++)
        {
          vitesse(num_face,dim) -= moyenne_vitesse(dim);
        }
    }

  if (je_suis_maitre())
    Cerr << "OK." << finl;

  calcul_vitesse_moyenne(vitesse, moyenne_vitesse);
  impression_moyenne();
}


void Traitement_particulier_NS_THI_VEF::conservation_Ec(void)
// force la conservation de l'energie cinetique (THI forcee
// par opposition a THI en decroissance libre)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone_VF=ref_cast(Zone_VF, zdisbase);
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VF.nb_faces();


  if (je_suis_maitre())
    Cerr << "Suppression de la vitesse moyenne. " ;

  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      for (int dim=0; dim<dimension; dim ++)
        {
          vitesse(num_face,dim) -= moyenne_vitesse(dim);
        }
    }

  if (je_suis_maitre())
    Cerr << "OK." << finl;


  double Ec;
  Ec = calcul_Ec_spatial(vitesse, "force");

  double nE=pow(Ec/Ec_init,0.5);
  Cerr << "Ec = " << Ec << ", on divise la vitesse par : " << nE << finl;
  vitesse/=nE;

}




int Traitement_particulier_NS_THI_VEF::ppcm(int a, int b)
// renvoie le plus petit commun multiple de a et b
{
  return a * b / pgcd(a,b);
}

int Traitement_particulier_NS_THI_VEF::pgcd(int a, int b)
// renvoie le plus grand commun diviseur de a et b
{
  while ( b )
    {
      int c = a%b;
      a = b;
      b = c;
    }
  return abs(a);
}


void Traitement_particulier_NS_THI_VEF::preparer_calcul_particulier()
{
  double temps_courant = mon_equation->inconnue().temps();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  moyennes_scal.resize(nb_champs_scalaires); //les scalaires : moyenne = 1 double par scalaire
  moyenne_vitesse.resize(dimension);

  volume_total = calcul_volume_elem();
  Cerr << "Volume total = " << volume_total << finl;
  if (init==0)
    {
      temps_retournement=1;
      Ec_init = calcul_Ec_spatial(vitesse, "init");
      Cout << "Calcul Ec_init  = " << Ec_init << finl;
    }
  if (Ec_init > 0.)
    {
      temps_retournement = (L_BOITE) / sqrt(2*Ec_init);
      if(je_suis_maitre())
        Cerr << "temps de retournement base sur une longueur de "<< L_BOITE << " et une energie cinetique de Ec_init=" << Ec_init << " : " << temps_retournement << finl;
    }
  else
    {
      temps_retournement = 1;
    }
  calcul_vitesse_moyenne(vitesse, moyenne_vitesse);
  init_calc_spectre();
  if (init==1) renorm_Ec();

  if ( (oui_calc_spectre != 0) && (temps_courant == 0) )
    {
      calcul_spectre();
      if ( (oui_calc_spectre != 0)
           && (temps_courant > compteur_perio_spectre*periode_calc_spectre) )
        {
          calcul_spectre();
          compteur_perio_spectre ++;
        }
    }

  sorties_globales();
}

void Traitement_particulier_NS_THI_VEF::post_traitement_particulier()
{
  double temps_courant = mon_equation->inconnue().temps();
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();

  calcul_vitesse_moyenne(vitesse, moyenne_vitesse);

  if (oui_conservation_Ec==1) conservation_Ec();
  if ( (oui_calc_spectre != 0) && (temps_courant > compteur_perio_spectre*periode_calc_spectre) )
    {
      calcul_spectre();
      compteur_perio_spectre ++;
    }
  sorties_globales();
}

double Traitement_particulier_NS_THI_VEF::calcul_volume_elem(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VF& zone_VF=ref_cast(Zone_VF, zdisbase);
  const DoubleVect& volumes = zone_VF.volumes();
  const int nb_elem = zone_VF.nb_elem();

  double volume = 0;

  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      volume += volumes(num_elem);
    }

  volume = mp_sum(volume);

  return volume;

}

