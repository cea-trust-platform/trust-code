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
// File:        stat_counters.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////
#include <stat_counters.h>
#include <math.h>
#include <EcrFicCollecte.h>
#include <Nom.h>
#include <Double.h>
// Declaration des identifiants de compteurs

Stat_Counter_Id mpi_sendrecv_counter_;
Stat_Counter_Id mpi_send_counter_;
Stat_Counter_Id mpi_recv_counter_;
Stat_Counter_Id mpi_bcast_counter_;
Stat_Counter_Id mpi_alltoall_counter_;
Stat_Counter_Id mpi_partialsum_counter_;
Stat_Counter_Id mpi_sendrecv_io_counter_;
Stat_Counter_Id mpi_sumdouble_counter_;
Stat_Counter_Id mpi_mindouble_counter_;
Stat_Counter_Id mpi_maxdouble_counter_;
Stat_Counter_Id mpi_sumint_counter_;
Stat_Counter_Id mpi_minint_counter_;
Stat_Counter_Id mpi_maxint_counter_;
Stat_Counter_Id mpi_barrier_counter_;

Stat_Counter_Id echange_vect_counter_;

Stat_Counter_Id solv_sys_counter_;
Stat_Counter_Id solv_sys_petsc_counter_;
Stat_Counter_Id diffusion_implicite_counter_;

Stat_Counter_Id nut_counter_;
Stat_Counter_Id dt_counter_;
Stat_Counter_Id convection_counter_;
Stat_Counter_Id diffusion_counter_;
Stat_Counter_Id decay_counter_;
Stat_Counter_Id gradient_counter_;
Stat_Counter_Id divergence_counter_;
Stat_Counter_Id source_counter_;
Stat_Counter_Id postraitement_counter_;
Stat_Counter_Id divers_counter_;
Stat_Counter_Id sauvegarde_counter_;
Stat_Counter_Id temporary_counter_;

Stat_Counter_Id assemblage_sys_counter_;
Stat_Counter_Id timestep_counter_;
Stat_Counter_Id interprete_scatter_counter_;
Stat_Counter_Id temps_total_execution_counter_;
Stat_Counter_Id initialisation_calcul_counter_;

Stat_Counter_Id m1;
Stat_Counter_Id m2;
Stat_Counter_Id m3;
// Initialisation des differents compteurs.
// L'ordre d'impression des compteurs est le meme que l'ordre de creation.

// SVP : mettez une courte description de ce que mesure exactement
//       le compteur (quelles operations) et quelle est la signification
//       de la quantite sommee (quand elle n'est pas nulle).
void declare_stat_counters()
{
  // Creation de l'objet statistiques
  if (les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe)
    {
      cerr << "Error : declare_stat_counters() has already been called\n";
      assert(0);
      return;
    }
  les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe
    = new Statistiques;

  // Niveau de debug par defaut
  statistiques().set_debug_level(9);

  // Temps total d'execution
  // Ce compteur est declare en premier (on calcule les pourcentages)
  // par rapport a celui-la dans Statistiques::dump()
  temps_total_execution_counter_ = statistiques().new_counter(1, "Temps total", 0);

  // Initialisation du calcul (jusqu'a l'entree dans Resoudre)
  initialisation_calcul_counter_ = statistiques().new_counter(1, "Preparer calcul", 0);

  // Boucle de calcul du pas de temps dans Resoudre::interpreter
  timestep_counter_ = statistiques().new_counter(1, "Resoudre (timestep loop)", 0);

  // Appels a SolveurSys::resoudre_systeme
  // quantity = nombre total d'iterations des solveurs
  solv_sys_counter_ = statistiques().new_counter(1, "SolveurSys::resoudre_systeme", 0);
  solv_sys_petsc_counter_ = statistiques().new_counter(1, "Solveurpetsc::resoudre_systeme", 0);
  diffusion_implicite_counter_ = statistiques().new_counter(1, "Equation_base::Gradient_conjugue_diff_impl", 0);

  // Appels a differents operateurs:
  dt_counter_ = statistiques().new_counter(1, "Operateur::calculer_pas_de_temps", 0);
  nut_counter_ = statistiques().new_counter(1, "ModeleTurbulence*::mettre_a_jour", 0);
  convection_counter_ = statistiques().new_counter(1, "Operateur_Conv::ajouter/calculer", 0);
  diffusion_counter_ = statistiques().new_counter(1, "Operateur_Diff::ajouter/calculer", 0);
  decay_counter_ = statistiques().new_counter(1, "Operateur_Decr::ajouter/calculer", 0);
  gradient_counter_ = statistiques().new_counter(1, "Operateur_Grad::ajouter/calculer", 0);
  divergence_counter_ = statistiques().new_counter(1, "Operateur_Div::ajouter/calculer", 0);
  source_counter_ = statistiques().new_counter(1, "Source::ajouter/calculer", 0);

  // Postraitement
  postraitement_counter_ = statistiques().new_counter(1, "Pb_base::postraiter", 0);
  sauvegarde_counter_ = statistiques().new_counter(1, "Probleme_base::sauver", 0);
  temporary_counter_ = statistiques().new_counter(1, "temporary", 0);

  // Assemblage de la matrice
  assemblage_sys_counter_ = statistiques().new_counter(1, "Assembleur::assembler", 0);

  // Divers
  divers_counter_ = statistiques().new_counter(1, "Divers", 0);

  // Marqueurs pouvant etre places n'importe ou:
  m1 = statistiques().new_counter(1, "m1", 0);
  m2 = statistiques().new_counter(1, "m2", 0);
  m3 = statistiques().new_counter(1, "m3", 0);

  // Appels a DoubleVect::echange_espace_virtuel()
  echange_vect_counter_ = statistiques().new_counter(2, "DoubleVect/IntVect::echange_espace_virtuel", 0);

  mpi_sendrecv_counter_  = statistiques().new_counter(2, "MPI_send_recv", "MPI_sendrecv");
  mpi_send_counter_      = statistiques().new_counter(2, "MPI_send",      "MPI_sendrecv");
  mpi_recv_counter_      = statistiques().new_counter(2, "MPI_recv",      "MPI_sendrecv");
  mpi_bcast_counter_     = statistiques().new_counter(2, "MPI_broadcast", "MPI_sendrecv");
  mpi_alltoall_counter_  = statistiques().new_counter(2, "MPI_alltoall",  "MPI_sendrecv");
  mpi_partialsum_counter_= statistiques().new_counter(2, "MPI_partialsum","MPI_allreduce");
  mpi_sumdouble_counter_ = statistiques().new_counter(2, "MPI_sumdouble", "MPI_allreduce");
  mpi_mindouble_counter_ = statistiques().new_counter(2, "MPI_mindouble", "MPI_allreduce");
  mpi_maxdouble_counter_ = statistiques().new_counter(2, "MPI_maxdouble", "MPI_allreduce");
  mpi_sumint_counter_    = statistiques().new_counter(2, "MPI_sumint",    "MPI_allreduce");
  mpi_minint_counter_    = statistiques().new_counter(2, "MPI_minint",    "MPI_allreduce");
  mpi_maxint_counter_    = statistiques().new_counter(2, "MPI_maxint",    "MPI_allreduce");
  mpi_barrier_counter_   = statistiques().new_counter(2, "MPI_barrier",   "MPI_allreduce");

  // Compte le temps de communication dans *_Fic_Par*
  // nombre = approx le nombre de synchro fichiers
  // quantity = nombre d'octets recus + nombre d'octets envoyes
  mpi_sendrecv_io_counter_ = statistiques().new_counter(2, "MPI_send_recv_io", "io");

  // Execution de Scatter::interpreter
  interprete_scatter_counter_ = statistiques().new_counter(2, "Scatter", 0);
}

void end_stat_counters()
{
  delete  les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe;
  les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe=0;
}
// Estimation du temps de latence MPI_allreduce
static double estimate_allreduce(double max_bench_time)
{
  Process::barrier();
  double t1 = Statistiques::get_time_now();
  double t2 = t1;
  int howmany = 100;
  int i = 0;
  for (i = 0; i < howmany; i++)
    Process::mp_sum((int)1);
  t2 = Statistiques::get_time_now();
  t2 = Process::mp_min(t2-t1);
  return t2 / (double) howmany;
}

inline void write_stat_file(const Nom& msg, const Stat_Results& stat, const Stat_Results& pas_de_temps, SFichier& stat_file)
{
  if (stat.max_time!=0)
    {
      stat_file << "Dont " << msg << " ";
      stat_file << stat.max_time/pas_de_temps.max_count;
      stat_file << " " << int(stat.max_time/pas_de_temps.max_time*100) << "%";
      double n = stat.max_count/pas_de_temps.max_count;
      if (stat.max_count>0) stat_file << " (" << n << " " << (n==1?"appel":"appels") << "/pas de temps)";
      stat_file << "\n";
    }
}

void print_statistics_analyse(const char * message, int mode_append)
{
  assert(message);

  Stat_Results temps_total;
  Stat_Results pas_de_temps;
  Stat_Results solveur;
  Stat_Results solveur_petsc;
  Stat_Results diffusion_implicite;
  Stat_Results dt;
  Stat_Results nut;
  Stat_Results convection;
  Stat_Results diffusion;
  Stat_Results decay;
  Stat_Results gradient;
  Stat_Results divergence;
  Stat_Results source;
  Stat_Results postraitement;
  Stat_Results sauvegarde;
  Stat_Results temporary;
  Stat_Results divers;
  Stat_Results assemblage;
  Stat_Results echange_espace_virtuel;
  Stat_Results comm_sendrecv;
  Stat_Results comm_allreduce;
  Stat_Results sendrecv;
  Stat_Results marqueur1;
  Stat_Results marqueur2;
  Stat_Results marqueur3;

  // Stop the counters
  statistiques().stop_counters();

  // Build the filename to print info:
  Nom TU(Objet_U::nom_du_cas());
  TU+=".TU";

  // Print minimal infos:
  statistiques().get_stats(temps_total_execution_counter_, temps_total);
  statistiques().get_stats(timestep_counter_, pas_de_temps);
  if (Process::je_suis_maitre())
    {
      // Ouverture du fichier principal (pour tous les processeurs)
      SFichier stat_file(TU, mode_append ? (ios::out | ios::app) : (ios::out));

      stat_file << message << "\n\n";
      stat_file << "Temps total                       " << temps_total.max_time << "\n";
      stat_file << finl;
    }

  // If more than one time step:
  if (pas_de_temps.max_count > 0)
    {
      statistiques().get_stats(solv_sys_counter_, solveur);
      statistiques().get_stats(solv_sys_petsc_counter_, solveur_petsc);
      statistiques().get_stats(diffusion_implicite_counter_, diffusion_implicite);
      statistiques().get_stats(dt_counter_, dt);
      statistiques().get_stats(nut_counter_, nut);
      statistiques().get_stats(convection_counter_, convection);
      statistiques().get_stats(diffusion_counter_, diffusion);
      statistiques().get_stats(decay_counter_, decay);
      statistiques().get_stats(gradient_counter_, gradient);
      statistiques().get_stats(divergence_counter_, divergence);
      statistiques().get_stats(source_counter_, source);
      statistiques().get_stats(postraitement_counter_, postraitement);
      statistiques().get_stats(sauvegarde_counter_, sauvegarde);
      statistiques().get_stats(temporary_counter_, temporary);
      statistiques().get_stats(assemblage_sys_counter_, assemblage);
      statistiques().get_stats(echange_vect_counter_, echange_espace_virtuel);
      statistiques().get_stats_familly("MPI_sendrecv", comm_sendrecv);
      statistiques().get_stats_familly("MPI_allreduce", comm_allreduce);
      statistiques().get_stats(m1, marqueur1);
      statistiques().get_stats(m2, marqueur2);
      statistiques().get_stats(m3, marqueur3);
      statistiques().get_stats(divers_counter_, divers);

      double allreduce_peak_perf = 0.;
      //if (temps_total.max_time > 10.)
      allreduce_peak_perf = estimate_allreduce(0.1 /* benchmark time */);

      // Estimation de la bande passante reseau (basee uniquement sur les operations
      //  send_recv_start / send_recv_finish)
      statistiques().get_stats(mpi_sendrecv_counter_, sendrecv);
      double bandwidth = 1.1e30;
      if (sendrecv.time > 0)
        bandwidth = sendrecv.quantity / (sendrecv.time+DMINFLOAT);
      double max_bandwidth = Process::mp_max(bandwidth);

      // Calcul du temps d'attente du aux synchronisations
      // On prend le temps total de communication et on retranche le temps
      // theorique calcule a partir de allreduce_peak_perf et de la bande passante maxi
      double theoric_comm_time = comm_allreduce.count * allreduce_peak_perf + comm_sendrecv.quantity / (max_bandwidth+1e-30);
      // Je suppose que le temps minimum pour realiser les communications sur un proc
      //  depend du processeur qui a le plus de donnees a envoyer:
      theoric_comm_time = Process::mp_max(theoric_comm_time);
      double wait_time = (comm_sendrecv.time + comm_allreduce.time) - theoric_comm_time;
      double wait_fraction;
      if (temps_total.time == 0)
        wait_fraction = 0.;
      else
        wait_fraction = wait_time / (temps_total.time+DMINFLOAT);
      wait_fraction = 0.1 * floor(wait_fraction * 1000);
      if (wait_fraction < 0.) wait_fraction = 0.;
      if (wait_fraction > 100.) wait_fraction = 100.;

      double max_wait_fraction = Process::mp_max(wait_fraction);
      double min_wait_fraction = Process::mp_min(wait_fraction);
      double avg_wait_fraction = Process::mp_sum(wait_fraction) / Process::nproc();

      double total_quantity = Process::mp_sum(sauvegarde.max_quantity);
      // Print into .TU file

      if (Process::je_suis_maitre())
        {
          SFichier stat_file(TU, mode_append ? (ios::out | ios::app) : (ios::out));

          stat_file << "Timesteps                         " << pas_de_temps.max_count << "\n";
          stat_file << "Secondes / pas de temps           " << pas_de_temps.max_time / pas_de_temps.max_count << "\n";
          write_stat_file("solveurs Ax=B               ", solveur               , pas_de_temps, stat_file);
          write_stat_file("solveur diffusion_implicite ", diffusion_implicite   , pas_de_temps, stat_file);
          write_stat_file("assemblage matrice_implicite", assemblage            , pas_de_temps, stat_file);
          write_stat_file("operateurs convection       ", convection            , pas_de_temps, stat_file);
          write_stat_file("operateurs diffusion        ", diffusion             , pas_de_temps, stat_file);
          write_stat_file("operateurs decroissance     ", decay                 , pas_de_temps, stat_file);
          write_stat_file("operateurs gradient         ", gradient              , pas_de_temps, stat_file);
          write_stat_file("operateurs divergence       ", divergence            , pas_de_temps, stat_file);
          write_stat_file("operateurs source           ", source                , pas_de_temps, stat_file);
          write_stat_file("operations postraitement    ", postraitement         , pas_de_temps, stat_file);
          write_stat_file("calcul dt                   ", dt                    , pas_de_temps, stat_file);
          write_stat_file("modele turbulence           ", nut                   , pas_de_temps, stat_file);
          write_stat_file("operations sauvegarde       ", sauvegarde            , pas_de_temps, stat_file);
          write_stat_file("marqueur1                   ", marqueur1             , pas_de_temps, stat_file);
          write_stat_file("marqueur2                   ", marqueur2             , pas_de_temps, stat_file);
          write_stat_file("marqueur3                   ", marqueur3             , pas_de_temps, stat_file);
          write_stat_file("calcul divers               ", divers                , pas_de_temps, stat_file);
          if (echange_espace_virtuel.max_count > 0)
            {
              stat_file << "Nb echange_espace_virtuel / pas de temps " << echange_espace_virtuel.max_count / pas_de_temps.max_count << "\n";
            }
          if (comm_allreduce.max_count > 0)
            {
              double tmp = comm_allreduce.max_count / pas_de_temps.max_count;
              stat_file << "Nb MPI_allreduce / pas de temps " << tmp << "\n";
              if (tmp>30)
                {
                  stat_file << "-----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                  stat_file << "Warning: The number of MPI_allreduce calls per time step is high. Contact TRUST support if you plan to run massive parallel calculation." << finl;
                  stat_file << "-----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                }
            }
          if (solveur.max_count > 0)
            {
              stat_file << "Nb solveur / pas de temps         " << solveur.max_count / pas_de_temps.max_count << "\n";
              double avg_time = solveur.max_time / solveur.max_count;
              stat_file << "Secondes / solveur                " << avg_time << "\n";
              stat_file << "Iterations / solveur              " << solveur.max_quantity / solveur.max_count << "\n";
            }
          if (sauvegarde.max_count > 0)
            {
              stat_file << "I/O:" << finl;
              char s[20] = "";
              stat_file << "Nb sauvegardes         : " << sauvegarde.max_count << "\n";
              stat_file << "Secondes / sauvegarde  : " << sauvegarde.max_time/sauvegarde.max_count << "\n";
              sprintf(s,"%2.2f",total_quantity/(1024*1024));
              stat_file << "Donnees ecrites [Mo]   : " << s << "\n";
              stat_file << "Debit           [Mo/s] : " << int(total_quantity/(1024*1024)/sauvegarde.max_time) << "\n";
            }
          if (comm_sendrecv.max_count > 0)
            {
              if (solveur_petsc.max_count>0)
                {
                  stat_file << "---------------------------------------------------------------------------------------------------------" << finl;
                  stat_file << "Warning: One or several PETSc solvers are used and thus the communication time below are under-estimated." << finl;
                  stat_file << "To print also the additional time spent in PETSc solvers, run the calculation with -log_summary option."   << finl;
                  stat_file << "---------------------------------------------------------------------------------------------------------" << finl;
                }
              double fraction;
              fraction = (comm_sendrecv.avg_time+comm_allreduce.avg_time) / (temps_total.max_time+0.001);
              fraction = 0.1 * floor(fraction * 1000);
              if (fraction > 100.) fraction = 100.;
              stat_file << "Communications avg        " << fraction << " % of total time\n";
              fraction = (comm_sendrecv.max_time+comm_allreduce.max_time) / (temps_total.max_time+0.001);
              fraction = 0.1 * floor(fraction * 1000);
              if (fraction > 100.) fraction = 100.;
              stat_file << "Communications max        " << fraction << " % of total time\n";
              fraction = (comm_sendrecv.min_time+comm_allreduce.min_time) / (temps_total.max_time+0.001);
              fraction = 0.1 * floor(fraction * 1000);
              stat_file << "Communications min        " << fraction << " % of total time\n";

              stat_file << "Network latency benchmark ";
              if (allreduce_peak_perf == 0.)
                stat_file << "not measured (total running time too short <10s)\n";
              else
                stat_file << allreduce_peak_perf << " s\n";

              stat_file << "Network bandwidth max     " << max_bandwidth * 1.e-6
                        << " MB/s\n";
              stat_file << "Total network traffic     "
                        << comm_sendrecv.avg_quantity * Process::nproc()
                        / pas_de_temps.max_count * 1e-6
                        << " MB / timestep\n";
              stat_file << "Average message size      "
                        << comm_sendrecv.avg_quantity / comm_sendrecv.avg_count * 1e-3
                        << " kB\n";
              stat_file << "Min waiting time          "
                        << min_wait_fraction << " % of total time\n";
              stat_file << "Max waiting time          "
                        << max_wait_fraction << " % of total time\n";
              stat_file << "Avg waiting time          "
                        << avg_wait_fraction << " % of total time\n";
            }
          stat_file << "\n";
          stat_file << "Timesteps = nombre de pas de temps\n";
          stat_file << "Nb solveur = nombre de resolutions de systeme lineaire\n";
          stat_file << "Nb assemblage implicite = nombre d'assemblage de matrice pour le schema implicite\n";
          stat_file << "Iterations = nombre moyen d'iterations du solveur\n";
          stat_file << "Communications = fraction du temps passe dans les\n";
          stat_file << "                 communications entre processeurs (hors io fichiers)\n";
          stat_file << "Network latency = temps d'un mpsum mesure par un bench interne sur 0.1s\n";
          stat_file << "Network bandwidth = maximum sur l'ensemble des processeurs\n";
          stat_file << "                    de la bande passante moyenne des operations send_recv\n";
          stat_file << "Waiting time = estimation du temps d'attente des differents processeurs\n\n";
          stat_file << "Max_waiting_time grand  => probablement mauvais decoupage\n";
          stat_file << "Communications > 30%    => trop de processeurs ou reseau trop lent\n";
          stat_file << finl;
        }
    }
  // Restart counters
  statistiques().restart_counters();
}

