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
// File:        Statistiques.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Statistiques_included
#define Statistiques_included

#include <assert.h>

class Stat_Counter_Id;
class Stat_Results;
class Stat_Internals;
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
                              const char * const familly = 0);

  // L'appel a begin_count est en deux etages pour mettre le test de debug_level
  // en inline et pas les details d'implementation.
  // Les deux fonctions suivantes peuvent etre appelees sur un seul processeur
  inline void begin_count(const Stat_Counter_Id& counter_id);
  inline void end_count(const Stat_Counter_Id& counter_id, int quantity = 0, int count = 1);


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

  // Exploitation des resultats
  // !!! Les 4 fonctions suivantes doivent etre appelees simultanement
  // sur tous les processeurs
  void stop_counters();
  void restart_counters();
  void get_stats(const Stat_Counter_Id& counter_id, Stat_Results& result);
  void get_stats_familly(const char * familly, Stat_Results& result);
  inline double get_total_time()
  {
    return total_time_;
  };
  static double get_time_now();

protected:
  // Les deux fonctions suivantes peuvent etre appelees sur un seul processeur
  void begin_count_(const int id_);
  void end_count_(const int id_, int quantity, int count);

  int debug_level_;
  Stat_Internals * stat_internals;
  double total_time_;
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
protected:
  Stat_Counter_Id(int id, int level) : id_(id), level_(level) {};
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
  // Cumul de temps, min max et moyenne sur les differents processeurs
  double time, min_time, max_time, avg_time;
  // Nombre de comptages
  double count, min_count, max_count, avg_count;
  // Cumul des quantites comptees
  double quantity, min_quantity, max_quantity, avg_quantity;
};

inline void Statistiques::begin_count(const Stat_Counter_Id& counter_id)
{
  assert(counter_id.initialized());
  if (counter_id.level_ <= debug_level_)
    begin_count_(counter_id.id_);
}

// Description: Arret du compteur counter_id. On ajoute quantity a la somme des
//  'quantity' (par defaut 0) stockees pour ce compteur
//  et on ajoute 'count' (par defaut 1) au compteur d'appels a end_count pour ce compteur
//  (quantity sert a sommer par exemple la taille des donnees traitees pendant ce comptage
//  et count sert a compter combien de fois on a appele ce compteur)
inline void Statistiques::end_count(const Stat_Counter_Id& counter_id,
                                    int quantity,
                                    int count)
{
  assert(counter_id.initialized());
  if (counter_id.level_ <= debug_level_)
    end_count_(counter_id.id_, quantity, count);
}
#endif
