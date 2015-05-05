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
// File:        Execute_parallel.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#include <Execute_parallel.h>
#include <PE_Groups.h>
#include <Vect_Deriv_Comm_Group.h>
#include <LecFicDiffuse_JDD.h>
#include <Journal.h>
#include <Interprete_bloc.h>
#include <Param.h>
#include <TriouError.h>

Implemente_instanciable(Execute_parallel,"Execute_parallel",Interprete);

Entree& Execute_parallel::readOn(Entree& is)
{
  Cerr << "Execute_parallel::readOn() not coded" << finl;
  exit();
  return is;
}

Sortie& Execute_parallel::printOn(Sortie& os) const
{
  Cerr << "Execute_parallel::printOn() not coded" << finl;
  exit();
  return os;
}

// Description:
//  Cree une partition des nproc processeurs du calcul pour interpreter
//  N jeux de donnees differents. La syntaxe du jeu de donnees est la suivante
//  Execute_parallel {
//    liste_cas N cas1 cas2 cas3 ...
//    [ nb_procs N nproc1 nproc2 nproc3 ... ]
//  }
//  "cas1" est le nom du cas (on lit le fichier cas1.data sur disque)
//  nproc1 est le nombre de processeurs a utiliser pour ce cas
//  Par defaut, on utilise 1 processeur pour chaque cas
//  Les sorties Cerr et Cout sont redirigees vers le journal du processeur
//  maitre de chaque cas.
Entree& Execute_parallel::interpreter(Entree& is)
{
  Cerr << "Execute_parallel::interpreter to run several cases:" << finl;

  Noms liste_cas;
  ArrOfInt nb_procs;

  int disable_journal = 0;

  Param param(que_suis_je());
  param.ajouter("liste_cas", &liste_cas, Param::REQUIRED);
  param.ajouter("nb_procs", &nb_procs);
  param.ajouter_flag("disable_journal", &disable_journal);
  param.lire_avec_accolades_depuis(is);
  // Si on n'a pas donne nb_procs, on suppose que ca vaut 1
  const int n_calculs = liste_cas.size();
  if (nb_procs.size_array() == 0)
    {
      Cerr << "Nb_procs not given, we assume that calculations are sequential."
           << finl;
      nb_procs.resize_array(n_calculs);
      nb_procs = 1;
    }
  // Verification de la taille des tableaux
  if (nb_procs.size_array() != n_calculs)
    {
      Cerr << "Error : nb_procs array must have " << n_calculs
           << " values." << finl;
      barrier();
      exit();
    }
  if (n_calculs == 0)
    return is;

  // Verification du contenu :
  if (min_array(nb_procs) < 1)
    {
      Cerr << "Error : processor numbers must be >= 1" << finl;
      barrier();
      exit();
    }
  int count = 0;
  for (int i = 0; i < n_calculs; i++)
    count += nb_procs[i];
  if (count > nproc())
    {
      Cerr << "Error : computations require " << count << " processors." << finl;
      Cerr << "but only " << nproc() << " processors has been asked." << finl;
      barrier();
      exit();
    }

  // Creation des N groupes de processeurs
  // (les groupes sont detruits quand le VECT est detruit)
  VECT(DERIV(Comm_Group)) groupes(n_calculs);
  count = 0;
  Nom log_courant("");
  for (int i = 0; i < n_calculs; i++)
    {
      const int n = nb_procs[i];
      Nom log("");
      Nom log1(Objet_U::nom_du_cas());
      log1+="_";
      char s[20];
      sprintf(s, "%05d", count);
      log1+=s;
      log1+=".log";
      if (n==1)
        {
          log+=log1;
          log+=" file";
        }
      else
        {
          Nom log2(Objet_U::nom_du_cas());
          log2+="_";
          char s2[20];
          sprintf(s2, "%05d", count+n-1);
          log2+=s2;
          log2+=".log";
          log="log files from ";
          log+=log1;
          log+=" to ";
          log+=log2;
        }
      Cerr << "Error and standard outputs are redirected into " << log << " for case " << liste_cas[i] << finl;
      // On stocke dans log_courant car reutilise plus loin
      if (Process::me()>=count && Process::me()<=count+n-1)
        log_courant=log;

      ArrOfInt tab(n);
      for (int j = 0; j < n; j++)
        tab[j] = count++;
      PE_Groups::create_group(tab, groupes[i]);
    }
  // On lancer une exception si un calcul s'arrete donc
  // on change le comportement de Process par defaut (MPI_Abort)
  Process::exception_sur_exit=1;
  Cerr << n_calculs << " cases are running..." << finl;
  // Chaque processeur entre dans son groupe et interprete le jeu de donnees
  Nom ancien_nom_du_cas(nom_du_cas());
  const int old_journal_level = get_journal_level();
  for (int i = 0; i < n_calculs; i++)
    {
      if (PE_Groups::enter_group(groupes[i].valeur()))
        {
          set_Cerr_to_journal(1);
          if (disable_journal)
            change_journal_level(0);
          Nom nom_fichier(liste_cas[i]);
          get_set_nom_du_cas() = nom_fichier;
          Journal(1) << "Execute_parallel: Entering subgroup " << i
                     << " to run case " << nom_fichier << finl;

          nom_fichier += ".data";
          {
            // Ouverture du fichier (on cree l'objet LecFicDiffuse a l'interieur
            // de l'accolade pour detruire le fichier avant de sortir du groupe)
            LecFicDiffuse_JDD data_file(nom_fichier);
            data_file.set_check_types(1);
            // On cree un nouvel interprete. A la fin de la lecture du cas
            // les objets seront detruits.
            Interprete_bloc interp;
            // On utilise les exceptions pour des calculs qui s'arretent
            int ok=1;
            try
              {
                interp.interpreter_bloc(data_file,
                                        Interprete_bloc::FIN /* on attend FIN a la fin du fichier */,
                                        0 /* verifie_sans_interpreter=0 */);
              }
            catch (TriouError err)
              {
                assert(err.get_pe()==Process::me());
                ok=0;
              }
            set_Cerr_to_journal(0);
            if (ok)
              Cerr << "Case " << liste_cas[i] << " has finished. See " << log_courant << finl;
            else
              Cerr << "!!! Case " << liste_cas[i] << " has failed. See " << log_courant << " !!!" << finl;
          }
          Journal(1) << "Execute_parallel: Exiting subgroup " << i << finl;
          PE_Groups::exit_group();
        }
    }
  // on revient au comportement standard de trio
  Process::exception_sur_exit=0;
  change_journal_level(old_journal_level);
  // On attend que tous les processeurs aient fini d'executer leur calcul.
  barrier();
  get_set_nom_du_cas() = ancien_nom_du_cas;
  Cerr << finl << "End of Execute_parallel::interpreter" << finl;
  return is;
}
