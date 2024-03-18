/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Statistiques.h>
#include <EcrFicPartage.h>
#include <SChaine.h>
#include <petsc_for_kernel.h>
#include <communications.h>
#ifdef VTRACE
#include <vt_user.h>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// The first part of this file aims at giving basic functions for getting time for Windows and Unix boots.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef MICROSOFT
//struct timeval { time_t tv_usec; time_t tv_sec; };

#include <windows.h>

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Win32 timing specific function.
// Returns 0 on non-capable machines (Pentium 75 & inferior mostly)
// /////////////////////////////////////////////////////////////////////////
LARGE_INTEGER GetFrequency()
{
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return freq;
}

double  Now()
{
  LARGE_INTEGER tick, freq;
  freq = GetFrequency();
  QueryPerformanceCounter(&tick);
  return ((double)tick.QuadPart) / freq.QuadPart;
}





int gettimeofday(struct timeval *tv ,int toto)
{
  double date=Now();
  tv->tv_sec=date;
  const int MILLION = 1000000;
  tv->tv_usec=(date-tv->tv_sec)*MILLION;
  return 0;
}
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <stdio.h>
#include <TRUSTArray.h>
#include <communications.h>

#include <map>
// Le pointeur declare dans Statistiques.h
Statistiques * les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe = 0;

#define TIME_IS_gettimeofday

#ifdef TIME_IS_gettimeofday
#define GetTimeOfDay_or_rusage
#endif
#ifdef TIME_IS_getrusage
#define GetTimeOfDay_or_rusage
#endif

#define BUFLEN 100

static const double INITIAL_MIN = 1000.0;

// =================================================================
// =================================================================
// DEFINITION DE LA CLASSE TIME (interne au systeme de statistiques)
// =================================================================
// =================================================================

#ifdef GetTimeOfDay_or_rusage
// Sur un PC a 1.5Ghz, le temps d'execution de get_time est de
// 1 micro-seconde.
// getrusage n'est pas utilisable pour le grain fin: la precision
// n'est que de 0.01 s.
static const int MILLION = 1000000;
class Time
{
public:
  Time()
  {
    time_.tv_sec = 0;
    time_.tv_usec = 0;
  }
  static const char * const description;
  void get_time()
  {
#ifdef TIME_IS_gettimeofday
    gettimeofday(& time_, 0);
#else
    struct rusage ru;
    // Sous Linux, RUSAGE_BOTH ne fonctionne pas
    getrusage(RUSAGE_SELF, &ru);
    time_ = ru.ru_utime; // Temps utilisateur ecoule (non systeme)
#endif
  }
  // Calcule :
  //     time_begin = time_end - time_begin
  //     time_ += time_end - time_begin
  void add_time(Time& time_begin, const Time& time_end)
  {
    time_begin.time_.tv_sec = time_end.time_.tv_sec - time_begin.time_.tv_sec;
    time_begin.time_.tv_usec = time_end.time_.tv_usec - time_begin.time_.tv_usec;
    if (time_.tv_usec < 0)
      {
        time_begin.time_.tv_usec += MILLION;
        time_begin.time_.tv_sec--;
      }
    time_.tv_sec += time_begin.time_.tv_sec;
    time_.tv_usec += time_begin.time_.tv_usec;
    if (time_.tv_usec >= MILLION)
      {
        time_.tv_usec -= MILLION;
        time_.tv_sec++;
      }
  }
  void add(Time& time)
  {
    time_.tv_sec += time.time_.tv_sec;
    time_.tv_usec += time.time_.tv_usec;
    if (time_.tv_usec >= MILLION)
      {
        time_.tv_usec -= MILLION;
        time_.tv_sec++;
      }
  }
  void minus(Time& time)
  {
    time_.tv_sec -= time.time_.tv_sec;
    time_.tv_usec -= time.time_.tv_usec;
    if (time_.tv_usec < 0)
      {
        time_.tv_usec += MILLION;
        time_.tv_sec--;
      }
  }
  double second() const
  {
    return (double) time_.tv_sec + 0.000001 * (double) time_.tv_usec;
  }

private:
  struct timeval time_;
};
#ifdef TIME_IS_gettimeofday
const char * const Time::description = "gettimeofday (elapsed CPU time)";
#else
const char * const Time::description = "getrusage (user CPU time)";
#endif
#endif

// =================================================================
// =================================================================

#ifdef TIME_IS_pentium_hardware_clock_counter
// get_time s'execute en quelques cycles (beaucoup plus fin que
// gettimeofday). Marche uniquement sur plateforme Intel.
class Time
{
public:
  Time() : time_(0);
  { }
  Time(const Time& t) : time_(t.time_)
  { }
  static const char * const description;
  void get_time()
  {
    // On recupere le clock counter du processeur
    // (int 64 bits incremente de 1 a chaque instruction)
    __asm__ __volatile__("rdtsc\n\t" : "=A" (time_) );
  }
  void add_time(Time& time_begin, const Time& time_end)
  {
    time_begin.time_ = time_end.time_ - time_begin.time_;
    time_ += time_begin.time_;
  }
  double second()
  {
    if (seconds_per_hardware_clock == 0.)
      calibrate();
    return (double) time_ * seconds_per_hardware_clock;
  }
  void calibrate();
private:
  static double seconds_per_hardware_clock;
  unsigned long long time_; // Entier 64 bits
};

const char * const Time::description =
  "hardware clock counter (elapsed cpu time)";

double Time::seconds_per_hardware_clock = 0;

void Time::calibrate()
{
  Cerr << "Time::calibrate() for 1 second " << finl;
  timeval t_begin, now;
  Time ticks_begin, ticks_now;

  gettimeofday(& t_begin, 0);
  ticks_begin.get_time();
  unsigned int delta;
  do
    {
      gettimeofday(& now, 0);
      ticks_now.get_time();
      delta = now.tv_usec - t_begin.tv_usec;
      delta += (now.tv_sec - t_begin.tv_sec) * 1000000;
    }
  while (delta < 1000000);
  unsigned long long delta_ticks = ticks_now.time_ - ticks_begin.time_;
  seconds_per_hardware_clock = delta * 0.000001 / (double) delta_ticks;

  Cerr << " Result : hardware_clocks_per_second = ";
  Cerr << 1. / seconds_per_hardware_clock << finl;
}
#endif

/*! @brief Cette methode renvoie un temps en secondes depuis une origine qui depend de la methode de mesure de la classe Time.
 *
 * Precision, de l'ordre de 1ms maximum.
 *
 */
double Statistiques::get_time_now()
{
  Time t;
  t.get_time();
  return t.second();
}

static const int MAXCOUNTERS = 1000; ///< Maximum number of counter (static) and size of the tables in Stat_Internals

/*! @brief Interne object of the class Statistiques containing diverse informations about counters
 *
 * This is an internal object of the Statistiques class that encapsulate informations about counters. For each numerical simulation, a unique Stat_Internals is created at the start of the computation and associated with the Statistiques type object.
 *
 *
 */
class Stat_Internals
{
public:

  Stat_Internals();

  int nb_counters; ///< number of counters
  int nb_comm_counters;  //< number of communication counters
  int max_debug_level_during_session; ///< Debug level which change the quantity of output in the performance log file

  /*!
   * A set of tables of the size MAX_COUNTER is created, containing the needed information for the creation of the performance file
   */

  Time time_begin[MAXCOUNTERS]; ///< Contains the time when each counter is firstly start

  Time counter_time[MAXCOUNTERS];  ///< Contains the total CPU time for each counter

  int counter_running[MAXCOUNTERS]; ///< For each counter, is equal to 1 if the counter is still running, 0 otherwise

  unsigned long long counter_nb[MAXCOUNTERS]; ///< For each counter, count the number of time it has been called

  unsigned long long counter_quantity[MAXCOUNTERS]; ///< A user based quantity, which depends on the counter and is updated in the end_count function

  const char * description[MAXCOUNTERS]; ///< Table of the counters names

  const char * family[MAXCOUNTERS]; ///< Table of the counters family for which we want joined stats

  int counter_level[MAXCOUNTERS]; ///< Table of the counter level

  int change_level_forbidden; ///< If = 1, no change of level allowed, 0 otherwise
  int debug_level_before;  ///< Save the debug level during the printing process


  std::map<const char*, int> description_to_id;  ///< mapping between counter description and counter id

  int counter_comm[MAXCOUNTERS]; ///< Equal to 1 if it is a communication counter, 0 otherwise

  bool comm_domaines_on[MAXCOUNTERS];

  int index_in_communication_tracking_info[MAXCOUNTERS]; ///< if the counter is a communication, give its index inside the communication_tracking_info array (-1 otherwise)

  /*! @brief Table containing communication times of each wanted part of the code
   *
   * 1ere dimension : les differents types de communication
   * 2eme dimension : le temps passe dans chaque domaine  de communication qu'on a definie pour chaque compteur
   * ==> quand j different de 0 : l'element (i,j) du tableau contient le temps passe dans le domaine j par la communication i
   * ==> quand j = 0 : l'element (i,j) contient le temps total (dans tout le code) consomme par la communication i
   */

  Stat_Results** communication_tracking_info;


  /*! @brief This table contains and compute time quantities at the end of the last calculation iteration for each counter
   *
   * 1: Total time passed by in each counter (without counting the last iteration)
   * 2: Averaged time passed by for each counter on an iteration
   * 3: Minimum time passed by for each counter on an iteration
   * 4: Maximum time passed by for each counter on an iteration
   * 5: Variance of the time passed by for each counter on an iteration
   */
  double counters_avg_min_max_var_per_step[MAXCOUNTERS][5];
};
/*! @brief Constructor of the class Sat_Internals that sets everything to 0
 *
 */
Stat_Internals::Stat_Internals() :
  nb_counters(0),
  nb_comm_counters(0),
  max_debug_level_during_session(0),
  change_level_forbidden(0),
  debug_level_before(0),
  communication_tracking_info(0)
{
  for (int i = 0; i < MAXCOUNTERS; i++)
    {
      counter_running[i] = 0;
      counter_nb[i] = 0;
      counter_quantity[i] = 0;
      description[i] = 0;
      family[i] = 0;
      counter_level[i] = 0;
      counter_comm[i] = 0;
      comm_domaines_on[i] = false;
      index_in_communication_tracking_info[i] = -1;

      for(int j = 0; j < 5; j++)
        {
          if(j==2)  //temps minimum ecoule lors d'une iteration : on l'initialise par une valeur arbitrairement grande
            counters_avg_min_max_var_per_step[i][j] = INITIAL_MIN;
          else
            counters_avg_min_max_var_per_step[i][j] = 0.0;
        }
    }
}

// =========================================================================
// =========================================================================

//               IMPLEMENTATION DE LA CLASSE Statistiques
// =========================================================================
// =========================================================================
Statistiques::Statistiques()
{
  total_time_ = 0;
  debug_level_ = 0;
  three_first_steps_elapsed_ = false;
  stat_internals = new Stat_Internals();
}

Statistiques::~Statistiques()
{
  //Cerr << "Stat::~Stat called !!! " << finl;

  delete stat_internals;
  stat_internals = 0;
}
/*!@brief Function that change the debug level
 *
 * @param level the new debug level
 * During the printing of the performance, the debug level is set to 0
 */
void Statistiques::set_debug_level(int level)
{
  if (si.change_level_forbidden)
    return;

  debug_level_ = level;
  if (level > si.max_debug_level_during_session)
    si.max_debug_level_during_session = level;
}

// Voir Statistiques.h pour le commentaire...

Stat_Counter_Id Statistiques::new_counter(int level,
                                          const char * const description,
                                          const char * const family,
                                          int comm)
{
  Stat_Internals& si = *stat_internals;
  assert(description); // Pointeur non nul ?
  if (si.nb_counters >= MAXCOUNTERS)
    {
      Cerr << "Stat_Counter_Id::new_counter : too much counters." << finl;
      Cerr << " Check this : new_counter must be called only once per counter." << finl;
      Cerr << " (counter " << description << ")" << finl;
      Cerr << " Otherwise, increase MAXCOUNTERS in Statistiques.cpp." << finl;
      assert(0);
      Process::exit();
    }
  int length = (int)strlen(description);
  if (family) length += (int)strlen(family);
  if (length > BUFLEN - 10)
    {
      Cerr << "Statistiques::new_counter : description or family : string too long"
           << finl;
      Cerr << description << finl;
      assert(0);
      Process::exit();
    }

  if (level < 1)
    level = 1;

  const int id = si.nb_counters;
  Stat_Counter_Id new_id(id, level);
  si.counter_level[id] = level;
  si.description[id] = description;
  si.family[id] = family;
  si.nb_counters++;
  si.description_to_id[description] = id;

  si.counter_comm[id] = comm;
  if (comm)
    {
      si.index_in_communication_tracking_info[id] = si.nb_comm_counters;
      si.nb_comm_counters++;
    }

  return new_id;
}

void Statistiques::begin_count_(const int id)
{
  Stat_Internals& si = *stat_internals;
  assert(id < si.nb_counters);

  if(si.counter_running[id])
    {
      if (si.counter_nb[id]<=3) // Pour ne pas saturer les logs...
        Process::Journal() << "WARNING (only shown 3 times): counter " << si.description[id] << " " << (int)si.counter_nb[id]
                           << " has been started while it's already running !" << finl;
      return;
    }


  // demarrage des compteurs seulement apres les 3 premieres iterations car elles peuvent fausser les statistiques
  // (on demarre le compteur du temps total sans attendre que les 3 premiers pas de temps soient ecoules)
  bool is_temps_total = strcmp(si.description[id], "Temps total") == 0;
  if ( (JUMP_3_FIRST_STEPS && (is_temps_total || three_first_steps_elapsed_ ) )
       || !JUMP_3_FIRST_STEPS)
    {
      si.time_begin[id].get_time();
      si.counter_running[id] = 1;
#ifdef PETSCKSP_H
      Nom info(si.description[id]);
      info+="\n";
      PetscInfo(0,"%s",info.getChar());
#endif
#ifdef VTRACE
      // Level 1 only to avoid MPI calls
      if (si.counter_level[id]==1) VT_USER_START(si.description[id]);
#endif
    }

}

void Statistiques::end_count_(const int id, int quantity, int count)
{
  Stat_Internals& si = *stat_internals;
  assert(id < si.nb_counters);

  // arret des compteurs seulement apres les 3 premieres iterations (car on les demarre seulement apres 3 iterations)
  bool is_temps_total = strcmp(si.description[id], "Temps total") == 0;
  if ( (JUMP_3_FIRST_STEPS && (is_temps_total || three_first_steps_elapsed_ ) )
       || !JUMP_3_FIRST_STEPS)
    {
      if (si.counter_running[id])
        {
          Time time_end;
          time_end.get_time();
          si.counter_time[id].add_time(si.time_begin[id], time_end);
          si.counter_running[id] = 0;
          si.counter_nb[id] += count;
          si.counter_quantity[id] += quantity;
#ifdef VTRACE
          // Level 1 only to avoid MPI calls
          if (si.counter_level[id]==1) VT_USER_END(si.description[id]);
#endif
        }
    }


}

// Si le compteur n'est pas arrete, renvoie le temps depuis
// le dernier demarrage du compteur. Sinon, renvoie le dernier
// intervalle de temps mesure.
double Statistiques::last_time(const Stat_Counter_Id& counter_id)
{
  Stat_Internals& si = *stat_internals;
  if (si.counter_running[counter_id.id_])
    {
      Time time_begin(si.time_begin[counter_id.id_]);
      Time time_end;
      time_end.get_time();
      Time delta;
      delta.add_time(time_begin, time_end);
      return delta.second();
    }
  else
    {
      return si.time_begin[counter_id.id_].second();
    }
}


/// static => do not export this function ; used in dum
static void print_stat(Sortie& perfs,
                       Sortie& perfs_globales,
                       const char * message,
                       const char * description,
                       int level,
                       double time,
                       double nb,
                       double quantity,
                       double temps_total_max,
                       int skip_globals,
                       int skip_local,
                       double avg_time_per_step = 0,
                       double min_time_per_step = 0,
                       double max_time_per_step = 0,
                       double var_time_per_step = 0)
{
  char tampon[BUFLEN+200];
  if (! skip_local)
    {
  if (Process::is_parallel())
        {
          double percent_time = (temps_total_max==0 ? 0 : time / temps_total_max * 100.);

          snprintf(tampon, BUFLEN + 300, "%s \t %d \t %s \t %d \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e",
                   message, Process::me(), description, (True_int)level, time, percent_time, 0.0, 0.0, 0.0,
                   nb, 0.0, 0.0, 0.0, avg_time_per_step, min_time_per_step == INITIAL_MIN? 0.0 : min_time_per_step, max_time_per_step, var_time_per_step, quantity, 0.0 , 0.0, 0.0);
          perfs << tampon << finl;
        }
    }
  if (! skip_globals )
    {
      ArrOfDouble tmp(7);
      /// Min of each quantity on the set of processor
      tmp[0]=time;
      tmp[1]=nb;
      tmp[2]=quantity;
      tmp[3]=avg_time_per_step;
      tmp[4]=min_time_per_step == INITIAL_MIN? 0.0 : min_time_per_step;
      tmp[5]=max_time_per_step;
      tmp[6]=var_time_per_step;
      mp_min_for_each_item(tmp);

      double min_time = tmp[0];
      double min_nb = tmp[1];
      double min_quantity = tmp[2];
      double min_avg_time_per_step = tmp[3];
      double min_min_time_per_step = tmp[4];
      double min_max_time_per_step = tmp[5];
      double min_var_time_per_step = tmp[6];

      /// Max of each quantity on the set of processor
      tmp[0]=time;
      tmp[1]=nb;
      tmp[2]=quantity;
      tmp[3]=avg_time_per_step;
      tmp[4]=min_time_per_step == INITIAL_MIN? 0.0 : min_time_per_step;
      tmp[5]=max_time_per_step;
      tmp[6]=var_time_per_step;
      mp_max_for_each_item(tmp);

      double max_time = tmp[0];
      double max_nb = tmp[1];
      double max_quantity = tmp[2];
      double max_avg_time_per_step = tmp[3];
      double max_min_time_per_step = tmp[4];
      double max_max_time_per_step = tmp[5];
      double max_var_time_per_step = tmp[6];

      /// Average of each quantity on the set of processor
      tmp[0]=time;
      tmp[1]=nb;
      tmp[2]=quantity;
      tmp[3]=avg_time_per_step;
      tmp[4]=min_time_per_step == INITIAL_MIN? 0.0 : min_time_per_step;
      tmp[5]=max_time_per_step;
      tmp[6]=var_time_per_step;
      mp_sum_for_each_item(tmp);

      double avg_time = tmp[0]/ Process::nproc();
      double avg_nb = tmp[1]/ Process::nproc();
      double avg_quantity = tmp[2]/ Process::nproc();
      double avg_avg_time_per_step = tmp[3]/ Process::nproc();
      double avg_min_time_per_step = tmp[4]/ Process::nproc();
      double avg_max_time_per_step = tmp[5]/ Process::nproc();
      double avg_var_time_per_step = tmp[6]/ Process::nproc();


      /// Standard Deviation of each quantity on the set of processor

      tmp[0]=time;
      tmp[1]=nb;
      tmp[2]=quantity;
      tmp[3]=avg_time_per_step;
      tmp[4]=min_time_per_step == INITIAL_MIN? 0.0 : min_time_per_step;
      tmp[5]=max_time_per_step;
      tmp[6]=var_time_per_step;

      double * time_offset_ = new double[Process::nproc()];
      double * nb_offset_ = new double [Process::nproc()];
      double * quantity_offset_ = new double[Process::nproc()];


#ifdef MPI_
      MPI_Gather(&tmp[0], 1,MPI_DOUBLE, time_offset_, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      MPI_Gather(&tmp[1], 1,MPI_DOUBLE, nb_offset_, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      MPI_Gather(&tmp[2], 1,MPI_DOUBLE, quantity_offset_, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#endif

      if (Process::je_suis_maitre())
        {
          double percent_time = (temps_total_max==0 ? 0 : avg_time / temps_total_max * 100.);

          double SD_time = 0.0;
          double SD_nb = 0.0;
          double SD_quantity = 0.0;

          for (int proc_i =0 ; proc_i< Process::nproc(); proc_i ++)
            {
              SD_time += (time_offset_[proc_i]-avg_time)*(time_offset_[proc_i]-avg_time);
              SD_nb += (nb_offset_[proc_i]-avg_nb)*(nb_offset_[proc_i]-avg_nb);
              SD_quantity += (quantity_offset_[proc_i]-avg_quantity)*(quantity_offset_[proc_i]-avg_quantity);
            }
          SD_time = sqrt(SD_time/Process::nproc());
          SD_nb = sqrt(SD_nb/Process::nproc());
          SD_quantity = sqrt(SD_quantity/Process::nproc());

          snprintf(tampon, BUFLEN + 300, "%s \t %d \t %s \t %d \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e \t %e",
                   message, Process::me(), description, (True_int)level, time, percent_time, min_time, max_time, SD_time,
                   nb, min_nb, max_nb, SD_nb, avg_time_per_step, avg_min_time_per_step == INITIAL_MIN? 0.0 : avg_min_time_per_step, avg_max_time_per_step, avg_var_time_per_step, avg_quantity, min_quantity , max_quantity, SD_quantity);

          perfs_globales << tampon << finl;
        }
      delete[] time_offset_;
      delete[] nb_offset_;
      delete[] quantity_offset_ ;
    }
}
/// This function construct a .csv file in which counter stats are printed
void Statistiques::dump(const char * message, int mode_append)
{
  assert(message);

  Stat_Internals& si = *stat_internals;

  char tampon[BUFLEN+250];
  stop_counters();

  SChaine perfs;   ///< String that contains stats on each processor
  SChaine perfs_globales;   ///< String that contains stats average on the processors : processor number = -1
  SChaine File_header;      ///< String at the start of the file

  if ( (Process::je_suis_maitre()) && (message == "Statistiques d'initialisation du calcul") )
    {
      File_header << "# Detailed performance log file. See the associated validation form for an example of data analysis"<< finl;
      File_header << "# The time was measured by the following method :" << Time::description << finl;
      File_header << "# Processor number equal to -1 corresponds to the performance of the calculation averaged on the processors during the simulation step" << finl;
      File_header << "# If a counter does not belong in any particular family, then counter family equal to 0" << finl;
      File_header << "# Count means the number of time the counter is called during the overall calculation step." << finl;
      File_header << "# Min, max and SD accounts respectively for the minimum, maximum and Standard Deviation of the quantity of the previous row." << finl;
      File_header << "# Quantity is a custom variable that depends on the counter. It is used to compute bandwidth for communication counters for example." << finl;
      File_header << "#" << finl << "#" << finl;
      snprintf(tampon, BUFLEN + 150, "%22s \t %10s \t %30s \t %30s \t %5s \t %10s \t %10s \t %10s \t %10s \t %20s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s", "Overall simulation step", "Processor Number", "Counter family",
               "Counter name", "Counter level", "Percentage of total time", "time (s)", "min", "max", "SD", "count", "min", "max", "SD", "time_per_step", "min", "max", "SD", "Quantity", "min", "max", "SD");
      File_header << tampon << finl;
    }


  if (Process::is_parallel())
    {
      perfs.precision(5);
      perfs_globales.precision(5);
    }

  /// Check if all of the processors see the same number of counter, if not print an error message in perfs_globales
  int skip_globals = 0;
  int skip_local = 0;
  {
    int min_nb_of_counters = (int) Process::mp_min(si.nb_counters);
    int max_nb_of_counters = (int) Process::mp_max(si.nb_counters);
    if (min_nb_of_counters != max_nb_of_counters)
      {
        if (Process::je_suis_maitre())
          {
            perfs_globales << "Unable to collect statistics :" << finl
                           << " there is not the same number of counters on all"
                           " processors."<< finl;
          }
        skip_globals = 1;
      }
  }

  /// Time of reference for statistics computation
  double temps_total_max = 0.;

  /// Compute Divers (time not counted by other counters in time step loop):
  if (si.counter_nb[2]>0)
    {
      if (strcmp(si.description[2],"Resoudre (timestep loop)"))
        {
          Cerr << "Error in Statistiques::dump(...)" << finl;
          Process::exit();
        }
      int id=-1;
      /// Look for counter Divers
      for (int i = 0; i < si.nb_counters; i++)
        if (!strcmp(si.description[i],"Divers"))
          id=i;
      /// Compute CPU Divers = counter(time step) - sum(other counters))
      si.counter_time[id].add(si.counter_time[2]);
      for (int i = 3; i < si.nb_counters; i++)
        {
          if (i!=id && si.counter_level[i]==1)
            si.counter_time[id].minus(si.counter_time[i]);
        }
    }

  /// Extract information of each counter
  for (int i = 0; i < si.nb_counters; i++)
    {
      char name_description[BUFLEN];

      if (si.family[i])
        snprintf(name_description, BUFLEN, "%10s \t %10s", si.family[i], si.description[i]); ///< name_description is a string that give the family and the description of a counter
      else
        snprintf(name_description, BUFLEN, "%40d  \t %s", 0, si.description[i]); ///< if the counter does not belong in a family, family becomes 0

      double time;
      double nb; ///< number of time the counter is open and closed
      double quantity; ///< A custom quantity which depends on the counter. Used for example to compute the bandwidth

      /// Update the time statistics of communication counters using the table communication_tracking_info as if GET_COMM_DETAILS is equal to 1, the stats in stat_internals are not up to date
      if(GET_COMM_DETAILS && si.counter_comm[i] && si.communication_tracking_info )
        {
          int index =  si.index_in_communication_tracking_info[i]; ///< Finds the index associated with the counter i in the communication_tracking_info table
          time = si.communication_tracking_info[index][0].time;
          nb = si.communication_tracking_info[index][0].count;
          quantity = si.communication_tracking_info[index][0].quantity;
        }
      else
        {
          time = si.counter_time[i].second();
          nb = (double) si.counter_nb[i];
          quantity = (double) si.counter_quantity[i];
        }

      int level = si.counter_level[i];
      double avg_time_per_step = si.counters_avg_min_max_var_per_step[i][1];
      double min_time_per_step = si.counters_avg_min_max_var_per_step[i][2];
      double max_time_per_step = si.counters_avg_min_max_var_per_step[i][3];
      double var_time_per_step = si.counters_avg_min_max_var_per_step[i][4];

      if (i == 0) ///< Counter with id == 0 corresponds to the total time
        {
          temps_total_max = time;
          total_time_+=time;     ///< Update total_time_
        }

      if( JUMP_3_FIRST_STEPS && i == 2 ) ///< The first three time steps are disregard , id==2 corresponds to the counter associated with the loop that compute the time step
        temps_total_max = time;

      assert(var_time_per_step >= 0.);
      print_stat(perfs,perfs_globales,message,name_description,
                 level,time,nb,quantity,temps_total_max, skip_globals, skip_local,
                 avg_time_per_step, min_time_per_step, max_time_per_step, sqrt(var_time_per_step));

    }

  // Affichage par famille de compteur
  int * drapeaux = new int[si.nb_counters];
  for (int i = 0; i < si.nb_counters; i++)
    drapeaux[i] = 0;
  int next = 0;

  // Next designe le premier compteur d'une famille en cours
  while (next < si.nb_counters)
    {
      int level = 0;
      double time = 0.;
      double nb = 0.;
      double quantity = 0.;
      double avg_time_per_step = 0.;
      double min_time_per_step = 0.;
      double max_time_per_step = 0.;
      double var_time_per_step = 0.;


      // Trouve le premier compteur de la famille suivante
      // (famille non nulle et pas encore traitee)
      while (next < si.nb_counters
             && (si.family[next] == 0 || drapeaux[next] != 0))
        next++;
      if (next >= si.nb_counters)
        break;


      /// Aggregated stats by family
      for (int i = next; i < si.nb_counters; i++)
        {
          if (si.family[i] != 0)
            {
              if (strcmp(si.family[i], si.family[next]) == 0)
                {
                  // OK, on veut compter celui-la
                  drapeaux[i] = 1;

                  //si l'option GET_COMM_DETAILS est activee, les infos des compteurs de communication dans les stat_internals ne sont plus a jour
                  //elles sont neanmoins stockees dans le tableau communication_tracking_times
                  if(GET_COMM_DETAILS && si.counter_comm[i] && si.communication_tracking_info )
                    {
                      // correspondance entre l'identifiant du compteur de communication et son indice dans le tableau communication_tracking_times
                      int index = si.index_in_communication_tracking_info[i];
                      time += si.communication_tracking_info[index][0].time;
                      nb   += si.communication_tracking_info[index][0].count;
                      quantity += si.communication_tracking_info[index][0].quantity;

                    }
                  else
                    {
                      time += si.counter_time[i].second();
                      nb += (double) si.counter_nb[i];
                      quantity += (double) si.counter_quantity[i];
                    }

                  avg_time_per_step += si.counters_avg_min_max_var_per_step[i][1];
                  var_time_per_step += si.counters_avg_min_max_var_per_step[i][4];

                }
            }
        }



      char name_family_description[BUFLEN];
      snprintf(name_family_description, BUFLEN, "%10s \t %10s", si.family[next], "Aggregated over the family"); ///< name_description is a string that give the family and the description of a counter

      assert(var_time_per_step >= 0.);
      print_stat(perfs,perfs_globales,message,name_family_description,
                 level,time,nb,quantity,temps_total_max, skip_globals, skip_local,
                 avg_time_per_step, min_time_per_step, max_time_per_step, sqrt(var_time_per_step));

      next++;
    }
  delete[] drapeaux;

  if (Process::is_parallel())
    perfs << finl;

  if (Process::je_suis_maitre())
    perfs_globales << finl;

  // Les fichiers sont ouverts en mode append.
  {
    Nom CSV(Objet_U::nom_du_cas());
    CSV+="_log_perf.csv";
    EcrFicPartage file(CSV, mode_append ? (ios::out | ios::app) : (ios::out));
    if (Process::je_suis_maitre())
      file << File_header.get_str();
    file << perfs_globales.get_str();
    file << perfs.get_str();
    file.syncfile();
  }


  restart_counters();
}

void Statistiques::reset_counters()
{
  Stat_Internals& si = *stat_internals;
  Time time_zero;
  for (int i = 0; i < MAXCOUNTERS; i++)
    {
      si.time_begin[i] = time_zero;
      si.counter_time[i] = time_zero;
      si.counter_running[i] = 0;
      si.counter_nb[i] = 0;
      si.counter_quantity[i] = 0;

      for(int j = 0; j < 5; j++)
        {
          si.counters_avg_min_max_var_per_step[i][j] = j==2 ? INITIAL_MIN : 0.0;   //l'indice 2 correspond au min courant
        }
    }

  for (int i = 0; i < si.nb_comm_counters; i++)
    for (int j = 0; j < MAXCOUNTERS; j++)
      si.communication_tracking_info[i][j].reset();

}

void Statistiques::reset_counter(int counter_id)
{
  Stat_Internals& si = *stat_internals;
  Time time_zero;

  si.time_begin[counter_id] = time_zero;
  si.counter_time[counter_id] = time_zero;
  si.counter_running[counter_id] = 0;

  si.counter_nb[counter_id] = 0;
  si.counter_quantity[counter_id] = 0;

}


void Statistiques::stop_counters()
{
  Stat_Internals& si = *stat_internals;
  assert(si.change_level_forbidden == 0);
  // Bloquer tous les compteurs pendant le dump (on fait appel a des communications)
  si.change_level_forbidden = 1;
  si.debug_level_before = debug_level_;
  debug_level_ = 0;
  int i;
  // Arret de tous les compteurs
  for (i = 0; i < si.nb_counters; i++)
    if (si.counter_running[i])
      end_count_(i, 0, 0);
}

void Statistiques::restart_counters()
{
  Stat_Internals& si = *stat_internals;
  assert(si.change_level_forbidden == 1);
  // Reactiver les compteurs
  debug_level_ = si.debug_level_before;
  si.change_level_forbidden = 0;
}



void Statistiques::get_stats(const Stat_Counter_Id& counter_id,
                             Stat_Results& result)
{
  Stat_Internals& si = *stat_internals;
  int id = counter_id.id_;
  assert(id < si.nb_counters);
  result.time = si.counter_time[id].second();
  result.count = (double) si.counter_nb[id];
  result.quantity = (double) si.counter_quantity[id];
  result.compute_min_max_avg();
}

void Statistiques::get_stats_familly(const char * familly, Stat_Results& result)
{
  assert(familly);
  Stat_Internals& si = *stat_internals;
  result.time = 0.;
  result.count = 0.;
  result.quantity = 0.;
  int i;
  for (i = 0; i < si.nb_counters; i++)
    {
      if (si.family[i] != 0)
        {
          if (strcmp(si.family[i], familly) == 0)
            {
              result.time += si.counter_time[i].second();
              result.count += (double) si.counter_nb[i];
              result.quantity += (double) si.counter_quantity[i];
            }
        }
    }
  result.compute_min_max_avg();
}


void Statistiques::cumulate_stats(int counter_id, Stat_Results& result)
{
  Stat_Internals& si = *stat_internals;
  assert(counter_id < si.nb_counters);
  result.time += si.counter_time[counter_id].second();
  result.count += (double) si.counter_nb[counter_id];
  result.quantity += (double) si.counter_quantity[counter_id];
}



//retourne l'identifiant du compteur de communication ayant index comme indice dans le tableau communication_tracking_info
int Statistiques::get_counter_id_from_index_in_comm_tracking_info(int index) const
{
  Stat_Internals& si = *stat_internals;
  int counter_id = -1;
  for (int i = 0; i < si.nb_counters; i++)
    if ( si.index_in_communication_tracking_info[i] == index )
      counter_id=i;


  if(counter_id == -1)
    {
      Cerr << "Statistiques::get_counter_id_from_index_in_comm_tracking_info() : error! The provided index does not correspond to any counter..." << finl;
      Process::exit();
    }

  return counter_id;
}

int Statistiques::get_index_in_comm_tracking_info_from_counter_id(int id) const
{
  return ( stat_internals->index_in_communication_tracking_info[id] );
}


int Statistiques::get_counter_id_from_description(const char* desc) const
{
  return ( stat_internals -> description_to_id[desc] );
}
int Statistiques::allocate_communication_tracking_times()
{
  Stat_Internals& si = *stat_internals;

  si.communication_tracking_info = new Stat_Results*[si.nb_comm_counters];
  for(int i = 0; i < si.nb_comm_counters; i++)
    si.communication_tracking_info[i] = new Stat_Results[MAXCOUNTERS+1];

  return 0;
}

int Statistiques::delete_communication_tracking_times()
{
  Stat_Internals& si = *stat_internals;
  for(int i = 0; i < si.nb_comm_counters; ++i)
    {
      delete [] si.communication_tracking_info[i];
      si.communication_tracking_info[i] = 0;
    }
  delete [] si.communication_tracking_info;
  si.communication_tracking_info = 0;
  return 0;
}


void Statistiques::begin_communication_tracking(int cid)
{

  Stat_Internals& si = *stat_internals;
  if(!GET_COMM_DETAILS || si.counter_comm[cid] )
    return;

  for (int i = 0; i < si.nb_comm_counters; i++)
    {
      // correspondance entre l'indice dans le tableau communication_tracking_times et l'id du compteur associe
      int counter_id = get_counter_id_from_index_in_comm_tracking_info(i);

      Stat_Results& comm_total_time = si.communication_tracking_info[i][0];
      cumulate_stats(counter_id, comm_total_time);
      for (int j = 0; j < si.nb_counters; j++)
        {
          //si d'autres domaines de communication sont activees,
          //on met a jour leurs donnees
          if (si.comm_domaines_on[j])
            {
              Stat_Results& comm_time_in_domaine_j = si.communication_tracking_info[i][j + 1];
              cumulate_stats(counter_id, comm_time_in_domaine_j);

            }
        }
      reset_counter(counter_id);

    }

  si.comm_domaines_on[cid] = true;

}
void Statistiques::end_communication_tracking(int cid)
{
  Stat_Internals& si = *stat_internals;
  if(!GET_COMM_DETAILS || si.counter_comm[cid] )
    return;

  assert(cid >=0);

  if (si.comm_domaines_on[cid] == false)
    {
      if (si.counter_nb[cid]<=3) // Moins verbeux mais suppose que l'on saute les trois premiers pas de temps
        Process::Journal() << "Error! end_communication_tracking (" << si.description[cid] << ") has not been activated" <<finl;
      return;
    }

  for (int i = 0; i < si.nb_comm_counters; i++)
    {
      // correspondance entre l'indice dans le tableau communication_tracking_times et l'id du compteur associe
      int counter_id = get_counter_id_from_index_in_comm_tracking_info(i);

      Stat_Results& comm_time_in_domaine_cid = si.communication_tracking_info[i][cid + 1];
      cumulate_stats(counter_id, comm_time_in_domaine_cid);

    }

  si.comm_domaines_on[cid] = false;
}


void Statistiques::print_communciation_tracking_details(const char* message, int mode_append)
{
  assert(message); // message non nul !

  Stat_Internals& si = *stat_internals;

  stop_counters();

  //on recupere les donnees de communication produites depuis la derniere remise a zero
  for (int i = 0; i < si.nb_comm_counters; i++)
    {
      // correspondance entre l'indice dans le tableau communication_tracking_times et l'id du compteur associe
      int counter_id = get_counter_id_from_index_in_comm_tracking_info(i);

      Stat_Results& comm_total_time = si.communication_tracking_info[i][0];
      cumulate_stats(counter_id, comm_total_time);

    }

  SChaine comm;
  char desc[BUFLEN+100];

  comm << "*************************************************************************************************" << finl;
  comm << message << finl;
  comm << "Block divided into 2 parts:" << finl;
  comm << "Part 1: for each function, gives the percentage consumed by communications" << finl;
  comm << "Part 2: for each type of communication, gives the percentage of time spent in each function" << finl;
  comm << "        Note that some counters are nested within each other, " << finl;
  comm << "        that's why the sum of the percentages does not necessarily add up to 100% in this part." << finl;
  comm << "*************************************************************************************************" << finl;

  comm << finl;

  //parcours des domaines de communication
  //on laisse tomber temps total
  for (int i = 1; i < si.nb_counters; i++)
    {

      double tot_communication_in_domaine_i = 0;
      for(int j = 0; j < si.nb_comm_counters; j++)
        {
          //on ne compte pas echange_espace_vectoriel (redondant avec send_recv)
          int counter_id = get_counter_id_from_index_in_comm_tracking_info(j);
          if( strcmp(si.description[counter_id], "DoubleVect/IntVect::echange_espace_virtuel") )
            tot_communication_in_domaine_i += si.communication_tracking_info[j][i+1].time;
        }

      double tot_avg_communication_in_domaine_i = Process::mp_sum(tot_communication_in_domaine_i);
      tot_avg_communication_in_domaine_i /= Process::nproc();

      double tot_time_in_domaine_i = si.counter_time[i].second();
      int comm_pourcent = tot_time_in_domaine_i != 0.0 ?  (int)std::lrint(tot_communication_in_domaine_i / tot_time_in_domaine_i * 100) : 0;
      int avg_comm_pourcent = Process::mp_sum(comm_pourcent) / Process::nproc();

      if (tot_avg_communication_in_domaine_i)
        {
          comm << si.description[i] << " : " << tot_avg_communication_in_domaine_i << "s de communications (soit ";
          comm << avg_comm_pourcent << "% du temps total consomme par ce compteur)\n";

          for(int j = 0; j < si.nb_comm_counters; j++)
            {
              //on ne compte pas echange_espace_vectoriel
              int counter_id = get_counter_id_from_index_in_comm_tracking_info(j);
              if( strcmp(si.description[counter_id], "DoubleVect/IntVect::echange_espace_virtuel") )
                {
                  Stat_Results& communication_of_type_j = si.communication_tracking_info[j][i+1];
                  double avg_communication_of_type_j = Process::mp_sum(communication_of_type_j.time);
                  avg_communication_of_type_j /= Process::nproc();

                  int communication_type_pourcentage = tot_communication_in_domaine_i != 0.0 ? (int)std::lrint(communication_of_type_j.time / tot_communication_in_domaine_i * 100) : 0;
                  int avg_communication_type_pourcentage = Process::mp_sum(communication_type_pourcentage);
                  avg_communication_type_pourcentage /= Process::nproc();

                  if(avg_communication_of_type_j)
                    {
#ifdef INT_is_64_
                      snprintf(desc, BUFLEN + 100, "%10s %-25s %.2e s (%2li%%)\n", "\tdont", si.description[counter_id], avg_communication_of_type_j,avg_communication_type_pourcentage);
#else
                      snprintf(desc, BUFLEN + 100, "%10s %-25s %.2e s (%2i%%)\n", "\tdont", si.description[counter_id], avg_communication_of_type_j,avg_communication_type_pourcentage);
#endif
                      comm << desc;
                    }
                }

            }

          comm << "\n";
        }

    }


  double* send_recv_family = (double *)calloc(si.nb_counters+1, sizeof(double));
  double* all_reduce_family = (double *)calloc(si.nb_counters+1, sizeof(double));

  //parcours des compteurs de communication
  for(int i = 0; i < si.nb_comm_counters; i++)
    {
      int counter_id = get_counter_id_from_index_in_comm_tracking_info(i);
      double total_time_of_communication_i = si.communication_tracking_info[i][0].time;
      double total_avg_time_of_communication_i = Process::mp_sum(total_time_of_communication_i);
      total_avg_time_of_communication_i /= Process::nproc();

      if(total_avg_time_of_communication_i == 0.0) continue;

      comm
          << "\n-----------------------------------------------------------------"
          << "\n";
      comm
          <<                        si.description[counter_id]
          << "\n";
      comm
          << "-----------------------------------------------------------------"
          << "\n";
      comm << "Temps total : " << total_avg_time_of_communication_i << "s\n";

      if( si.family[counter_id] != 0 )
        {
          if( !strcmp(si.family[counter_id], "MPI_allreduce" ) )
            all_reduce_family[0] += total_time_of_communication_i;
          else if ( !strcmp(si.family[counter_id], "MPI_sendrecv" ) )
            send_recv_family[0] += total_time_of_communication_i;
        }

      // first counter is "Temps total", we can skip it
      for (int j = 1; j < si.nb_counters; j++)
        {
          double communication_in_domaine_j = si.communication_tracking_info[i][j+1].time;
          double avg_communication_in_domaine_j = Process::mp_sum(communication_in_domaine_j);
          avg_communication_in_domaine_j /= Process::nproc();

          int pourcentage = total_time_of_communication_i != 0.0 ? (int)std::lrint(communication_in_domaine_j / total_time_of_communication_i * 100) : 0;
          int avg_pourcentage = Process::mp_sum(pourcentage);
          avg_pourcentage /= Process::nproc();
          if(avg_pourcentage)
            comm << "\tdont " << si.description[j] << " : " << avg_communication_in_domaine_j << "s (" << avg_pourcentage << "%) \n";

          if (si.family[counter_id] != 0)
            {
              if( !strcmp(si.family[counter_id], "MPI_allreduce" ) )
                all_reduce_family[j+1] += communication_in_domaine_j;

              else if ( !strcmp(si.family[counter_id], "MPI_sendrecv" ) )
                send_recv_family[j+1] += communication_in_domaine_j;
            }

        }
    }
  comm << finl;

  //affichage des informations sur les familles de commpteurs
  comm
      << "\n-----------------------------------------------------------------"
      << "\n";
  comm
      << "                    Famille MPI_allreduce                         "
      << "\n";
  comm
      << "-----------------------------------------------------------------"
      << "\n";

  double tot_avg_all_reduce = Process::mp_sum(all_reduce_family[0]);
  tot_avg_all_reduce /= Process::nproc();
  comm << "Temps total : " << tot_avg_all_reduce << "s\n";

  for (int i = 1; i < si.nb_counters; i++)
    {
      double avg_all_reduce_in_domaine_i = Process::mp_sum(all_reduce_family[i]);
      avg_all_reduce_in_domaine_i /= Process::nproc();

      int pourcentage = all_reduce_family[0] != 0.0 ? (int)std::lrint(all_reduce_family[i] / all_reduce_family[0] * 100) : 0;
      int avg_pourcentage = Process::mp_sum(pourcentage);
      avg_pourcentage /= Process::nproc();

      if(avg_pourcentage)
        comm << "\tdont " << si.description[i-1] << " : " << avg_all_reduce_in_domaine_i << "s (" << avg_pourcentage << "%) \n";

    }


  comm
      << "\n-----------------------------------------------------------------"
      << "\n";
  comm
      << "                    Famille MPI_sendrecv                           "
      << "\n";
  comm
      << "-----------------------------------------------------------------"
      << "\n";

  double tot_avg_send_recv = Process::mp_sum(send_recv_family[0]);
  tot_avg_send_recv /= Process::nproc();
  comm << "Temps total : " << tot_avg_send_recv << "s\n";

  for (int i = 1; i < si.nb_counters; i++)
    {
      double avg_send_recv_in_domaine_i = Process::mp_sum(send_recv_family[i]);
      avg_send_recv_in_domaine_i /= Process::nproc();

      int pourcentage = send_recv_family[0] != 0.0 ? (int)std::lrint(send_recv_family[i] / send_recv_family[0] * 100) : 0;
      int avg_pourcentage = Process::mp_sum(pourcentage);
      avg_pourcentage /= Process::nproc();

      if(avg_pourcentage)
        comm << "\tdont " << si.description[i-1] << " : " << avg_send_recv_in_domaine_i << "s (" << avg_pourcentage << "%) \n";

    }
  comm << "\n\n";

  free(send_recv_family);
  free(all_reduce_family);

  if (Process::je_suis_maitre())
    {
      Nom TU(Objet_U::nom_du_cas());
      TU += "_comm.TU";
      SFichier comm_file(TU, mode_append ? (ios::out | ios::app) : (ios::out));
      comm_file << comm.get_str();
    }

  restart_counters();

}

void Statistiques::compute_avg_min_max_var_per_step(int tstep)
{

  Stat_Internals& si = *stat_internals;

  int step = JUMP_3_FIRST_STEPS ? tstep - 3 : tstep;
  double counter_time_step;

  for (int i = 0; i < si.nb_counters; i++)
    {
      //suivant que l'option GET_COMM_DETAILS est activee ou non,
      //les informations de temps des compteurs de communication sont a jour uniquement dans le tableau communication_tracking_times
      if ( (GET_COMM_DETAILS && !si.counter_comm[i]) || !GET_COMM_DETAILS )
        {
          counter_time_step =  si.counter_time[i].second() - si.counters_avg_min_max_var_per_step[i][0]; //temps passe par le compteur i durant l'iteration tstep
          si.counters_avg_min_max_var_per_step[i][0] = si.counter_time[i].second();
        }
      else
        {
          if(si.communication_tracking_info == 0)
            {
              Cerr << "compute_avg_min_max_var_per_step: error ! communication_tracking_times hasn't been allocated yet... (disable the option GET_COMM_DETAILS in file Statistiques.h)" <<finl;
              Process::exit();
            }

          // correspondance entre l'identifiant du compteur de communication et son indice dans le tableau communication_tracking_times
          int index = si.index_in_communication_tracking_info[i] ;
          counter_time_step = si.communication_tracking_info[index][0].time - si.counters_avg_min_max_var_per_step[i][0]; //temps passe par le compteur i durant l'iteration tstep
          si.counters_avg_min_max_var_per_step[i][0] = si.communication_tracking_info[index][0].time;


        }

      double old_moy = si.counters_avg_min_max_var_per_step[i][1];
      //moyenne courante
      si.counters_avg_min_max_var_per_step[i][1] = ((si.counters_avg_min_max_var_per_step[i][1] * step) + counter_time_step) / (step + 1);
      //min courant
      si.counters_avg_min_max_var_per_step[i][2] = std::min(si.counters_avg_min_max_var_per_step[i][2],counter_time_step);
      //max courant
      si.counters_avg_min_max_var_per_step[i][3] = std::max(si.counters_avg_min_max_var_per_step[i][3],counter_time_step);
      //variance courante
      si.counters_avg_min_max_var_per_step[i][4] = (si.counters_avg_min_max_var_per_step[i][4] + old_moy*old_moy)* step;
      si.counters_avg_min_max_var_per_step[i][4] += counter_time_step*counter_time_step;
      si.counters_avg_min_max_var_per_step[i][4] /= step+1;
      si.counters_avg_min_max_var_per_step[i][4] -= si.counters_avg_min_max_var_per_step[i][1] * si.counters_avg_min_max_var_per_step[i][1];
      //avec les erreurs d'arrondis, on peut obtenir une variance negative
      if (si.counters_avg_min_max_var_per_step[i][4] < 0)
        si.counters_avg_min_max_var_per_step[i][4] = 0.0;

    }
}


const char* Statistiques::get_counter_family(int id)
{
  return ( stat_internals -> family[id]);
}

int Statistiques::get_nb_comm_counters() const
{
  return ( stat_internals -> nb_comm_counters);
}

Stat_Results Statistiques::get_communication_tracking_info(int i, int j) const
{
  return ( stat_internals -> communication_tracking_info[i][j]);
}

void Stat_Results::compute_min_max_avg()
{
  const int nproc = Process::nproc();
  ArrOfDouble tmp(3);
  // Min
  tmp[0]=time;
  tmp[1]=count;
  tmp[2]=quantity;
  mp_min_for_each_item(tmp);
  min_time = tmp[0];
  min_count = tmp[1];
  min_quantity = tmp[2];

  // Max
  tmp[0]=time;
  tmp[1]=count;
  tmp[2]=quantity;
  mp_max_for_each_item(tmp);
  max_time = tmp[0];
  max_count = tmp[1];
  max_quantity = tmp[2];

  // Average
  tmp[0]=time;
  tmp[1]=count;
  tmp[2]=quantity;
  mp_sum_for_each_item(tmp);
  avg_time = tmp[0] / nproc;
  avg_count = tmp[1] / nproc;
  avg_quantity = tmp[2] / nproc;
}



