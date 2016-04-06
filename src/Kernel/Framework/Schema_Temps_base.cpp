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
// File:        Schema_Temps_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/102
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation.h>
#include <LecFicDiffuse.h>
#include <EcrFicCollecte.h>
#include <Debog.h>
#include <stat_counters.h>
#include <Param.h>
#include <communications.h>
#include <sys/stat.h>
#include <SFichier.h>
#include <DoubleTab.h>
#include <Matrice_Morse.h> // necessaire pour visual

Implemente_base_sans_constructeur(Schema_Temps_base,"Schema_Temps_base",Objet_U);

void Schema_Temps_base::initialize()
{
  // GF je remets le calculer_pas_de_temps car des schemas en temps implicites s'en servent pour dimensionner (en particulier ovap)
  pb_base().calculer_pas_de_temps();
  // je le mets une deuxieme fois pour alternant....
  pb_base().calculer_pas_de_temps();
  Nom fichier(nom_du_cas());
  fichier+=".dt_ev";
  if (je_suis_maitre())
    {
      struct stat f;
      // On initialise le fichier .dt_ev s'il n'existe pas ou si c'est un demarrage de calcul sans reprise
      if (nb_pas_dt_==0 && ((stat(fichier,&f)) || !pb_base().reprise_effectuee()))
        {
          SFichier fic(fichier,(schema_impr() ? (ios::out) : (ios::app)));
          if (schema_impr())
            fic << "# temps\t\t dt\t\t facsec\t\t residu=max|Ri|\t dt_stab\t ";
          for (int i=0; i<pb_base().nombre_d_equations(); i++)
            fic << pb_base().equation(i).expression_residu();
        }
    }

  if ((nb_pas_dt_==0) && ( ((mode_dt_start_==0) && est_egal(tinit_,0.)) || (mode_dt_start_==-1)) )
    {
      //On divise par facsec_ car multiplication par facsec_ dans corriger_dt_calcule()
      //quel que soit le mode d initialisation de dt_
      dt_=dt_min_/facsec_;
    }
  else if (mode_dt_start_>0)
    {
      dt_=mode_dt_start_/facsec_;
    }
  else
    {
      mettre_a_jour_dt_stab();
      dt_=dt_stab_;
    }
  dt_stab_=dt_;
}

double Schema_Temps_base::computeTimeStep(bool& is_stop) const
{

  is_stop=false;
  // Correction en premier du pas de temps
  double dt = dt_stab_;
  if (!corriger_dt_calcule(dt)) // Change le contenu de dt
    is_stop=true;
  // Mise a jour immediate de l'attribut dt_ afin que pas_de_temps()
  // soit a jour tout le temps (en particulier au moment du postraitement)
  // Ce n'etait pas le cas pour les versions <= 1.6.3 et les champs dependant
  // de dt n'etaient pas juste si facsec<>1
  //  dt_ = dt;

  if (this->stop())
    {
      is_stop=true;
      return 0;
    }
  else
    return dt;
}

bool Schema_Temps_base::initTimeStep(double dt)
{

  dt_=dt;
  stationnaire_atteint_ = -1;
  residu_=0;

  return true;
}

// Description:
// Calculate the U(n+1) unknown for each equation (if solved) of the problem with the selected time scheme
bool Schema_Temps_base::iterateTimeStep(bool& converged)
{
  // Loop on the equations of the problem:
  for(int i=0; i<pb_base().nombre_d_equations(); i++)
    {
      Equation_base& equation=pb_base().equation(i);
      // Calculate (if equation is solved) the unknown equation U(n+1)
      // according to the selected time scheme:
      if (!equation.equation_non_resolue())
        faire_un_pas_de_temps_eqn_base(equation);
      else
        {
          Cout<< "====================================================" << finl;
          Cout<< equation.que_suis_je()<<" equation is not solved."<<finl;
          Cout<< "====================================================" << finl;
          // On calcule une fois la derivee pour avoir les flux bord
          if (equation.schema_temps().nb_pas_dt()==0)
            {
              DoubleTab inconnue_valeurs(equation.inconnue().valeurs());
              equation.derivee_en_temps_inco(inconnue_valeurs);
            }
        }
    }
  converged=true;
  return true;
}

// Description:
//    Renvoie 1 s'il y a lieu d'effectuer une impression (cf dt_impr)
//    Renvoie 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si il y lieu d'effectuer une impression 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Schema_Temps_base::limpr() const
{
  if (dt_impr_<=0)
    return 0; // Negative value for dt_impr : we never print
  if (nb_pas_dt_==0)
    return 0; // No solve cause 0 time step : we print nothing
  if (dt_impr_<=dt_)
    return 1; // We print every time step if dt_impr is lower than dt
  if (nb_pas_dt_<=1)
    return 1; // We print the first time step
  if (tmax_<=temps_courant_ || nb_pas_dt_max_<=nb_pas_dt_ || stationnaires_atteints_)
    return 1; // We print the last time step

  // 26/01/2010 : On utilise desormais la fonction "modf(operation , & partie entiere)" de math.h
  // Cette fonction decompose le resultat de "operation" en une partie entiere et une partie decimale.
  // Ainsi, on ne transforme plus un double en int avec tout les risques que cela comporte.
  // ex : modf(9/7,&i) donne 1.000000 pour la partie entiere et 0.285714 pour le partie decimale
  //
  // epsilon permet d'assurer que le resultat de l'operation est independant de la precision machine :
  // ex : 0.99999999/1.00000000 peut donner 0.99999999 ou 1.00000000 suivant les machines
  // alors que
  // ex : 0.99999999/1.00000000 + 1.e-8 donne tjrs 1.00000000.
  double i, j, epsilon = 1.e-8;
  modf(temps_courant_/dt_impr_ + epsilon, &i);
  modf(temps_precedent_/dt_impr_ + epsilon, &j);
  return ( i>j );
}

void Schema_Temps_base::validateTimeStep()
{
  // Update the problem:
  Probleme_base& problem=pb_base();
  problem.mettre_a_jour(temps_courant_+dt_);
  if (je_suis_maitre())
    {
      Nom fichier(nom_du_cas());
      fichier+=".dt_ev";
      SFichier fic(fichier,ios::app);
      // On supprime la precision d'impression pour avoir le meme format que la sauvegarde.
      // fic.precision(precision_impr());
      fic.setf(ios::scientific);
      if (schema_impr())
        fic << finl << temps_courant_ << "\t " << dt_ << "\t " << facsec_ <<"\t " << residu_ << "\t " << dt_stab_ << "\t ";
      for (int i=0; i<pb_base().nombre_d_equations(); i++)
        pb_base().equation(i).imprime_residu(fic);
      // Impression du temps CPU estime restant
      if (limpr() && nb_pas_dt()>0 && pas_de_temps()>0)
        {
          // On calcule le temps CPU moyen par pas de temps, inconvenient il peut varier fortement au cours du temps si divergence du calcul ou au contraire acceleration
          // Mais Statistiques ne permet pas d'avoir le temps CPU du dernier pas de temps (last_time appele ici renverrait le temps CPU depuis le debut du pas de temps)
          double cpu_per_timestep           = statistiques().last_time(temps_total_execution_counter_) / nb_pas_dt();
          double nb_pas_selon_tmax          = (temps_max()-temps_courant()) / pas_de_temps();
          double nb_pas_selon_nb_pas_dt_max = nb_pas_dt_max() - nb_pas_dt();
          double nb_pas_avant_fin= dmin(nb_pas_selon_tmax,nb_pas_selon_nb_pas_dt_max);
          double seconds_to_finish  = nb_pas_avant_fin * cpu_per_timestep;
          int percent=(int)((1.-nb_pas_avant_fin/(nb_pas_avant_fin+ nb_pas_dt()))*100);    // marche meme si c'est ltemps max qui limite
          int integer_limit=(int)(pow(2.0,(double)((sizeof(True_int)*8)-1))-1);
          if (seconds_to_finish<integer_limit)
            {
              int h  = (int)(seconds_to_finish/3600);
              int mn = (int)((seconds_to_finish-3600*h)/60);
              int s  = (int)(seconds_to_finish-3600*h-60*mn);
              Cout << finl << "Estimated CPU time to finish the run (according to " << (nb_pas_selon_tmax<nb_pas_selon_nb_pas_dt_max?"tmax":"nb_pas_dt_max") << " value) : ";
              if (seconds_to_finish<1)
                Cout << seconds_to_finish << " s";
              else
                Cout << h << "h" << mn << "mn" << s << "s";
              Cout << ". Progress: "<<percent<< finl;
            }
        }
    }
  // Update time scheme:
  mettre_a_jour();
}

// Description:
// Retourne 1 si lors du dernier pas de temps, le probleme n'a pas evolue.
// Precondition: validateTimeStep()
// Parametre: temps
//    Signification: le temps a atteindre
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: 1 si le probleme n'a pas evolue, 0 sinon.
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
bool Schema_Temps_base::isStationary() const
{
  if (stationnaire_atteint_ && (nb_pas_dt_ >= 2))
    {
      Cerr << "---------------------------------------------------------"
           << finl
           << "The problem " << pb_base().le_nom()
           << " has reached the steady state"
           << finl << finl;
      return true;
    }
  return false;
}

void Schema_Temps_base::abortTimeStep()
{
  Probleme_base& prob=pb_base();
  for(int i=0; i<prob.nombre_d_equations(); i++)
    prob.equation(i).abortTimeStep();

  dt_ = prob.calculer_pas_de_temps();
}

void Schema_Temps_base::associer_pb(const Probleme_base& un_probleme)
{
  //assert(!mon_probleme.non_nul());
  mon_probleme=un_probleme;
}



void Schema_Temps_base::set_param(Param& param)
{
  param.ajouter("tinit",&tinit_);
  param.ajouter( "tmax",&tmax_);
  param.ajouter_non_std( "tcpumax",(this));
  param.ajouter( "dt_min",&dt_min_);
  param.ajouter( "dt_max",&dt_max_);
  param.ajouter( "dt_sauv",&dt_sauv_);
  param.ajouter( "dt_impr",&dt_impr_);
  param.ajouter( "facsec",&facsec_);
  param.ajouter( "seuil_statio",&seuil_statio_);
  param.ajouter( "seuil_statio_relatif_deconseille",&seuil_statio_relatif_deconseille_);
  param.ajouter( "diffusion_implicite",&ind_diff_impl_);
  param.ajouter( "seuil_diffusion_implicite",&seuil_diff_impl_);
  param.ajouter( "impr_diffusion_implicite",&impr_diff_impl_);
  param.ajouter( "no_error_if_not_converged_diffusion_implicite",&no_error_if_not_converged_diff_impl_);
  param.ajouter( "no_conv_subiteration_diffusion_implicite",&no_conv_subiteration_diff_impl_);
  param.ajouter_non_std( "dt_start",(this));
  // param.ajouter( "nb_pas_dt_max",&nb_pas_dt_max_);
  // nb_pas_dt_max non standard pour valgrind
  param.ajouter_non_std( "nb_pas_dt_max",(this));
  param.ajouter( "niter_max_diffusion_implicite",&niter_max_diff_impl_);
  param.ajouter( "precision_impr",&precision_impr_);
  param.ajouter_non_std( "periode_sauvegarde_securite_en_heures",(this));
  param.ajouter_non_std( "no_check_disk_space",(this));
}
// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime le schema en temps sur un flot de sortie.
//    !! Attention n'est pas symetrique de la lecture !!
//    On ecrit les differents parametres du schema en temps.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Schema_Temps_base::printOn(Sortie& os) const
{
  os << "dt " << dt_ << finl;
  os << "temps_courant " << temps_courant_ << finl ;
  os << "tinit " << tinit_ << finl;
  os << "tmax " << tmax_ << finl ;
  os << "tcpumax " << tcpumax_ << finl ;
  os << "nb_pas_dt " << nb_pas_dt_ << finl;
  os << "nb_pas_dt_max " <<  nb_pas_dt_max_ << finl;
  os << "dt_min " << dt_min_ << finl;
  os << "dt_max " << dt_max_ << finl;
  os << "facsec " << facsec_ << finl;
  os << "seuil_statio" << seuil_statio_ << finl;
  os << "seuil_statio_relatif_deconseille" << seuil_statio_relatif_deconseille_ << finl;
  os << "dt_sauv " << dt_sauv_ << finl;
  os << "limite_cpu_sans_sauvegarde " << limite_cpu_sans_sauvegarde_ << finl;
  os << "dt_impr " << dt_impr_ << finl;
  os << "precision_impr " << precision_impr_ << finl;
  os << "stationnaire_atteint " << stationnaire_atteint_ << finl;
  os << "diffusion_implicite " << ind_diff_impl_ << finl ;
  os << "seuil_diffusion_implicite " << seuil_diff_impl_ << finl ;
  os << "impr_diffusion_implicite " << impr_diff_impl_ << finl ;
  os << "niter_max_diffusion_implicite " << niter_max_diff_impl_ << finl ;
  os << "no_file_allocation " << file_allocation_ << finl ;
  os << "fin " << finl;
  return os ;
}


// Description:
//     Lecture d'un schema en temps a partir d'un flot d'entree.
//     Le format de lecture attendu est le suivant:
//     {
//      [Motcle valeur_reelle]
//     }
//     Les mots clefs peuvent etre:
//     tinit, tmax, nb_pas_dt_max, dt_min, dt_max,
//     dt_sauv, dt_impr, facsec, seuil_statio,
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: motclef inconnu a cet endroit
// Effets de bord:
// Postcondition: le schema en temps est lu
Entree& Schema_Temps_base::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" time scheme"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  if (dt_min_==0)
    {
      Cerr<<"dt_min has not been read or is set to 0."<<finl;
      Cerr<<"Assign a value strictly positive to dt_min."<<finl;
      exit();
    }
  temps_courant_=tinit_;
  temps_precedent_=tinit_;
  lu_=1;
  Cerr << "The next backup, by security, will take place after " << limite_cpu_sans_sauvegarde_/3600 << " hours of calculation." << finl;
  return is ;
}

int Schema_Temps_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="dt_start")
    {
      is>>motlu;
      Motcles les_mots2(4);
      les_mots2[0]="dt_std";
      les_mots2[1]="dt_min";
      les_mots2[2]="dt_calc";
      les_mots2[3]="dt_fixe";
      int rang2=les_mots2.search(motlu);
      switch(rang2)
        {
        case 0:
        case 1:
        case 2:
          {
            mode_dt_start_=-rang2;
            break;
          }
        case 3:
          {
            is >>mode_dt_start_;
            break;
          }
        default:
          {
            Cerr<<" We do not understand "<<motlu <<"in Schema_Temps_base::lire_motcle_non_standard"<<finl;
            Cerr<<"keywords understood "<<les_mots2<<finl;
            exit();
          }
        }
      return 1;
    }
  else if (mot=="nb_pas_dt_max")
    lire_nb_pas_dt_max(is);
  else if (mot=="periode_sauvegarde_securite_en_heures")
    lire_periode_sauvegarde_securite_en_heures(is);
  else if (mot=="tcpumax")
    lire_temps_cpu_max(is);
  else if (mot=="no_check_disk_space")
    file_allocation_=0;
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}

// Description
// Lecture du nombre de pas de temps maximal
Entree& Schema_Temps_base::lire_nb_pas_dt_max(Entree& is)
{
  if (Process::je_suis_maitre())
    Cerr << "Reading of the maximum number of time steps"  << finl;
  is >> nb_pas_dt_max_;
  return is;
}

Entree& Schema_Temps_base::lire_periode_sauvegarde_securite_en_heures(Entree& is)
{
  if (Process::je_suis_maitre())
    Cerr << "Reading of the safety backup period in hours"  << finl;
  is >> periode_cpu_sans_sauvegarde_;
  periode_cpu_sans_sauvegarde_*=3600; // Conversion en secondes
  limite_cpu_sans_sauvegarde_ = periode_cpu_sans_sauvegarde_;
  return is;
}


Entree& Schema_Temps_base::lire_temps_cpu_max(Entree& is)
{
  if (Process::je_suis_maitre())
    Cerr << "Reading the max cpu time allowed"  << finl;
  is >> tcpumax_;
  tcpumax_*=3600; // Conversion en secondes
  return is;
}

// Description:
//    Constructeur par defaut d'un schema en temps.
//    Initialise differents membres de la classe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet est initialise
Schema_Temps_base::Schema_Temps_base()
{
  lu_=0;
  dt_ = 0;
  tinit_ = -DMAXFLOAT;
  dt_min_ = 1.e-16;
  dt_sauv_ = dt_impr_ = tmax_ = dt_max_ = tcpumax_ = 1.e30;
  periode_cpu_sans_sauvegarde_ = 23 * 3600; // Par defaut 23 heures
  limite_cpu_sans_sauvegarde_ = periode_cpu_sans_sauvegarde_;
  temps_cpu_ecoule_ = 0;
  precision_impr_ = 3;
  stationnaire_atteint_=0;
  stationnaires_atteints_=0;
  residu_=0;
  mode_dt_start_=-2; // Nouveau 1.5.6 : desormais le premier pas de temps est calcule et ne vaut donc plus dt_min (par defaut, donc est equivalent a dt_start dt_calc)
  nb_pas_dt_ = 0;
  nb_impr_ = 0;
  nb_pas_dt_max_ = (int)(pow(2.0,(double)((sizeof(True_int)*8)-1))-1);
  seuil_statio_ = 1.e-12;
  seuil_statio_relatif_deconseille_ = 0;
  facsec_ = 1.;
  ind_tps_final_atteint=0;
  ind_nb_pas_dt_max_atteint=0;
  ind_temps_cpu_max_atteint=0;
  ind_diff_impl_=0 ;
  seuil_diff_impl_=1.e-6 ;
  impr_diff_impl_=0;
  no_conv_subiteration_diff_impl_=0;
  no_error_if_not_converged_diff_impl_=0;
  niter_max_diff_impl_ = 1000; // Above 100 iterations, diffusion implicit algorithm is may be diverging
  schema_impr_=1;
  file_allocation_=0; // Desactive car pose probleme sur platine sur les gros maillages
}
// Description:
//    Impression du numero du pas de temps, la valeur du pas de temps.
//    et du temps courant.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/osrtie
// Retour: int
//    Signification: renvoie toujours 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Schema_Temps_base::impr(Sortie& os) const
{
  os << finl;
  os << "-------------------------------------------------------------------" << finl;
  os << "We treat the time step number "<< nb_pas_dt_ << " , for the time scheme ..." << finl
     << "   dt = " << dt_ << finl
     << "   time = " << temps_courant_ << finl;
  return 0;
}

// Description:
//    Impression du numero du pas de temps, la valeur du pas de temps.
//    et du temps courant.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Schema_Temps_base::impr(Sortie& os,Probleme_base& pb) const
{
  os << finl;
  os << " Problem : " << pb.le_nom() << finl;
  os << "We treat the time step number "<< nb_pas_dt_ << finl
     << "   dt = " << dt_ << finl
     << "   time = " << temps_courant_ << finl
     << "   time scheme : " << pb.schema_temps() << finl;
  return 0;
}
int Schema_Temps_base::impr(Sortie& os,const Probleme_base& pb) const
{
  os << finl;
  os << " Problem : " << pb.le_nom() << finl;
  os << "We treat the time step number "<< nb_pas_dt_ << finl
     << "   dt = " << dt_ << finl
     << "   time = " << temps_courant_ << finl
     << "   time scheme : " << pb.schema_temps() << finl;
  return 0;
}

extern "C" {
  int ccc_tremain(double*);
}
// Description:
//    Mise a jour du temps courant (t+=dt)
//    et du nombre de pas de temps effectue (nb_pas_dt_++).
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: retourne toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Schema_Temps_base::mettre_a_jour()
{
  temps_precedent_ = temps_courant_;
  temps_courant_ += dt_;
  nb_pas_dt_++;
  // Compute next time step stability:
  mettre_a_jour_dt_stab();
  assert_parallel(dt_stab_);
  assert_parallel(temps_courant_);
  if (!ind_tps_final_atteint)
    ind_tps_final_atteint = (temps_courant_ >= tmax_);

  if (!ind_nb_pas_dt_max_atteint)
    ind_nb_pas_dt_max_atteint = (nb_pas_dt_ >= nb_pas_dt_max_);

  if (!ind_temps_cpu_max_atteint)
    ind_temps_cpu_max_atteint = (temps_cpu_ecoule_ >= tcpumax_);

  // On incremente limite_cpu_sans_sauvegarde_
  if ( temps_cpu_ecoule_ > limite_cpu_sans_sauvegarde_ )
    {
      limite_cpu_sans_sauvegarde_ += periode_cpu_sans_sauvegarde_;
      //Finalement, on double la limite, ainsi par defaut sauvegarde de securite 10h, 20h, 40h, 80h, 160h,....
      //limite_cpu_sans_sauvegarde_ = 2 * limite_cpu_sans_sauvegarde_;
      Cerr << "The next backup, by security, will take place after " << limite_cpu_sans_sauvegarde_/3600 << " hours of calculation." << finl;
    }
  temps_cpu_ecoule_ = statistiques().last_time(temps_total_execution_counter_);

#ifdef LIBCCC_USER
  if (je_suis_maitre() && limpr())
    {
      Cout << "[CCRT] ";
      double second_remain;
      int error = ccc_tremain(&second_remain);
      if(!error)
        {
          int hour_remain = (int)(second_remain/3600);
          second_remain-=hour_remain*3600;
          int minute_remain = (int)(second_remain/60);
          second_remain-=minute_remain*60;
          Cout << hour_remain <<"h"<<minute_remain<<"mn"<<second_remain<<"s before job is killed on CCRT." << finl;
        }
      else
        Cout << "Error." << finl;
    }
#endif
  return 1;
}
// Fait une sauvegarde de protection sur des runs de 24h sur machines du CCRT
int Schema_Temps_base::lsauv() const
{
  if ( temps_cpu_ecoule_ > limite_cpu_sans_sauvegarde_ )
    {
      Cerr << "After these " << limite_cpu_sans_sauvegarde_/3600 << " hours of calculation, a backup will be made by security." << finl;
      return 1;
    }
  else
    {
      if (dt_sauv_<=dt_)
        return 1;
      else
        {
          // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
          double i, j, epsilon = 1.e-8;
          modf(temps_courant_/dt_sauv_ + epsilon, &i);
          modf(temps_precedent_/dt_sauv_ + epsilon, &j);
          return ( i>j ) ;
        }
    }
  return 0;
}

void Schema_Temps_base::mettre_a_jour_dt_stab()
{
  imprimer(Cout);
  dt_stab_=pb_base().calculer_pas_de_temps();
}
// Description:
//    Imprime le pas de temps sur un flot de sortie s'il y a lieu.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Schema_Temps_base::imprimer(Sortie& os) const
{
  if (limpr())
    {
      nb_impr_++;
      if (je_suis_maitre() && schema_impr())
        {
          impr(os);
        }
    }
}

// Description:
//    Imprime le pas de temps sur un flot de sortie s'il y a lieu.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Schema_Temps_base::imprimer(Sortie& os, Probleme_base& pb) const
{
  if (limpr() && je_suis_maitre())
    impr(os,pb);
}
void Schema_Temps_base::imprimer(Sortie& os, const Probleme_base& pb) const
{
  if (limpr() && je_suis_maitre())
    impr(os,pb);
}

// Description:
//    Sauvegarde le temps courant et le nombre de pas de temps
//    sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour la sauvegarde
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Schema_Temps_base::sauvegarder(Sortie& os) const
{
  return 1;
}

// Description:
//    Reprise (lecture) du temps courant et du nombre de pas de
//    temps effectues a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Schema_Temps_base::reprendre(Entree& is)
{
  return 1;
}

// Description:
//    Renvoie 1 si le fichier (d'extension) .stop contient un 1
//    Renvoie 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le fichier (d'extension) .stop contient 1, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Schema_Temps_base::stop_lu() const
{
  int stop_lu_l = 0;
  Nom nomfic(nom_du_cas());
  nomfic+=".stop";
  if (nb_pas_dt_ < 1)
    {
      if(je_suis_maitre())
        {
          SFichier ficstop(nomfic);
          ficstop << stop_lu_l;
        }
    }
  else
    {
      LecFicDiffuse ficstop(nomfic);
      // On verifie s'il reste de la place
      // sur le disque en fonction de la
      // taille du probleme estimee
      //int ram=
#ifdef HPPA
      Nom command("bdf");
#else
      Nom command("df -k");
#endif
      // Essayer avec stat -f -t . | awk '{print 4*$8}' ? Ca semble plus rapide...
      // Mieux : stat -f -t . | cut -d " " -f8
      command+=" . | awk '/\\// && /%/ {print $(NF-2)\" Kbytes available sur \" $NF}'";
      //Cerr << system(command) << finl;
      ficstop >> stop_lu_l;
    }
  return stop_lu_l;
}

// Description:
//    Corrige le pas de temps calcule que l'on passe en parametre
//    et verifie qu'il n'est pas "trop" petit (< dt-min_).
//    La correction est la suivante:
//       delta_t = min((facteur de securite * dt_calc), dt_max)
//    Et on verifie que delta_t est "suffisamment" plus grand que dt_min_.
// Precondition:
// Parametre: double& dt_calc
//    Signification: le pas de temps calcule a verifier
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception: le pas de temps calcule est inferieur a dt_min
// Effets de bord:
// Postcondition:
bool Schema_Temps_base::corriger_dt_calcule(double& dt_calc) const
{
  // Print the RAM
  if (limpr())
    {
      Cout << finl;
      imprimer_ram_totale();
    }
  // Compute the time step dt as the minimal value between dt_max_ and stability time step (dt_stab) * security factor (facsec)
  double dt = min (dt_max_, dt_stab_ * facsec_);
  if ((dt - dt_min_)/(dt+DMINFLOAT) < -1.e-6)
    {
      // Calculation stops if time step dt is less than dt_min
      Cerr << "---------------------------------------------------------" << finl;
      Cerr << "Problem with the time step " << dt << " which is less than dt_min " << dt_min_ << finl;
      Cerr << "Lower dt_min value or check why the time step decreases..." << finl;
      Cerr << "Results are saved to provide help." << finl;
      Cerr << "---------------------------------------------------------" << finl;
      Probleme_base& pb = ref_cast_non_const(Probleme_base, mon_probleme.valeur());
      pb.postraiter(1);
      pb.sauver();
      Process::exit();
    }
  dt_calc=dt;
  return 1;
}


// Description:
//    Renvoie 1 si il y lieu de stopper le calcul pour differente raisons:
//        - le temps final est atteint
//        - le nombre de pas de temps maximum est depasse
//        - l'etat stationnaire est atteint
//        - indicateur d'arret fichier (voir int Schema_Temps_base::stop_lu())
//    Renvoie 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si il y a lieu de stopper le calcul 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Schema_Temps_base::stop() const
{
  if (temps_final_atteint())
    {
      Cerr << "---------------------------------------------------------"
           << finl
           << "The problem " << pb_base().le_nom()
           << " wants to stop : final time reached"
           << finl << finl;
      return 1;
    }

  if ((nb_pas_dt_max_atteint()) || nb_pas_dt_max_==0)
    {
      Cerr << "---------------------------------------------------------"
           << finl
           << "The problem " << pb_base().le_nom()
           << " wants to stop : the maximum number of time steps reached"
           << finl << finl;
      return 1;
    }

  if (temps_cpu_max_atteint())
    {
      Cerr << "---------------------------------------------------------"
           << finl
           << "The problem " << pb_base().le_nom()
           << " wants to stop : max cpu time reached"
           << finl << finl;
      return 1;
    }

  if (stop_lu())
    {
      Cerr << "---------------------------------------------------------"
           << finl
           << "The problem " << pb_base().le_nom()
           << " wants to stop : stop file detected"
           << finl << finl;
      return 1;
    }
  return 0;
}


Probleme_base& Schema_Temps_base::pb_base()
{
  return mon_probleme.valeur();
}

const Probleme_base& Schema_Temps_base::pb_base() const
{
  return mon_probleme.valeur();
}

void Schema_Temps_base::ajouter_inertie(Matrice_Base& mat_morse,DoubleTab& secmem,const Equation_base& eqn) const
{
  // codage pour euler_implicite
  double dt=pas_de_temps();
  // On ne penalise pas les matrices et les secmems meme dans les cas
  // dirichlet , symetrie
  int pen=0;
  eqn.solv_masse().ajouter_masse(dt,secmem,eqn.inconnue().passe(),pen);
  eqn.solv_masse().ajouter_masse(dt,mat_morse,pen);
}

// Description:
// //Actualisation de stationnaire_atteint_ et residu_
// (critere residu_<seuil_statio_)
// Precondition:
// Parametre: double& t
//    Signification: le nouveau temps courant
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Schema_Temps_base::update_critere_statio(const DoubleTab& tab_critere, Equation_base& equation)
{
  DoubleVect& residu_equation = equation.get_residu();
  // En fonction de la taille du tableau residu_equation
  // on prend le max de tab_critere sur tout le tableau ou par colonne
  int size = residu_equation.size_array();
  if (size==0)
    {
      Cerr << "Error in Schema_Temps_base::update_critere_statio" << finl;
      Cerr << "Array residu_equation has a null size" << finl;
      for (int i=0; i<pb_base().nombre_d_equations(); i++)
        if (pb_base().equation(i).get_residu().size_array()==0)
          {
            Cerr << "The equation of kind " << pb_base().equation(i).que_suis_je() << " didn't size the array." << finl;
            Cerr << "Please, contact TRUST support." << finl;
            exit();
          }
    }
  else if (size==1)
    residu_equation(0) = mp_max_abs_vect(tab_critere);
  else
    mp_max_abs_tab(tab_critere, residu_equation);

  // On calcule le residu_initial_equation sur les 5 premiers pas de temps
  if (seuil_statio_relatif_deconseille_)
    {
      DoubleVect& residu_initial_equation = equation.residu_initial();
      if (nb_pas_dt()<6)
        {
          for (int i=0; i<size; i++)
            residu_initial_equation(i) = residu_equation(i);
          // On ne prend plus le max car celui ci est parfois grand au premier pas de temps:
          // residu_initial_equation(i) = max(residu_initial_equation(i), residu_equation(i));
        }
      // On normalise residu_equation par residu_initial_equation
      for (int i=0; i<size; i++)
        if (residu_initial_equation(i)>0)
          residu_equation(i) /= residu_initial_equation(i);
    }
  //double equation_residual = mp_max_abs_vect(residu_equation);
  double equation_residual = local_max_abs_vect(residu_equation);
  residu_ = max(residu_,equation_residual);
  set_stationnaire_atteint() *= ( equation_residual < seuil_statio_ );
}

// Impression du temps courant en tenant compte du dt
// pour la precision de l'impression (utilise dans les operateurs)
void Schema_Temps_base::imprimer_temps_courant(SFichier& os) const
{
  int precision_actuelle=os.get_precision();
  int precision_temps=max(precision_actuelle,(int)(2+log10(1/pas_de_temps())+(int)(log10(temps_courant()))));
  os.precision(precision_temps);
  os << temps_courant();
  os.precision(precision_actuelle);
}
