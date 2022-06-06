/****************************************************************************
* Copyright (c) 2020, CEA
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
class Stat_Counter_Id;
class Stat_Results;
class Stat_Internals;
class Time;

//si 1, permet de faire demarrer les compteurs seulement apres les 3 premiers pas de temps
static const int JUMP_3_FIRST_STEPS = 0;
//si 1, permet de traquer les communications dans les fonctions souhaitees
static const int GET_COMM_DETAILS = 1;

class Statistiques
{
public:
  Statistiques();
  ~Statistiques();

  // Niveau de debug (on peut le changer en cours de calcul, cela
  // permet d'activer ou non les compteurs)
  // !!! Cette fonction doit obligatoirement etre appelee simultanement sur tous
  // les processeurs.
  void set_debug_level(int level);

  // Creation d'un nouveau compteur
  // !!! Cette fonction doit obligatoirement etre appelee simultanement sur tous
  // les processeurs.
  // level : niveau de debug, superieur ou egal a 1 (compteur active si debug_level >= level)
  // description : description du compteur affichee a la fin du calcul (non nul !)
  //               Attention, on ne fait pas de copie locale de la chaine, le pointeur doit
  //               rester valide jusqu'a la fin de l'execution.
  // family : eventuellement zero, nom d'une famille de compteurs pour grouper les stats.
  Stat_Counter_Id new_counter(int level,
                              const char * const description,
                              const char * const familly = 0,
                              int comm = 0);

  // L'appel a begin_count est en deux etages pour mettre le test de debug_level
  // en inline et pas les details d'implementation.
  // Les deux fonctions suivantes peuvent etre appelees sur un seul processeur
  // track_comm permet d'indiquer si l'on souhaite egalement traquer les communications
  inline void begin_count(const Stat_Counter_Id& counter_id, bool track_comm = true);
  inline void end_count(const Stat_Counter_Id& counter_id, int quantity = 0, int count = 1, bool track_comm = true);


  // Renvoie le dernier intervalle de temps mesure par un compteur, en secondes
  double last_time(const Stat_Counter_Id& counter_id);

  // !!! Les trois fonctions suivantes doivent etre appelees simultanement
  // sur tous les processeurs.
  // Impression des statistiques par processeur et globales.
  // Tous les compteurs en cours sont arretes.
  // (les fichiers .TU sont ouverts en mode append)
  void dump(const char * message_info, int mode_append);
  // Remise a zero de tous les compteurs
  void reset_counters();
  // Remise a zero du compteur counter_id
  //(certaines donnees sont conservees, comme counters_avg_min_max_var_per_step et communication_tracking_info)
  void reset_counter(int counter_id);


  // Exploitation des resultats
  // !!! Les 4 fonctions suivantes doivent etre appelees simultanement
  // sur tous les processeurs
  void stop_counters();
  void restart_counters();
  void get_stats(const Stat_Counter_Id& counter_id, Stat_Results& result);
  void get_stats_familly(const char * familly, Stat_Results& result);
  void cumulate_stats(int counter_id, Stat_Results& result);

  inline double get_total_time()
  {
    return total_time_;
  };
  static double get_time_now();

  //calcule les moyennes, minimum, maximum et variance des temps passes dans chaque compteur lors d'une iteration
  void compute_avg_min_max_var_per_step(int tstep);


  //le tableau communication_tracking_times ne peut etre alloue qu'apres la declaration de tous les compteurs
  int allocate_communication_tracking_times();
  int delete_communication_tracking_times();

  /* Debut de la zone de code dont on veut traquer les communications :
   * on sauvegarde toutes les donnees des compteurs de communication dans le tableau communication_tracking_times,
   * puis on les remet a zero
   * Donnee d'entree:
   * 	- cid : identifiant de la zone qu'on souhaite initialiser
   */
  void begin_communication_tracking(int cid);

  /* Fin de la zone de code dont on veut traquer les communications :
   * on recupere toutes les donnees des compteurs de communication qui ont ete produites
   * depuis la derniere remise a zero des compteurs
   * Donnee d'entree:
   * 	- cid : identifiant de la zone
   */
  void end_communication_tracking(int cid);

  /* Affichage des statistiques de communication collectees:
   *  - pour chaque zone de communication declaree,
   *  on affiche le temps qu'on y a passe en moyenne sur chaque proc dans chaque type de communication
   *  - pour chaque type de communication,
   *   on affiche le temps passe en moyenne sur chaque proc dans chaque zone delcaree
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
  bool three_first_steps_elapsed_;  //si true, indique que les 3 premiers pas de temps sont passes

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

// Description: Arret du compteur counter_id. On ajoute quantity a la somme des
//  'quantity' (par defaut 0) stockees pour ce compteur
//  et on ajoute 'count' (par defaut 1) au compteur d'appels a end_count pour ce compteur
//  (quantity sert a sommer par exemple la taille des donnees traitees pendant ce comptage
//  et count sert a compter combien de fois on a appele ce compteur)
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
