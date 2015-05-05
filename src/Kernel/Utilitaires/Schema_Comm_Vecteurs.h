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
// File:        Schema_Comm_Vecteurs.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Schema_Comm_Vecteurs_included
#define Schema_Comm_Vecteurs_included

#include <ArrOfInt.h>
#include <ArrOfDouble.h>

class Schema_Comm_Vecteurs_Static_Data;

// .DESCRIPTION
//  Classe outil utilisee notamment par les methodes MD_Vector::echange_espace_virtuel()
//  Permet d'echanger avec d'autres processeurs des blocs d'ints ou de double
//  accessibles par des tableaux dans lesquels on lit et on ecrit directement
//  (contrairement a Schema_Comm qui utilise readOn et printOn, plus lent).
//  Pour des raisons de performances, la communication est separee en deux parties
//  - definition des tailles de buffers (permet d'allouer a l'avance les bufffers
//     begin_init()
//     add_send/recv_area_int/double(processeur, size)
//      (declaration des types, tailles et processeurs destinataire des blocs que l'on va envoyer,
//       on peut envoyer plusieurs blocs de types identiques ou differents a chaque processeur)
//     end_init()
//  - echange de donnees (autant de fois qu'on veut):
//     begin_comm()
//     for(each bloc to send) {
//      ArrOfInt/Double & buf = get_next_area_int/double(pe, size);
//      for (i=0; i<size; i++)
//        buf[i] = ...
//     }
//     exchange();
//     for (each bloc to recv) {
//       ... get_next_area_int/double(...)
//     end_comm();
class Schema_Comm_Vecteurs
{
public:
  Schema_Comm_Vecteurs();
  ~Schema_Comm_Vecteurs();
  void begin_init();
  inline void add_send_area_int(int pe, int size);
  inline void add_send_area_double(int pe, int size);
  inline void add_recv_area_int(int pe, int size);
  inline void add_recv_area_double(int pe, int size);
  void end_init();
  void begin_comm();
  void exchange();
  void end_comm();
  inline ArrOfInt& get_next_area_int(int pe, int array_size);
  inline ArrOfDouble& get_next_area_double(int pe, int array_size);
protected:
  inline void add(int pe, int size, ArrOfInt& procs, ArrOfInt& buf_sizes, int align_size);
  int check_buffers_full() const;
  int check_next_area(int pe, int byte_size) const;

  // Pour chaque processeur de send_proc ou recv_proc_, taille totale des buffers en "bytes"
  // Pendant la phase begin_init(), ces deux tableaux sont de taille nproc(), valeur nulle
  //  pour les processeurs a qui on ne parle pas.
  // ensuite, il sont de la meme taille que send_procs_ et recv_procs_
  ArrOfInt send_buf_sizes_;
  ArrOfInt recv_buf_sizes_;
  // Liste des proceseurs a qui on envoie des donnees
  ArrOfInt send_procs_;
  ArrOfInt recv_procs_;
  // A l'issue de la phase d'initialisation, les processeurs sont-ils dans l'ordre croissant ?
  int sorted_;
  // Taille du buffer requis pour ce schema
  int min_buf_size_;

  enum Status { RESET, BEGIN_INIT, END_INIT, BEGIN_COMM, EXCHANGED };
  Status status_;

  // Le buffer global est-il en cours d'utilisation ?
  static int buffer_locked_;
  // Zones temporaires de lecture/ecriture, renvoyees par get_next... et qui pointent dans buffer_
  static ArrOfDouble tmp_area_double_;
  static ArrOfInt tmp_area_int_;
  // Classe contenant des tableaux malloc (pour destruction automatique en fin d'execution)
  static Schema_Comm_Vecteurs_Static_Data sdata_;
};

// .DESCRIPTION
//  Donnees statiques communes a toutes les classes Schema_Comm_Vecteur,
//  avec destructeur pour liberer la memoire en fin d'execution
class Schema_Comm_Vecteurs_Static_Data
{
public:
  Schema_Comm_Vecteurs_Static_Data();
  ~Schema_Comm_Vecteurs_Static_Data();
  void init(int size);

  char  *buffer_base_;
  int buffer_base_size_;
  int buf_pointers_size_;
  // Pour chaque processeur entre 0 et nproc(), adresse des prochaines donnees a lire/ecrire
  // de ce proc dans le tableau buffer
  char **buf_pointers_;
};

// Taille en bytes d'un bloc de sz ints, arrondi aux 8 octets superieurs
#define BLOCSIZE_INT(sz) (sz<<2)
#define BLOCSIZE_DOUBLE(sz) (sz<<3)
#define ALIGN_SIZE(ptr,sz) ptr=sdata_.buffer_base_+((ptr-sdata_.buffer_base_+(sz-1))&(~(sz-1)))

inline void Schema_Comm_Vecteurs::add(int pe, int size, ArrOfInt& procs, ArrOfInt& buf_sizes,
                                      int align_size)
{
  assert(status_ == BEGIN_INIT);
  assert(size >= 0);
  int& x = buf_sizes[pe];
  if (x == 0 && size > 0)
    {
      const int n = procs.size_array();
      if (n > 0 && procs[n-1] > pe)
        sorted_ = 0;
      procs.append_array(pe);
    }
  x = ((x+align_size-1)&(~(align_size-1))) + size; // Padding before block
}

inline void Schema_Comm_Vecteurs::add_send_area_int(int pe, int size)
{
  add(pe, BLOCSIZE_INT(size), send_procs_, send_buf_sizes_, sizeof(int));
}

inline void Schema_Comm_Vecteurs::add_send_area_double(int pe, int size)
{
  add(pe, BLOCSIZE_DOUBLE(size), send_procs_, send_buf_sizes_, sizeof(double));
}

inline void Schema_Comm_Vecteurs::add_recv_area_int(int pe, int size)
{
  add(pe, BLOCSIZE_INT(size), recv_procs_, recv_buf_sizes_, sizeof(int));
}

inline void Schema_Comm_Vecteurs::add_recv_area_double(int pe, int size)
{
  add(pe, BLOCSIZE_DOUBLE(size), recv_procs_, recv_buf_sizes_, sizeof(double));
}

// Description: renvoie un tableau contenant les "size" valeurs suivantes
//  recues du processeur pe lors de la communication en cours.
//  Attention:
//  Le tableau renvoye est une reference a un tableau interne qui n'est valide que
//  jusqu'au prochain appel a une methode get_next_xxx.
inline ArrOfInt& Schema_Comm_Vecteurs::get_next_area_int(int pe, int size)
{
  ALIGN_SIZE(sdata_.buf_pointers_[pe], sizeof(int));
  assert(check_next_area(pe, BLOCSIZE_INT(size)));
  int *bufptr = (int *) (sdata_.buf_pointers_[pe]);
  // attention a l'arithmetique de pointeurs, ajout d'une taille en octets
  sdata_.buf_pointers_[pe] += BLOCSIZE_INT(size);
  tmp_area_int_.ref_data(bufptr, size);
  return tmp_area_int_;
}

inline ArrOfDouble& Schema_Comm_Vecteurs::get_next_area_double(int pe, int size)
{
  ALIGN_SIZE(sdata_.buf_pointers_[pe], sizeof(double));
  assert(check_next_area(pe, BLOCSIZE_DOUBLE(size)));
  double *bufptr = (double *) (sdata_.buf_pointers_[pe]);
  // attention a l'arithmetique de pointeurs, ajout d'une taille en octets
  sdata_.buf_pointers_[pe] += BLOCSIZE_DOUBLE(size);
  tmp_area_double_.ref_data(bufptr, size);
  return tmp_area_double_;
}


#undef BLOCSIZE_INT
#undef BLOCSIZE_DOUBLE
#endif
