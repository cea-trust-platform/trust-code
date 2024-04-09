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

#ifndef Statistiques_included
#define Statistiques_included

#include <assert.h>
#include <string>
class Stat_Counter_Id;
class Stat_Results;
class Stat_Internals;
class Time;


static const int JUMP_3_FIRST_STEPS = 0; ///< If equal to 1, counters will start after the 3 first time steps

static const int GET_COMM_DETAILS = 1; ///< If equal to 1, enable to track the communications of the wanted functions

class Statistiques
{
public:

  Statistiques();
  ~Statistiques();
  /*!@brief Function that change the debug level
   *
   *This function have to be called simultanely on each processor
   * @param level the new debug level
   * During the printing of the performance, the debug level is set to 0
   */
  void set_debug_level(int level);

  /*!@brief Create a new counter in the Statistiques object
   *
   * This function has to be called simultaneously on every processor
   * @param level If level <= debug_level, then the statistics of this counter will be printed at the end.
   * @param decsription String used as the counter name/short description
   * @param familly Parameter to regroup counter together (if they have the same family number) in order to create aggregated stats ; if equal to zero, then no aggregation
   * @param comm If equal to 1, then the counter is a communication counter, otherwise, it is a "basic" counter. Set to 0 by default
   */
  Stat_Counter_Id new_counter(int level,
                              const char * const description,
                              const char * const familly = 0,
                              int comm = 0);

  /*! @brief Start the count of a counter
   *
   * @param counter_id The ID of the counter that the user want to start. It is a global variable that is declare in stats_counters.cpp
   * @param track_comm Indicate if the user want to record the communication associated with the counter, set to true by default
   */
  inline void begin_count(const Stat_Counter_Id& counter_id, bool track_comm = true);

  /*! @brief End the count of a counter and update values in the Stat_Internals object of the Statistiques object
   *
   * @param counter_id The ID of the counter that the user want to start. It is a global variable that is declare in stats_counters.cpp
   * @param quantity A user defined quantity which depends on the counter (can be a number of iteration, a buffer size,.... It is set to zero by default
   * @count Parameter that count the number of time the counter has been used between begin_count and end_count. By default equal to 1.
   * @param track_comm Indicate if the user want to record the communication associated with the counter, set to true by default
   */
  inline void end_count(const Stat_Counter_Id& counter_id, int quantity = 0, int count = 1, bool track_comm = true);


  /*! @brief Give the last interval of time the counter has been used
   *
   * If the counter is still running, return the time since the last begin_count
   * If the counter is stopped, return the interval of time of the last count
   * @param counter_id The ID of the counter that the user want to start. It is a global variable that is declare in stats_counters.cpp
   */
  double last_time(const Stat_Counter_Id& counter_id);

  /*! @brief Function that create the file CASE_NAME_log_perf.csv
   *
   * This function prints the global and detailed performance log, depending on the debug_level.
   * It has to be called on every processor simultaneously
   * During this function, all counters are stop and then restart
   * @param message_info String that state the overall step of calculation we are in : initialization, resolution or post-processing
   */
  void dump(const char * message_info, int mode_append);

  /*! @brief Reset all counters
   *
   * Some variables are kept even after the reset : counters_avg_min_max_var_per_step and communication_tracking_info
   */
  void reset_counters();

  /*! @brief Reset the counter related to the counter_id
   *
   * Some variables are kept even after the reset : counters_avg_min_max_var_per_step and communication_tracking_info
   */
  void reset_counter(int counter_id);

  void stop_counters(); //< Stop all counters, has to be called on every processor simultaneously

  void restart_counters(); //< Restart all counters, has to be called on every processor simultaneously

  /*! @brief Get back the stats of a counter and store them in a Stat_Resluts type object
   *
   * @param counter_id Global variable link to a specific counter. It is declare in stats_counters.cpp
   * @param result A Stat_Results type object used to store the stats concerning the wanted counter
   */

  void get_stats(const Stat_Counter_Id& counter_id, Stat_Results& result);

  /*! @brief Get back and agregate the stats of a set of counters and store them in a Stat_Resluts type object
   *
   * @param familly Name of a set of counter for which we want aggregated stats. It is declare in stats_counters.cpp
   * @param result A Stat_Results type object used to store the stats concerning the wanted counter
   */

  void get_stats_familly(const char * familly, Stat_Results& result);

  /*! @brief Aggregate stats of the counter associated with counter ID to the stats in a Stat_Results type object
   *
   * @param counter_id Global variable link to a specific counter. It is declare in stats_counters.cpp
   * @param result A Stat_Results type object used to store the stats concerning the wanted counter
   */
  void cumulate_stats(int counter_id, Stat_Results& result);

  /// Get the protected variable total_time of a Statistics type object
  inline double get_total_time()
  {
    return total_time_;
  };

  static double get_time_now(); ///< Return the clock time

  /*! Compute the average, minimum, maximum and variance of the elapsed time on an iteration for each counter
   *
   * @param tstep Number of time step of the calculation
   */
  void compute_avg_min_max_var_per_step(int tstep);

  /// Create the table of communication stats after the declaration of all of the counters
  int allocate_communication_tracking_times();

  /// Delete the table of communication stats at the end of the calculation
  int delete_communication_tracking_times();

  /*! @brief Start communication tracking
   *
   * Counters stats are saved in the table communication_tracking_info
   * Update the entire table communication_tracking_info : the total time of communication for all communication counter and the communication time per domain j for each counter
   * /!\ reset the counter associated with cid
   * comm_domaines_on becomes true
   */
  void begin_communication_tracking(int cid);

  /*! @brief Update communications info regarding counter cid and stop the count
   *
   * Update the variable communication_tracking_info[i][cid+1] with the communication tracking associated with counter cid.
   * comm_domaines_on become flase
   */
  void end_communication_tracking(int cid);

  /*! Print communication stats
   *
   * For each declared communication domain/function, print the average time spend for each type of communication on each processor
   * And for each time of communication, print the average time spend on each domain/function on each processor
   */
  void print_communciation_tracking_details(const char * message, int mode_append);

  const char* get_counter_family(int id);

  int get_counter_id_from_index_in_comm_tracking_info(int index) const;

  int get_index_in_comm_tracking_info_from_counter_id(int id) const;

  int get_nb_comm_counters() const;

  Stat_Results get_communication_tracking_info(int i, int j) const;

  inline void set_three_first_steps_elapsed(bool b)
  {
    three_first_steps_elapsed_ = b;
  }

  int get_counter_id_from_description(const char* desc) const;

protected:
  // Les deux fonctions suivantes peuvent etre appelees sur un seul processeur
  void begin_count_(const int id_);
  void end_count_(const int id_, int quantity, int count);
  int debug_level_;
  Stat_Internals * stat_internals;
  double total_time_;
  bool three_first_steps_elapsed_;  ///< If TRUE, the 3 first time steps are elapsed

};



// Ceci est l'identifiant fourni a l'utilisateur pour acceder au compteur
// qu'il a cree.
class Stat_Counter_Id
{
public:
  Stat_Counter_Id() : id_(-1), level_(-1) {};
  int initialized(void) const
  {
    return id_ >= 0;
  };
  inline int id() const
  {
    return id_;
  };
protected:
  Stat_Counter_Id(int i, int level) : id_(i), level_(level) {};
  friend class Statistiques;
  int id_;
  int level_;
  // Le niveau de debug est stocke ici pour un acces rapide
  // lors de l'appel a begin et end, il est aussi stocke dans
  // Stat_Internals...
};

// La definition de ce pointeur est dans Statistiques.cpp
// NE PAS UTILISER CE POINTEUR DIRECTEMENT !
// Utiliser la methode d'acces statistiques()
class Statistiques;
extern Statistiques * les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe;

inline Statistiques& statistiques()
{
  assert(les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe);
  return *les_statistiques_trio_U_nom_long_pour_decourager_l_utilisation_directe;
}

// Les details d'implementation de Stat sont separes de Stat pour ne pas avoir
// a tout recompiler si on change Stat.
class Stat_Internals;

/*! @brief An object that is used to store counter data
 *
 */
class Stat_Results
{
public:
  Stat_Results() : time(0.0), min_time(0.0), max_time(0.0), avg_time(0.0),
    count(0.0), min_count(0.0), max_count(0.0), avg_count(0.0),
    quantity(0.0), min_quantity(0.0), max_quantity(0.0), avg_quantity(0.0) {};

  // Cumul de temps, min max et moyenne sur les differents processeurs
  double time, min_time, max_time, avg_time;
  // Nombre de comptages
  double count, min_count, max_count, avg_count;
  // Cumul des quantites comptees
  double quantity, min_quantity, max_quantity, avg_quantity;

  void compute_min_max_avg();  //calcule le min, le max et la moyenne des temps sur les procs

  inline void reset();

};

inline void Statistiques::begin_count(const Stat_Counter_Id& counter_id, bool track_comm)
{
  assert(counter_id.initialized());
  if (counter_id.level_ <= debug_level_)
    {
      begin_count_(counter_id.id_);
      if(track_comm) begin_communication_tracking(counter_id.id_);
    }
}

/*! @brief Arret du compteur counter_id.
 *
 * On ajoute quantity a la somme des 'quantity' (par defaut 0) stockees pour ce compteur
 *   et on ajoute 'count' (par defaut 1) au compteur d'appels a end_count pour ce compteur
 *   (quantity sert a sommer par exemple la taille des donnees traitees pendant ce comptage
 *   et count sert a compter combien de fois on a appele ce compteur)
 *
 */
inline void Statistiques::end_count(const Stat_Counter_Id& counter_id,
                                    int quantity,
                                    int count,
                                    bool track_comm)
{
  assert(counter_id.initialized());
  if (counter_id.level_ <= debug_level_)
    {
      end_count_(counter_id.id_, quantity, count);
      if(track_comm) end_communication_tracking(counter_id.id_);
    }
}

inline void Stat_Results::reset()
{

  time=0.0, min_time=0.0, max_time=0.0, avg_time=0.0;
  count=0.0, min_count=0.0, max_count=0.0, avg_count=0.0;
  quantity=0.0, min_quantity=0.0, max_quantity=0.0, avg_quantity=0.0;
}

#endif
