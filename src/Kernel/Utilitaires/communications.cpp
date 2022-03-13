/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        communications.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

// Fichiers comm.* renommes communications.* car
// il existe des includes systemes nommes comm.h (eg: paradis)
// qui peuvent creer des conflits en compilant avec mpiCC au lieu de g++
#include <communications.h>
#include <communications_array.h>
#include <InOutCommBuffers.h>
#include <Comm_Group.h>
#include <PE_Groups.h>
#include <Schema_Comm.h>
#include <TRUSTTabs.h>
#include <TRUSTArrays.h>
#include <Motcle.h>
#include <DerOu_.h>
#include <TRUSTTab.h>
#include <vector>
// Description:
//  Envoi de messages point-to-point synchrone entre la source et la cible.
//  La fonction envoyer se termine au plus tard lorsque le message a ete
//  recu par l'instruction "recevoir" correspondante  et l'envoi peut etre blocant
//  (le blocage est force en mode debug, selon Comm_Group::check_enabled() )
//  Ces communications utilisent Comm_Group::send() et Comm_Group::recv()
//  Exemples d'utilisations possibles:
//  1) communication point-a-point (processeur 2 vers processeur 5)
//       if (me() == 2)
//         envoyer(objet, 5, canal);
//       else if (me() == 5)
//         recevoir(objet, 2, canal);
//     ou de facon equivalente :
//       envoyer(objet, 2, 5, canal); // ne fait rien sur les processeurs autres que 2
//       recevoir(objet, 2, 5, canal); // ne fait rien sur les processeurs autres que 5
//  2) gather : le processeur 0 recupere les donnees de l'objet t de tout le monde
//       envoyer(t, me(), 0, canal); // Tout le monde envoie au processeur 0, sur le maitre, ne fait rien
//       if (je_suis_maitre())
//         for (i = 0; i < nproc(); i++) {
//           recevoir(t, 0, i; canal); // Si i==0, ne fait rien.
//           ... faire quelque chose avec t
//         }
//  3) broadcast : le processeur 0 envoie l'objet a tous les autres processeurs
//     Il n'est pas necessaire de faire le test "if (je_suis_maitre())"
//       envoyer(objet, 0, -1, canal); // Les processeurs autres que 0 ignorent l'appel
//       recevoir(objet, 0, -1, canal);// Le processeur 0 ignore l'appel
//     Il vaut mieux utiliser envoyer_broadcast() pour cet usage.
//  4) broadcast : ecriture equivalente
//       if (je_suis_maitre())
//         envoyer(objet, -1, canal); // Emission par le maitre a tous les processeurs, sauf lui-meme
//       else
//         recevoir(objet, 0, canal); // Reception d'un message du processeur maitre
//     Il vaut mieux utiliser envoyer_broadcast() pour cet usage.
// Dans l'implementation de envoyer_, on encapsule l'objet recu dans un buffer et on envoie le buffer.
// Ca marche donc pour les types complexes pourvu que Entree et Sortie sachent lire et ecrire l'objet.
// Chaque envoi necessite l'envoi de deux messages : d'abord la taille du buffer, puis le contenu.
template <class T> int envoyer_buffered_(const T& objet, int source, int cible, int canal)
{
  const int moi = Process::me();
  if (moi == cible)
    return 0;

  if (source != moi && source != -1)
    return 0;

  OutputCommBuffer buffer;
  buffer << objet;
  const char * data = buffer.get_buffer();
  const int sz   = buffer.get_buffer_size();
  const Comm_Group& grp = PE_Groups::current_group();
  if (cible < 0)
    {
      const int n = grp.nproc();
      for (int i = 0; i < n; i++)
        {
          if (i != moi)
            {
              grp.send(i, &sz, sizeof(int), canal);
              grp.send(i, data, sz, canal);
            }
        }
    }
  else
    {
      grp.send(cible, &sz, sizeof(int), canal);
      grp.send(cible, data, sz, canal);
    }
  return 1;
}

// Description: Reception d'un message en provenance du processeur source
template <class T> int recevoir_buffered_(T& objet, int source, int cible, int canal)
{
  const int moi = Process::me();
  if (moi == source)
    return 0;

  if (cible != moi && cible != -1)
    return 0;

  int sz;
  const Comm_Group& grp = PE_Groups::current_group();
  grp.recv(source, &sz, sizeof(int), canal);
  InputCommBuffer buffer;
  char * data = buffer.reserve_buffer(sz);
  grp.recv(source, data, sz, canal);
  buffer.create_stream();
  buffer >> objet;
  buffer.clear();
  return 1;
}

// Attention: le template ne marche que pour les types simples (pas Objet_U !)
// On ne passe pas par un buffer Entree / Sortie mais on envoie directement le tableau sous sa forme binaire.
// Un seul message envoye, sauf en mode check() ou on envoie aussi la taille pour verifier.
template <class T> int envoyer_array_(const T * objet, int n, int source, int cible, int canal)
{
  const Comm_Group& grp = PE_Groups::current_group();
  const int moi = grp.rank();
  if (moi == cible)
    return 0;

  if (source != moi && source != -1)
    return 0;

  const char * data = (const char*) objet;
  const int sz   = sizeof(T) * n;
  if (cible < 0)
    {
      const int nbproc = grp.nproc();
      for (int i = 0; i < nbproc; i++)
        {
          if (i != moi)
            {
              // En mode check, on verifie que n est bien le meme sur l'expediteur et le recepteur
              if (grp.check_enabled())
                grp.send(i, &sz, sizeof(int), canal);
              if (sz > 0)
                grp.send(i, data, sz, canal);
            }
        }
    }
  else
    {
      if (grp.check_enabled())
        grp.send(cible, &sz, sizeof(int), canal);
      if (sz > 0)
        grp.send(cible, data, sz, canal);
    }
  return 1;
}

template <class T> int recevoir_array_(T * objet, int n, int source, int cible, int canal)
{
  const Comm_Group& grp = PE_Groups::current_group();
  const int moi = grp.rank();
  if (moi == source)
    return 0;

  if (cible != moi && cible != -1)
    return 0;

  const int sz = sizeof(T) * n;
  if (grp.check_enabled())
    {
      int sz_check;
      grp.recv(source, &sz_check, sizeof(int), canal);
      if (sz_check != sz)
        {
          Cerr << "Fatal error in template <class T> int recevoir_array_ : incorrect size\n"
               << " sent=" << sz_check << " expected=" << sz << finl;
          Process::exit();
        }
    }
  char * data = (char*) objet;
  if (sz > 0)
    grp.recv(source, data, sz, canal);
  return 1;
}

// Pour les types simples, on passe par envoyer_array_ qui n'utilise pas un buffer mais envoie directement
// les valeurs. Plus rapide.
int envoyer(const int&   t, int source, int cible, int canal )
{
  return envoyer_array_(&t, 1, source, cible, canal);
}
#ifndef INT_is_64_
int envoyer(const long&     t, int source, int cible, int canal )
{
  return envoyer_array_(&t, 1, source, cible, canal);
}
#endif
int envoyer(const float&    t, int source, int cible, int canal )
{
  return envoyer_array_(&t, 1, source, cible, canal);
}
int envoyer(const double&   t, int source, int cible, int canal )
{
  return envoyer_array_(&t, 1, source, cible, canal);
}
int envoyer(const Objet_U& t, int source, int cible, int canal )
{
  return envoyer_buffered_(t, source, cible, canal);
}

int envoyer(const int&   t, int cible, int canal )
{
  return envoyer_array_(&t, 1, Process::me(), cible, canal);
}
#ifndef INT_is_64_
int envoyer(const long&     t, int cible, int canal )
{
  return envoyer_array_(&t, 1, Process::me(), cible, canal);
}
#endif
int envoyer(const float&    t, int cible, int canal )
{
  return envoyer_array_(&t, 1, Process::me(), cible, canal);
}
int envoyer(const double&   t, int cible, int canal )
{
  return envoyer_array_(&t, 1, Process::me(), cible, canal);
}
int envoyer(const Objet_U& t, int cible, int canal )
{
  return envoyer_buffered_(t, Process::me(), cible, canal);
}

int recevoir(int& t, int source, int cible, int canal )
{
  return recevoir_array_(&t, 1, source, cible, canal);
}
#ifndef INT_is_64_
int recevoir(long&     t, int source, int cible, int canal )
{
  return recevoir_array_(&t, 1, source, cible, canal);
}
#endif
int recevoir(float&    t, int source, int cible, int canal )
{
  return recevoir_array_(&t, 1, source, cible, canal);
}
int recevoir(double&   t, int source, int cible, int canal )
{
  return recevoir_array_(&t, 1, source, cible, canal);
}
int recevoir(Objet_U& t, int source, int cible, int canal )
{
  return recevoir_buffered_(t, source, cible, canal);
}

int recevoir(int& t, int source, int canal )
{
  return recevoir_array_(&t, 1, source, Process::me(), canal);
}
#ifndef INT_is_64_
int recevoir(long&     t, int source, int canal )
{
  return recevoir_array_(&t, 1, source, Process::me(), canal);
}
#endif
int recevoir(float&    t, int source, int canal )
{
  return recevoir_array_(&t, 1, source, Process::me(), canal);
}
int recevoir(double&   t, int source, int canal )
{
  return recevoir_array_(&t, 1, source, Process::me(), canal);
}
int recevoir(Objet_U& t, int source, int canal )
{
  return recevoir_buffered_(t, source, Process::me(), canal);
}

int envoyer_array(const int  * t, int n, int source, int cible, int canal )
{
  return envoyer_array_(t, n, source, cible, canal);
}
#ifndef INT_is_64_
int envoyer_array(const long    * t, int n, int source, int cible, int canal )
{
  return envoyer_array_(t, n, source, cible, canal);
}
#endif
int envoyer_array(const double  * t, int n, int source, int cible, int canal )
{
  return envoyer_array_(t, n, source, cible, canal);
}
int envoyer_array(const float   * t, int n, int source, int cible, int canal )
{
  return envoyer_array_(t, n, source, cible, canal);
}

int recevoir_array(const int  * t, int n, int source, int cible, int canal )
{
  return recevoir_array_(t, n, source, cible, canal);
}
#ifndef INT_is_64_
int recevoir_array(const long    * t, int n, int source, int cible, int canal )
{
  return recevoir_array_(t, n, source, cible, canal);
}
#endif
int recevoir_array(const double  * t, int n, int source, int cible, int canal )
{
  return recevoir_array_(t, n, source, cible, canal);
}
int recevoir_array(const float   * t, int n, int source, int cible, int canal )
{
  return recevoir_array_(t, n, source, cible, canal);
}

template <class T> int envoyer_broadcast_(T * objet, int n, int source)
{
  const Comm_Group& grp = PE_Groups::current_group();
  if (grp.check_enabled())
    {
      int copie_n = n;
      grp.broadcast(&copie_n, sizeof(int), source);
      if (copie_n != n)
        {
          Cerr << "Error in envoyer_broadcast_(T * objet, int n, int source)" << finl;
          Process::exit();
        }
    }
  grp.broadcast(objet, sizeof(T) * n, source);
  return 1;
}

// Description: Broadcast de l'objet par le processeur source a tous les autres processeurs.
//  Fonctionne pour des types complexes supportes par Entree ou Sortie.
template <class T> int envoyer_broadcast_buffered_(T& objet, int source)
{
  const Comm_Group& grp = PE_Groups::current_group();
  if (grp.rank() == source)
    {
      OutputCommBuffer buffer;
      buffer << objet;
      const char * data = buffer.get_buffer();
      int sz   = buffer.get_buffer_size();
      grp.broadcast(&sz, sizeof(int), source);
      grp.broadcast((char*) data, sz, source);
    }
  else
    {
      int sz;
      grp.broadcast(&sz, sizeof(int), source);
      InputCommBuffer buffer;
      char * data = buffer.reserve_buffer(sz);
      grp.broadcast((char*) data, sz, source);
      buffer.create_stream();
      buffer >> objet;
      buffer.clear();
    }
  return 1;
}

int envoyer_broadcast(int& t, int source)
{
  return envoyer_broadcast_(&t, 1, source);
}
#ifndef INT_is_64_
int envoyer_broadcast(long&     t, int source)
{
  return envoyer_broadcast_(&t, 1, source);
}
#endif
int envoyer_broadcast(long long&     t, int source)
{
  return envoyer_broadcast_(&t, 1, source);
}
int envoyer_broadcast(float&    t, int source)
{
  return envoyer_broadcast_(&t, 1, source);
}
int envoyer_broadcast(double&   t, int source)
{
  return envoyer_broadcast_(&t, 1, source);
}
int envoyer_broadcast(Objet_U& t, int source)
{
  return envoyer_broadcast_buffered_(t, source);
}

int envoyer_broadcast_array(int  * t, int n, int source)
{
  return envoyer_broadcast_(t, n, source);
}
#ifndef INT_is_64_
int envoyer_broadcast_array(long    * t, int n, int source)
{
  return envoyer_broadcast_(t, n, source);
}
#endif
int envoyer_broadcast_array(float   * t, int n, int source)
{
  return envoyer_broadcast_(t, n, source);
}
int envoyer_broadcast_array(double  * t, int n, int source)
{
  return envoyer_broadcast_(t, n, source);
}
int envoyer_broadcast_array(char    * t, int n, int source)
{
  return envoyer_broadcast_(t, n, source);
}

// Template qui marche pour tout objet T qui ressemble a un vecteur (possede un operator[])
//  de taille nproc()
template <class T> int envoyer_all_to_all_(const T& src, T& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  Schema_Comm sch;
  const int nproc = grp.nproc();
  const int moi = grp.rank();
  dest[moi] = src[moi];
  ArrOfInt send_pe_list(1);
  ArrOfInt recv_pe_list(1);
  for (int p = 1; p < nproc; p++)
    {
      const int send_to = (moi + p) % nproc;
      const int recv_from = (moi + nproc - p) % nproc;
      send_pe_list[0] = send_to;
      recv_pe_list[0] = recv_from;
      sch.set_send_recv_pe_list(send_pe_list, recv_pe_list);
      sch.begin_comm();
      sch.send_buffer(send_to) << src[send_to];
      sch.echange_taille_et_messages();
      sch.recv_buffer(recv_from) >> dest[recv_from];
      sch.end_comm();
    }
  return 1;
}

int envoyer_all_to_all(const DoubleTabs& src,     DoubleTabs& dest)
{
  // Je mets le assert ici car si on veut faire un envoyer_all_to_all(IntTab)
  //  le assert doit utiliser size_array()
  assert(src.size() == Process::nproc());
  assert(dest.size() == Process::nproc());
  return envoyer_all_to_all_(src, dest);
}

int envoyer_all_to_all(const VECT(ArrOfInt) & src, VECT(ArrOfInt) & dest)
{
  assert(src.size() == Process::nproc());
  assert(dest.size() == Process::nproc());
  return envoyer_all_to_all_(src, dest);
}

// Description: On suppose que les tableaux en entree et en sortie
//  sont de taille nproc() . On envoie src[0] au proc 0,
//  src[1] au proc 1, etc... la valeur recue du processeur 0 et mise dans
//  dest[0], processeur 1 dans dest[1], etc...
//  Il est autorise d'appeler la fonction avec le meme tableau src et dest.
int envoyer_all_to_all(const ArrOfInt& src, ArrOfInt& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.size_array() == grp.nproc());
  assert(dest.size_array() == grp.nproc());
  if (src.addr() == dest.addr())
    {
      ArrOfInt tmp;
      tmp.resize_array(grp.nproc(), Array_base::NOCOPY_NOINIT);
      grp.all_to_all(src.addr(), tmp.addr(), sizeof(int));
      dest.inject_array(tmp);
    }
  else
    {
      grp.all_to_all(src.addr(), dest.addr(), sizeof(int));
    }
  return 1;
}

// Description: On suppose que les tableaux en entree et en sortie
//  sont de taille nproc() . On envoie src[0] au proc 0,
//  src[1] au proc 1, etc... la valeur recue du processeur 0 et mise dans
//  dest[0], processeur 1 dans dest[1], etc...
//  Il est autorise d'appeler la fonction avec le meme tableau src et dest.
int envoyer_all_to_all(const ArrOfDouble& src, ArrOfDouble& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.size_array() == grp.nproc());
  assert(dest.size_array() == grp.nproc());
  if (src.addr() == dest.addr())
    {
      ArrOfDouble tmp;
      tmp.resize_array(grp.nproc(), Array_base::NOCOPY_NOINIT);
      grp.all_to_all(src.addr(), tmp.addr(), sizeof(double));
      dest.inject_array(tmp);
    }
  else
    {
      grp.all_to_all(src.addr(), dest.addr(), sizeof(double));
    }
  return 1;
}

// Description: On suppose que les tableaux en entree et en sortie
//  sont de taille nproc() . On envoie src[0] au proc 0,
//  src[1] au proc 1, etc... la valeur recue du processeur 0 et mise dans
//  dest[0], processeur 1 dans dest[1], etc...
//  Il est autorise d'appeler la fonction avec le meme tableau src et dest.
int envoyer_all_to_all(std::vector<long long>& src, std::vector<long long>& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(static_cast<int>(src.size()) == grp.nproc());
  assert(static_cast<int>(dest.size()) == grp.nproc());
  if (src.data() == dest.data())
    {
      std::vector<long long> tmp(grp.nproc(),0);
      grp.all_to_all(src.data(), tmp.data(), sizeof(long long));
      dest = tmp;
    }
  else
    {
      grp.all_to_all(src.data(), dest.data(), sizeof(long long));
    }
  return 1;
}

// Description:
//  Calcule la transposee d'une liste de processeurs:
//    On construit le tableau dest_list tel que:
//        x est dans la liste src_list sur le processeur y
//      si et seulement si
//        y est dans la liste dest_list sur le processeur x
//  En pratique, cette fonction permet de calculer la liste des
//  processeurs qui recoivent des messages si on connait la liste
//  des processeurs qui envoient des messages, et reciproquement.
// Parametre:     src_list
// Signification: Une liste de numeros de PEs presents dans ce groupe de comm.
//                Ce sont les indices des pes dans le groupe courant (0 <= pe < nproc_)
// Parametre:     dest_list
// Signification: Le tableau ou stocker le resultat. On le met a la bonne taille
//                et on le remplit. Les numeros de PEs sont mis dans l'ordre
//                croissant.
//                Ce sont les indices des pes dans le groupe courant (0 <= pe < nproc_)
// Precondition:
//  Cette fonction doit etre appelee simultanement sur tous les processeurs
//  du groupe.
// Valeur de retour:
//  nombre d'elements dans dest_list.
int reverse_send_recv_pe_list(const ArrOfInt& src_list, ArrOfInt& dest_list)
{
  const Comm_Group& grp = PE_Groups::current_group();
  const int np = grp.nproc();
  assert(src_list.size_array() == 0 || max_array(src_list) < np);
  assert(src_list.size_array() == 0 || min_array(src_list) >= 0);
  ArrOfInt flag(np);
  int i;
  const int src_size = src_list.size_array();
  for (i = 0; i < src_size; i++)
    {
      const int pe = src_list[i];
      flag[pe] = 1;
    }
  ArrOfInt flag_recv(np);
  envoyer_all_to_all(flag, flag_recv);
  int n = 0;
  dest_list.resize_array(np);
  for (i = 0; i < np; i++)
    if (flag_recv[i])
      dest_list[n++] = i;
  dest_list.resize_array(n);
  return 1;
}

// Description: renvoie le plus grand int i sur l'ensemble des processeurs du
//  groupe courant.
int mp_max(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MAX);
  return y;
}

// Description: renvoie le plus petit int i sur l'ensemble des processeurs du
//  groupe courant.
int mp_min(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_MIN);
  return y;
}

// Description: Calul de la somme partielle de i sur les processeurs 0 a me()-1
//  (renvoie 0 sur le processeur 0). Voir Comm_Group::mppartial_sum()
int mppartial_sum(int x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, Comm_Group::COLL_PARTIAL_SUM);
  return y;
}


void mpsum_multiple(double& x1, double& x2)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double x[2];
  double y[2];
  x[0] = x1;
  x[1] = x2;
  grp.mp_collective_op(x, y, 2, Comm_Group::COLL_SUM);
  x1 = y[0];
  x2 = y[1];
}

static void mp_collective_op(ArrOfInt& x, Comm_Group::Collective_Op op)
{
  int sz = x.size_array();
  assert_parallel(sz);
  if (sz > 0)
    {
      int *data = x.addr();
      int *tmp = new int[sz];
      const Comm_Group& grp = PE_Groups::current_group();
      grp.mp_collective_op(data, tmp, sz, op);
      memcpy(data, tmp, sz * sizeof(int));
      delete [] tmp;
    }
}

static void mp_collective_op(ArrOfDouble& x, Comm_Group::Collective_Op op)
{
  int sz = x.size_array();
  assert_parallel(sz);
  if (sz > 0)
    {
      double *data = x.addr();
      double *tmp = new double[sz];
      const Comm_Group& grp = PE_Groups::current_group();
      grp.mp_collective_op(data, tmp, sz, op);
      memcpy(data, tmp, sz * sizeof(double));
      delete [] tmp;
    }
}

void mp_sum_for_each_item(ArrOfInt& x)
{
  mp_collective_op(x, Comm_Group::COLL_SUM);
}
void mp_sum_for_each_item(ArrOfDouble& x)
{
  mp_collective_op(x, Comm_Group::COLL_SUM);
}
void mp_min_for_each_item(ArrOfInt& x)
{
  mp_collective_op(x, Comm_Group::COLL_MIN);
}
void mp_min_for_each_item(ArrOfDouble& x)
{
  mp_collective_op(x, Comm_Group::COLL_MIN);
}
void mp_max_for_each_item(ArrOfInt& x)
{
  mp_collective_op(x, Comm_Group::COLL_MAX);
}
void mp_max_for_each_item(ArrOfDouble& x)
{
  mp_collective_op(x, Comm_Group::COLL_MAX);
}

// Description: On suppose que les tableaux en entree et en sortie
//  sont tels que dimension(0)==nproc() et que les autres dimensions
//  sont identiques. On envoie src(0, i, j, ...) au proc 0,
//  src(0, i, j, ...) au proc 1, etc... la valeur recue du processeur 0 et mise dans
//  dest(0, i, j, ...) , processeur 1 dans dest(1, i, j, ...) , etc...
//  Il est autorise d'appeler la fonction avec le meme tableau src et dest.
void envoyer_all_to_all(const DoubleTab& src, DoubleTab& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.dimension(0) == grp.nproc());
  assert(dest.dimension(0) == grp.nproc());
  assert(src.size_array() == dest.size_array());

  if (dest.addr() == src.addr())
    {
      DoubleTab tmp(src);
      envoyer_all_to_all(src, tmp);
      dest = tmp;
    }
  else
    {
      const int sz = src.size() / grp.nproc() * sizeof(double);
      grp.all_to_all(src.addr(), dest.addr(), sz);
    }
}

// Description: renvoie le drapeau Comm_Group::check_enabled().
//  Ce drapeau indique si le code tourne en mode DEBUG du parallele.
//  Dans ce cas, des communications supplementaires ont lieu pour
//  verifier la coherence du parallelisme et le mode de communication
//  est force en mode synchrone.
//  Cette methode permet de tester le drapeau, et s'il est mis de
//  realiser des verifications supplementaires
int comm_check_enabled()
{
  return Comm_Group::check_enabled();
}

// Description: renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs,
//  sinon renvoie 0 (sur tous les procs)
int is_parallel_object(const int x)
{
  const int x1 = mp_min(x);
  const int x2 = mp_max(x);
  return x1 == x2;
}

// Description: renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs,
//  sinon renvoie 0 (sur tous les procs)
int is_parallel_object(const double x)
{
  const Comm_Group& grp = PE_Groups::current_group();
  double tab_x[2];
  tab_x[0] = x;
  tab_x[1] = x;
  double tab_y[2];
  Comm_Group::Collective_Op tab_op[] = { Comm_Group::COLL_MIN, Comm_Group::COLL_MAX };
  grp.mp_collective_op(tab_x, tab_y, tab_op, 2);
  return tab_y[0] == tab_y[1];
}

// Description: en mode comm_check_enabled(), verifie que le parametre
//  a la meme valeur sur tous les processeurs
void assert_parallel(const int x)
{
  if (!Comm_Group::check_enabled())
    return;
  if (!is_parallel_object(x))
    {
      Cerr << "Fatal error in assert_parallel(const int x). See log files" << finl;
      Process::Journal() << "Error in assert_parallel(const int x)          )"
                         << "This processor has this value: " << x << finl;
      Process::barrier();
      Process::exit();
    }
}

// Description: en mode comm_check_enabled(), verifie que le parametre
//  a la meme valeur sur tous les processeurs
void assert_parallel(const double x)
{
  if (!Comm_Group::check_enabled())
    return;
  if (!is_parallel_object(x))
    {
      Cerr << "Fatal error in assert_parallel(const double x). See log files" << finl;
      Process::Journal() << "Error in assert_parallel(const double x)          )"
                         << "This processor has this value: " << x << finl;
      Process::barrier();
      Process::exit();
    }
}

template <class T> int compare(const Objet_U& a, const Objet_U& b, int& erreur)
{
  if (sub_type(T, a))
    {
      const T& a_ = static_cast<const T&> (a);
      const T& b_ = static_cast<const T&> (b);
      if (! (a_ == b_))
        erreur = 1;
      return 1;
    }
  else
    {
      return 0;
    }
}

// Description: renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs,
//  sinon renvoie 0 (sur tous les procs)
int is_parallel_object(const Objet_U& obj)
{
  int erreur = 0;
  if (Process::je_suis_maitre())
    {
      DERIV(Objet_U) copie;
      const int n = Process::nproc();
      for (int i = 1; i < n; i++)
        {
          recevoir(copie, i, 67 /* tag */);
          if (erreur)
            continue; // On a deja trouve une erreur, on recoit les autres messages mais c'est tout...
          const Objet_U& obj2 = copie.valeur();
          if (obj2.que_suis_je() != obj.que_suis_je())
            {
              erreur = 1;
            }
          else
            {
              // L'operateur == n'est pas virtuel, on ne peut pas se contenter de comparer avec ==.
              // Il faut appeler == pour le sous-type specifique de l'objet.
              // L'ordre est important: d'abord le type le plus derive, puis sa classe de base.
              if (compare<IntTab> (obj, obj2, erreur));
              else if (compare<DoubleTab> (obj, obj2, erreur));
              else if (compare<IntVect> (obj, obj2, erreur));
              else if (compare<DoubleVect> (obj, obj2, erreur));
              else if (compare<ArrOfInt> (obj, obj2, erreur));
              else if (compare<ArrOfDouble> (obj, obj2, erreur));
              else if (compare<Motcle> (obj, obj2, erreur));
              else if (compare<Nom> (obj, obj2, erreur));
              else
                {
                  Cerr << "Internal error in is_parallel_objec() : it is not coded for " << obj.que_suis_je() << finl;
                  Process::exit();
                }
            }
        }
    }
  else
    {
      // On envoie un objet de type DERIV..
      DERIV(Objet_U) copie(obj);
      envoyer(copie, 0, 67 /* tag */);
    }

  envoyer_broadcast(erreur, 0 /* pe source */);
  return !erreur;
}

// Description: en mode comm_check_enabled(), verifie que le parametre
//  a la meme valeur sur tous les processeurs.
//  Ne fonctionne pas pour tous les types !
//  Pour l'instant ok pour Noms, Motcles et tableaux.
void assert_parallel(const Objet_U& obj)
{
  if (!Comm_Group::check_enabled())
    return;

  if (!is_parallel_object(obj))
    {
      Cerr << "Fatal error in assert_parallel(const Objet_U & obj): objects do not match, see log files" << finl;
      Process::Journal() << "Error in assert_parallel(const Objet_U & obj)"
                         << "This processor has this object : " << obj.que_suis_je() << finl
                         << obj << finl;
      Process::barrier();
      Process::exit();
    }
}
