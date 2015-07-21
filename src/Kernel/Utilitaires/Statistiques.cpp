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
// File:        Statistiques.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Statistiques.h>
#include <EcrFicPartage.h>
#include <SChaine.h>
#include <petsc_for_kernel.h>
#ifdef VTRACE
#include <vt_user.h>
#endif
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
  Time(const Time& t) : time_(t.time_)
  { }
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
    return (double) time_.tv_sec + 0.000001 * time_.tv_usec;
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
  cerr << "Time::calibrate() for 1 second\n";
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

  cerr << " Result : hardware_clocks_per_second = ";
  cerr << 1. / seconds_per_hardware_clock << "\n";
}
#endif

// Description: Cette methode renvoie un temps en secondes depuis une origine
//  qui depend de la methode de mesure de la classe Time. Precision, normalement
//  de l'ordre de 1ms ou inferieur.
double Statistiques::get_time_now()
{
  Time t;
  t.get_time();
  return t.second();
}

// =================================================================
// =================================================================


//    CLASSE STAT_INTERNALS (interne au systeme de statistiques)
// =================================================================
// =================================================================

// Je mets une limite en dur parce que:
// - un nombre trop eleve de compteurs est sans doute le signe d'un bug
//   (creation repetee du meme compteur)
// - dans un premier temps, ca suffit et c'est plus simple
static const int MAXCOUNTERS = 1000;
class Stat_Internals
{
public:
  Stat_Internals() {};
  int nb_counters;

  // Tous les compteurs sont crees, meme si le niveau est inferieur,
  // cela permet d'activer un niveau superieur au cours du calcul.
  // A la fin, on imprime les resultats correspondant au niveau le plus
  // eleve atteint au cours du calcul.
  int max_debug_level_during_session;

  Time time_begin[MAXCOUNTERS];

  // Cumul des temps cpu pour chaque compteur
  Time counter_time[MAXCOUNTERS];
  // Le compteur est-il en cours de comptage ?
  int counter_running[MAXCOUNTERS];
  // Nombre de comptages pour chaque compteur
  unsigned long long counter_nb[MAXCOUNTERS];
  // Cumul des quantites pour chaque compteur
  unsigned long long counter_quantity[MAXCOUNTERS];
  // Description des compteurs
  const char * description[MAXCOUNTERS];
  // Famille de chaque compteur
  const char * family[MAXCOUNTERS];
  // Niveau de debug du compteur (on n'a pas acces aux Stat_Counter_Id lors
  // de l'impression des statistiques, et on veut pouvoir afficher le niveau
  // quand meme).
  int counter_level[MAXCOUNTERS];

  int change_level_forbidden;
  int debug_level_before;
};

// =========================================================================
// =========================================================================

//               IMPLEMENTATION DE LA CLASSE Statistiques
// =========================================================================
// =========================================================================

Statistiques::Statistiques()
{
  total_time_ = 0;
  debug_level_ = 0;
  stat_internals = new Stat_Internals;

  Stat_Internals& si = *stat_internals;
  si.nb_counters = 0;
  si.max_debug_level_during_session = 0;
  int i;
  for (i = 0; i < MAXCOUNTERS; i++)
    {
      si.counter_running[i] = 0;
      si.counter_nb[i] = 0;
      si.counter_quantity[i] = 0;
      si.description[i] = 0;
      si.family[i] = 0;
      si.counter_level[i] = 0;
    }
  si.change_level_forbidden = 0;
  si.debug_level_before = 0;
}

Statistiques::~Statistiques()
{
  //cerr << "Stat::~Stat called !!!\n";
  delete stat_internals;
  stat_internals = 0;
}

void Statistiques::set_debug_level(int level)
{
  Stat_Internals& si = *stat_internals;
  // Lors de l'impression des stats, on bloque les compteurs.
  // Le niveau est mis a zero, et
  // on n'a pas le droit de changer de niveau.
  if (si.change_level_forbidden)
    return;

  debug_level_ = level;
  if (level > si.max_debug_level_during_session)
    si.max_debug_level_during_session = level;
}

// Voir Statistiques.h pour le commentaire...

Stat_Counter_Id Statistiques::new_counter(int level,
                                          const char * const description,
                                          const char * const family)
{
  Stat_Internals& si = *stat_internals;
  assert(description); // Pointeur non nul ?
  if (si.nb_counters >= MAXCOUNTERS)
    {
      Cerr << "Stat_Counter_Id::new_counter : too much counters." << finl;
      Cerr << " Check this : new_counter must be called only once per counter." << finl;
      Cerr << " (counter " << description << ")" << finl;
      Cerr << " Otherwise, increase MAXCOUNTERS is Statistiques.cpp." << finl;
      assert(0);
      Process::exit();
    }
  int length = strlen(description);
  if (family) length += strlen(family);
  if (length > BUFLEN - 10)
    {
      Cerr << "Statistiques::new_counter : description or family : string too long"
           << finl;
      Cerr << description << finl;
      assert(0);
      Process::exit();
    }
  //
  if (level < 1)
    level = 1;
  const int id = si.nb_counters;
  Stat_Counter_Id new_id(id, level);
  si.counter_level[id] = level;
  si.description[id] = description;
  si.family[id] = family;
  si.nb_counters++;
  return new_id;
}

void Statistiques::begin_count_(const int id)
{
  Stat_Internals& si = *stat_internals;
  assert(id < si.nb_counters);

  si.time_begin[id].get_time();
  si.counter_running[id] = 1;
#ifdef __PETSCKSP_H
  Nom info(si.description[id]);
  info+="\n";
  PetscInfo(0,info);
#endif
#ifdef VTRACE
  // Level 1 only to avoid MPI calls
  if (si.counter_level[id]==1) VT_USER_START(si.description[id]);
#endif
}

void Statistiques::end_count_(const int id, int quantity, int count)
{
  Stat_Internals& si = *stat_internals;
  assert(id < si.nb_counters);

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

// static => ne pas exporter cette fonction
static void print_stat(Sortie& perfs,
                       Sortie& perfs_globales,
                       const char * description,
                       int level,
                       double time,
                       double nb,
                       double quantity,
                       double temps_total_max,
                       int skip_globals)
{
  char tampon[BUFLEN+100];

  if (Process::nproc() > 1)
    {
      double percent_time = (temps_total_max==0 ? 0 : time / temps_total_max * 100.);
      sprintf(tampon, "%50s (%02d) %10.2f %5.1f%% %10.3g %10.3g",
              description, level, time, percent_time, nb, quantity);
      perfs << tampon << finl;
    }

  if (! skip_globals)
    {

      double min_time = Process::mp_min(time);
      double max_time = Process::mp_max(time);
      double avg_time = Process::mp_sum(time) / Process::nproc();
      double min_nb = Process::mp_min(nb);
      double max_nb = Process::mp_max(nb);
      double avg_nb = Process::mp_sum(nb) / Process::nproc();
      double min_quantity = Process::mp_min(quantity);
      double max_quantity = Process::mp_max(quantity);
      double avg_quantity = Process::mp_sum(quantity) / Process::nproc();

      if (Process::je_suis_maitre())
        {
          double percent_time = (temps_total_max==0 ? 0 : avg_time / temps_total_max * 100.);

          char s1[50], s2[50], s3[50];
          s1[0] = s2[0] = s3[0] = 0;
          if (min_time != max_time)
            sprintf(s1, "(%0.2f,%0.2f)", min_time, max_time);
          if (min_nb != max_nb)
            sprintf(s2, "(%0.3g,%0.3g)", min_nb, max_nb);
          if (min_quantity != max_quantity)
            sprintf(s3, "(%0.3g,%0.3g)", min_quantity, max_quantity);

          sprintf(tampon,
                  "%50s (%02d) %10.2f  %5.1f%% %-23s %10.3g %-23s %10.3g %-23s",
                  description, level,
                  avg_time, percent_time, s1,
                  avg_nb, s2,
                  avg_quantity, s3);

          perfs_globales << tampon << finl;
        }
    }
}

void Statistiques::dump(const char * message, int mode_append)
{
  assert(message); // message non nul !

  Stat_Internals& si = *stat_internals;

  char tampon[BUFLEN+100];

  stop_counters();

  SChaine perfs;
  SChaine perfs_globales;
  if (Process::nproc() > 1)
    {
      perfs.precision(3);
      perfs << "Statistics execution [PE " << Process::me() << "] : " << message << finl;
      perfs << " the time was measured by the following method :" << finl;
      perfs << " " << Time::description << finl;

      sprintf(tampon, "%55s %10s        %10s %10s",
              "Counter (level)", "time(s)", "count", "quantity");
      perfs << tampon << finl;
    }

  if (Process::je_suis_maitre())
    {
      sprintf(tampon,
              "%55s %10s         %-23s %10s %-23s %10s %-23s",
              "Counter (level)",
              "avg time", "(min,max)",
              "avg count", "(min,max)",
              "avg qty", "(min,max)");
      perfs_globales << "Statistics overall execution : " << message << finl;
      perfs_globales << " the time was measured by the following method :" << finl;
      perfs_globales << " " << Time::description << finl;
      perfs_globales << tampon << finl;
    }

  // Verification : on a le meme nombre de compteurs sur tous les procs ?
  int skip_globals = 0;
  {
    int min_nb = (int) Process::mp_min(si.nb_counters);
    int max_nb = (int) Process::mp_max(si.nb_counters);
    if (min_nb != max_nb)
      {
        if (Process::je_suis_maitre())
          {
            perfs_globales << "Unable to collect statistics :" << finl
                           << " there is not the same number of counters on all"
                           " processors.";
          }
        skip_globals = 1;
      }
  }

  // Temps de reference pour les pourcentages
  double temps_total_max = 0.;

  // Compute Divers (time not counted by other counters in time step loop):
  if (si.counter_nb[2]>0)
    {
      if (strcmp(si.description[2],"Resoudre (timestep loop)"))
        {
          Cerr << "Error in Statistiques::dump(...)" << finl;
          Process::exit();
        }
      int id=-1;
      // Look for counter Divers
      for (int i = 0; i < si.nb_counters; i++)
        if (!strcmp(si.description[i],"Divers"))
          id=i;
      // Compute CPU Divers = counter(time step) - sum(other counters))
      si.counter_time[id].add(si.counter_time[2]);
      for (int i = 3; i < si.nb_counters; i++)
        {
          if (i!=id && si.counter_level[i]==1)
            si.counter_time[id].minus(si.counter_time[i]);
        }
    }
  // Affichage des compteurs individuels
  for (int i = 0; i < si.nb_counters; i++)
    {
      char description[BUFLEN];

      if (si.family[i])
        sprintf(description, "%s:%s", si.family[i], si.description[i]);
      else
        sprintf(description, "%s", si.description[i]);

      int level = si.counter_level[i];
      double time = si.counter_time[i].second();
      double nb = si.counter_nb[i];
      double quantity = si.counter_quantity[i];

      if (i == 0)
        {
          temps_total_max = time;
          total_time_+=temps_total_max;     // Update total_time_
        }
      print_stat(perfs,perfs_globales,description,
                 level,time,nb,quantity,temps_total_max, skip_globals);
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
      // Trouve le premier compteur de la famille suivante
      // (famille non nulle et pas encore traitee)
      while (next < si.nb_counters
             && (si.family[next] == 0 || drapeaux[next] != 0))
        next++;
      if (next >= si.nb_counters)
        break;
      // Somme des valeurs des compteurs de cette famille
      for (int i = next; i < si.nb_counters; i++)
        {
          if (si.family[i] != 0)
            {
              if (strcmp(si.family[i], si.family[next]) == 0)
                {
                  // OK, on veut compter celui-la
                  drapeaux[i] = 1;
                  time += si.counter_time[i].second();
                  nb += si.counter_nb[i];
                  quantity += si.counter_quantity[i];
                }
            }
        }
      const char * description = si.family[next];
      print_stat(perfs,perfs_globales,description,
                 level,time,nb,quantity,temps_total_max,skip_globals);

      next++;
    }
  delete[] drapeaux;

  if (Process::nproc() > 1)
    perfs << finl;

  if (Process::je_suis_maitre())
    perfs_globales << finl;

  // Les fichiers sont ouverts en mode append.
  {
    Nom TU(Objet_U::nom_du_cas());
    TU+="_detail.TU";
    EcrFicPartage file(TU, mode_append ? (ios::out | ios::app) : (ios::out));
    if (Process::je_suis_maitre())
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
    }
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

static void compute_min_max_avg(Stat_Results& result)
{
  const int nproc = Process::nproc();
  result.min_time = Process::mp_min(result.time);
  result.max_time = Process::mp_max(result.time);
  result.avg_time = Process::mp_sum(result.time) / nproc;
  result.min_count = Process::mp_min(result.count);
  result.max_count = Process::mp_max(result.count);
  result.avg_count = Process::mp_sum(result.count) / nproc;
  result.min_quantity = Process::mp_min(result.quantity);
  result.max_quantity = Process::mp_max(result.quantity);
  result.avg_quantity = Process::mp_sum(result.quantity) / nproc;
}

void Statistiques::get_stats(const Stat_Counter_Id& counter_id,
                             Stat_Results& result)
{
  Stat_Internals& si = *stat_internals;
  int id = counter_id.id_;
  assert(id < si.nb_counters);
  result.time = si.counter_time[id].second();
  result.count = si.counter_nb[id];
  result.quantity = si.counter_quantity[id];
  compute_min_max_avg(result);
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
              result.count += si.counter_nb[i];
              result.quantity += si.counter_quantity[i];
            }
        }
    }
  compute_min_max_avg(result);
}

