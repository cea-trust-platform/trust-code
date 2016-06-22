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
// File:        Echange_contact_Correlation_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_Correlation_VDF.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Zone_VDF.h>
#include <Milieu_base.h>
#include <Champ_front_fonc.h>
#include <Champ_Uniforme.h>
#include <Conduction.h>
#include <Solv_TDMA.h>
#include <communications.h>
#include <SFichier.h>

Implemente_instanciable(Echange_contact_Correlation_VDF,"Paroi_Echange_contact_Correlation_VDF",Echange_global_impose);




Sortie& Echange_contact_Correlation_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_Correlation_VDF::readOn(Entree& is )
{
  //  Motcles les_mots(12);
  // Rajout Cyril MALOD (14/09/2006)
  Reprise_temperature=0;
  Motcles les_mots(13);
  {
    les_mots[0] = "dir";
    les_mots[1] = "Tinf";
    les_mots[2] = "Tsup";
    les_mots[3] = "lambda";
    les_mots[4] = "rho";
    les_mots[5] = "Cp";
    les_mots[6] = "dt_impr";
    les_mots[7] = "mu";
    les_mots[8] = "debit";
    les_mots[9] = "Dh";
    les_mots[10] = "volume";
    les_mots[11] = "Nu";
    // Rajout Cyril MALOD (14/09/2006)
    les_mots[12] = "Reprise_correlation";
  }

  dt_impr = 1e10;
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "On attendait une { a la lecture d'une " << que_suis_je() << finl;
      Cerr << "et non : " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {

      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> dir;
            if (dir<0 ||dir>2)
              {
                Cerr << "La direction doit etre 0, 1 ou 2 dans  " << que_suis_je() << finl;
                exit();
              }
            break;
          }
        case 1 :
          {
            is >> Tinf;
            break;
          }
        case 2 :
          {
            is >> Tsup;
            break;
          }
        case 3 :
          {
            Nom tmp;
            is >> tmp;
            lambda_T.setNbVar(1);
            lambda_T.setString(tmp);
            lambda_T.addVar("T");
            lambda_T.parseString();
            break;
          }
        case 4 :
          {
            Nom tmp;
            is >> tmp;
            rho_T.setNbVar(1);
            rho_T.setString(tmp);
            rho_T.addVar("T");
            rho_T.parseString();
            break;
          }
        case 5 :
          {
            is >> Cp;
            break;
          }
        case 6 :
          {
            is >> dt_impr;
            break;
          }
        case 7 :
          {
            Nom tmp;
            is >> tmp;
            mu_T.setNbVar(1);
            mu_T.setString(tmp);
            mu_T.addVar("T");
            mu_T.parseString();
            break;
          }
        case 8 :
          {
            is >> debit;
            break;
          }
        case 9 :
          {
            is >> diam;
            break;
          }
        case 10 :
          {
            Nom tmp;
            is >> tmp;
            fct_vol.setNbVar(2);
            fct_vol.setString(tmp);
            fct_vol.addVar("Dh");
            fct_vol.addVar("S");
            fct_vol.parseString();
            break;
          }
        case 11 :
          {
            Nom tmp;
            is >> tmp;
            fct_Nu.setNbVar(2);
            fct_Nu.setString(tmp);
            fct_Nu.addVar("Re");
            fct_Nu.addVar("Pr");
            fct_Nu.parseString();
            break;
          }
          // Rajout Cyril MALOD (14/09/2006)
        case 12 :
          {
            Reprise_temperature=1;
            break;
          }
        default :
          {
            Cerr << "On ne comprend le mot : " << motlu << "dans " << que_suis_je() << finl;
            exit();
          }
        }
      is >> motlu;
    }



  T_ext().typer("Champ_front_fonc");
  T_ext()->fixer_nb_comp(1);
  h_imp_.typer("Champ_front_fonc");
  h_imp_->fixer_nb_comp(1);
  return is ;
}



/**
 * Calcule le coeff d echange local dans la maille solide.
 */
void Echange_contact_Correlation_VDF::calculer_h_mon_pb(DoubleTab& tab)
{
  // forcement local
  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const Milieu_base& mon_milieu = mon_eqn.milieu();
  const Zone_VDF& ma_zvdf = ref_cast(Zone_VDF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  Nom nom_bord=frontiere_dis().frontiere().le_nom();
  calculer_h_solide(tab,mon_eqn,ma_zvdf,ma_front_vf,mon_milieu);
}



/**
 * Complete et initialise les attributs de la classe
 */
void Echange_contact_Correlation_VDF::completer()
{
  Echange_impose_base::completer();
  trier_coord();
  T=min(Tinf,Tsup);
  calculer_prop_physique();
  if (T(0)==Tinf)
    U=debit/rho(0);
  else U = debit/rho(N-1);

  for (int i=0; i<N; i++)
    h_correlation(i) = calculer_coefficient_echange(i);

  DoubleTab& mon_h= h_imp_->valeurs();
  // calculer_h_mon_pb(mon_h);


  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const int nb_faces_bord = ma_front_vf.nb_faces();
  DoubleTab& Text_valeurs = T_ext().valeur().valeurs();
  Text_valeurs.resize(nb_faces_bord,1);

  mon_h.resize(nb_faces_bord,1);

  for (int ii=0; ii<nb_faces_bord; ii++)
    {
      mon_h(ii,0) = h_correlation(correspondance_solide_fluide(ii));
      Text_valeurs(ii,0) = T(correspondance_solide_fluide(ii));
    }

  init_tab_echange();

  //Mise a jour car pas de dependance a des donnees d un autre probleme
  //dans le cas de probleme couple
  //Methode completer() peut etre a remplacer par methode initialiser()
  Equation_base& mon_eqn = zone_Cl_dis().equation();
  const double temps = mon_eqn.schema_temps().temps_courant();
  mettre_a_jour(temps);
}

/**
 * Initialise le tab tab_ech pour le parallele
 */
void Echange_contact_Correlation_VDF::init_tab_echange()
{
  DoubleVect y_envoye(2);
  DoubleVect y_recu(2);
  y_envoye(0) = coord(1); // y_min
  y_envoye(1) = coord(N-2); // y_max


  const Joints& joints = zone_Cl_dis().zone().faces_joint();
  const int nb_voisins = joints.size();

  const int ME=Process::me();
  tab_ech.resize(Process::nproc(),Process::nproc());
  tab_ech=-1;
  for (int i=0; i<nb_voisins; i++)
    {
      int mon_voisin = joints[i].PEvoisin();
      Cerr << " Echange_contact_Correlation_VDF::init_tab_echange() A verifier pour envoi blocant" << finl;
      exit();
      envoyer(y_envoye,ME,mon_voisin,ME);

      recevoir(y_recu,mon_voisin,ME,mon_voisin);
      if (y_recu(1)<coord(1))
        {
          tab_ech(mon_voisin,ME)=1; // mon_voisin m'enverra le max
        }
      else if (y_recu(0)>coord(N-2))
        {
          tab_ech(mon_voisin,ME)=0; // mon_voisin m'enverra le min
        }
    }
}


/**
 * Calcule rho, mu et lambda du fluide pour la temperature courante
 */
void Echange_contact_Correlation_VDF::calculer_prop_physique()
{
  for (int i=0; i<N; i++)
    {
      rho_T.setVar("T",T(i));
      mu_T.setVar("T",T(i));
      lambda_T.setVar("T",T(i));
      rho(i) = rho_T.eval();
      mu(i) = mu_T.eval();
      lambda(i) = lambda_T.eval();
    }
}

/**
 * Calcule le coeff d echange suivant la correlation entree dans le jdd
 */
double Echange_contact_Correlation_VDF::calculer_coefficient_echange(int i)
{
  double Re,Pr;
  Re = fabs(getQh()*getDh()/getMu(i));
  Pr = getMu(i)*getCp()/getLambda(i);
  fct_Nu.setVar("Re",Re);
  fct_Nu.setVar("Pr",Pr);
  return fct_Nu.eval()*getLambda(i)/getDh();
}

/**
 * Calcul du terme source  de puissance volumique dans l'equation d'energie 1D fluide.
 */
void Echange_contact_Correlation_VDF::calculer_Q()
{
  const Zone_VDF& ma_zvdf = ref_cast(Zone_VDF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const int ndeb = ma_front_vf.num_premiere_face();
  const int nb_faces_bord = ma_front_vf.nb_faces();
  const DoubleVect& surfaces = ma_zvdf.face_surfaces();
  const IntTab& face_voisins = ma_zvdf.face_voisins();
  Champ_Inc_base& inco = zone_Cl_dis().equation().inconnue();
  DoubleTab& Ts = inco.valeurs();
  DoubleTab& mon_h= h_imp_->valeurs();


  Qvol=0.;
  for(int iface=0; iface<nb_faces_bord; iface++)
    {
      int corresp = correspondance_solide_fluide(iface);
      int elem =  face_voisins(ndeb+iface,0);
      if(elem == -1)
        elem =  face_voisins(ndeb+iface,1);
      Qvol(corresp)+=mon_h(iface,0)*(Ts(elem)-T(corresp))*surfaces(ndeb+iface);
    }
  for (int i=0; i<N; i++)
    Qvol(i)/=vol(i);
}



void Echange_contact_Correlation_VDF::trier_coord()
{
  const Zone_VDF& ma_zvdf = ref_cast(Zone_VDF,zone_Cl_dis().zone_dis().valeur());
  const DoubleVect& surfaces = ma_zvdf.face_surfaces();
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());

  const int ndeb = ma_front_vf.num_premiere_face();
  const int nb_faces_bord = ma_front_vf.nb_faces();

  IntVect face_triee(nb_faces_bord);
  const DoubleTab& xv = ma_zvdf.xv();
  int i, j ,tmp;

  // On trie les faces par coordonnees croissante (selon la coordonnee correspondante a la direction du tube)
  if (nb_faces_bord>1)
    {
      for(i=0; i<nb_faces_bord; i++)
        face_triee(i) = ndeb+i;

      for(i=1; i<nb_faces_bord; i++)
        {
          j=i-1;
          tmp = face_triee(i);
          while ( j>=0 && xv(face_triee(j),dir)>xv(tmp,dir))
            {
              face_triee(j+1) = face_triee(j);
              j--;
            }
          face_triee(j+1) = tmp;
        }
    }
  // On determine le nb de mailles 1D dans le fluide fictif
  // et on remplit les correspondances elements solides -> elements fluides
  correspondance_solide_fluide.resize(nb_faces_bord);
  N=3; // on rajoute les noeuds des bords
  correspondance_solide_fluide(face_triee(0)-ndeb) = 1;
  for(i=1; i<nb_faces_bord; i++)
    {
      if (xv(face_triee(i),dir) != xv(face_triee(i-1),dir))
        N++;
      correspondance_solide_fluide(face_triee(i)-ndeb) = N-2;
    }

  U.resize(N);
  T.resize(N);
  Qvol.resize(N);
  rho.resize(N);
  mu.resize(N);
  lambda.resize(N);
  vol.resize(N);
  coord.resize(N);
  h_correlation.resize(N);



  // calcul des volumes des tranches
  DoubleVect surf(N);
  surf=0.;
  vol=0.;
  surf(0) = surf(N-1) = 0.; // ne sert pas
  for(int iface=0; iface<nb_faces_bord; iface++)
    {
      int corresp = correspondance_solide_fluide(iface);
      surf(corresp)+=surfaces(ndeb+iface);
      coord(corresp) = xv(ndeb+iface,dir);
    }
  coord(0) = coord(1)-0.5*(coord(2)-coord(1)); // le premier point du maillage
  coord(N-1) = coord(N-2)+0.5*(coord(N-2)-coord(N-3)); // le dernier point du maillage
  for (i=1; i<N-1; i++)
    {
      vol(i)=volume(surf(i),diam);
    }
  vol(0) = vol(N-1) = 1.; // ne sert pas mais evite une division par zero dans le calcul de Qvol.
}



/**
 * Calcule le volume d'une tranche de la section de surface laterale s et de diametre hydraulique d.
 */
double Echange_contact_Correlation_VDF::volume(double s, double d)
{
  fct_vol.setVar("Dh",d);
  fct_vol.setVar("S",s);

  return fct_vol.eval();
}


/**
 * Calcule les CL a appliquer sur l'equation d'energie
 */
void Echange_contact_Correlation_VDF::calculer_CL()
{
  const int ME=Process::me();

  const Joints& joints = zone_Cl_dis().zone().faces_joint();
  const int nb_voisins = joints.size();

  DoubleVect les_cl_envoyees(2);
  DoubleVect les_cl_recues(2);
  les_cl_envoyees(0) = T(1);
  les_cl_envoyees(1) = T(N-2);

  T_CL0=Tinf;
  T_CL1=Tsup;
  for (int i=0; i<nb_voisins; i++)
    {
      int mon_voisin = joints[i].PEvoisin();

      Cerr << " Echange_contact_Correlation_VDF::calculer_CL() A verifier pour envoi blocant" << finl;
      exit();
      envoyer(les_cl_envoyees,ME,mon_voisin,ME);

      recevoir(les_cl_recues,mon_voisin,ME,mon_voisin);
      if (tab_ech(mon_voisin,ME) == 1)
        {
          T_CL0 = 0.5*(les_cl_recues(1)+T(1));
          T(0) = T_CL0;
        }
      else if (tab_ech(mon_voisin,ME) == 0)
        {
          T_CL1 = 0.5*(les_cl_recues(0)+T(N-2));
          T(N-1) = T_CL1;
        }
      else
        {
          Cerr << "Erreur de communication entre les processeurs " <<ME<< " et " <<mon_voisin << finl;
          Cerr << "dans Echange_contact_Correlation_VDF::calculer_CL" << finl;
          if (Process::nproc()>1) Cerr << "Verifier que votre decoupage est bien en tranches selon l'axe du modele 1D." << finl;
          exit();

        }
    }
}


/**
 * Calcule la vitesse par conservation de la masse
 */
void Echange_contact_Correlation_VDF::calculer_Vitesse()
{
  for (int i=0; i<N; i++)
    U(i) = debit/rho(i);
}


/**
 * Calcule la temperature 1D dans le fluide en resolvant la conservation de l'energie
 */
void Echange_contact_Correlation_VDF::calculer_Tfluide()
{
  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const double dt = mon_eqn.schema_temps().pas_de_temps();
  DoubleVect ma(N); // diagonale
  DoubleVect mb(N-1); // sous-diagonale
  DoubleVect mc(N-1); // sur-diagonale
  DoubleVect sm(N);
  const int sgn = (debit>0) ? 1 : -1; // schema amont pour le transport


  ma(0) = 1.;
  ma(N-1) = 1.;
  sm(0) = T_CL0;
  sm(N-1) = T_CL1;
  mc(0) = 0.;
  mb(N-2) = 0.;
  int i;
  for (i=1; i<N-1; i++)
    {
      const double dtrhoCp = dt/rho(i)/Cp;
      const double dz1 = coord(i+1)-coord(i);
      const double dz2 = coord(i)-coord(i-1);
      const double l1 = 0.5*(lambda(i+1)+lambda(i));
      const double l2 = 0.5*(lambda(i)+lambda(i-1));

      ma(i)=1+dtrhoCp*(sgn*Cp*debit/dz1+2*(l1/dz1+l2/dz2)/(dz1+dz2));

      sm(i) = T(i)+dtrhoCp*Qvol(i);

      mc(i) = dtrhoCp*(0.5*(1-sgn)*Cp*debit/dz1-2*l1/dz1/(dz1+dz2));

      mb(i-1) = dtrhoCp*(-2*l2/dz2/(dz1+dz2));
      mb(i-1) += dtrhoCp*(-0.5*(1+sgn)*Cp*debit/dz1);
    }




  DoubleVect xx(N);
  xx=0.;
  Solv_TDMA::resoudre(ma,mb,mc,sm,xx,N);
  for (i=0; i<N; i++)
    T(i) = xx(i);
}




/**
 * Mise a jour de la vitesse, temperature fluide et coeff d'echange
 */
void Echange_contact_Correlation_VDF::mettre_a_jour(double temps)
{

  // Nom des fichiers de sauvegarde
  Nom Fichier_sauv_nom=zone_Cl_dis().equation().probleme().le_nom();
  Fichier_sauv_nom+="_";
  Fichier_sauv_nom+=frontiere_dis().frontiere().le_nom();
  Fichier_sauv_nom+=".sauv";

  // Operation de reprise du champ de temperature dans le fluide
  const int ME = Process::me();
  const int nbproc = Process::nproc();
  FILE *Fichier_sauv;
  if (Reprise_temperature==1)
    {
      if (nbproc>1)
        {
          Cerr << "A verifier pour envoi blocant" << finl;
          exit();
          envoyer(T,ME,0,ME);                                                                // J'envoie T pour connaitre la dimension de son tableau
          if (je_suis_maitre())
            {
              double temps_lu=0., Temperature_reprise=0.;
              char tmp_lu;
              //int code;
              Fichier_sauv=fopen(Fichier_sauv_nom,"r");                                // J'ouvre le fichier de sauvgarde en lecture seule
              if (Fichier_sauv!=NULL)
                {
                  if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                                // Je lis le premier mot "Temps" pour rien
                  if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                                // Je lis le deuxieme mot "=" pour rien
                  if (!fscanf(Fichier_sauv,"%lf",&temps_lu)) exit();                                // Je lis le temps et je le mets dans temps_lu
                  if (temps_lu==temps)
                    {
                      for (int p=0; p<nbproc; p++)                                      // Je boucle sur le nombre de processeur
                        {
                          Cerr << "Reprise du champs de temperature (" << Fichier_sauv_nom << ") pour la correlation sur le processeur " << p << "...";
                          DoubleVect T_tmp;
                          recevoir(T_tmp,p,0,p);                                        // Je recupere T pour connaitre la dimension de son tableau au niveau du processeur maitre.
                          for (int i=0; i<T_tmp.size(); i++)                      // Je boucle sur le nombre de temperature a definir
                            {
                              if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                // Je lis le mot "T(i)" pour rien
                              if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                // Je lis le mot "=" pour rien
                              if (!fscanf(Fichier_sauv,"%lf",&Temperature_reprise)) exit();// Je lis la valeur de la temperature et je la mets dans Temperature_reprise
                              T_tmp(i) = Temperature_reprise;                        // J'egale la variable du calcul T(i) a Temperature_reprise
                            }
                          envoyer(T_tmp,0,p,0);                                        // J'envoie T_tmp a tous les processeurs
                          Cerr << "   OK!" << finl;
                        }
                      fclose(Fichier_sauv);
                    }
                  else
                    {
                      Cerr << "\nLe temps indique dans le fichier \""<< Fichier_sauv_nom << "\" "<< temps_lu << finl;
                      Cerr << "est different du temps de reprise du calcul "<< temps << " !!!" << finl << finl;
                      Cerr << "Vous ne pouvez pas faire de reprise pour la correlation. Supprimez" << finl;
                      Cerr << "dans la correlation le mot clef \"Reprise\". La phase transitoire" << finl;
                      Cerr << "du calcul sera fausse, mais l'etat stationnaire sera juste." << finl << finl;
                      fclose(Fichier_sauv);
                      exit();
                    }
                }
              else
                {
                  Cerr << "\nLa reprise ne peut pas etre faite : Le fichier \"" << Fichier_sauv_nom << "\" est manquant !!!" << finl << finl;
                  exit();
                }
            }
          recevoir(T,0,ME,0);                                                                // Je recupere T=T_tmp au niveau de chaque processeur
        }
      else
        {
          Fichier_sauv=fopen(Fichier_sauv_nom,"r");
          double temps_lu=0., Temperature_reprise=0.;
          char tmp_lu;

          if (Fichier_sauv!=NULL)
            {
              if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                        // Je lis le premier mot "Temps" pour rien
              if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                        // Je lis le deuxieme mot "=" pour rien
              if (!fscanf(Fichier_sauv,"%lf",&temps_lu)) exit();                        // Je lis le temps et je le mets dans temps_lu
              if (temps_lu==temps)
                {
                  Cerr << "Reprise du champs de temperature (" << Fichier_sauv_nom << ") pour la correlation...";
                  for (int i=0; i<N; i++)
                    {
                      if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                // Je lis le mot "T(i)" pour rien
                      if (!fscanf(Fichier_sauv,"%s",&tmp_lu)) exit();                // Je lis le mot "=" pour rien
                      if (!fscanf(Fichier_sauv,"%lf",&Temperature_reprise)) exit();// Je lis la valeur de la temperature et je la mets dans Temperature_reprise
                      T(i) = Temperature_reprise;                        // J'egale la variable du calcul T(i) a Temperature_reprise
                    }
                  fclose(Fichier_sauv);
                  Cerr << "   OK!" << finl;
                }
              else
                {
                  Cerr << "\nLe temps indique dans le fichier \""<< Fichier_sauv_nom << "\" "<< temps_lu << finl;
                  Cerr << "est different du temps de reprise du calcul "<< temps << " !!!" << finl << finl;
                  Cerr << "Vous ne pouvez pas faire de reprise pour la correlation. Supprimez" << finl;
                  Cerr << "dans la correlation le mot clef \"Reprise\". La phase transitoire" << finl;
                  Cerr << "du calcul sera fausse, mais l'etat stationnaire sera juste." << finl << finl;
                  fclose(Fichier_sauv);
                  exit();
                }
            }
          else
            {
              Cerr << "\nLa reprise ne peut pas etre faite : Le fichier \"" << Fichier_sauv_nom << "\" est manquant !!!" << finl << finl;
              exit();
            }
        }
      Reprise_temperature=0;
    }

  calculer_CL();
  calculer_prop_physique();
  calculer_Q();
  calculer_Vitesse();
  calculer_Tfluide();



  for (int i=0; i<N; i++)
    h_correlation(i) = calculer_coefficient_echange(i);




  DoubleTab& mon_h= h_imp_->valeurs();
  calculer_h_mon_pb(mon_h);
  const int taille=mon_h.dimension(0);
  DoubleTab& Text_valeurs = T_ext().valeur().valeurs();

  for (int ii=0; ii<taille; ii++)
    {
      mon_h(ii,0) = 1./(1./mon_h(ii,0)+1./h_correlation(correspondance_solide_fluide(ii)));
      Text_valeurs(ii,0) = T(correspondance_solide_fluide(ii));
    }

  const Equation_base& mon_eqn = zone_Cl_dis().equation();

  if (limpr(temps,mon_eqn.schema_temps().pas_de_temps())) imprimer(temps);

  Echange_global_impose::mettre_a_jour(temps);

  // Operation de sauvegarde du champ de temperature dans le fluide en vue d'une reprise
  if (nbproc>1)
    {
      envoyer(T,ME,0,ME);                                                                // J'envoie T au processeur maitre
      if (je_suis_maitre())
        {
          Fichier_sauv=fopen(Fichier_sauv_nom,"w");                // J'ouvre le fichier de sauvegarde en ecriture
          fprintf(Fichier_sauv,"Temps\t=\t%f\n",temps);                                // J'imprime le temps
          int j=0;
          for (int p=0; p<nbproc; p++)                                              // Je boucle sur le nombre de processeur
            {
              DoubleVect T_tmp;
              recevoir(T_tmp,p,0,p);                                                // Je recupere les temperature de chaque processeur
              for (int i=0; i<T_tmp.size(); i++)
                {
                  fprintf(Fichier_sauv,"T(%i)\t=\t%f\n",(True_int)j,T_tmp(i));        // J'imprime les temperatures dans le fichier de sauvegarde
                  j=j+1;
                }
            }
          fclose(Fichier_sauv);
        }
    }
  else
    {
      Fichier_sauv=fopen(Fichier_sauv_nom,"w");
      fprintf(Fichier_sauv,"Temps\t=\t%f\n",temps);
      for (int i=0; i<N; i++)
        {
          fprintf(Fichier_sauv,"T(%i)\t=\t%f\n",(True_int)i,T(i));
        }
      fclose(Fichier_sauv);
    }
}

int Echange_contact_Correlation_VDF::compatible_avec_eqn(const Equation_base& eqn) const
{
  if ( sub_type(Conduction, eqn) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}
void Echange_contact_Correlation_VDF::calculer_h_solide(DoubleTab& tab,const Equation_base& une_eqn,const Zone_VDF& zvdf_2,const Front_VF& front_vf,const    Milieu_base& le_milieu)
{
  DoubleVect e;
  const IntTab& face_voisins = zvdf_2.face_voisins();
  int i;
  int nb_comp = le_milieu.conductivite()->nb_comp();
  int ndeb = front_vf.num_premiere_face();
  int nfin = ndeb + front_vf.nb_faces();

  e.resize(front_vf.nb_faces());

  for (int face=ndeb; face<nfin; face++)
    e(face-ndeb) = zvdf_2.dist_norm_bord(face);

  // Calcul de tab = 1/(e/lambda + 1/h_paroi) =1/(e/lambda+invhparoi)
  if(!sub_type(Champ_Uniforme,le_milieu.conductivite().valeur()))
    {
      //Cerr << "raccord local homogene et conductivite non uniforme" << finl;
      const DoubleTab& tab_lambda = le_milieu.conductivite().valeurs();
      if (tab_lambda.nb_dim() == 1)
        {
          assert(nb_comp==1);
          for (int face=ndeb; face<nfin; face++)
            {
              int elem = face_voisins(face,0);
              if (elem == -1)
                elem = face_voisins(face,1);
              assert(tab_lambda(elem)!=0.);
              for(i=0; i<nb_comp; i++)
                tab(face-ndeb,i) = tab_lambda(elem)/e(face-ndeb);
            }
        }
      else
        for (int face=ndeb; face<nfin; face++)
          {
            int elem = face_voisins(face,0);
            if (elem == -1)
              elem = face_voisins(face,1);
            for(i=0; i<nb_comp; i++)
              {
                assert(le_milieu.conductivite()(elem,i)!=0.);
                tab(face-ndeb,i) = tab_lambda(elem,i)/e(face-ndeb);
              }
          }
    }
  else  // la conductivite est un Champ uniforme
    {
      for (int face=ndeb; face<nfin; face++)
        {
          for(i=0; i<nb_comp; i++)
            {
              assert(le_milieu.conductivite()(0,i)!=0.);
              tab(face-ndeb,i) = le_milieu.conductivite()(0,i)/e(face-ndeb);
            }
        }
    }
}


/**
 * Teste si l'impression est demandee
 */
int Echange_contact_Correlation_VDF::limpr(double temps_courant,double dt) const
{
  const Schema_Temps_base& sch = zone_Cl_dis().equation().schema_temps();
  // Test un peu tordu, mais ca fonctionne !!!! (CM 05/07/2007)
  if (dt_impr<=dt || ((sch.temps_max()<=temps_courant || sch.nb_pas_dt_max()<=(sch.nb_pas_dt()+1) || (temps_courant!=sch.temps_courant() && sch.nb_pas_dt()==0)) && dt_impr!=1e10))
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      static const double epsilon = 1.e-9;
      double i, j;
      modf(temps_courant/dt_impr + epsilon, &i);
      modf((temps_courant-dt)/dt_impr + epsilon, &j);
      return ( i>j );
    }
}


/**
 * Imprime les resultats
 */
void Echange_contact_Correlation_VDF::imprimer(double temps) const
{
  const int ME = Process::me();
  const int nbproc = Process::nproc();

  if (nbproc>1)
    {
      envoyer(coord,ME,0,ME);
      envoyer(T,ME,0,ME);
      envoyer(U,ME,0,ME);
      envoyer(h_correlation,ME,0,ME);
      envoyer(rho,ME,0,ME);
      envoyer(mu,ME,0,ME);
      envoyer(lambda,ME,0,ME);
      envoyer(Qvol,ME,0,ME);
      envoyer(vol,ME,0,ME);

      if (je_suis_maitre())
        {
          Nom nom_bord=frontiere_dis().frontiere().le_nom();
          nom_bord+="_";
          nom_bord+=Nom(temps);
          nom_bord+=".dat";
          SFichier fic(nom_bord);
          fic.precision(zone_Cl_dis().equation().schema_temps().precision_impr());
          fic.setf(ios::scientific);
          double Qt=0.;
          fic << "# X                T                U                h                rho                mu                lambda                        Q[W]" << finl;

          for (int p=0; p<nbproc; p++)
            {
              DoubleVect coord_tmp;
              DoubleVect T_tmp;
              DoubleVect U_tmp;
              DoubleVect h_tmp;
              DoubleVect rho_tmp;
              DoubleVect mu_tmp;
              DoubleVect lambda_tmp;
              DoubleVect Qvol_tmp;
              DoubleVect vol_tmp;

              recevoir(coord_tmp,p,0,p);
              recevoir(T_tmp,p,0,p);
              recevoir(U_tmp,p,0,p);
              recevoir(h_tmp,p,0,p);
              recevoir(rho_tmp,p,0,p);
              recevoir(mu_tmp,p,0,p);
              recevoir(lambda_tmp,p,0,p);
              recevoir(Qvol_tmp,p,0,p);
              recevoir(vol_tmp,p,0,p);

              for (int i =0; i<coord_tmp.size(); i++)
                {
                  fic << coord_tmp(i) << " \t" << T_tmp(i) << " \t" << U_tmp(i) << " \t" << h_tmp(i) << " \t" << rho_tmp(i) << " \t" << mu_tmp(i) << " \t" << lambda_tmp(i) << " \t" << Qvol_tmp(i)*vol_tmp(i) << finl;
                  Qt+=Qvol_tmp(i)*vol_tmp(i);
                }

            }


          fic << "# Q total[W] = " << Qt << finl;
          fic << finl;
        }
    }
  else
    {
      Nom nom_bord=frontiere_dis().frontiere().le_nom();
      nom_bord+="_";
      nom_bord+=Nom(temps);
      nom_bord+=".dat";
      SFichier fic(nom_bord);
      fic.precision(zone_Cl_dis().equation().schema_temps().precision_impr());
      fic.setf(ios::scientific);
      double Qt=0.;
      fic << "# X                 T                 U                 h                 rho                 mu                 lambda                  Q[W]" << finl;
      for (int i =0; i<N; i++)
        {
          fic << coord(i) << " \t" << T(i) << " \t" << U(i) << " \t" << h_correlation(i) << " \t" << rho(i) << " \t" << mu(i) << " \t" << lambda(i) << " \t" << Qvol(i)*vol(i) << finl;
          Qt+=Qvol(i)*vol(i);
        }
      fic << "# Q total[W] = " << Qt << finl;
      fic << finl;

    }
}
