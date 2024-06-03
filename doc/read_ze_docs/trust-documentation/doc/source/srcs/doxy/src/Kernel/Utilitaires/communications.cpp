/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <InOutCommBuffers.h>
#include <communications.h>
#include <Schema_Comm.h>
#include <TRUSTArrays.h>
#include <TRUSTTabs.h>
#include <TRUSTTab.h>
#include <Motcle.h>

/*! @brief Envoi de messages point-to-point synchrone entre la source et la cible.
 *
 * La fonction envoyer se termine au plus tard lorsque le message a ete
 *   recu par l'instruction "recevoir" correspondante  et l'envoi peut etre blocant
 *   (le blocage est force en mode debug, selon Comm_Group::check_enabled() )
 *   Ces communications utilisent Comm_Group::send() et Comm_Group::recv()
 *   Exemples d'utilisations possibles:
 *   1) communication point-a-point (processeur 2 vers processeur 5)
 *        if (me() == 2)
 *          envoyer(objet, 5, canal);
 *        else if (me() == 5)
 *          recevoir(objet, 2, canal);
 *      ou de facon equivalente :
 *        envoyer(objet, 2, 5, canal); // ne fait rien sur les processeurs autres que 2
 *        recevoir(objet, 2, 5, canal); // ne fait rien sur les processeurs autres que 5
 *   2) gather : le processeur 0 recupere les donnees de l'objet t de tout le monde
 *        envoyer(t, me(), 0, canal); // Tout le monde envoie au processeur 0, sur le maitre, ne fait rien
 *        if (je_suis_maitre())
 *          for (i = 0; i < nproc(); i++) {
 *            recevoir(t, 0, i; canal); // Si i==0, ne fait rien.
 *            ... faire quelque chose avec t
 *          }
 *   3) broadcast : le processeur 0 envoie l'objet a tous les autres processeurs
 *      Il n'est pas necessaire de faire le test "if (je_suis_maitre())"
 *        envoyer(objet, 0, -1, canal); // Les processeurs autres que 0 ignorent l'appel
 *        recevoir(objet, 0, -1, canal);// Le processeur 0 ignore l'appel
 *      Il vaut mieux utiliser envoyer_broadcast() pour cet usage.
 *   4) broadcast : ecriture equivalente
 *        if (je_suis_maitre())
 *          envoyer(objet, -1, canal); // Emission par le maitre a tous les processeurs, sauf lui-meme
 *        else
 *          recevoir(objet, 0, canal); // Reception d'un message du processeur maitre
 *      Il vaut mieux utiliser envoyer_broadcast() pour cet usage.
 *  Dans l'implementation de envoyer_, on encapsule l'objet recu dans un buffer et on envoie le buffer.
 *  Ca marche donc pour les types complexes pourvu que Entree et Sortie sachent lire et ecrire l'objet.
 *  Chaque envoi necessite l'envoi de deux messages : d'abord la taille du buffer, puis le contenu.
 *
 */
template <typename T>
int envoyer_buffered_(const T& objet, int source, int cible, int canal)
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

/*! @brief Reception d'un message en provenance du processeur source
 *
 */
template <typename T>
int recevoir_buffered_(T& objet, int source, int cible, int canal)
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

/*! @brief Broadcast de l'objet par le processeur source a tous les autres processeurs.
 *
 * Fonctionne pour des types complexes supportes par Entree ou Sortie.
 *
 */
template <typename T>
int envoyer_broadcast_buffered_(T& objet, int source)
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

int envoyer(const Objet_U& t, int source, int cible, int canal)
{
  return envoyer_buffered_(t, source, cible, canal);
}

int envoyer(const Objet_U& t, int cible, int canal)
{
  return envoyer_buffered_(t, Process::me(), cible, canal);
}

int recevoir(Objet_U& t, int source, int cible, int canal)
{
  return recevoir_buffered_(t, source, cible, canal);
}

int recevoir(Objet_U& t, int source, int canal)
{
  return recevoir_buffered_(t, source, Process::me(), canal);
}

int envoyer_broadcast(Objet_U& t, int source)
{
  return envoyer_broadcast_buffered_(t, source);
}

// Template qui marche pour tout objet T qui ressemble a un vecteur (possede un operator[]) de taille nproc()
template <typename T>
int envoyer_all_to_all_(const T& src, T& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.size() == Process::nproc());
  assert(dest.size() == Process::nproc());
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

int envoyer_all_to_all(const TRUST_Vector<TRUSTTab<double>>& src, TRUST_Vector<TRUSTTab<double>>& dest)
{
  return envoyer_all_to_all_(src, dest);
}

int envoyer_all_to_all(const TRUST_Vector<TRUSTArray<int>>& src, TRUST_Vector<TRUSTArray<int>>& dest)
{
  return envoyer_all_to_all_(src, dest);
}

/*! @brief On suppose que les tableaux en entree et en sortie sont de taille nproc() .
 *
 * On envoie src[0] au proc 0,
 *   src[1] au proc 1, etc... la valeur recue du processeur 0 et mise dans
 *   dest[0], processeur 1 dans dest[1], etc...
 *   Il est autorise d'appeler la fonction avec le meme tableau src et dest.
 *
 */
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

/*! @brief Calcule la transposee d'une liste de processeurs: On construit le tableau dest_list tel que:
 *
 *         x est dans la liste src_list sur le processeur y
 *       si et seulement si
 *         y est dans la liste dest_list sur le processeur x
 *   En pratique, cette fonction permet de calculer la liste des
 *   processeurs qui recoivent des messages si on connait la liste
 *   des processeurs qui envoient des messages, et reciproquement.
 *
 * @param (src_list) Une liste de numeros de PEs presents dans ce groupe de comm. Ce sont les indices des pes dans le groupe courant (0 <= pe < nproc_)
 * @param (dest_list) Le tableau ou stocker le resultat. On le met a la bonne taille et on le remplit. Les numeros de PEs sont mis dans l'ordre croissant. Ce sont les indices des pes dans le groupe courant (0 <= pe < nproc_)
 */
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

int mp_operations_commun_(int x, Comm_Group::Collective_Op op)
{
  const Comm_Group& grp = PE_Groups::current_group();
  int y;
  grp.mp_collective_op(&x, &y, 1, op);
  return y;
}

/*! @brief renvoie le plus grand int i sur l'ensemble des processeurs du groupe courant.
 *
 */
int mp_max(int x) { return mp_operations_commun_(x,Comm_Group::COLL_MAX); }

/*! @brief renvoie le plus petit int i sur l'ensemble des processeurs du groupe courant.
 *
 */
int mp_min(int x) { return mp_operations_commun_(x,Comm_Group::COLL_MIN); }

/*! @brief Calul de la somme partielle de i sur les processeurs 0 a me()-1 (renvoie 0 sur le processeur 0).
 *
 * Voir Comm_Group::mppartial_sum()
 *
 */
int mppartial_sum(int x) { return mp_operations_commun_(x,Comm_Group::COLL_PARTIAL_SUM); }

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

/*! @brief On suppose que les tableaux en entree et en sortie sont tels que dimension(0)==nproc() et que les autres dimensions
 *
 *   sont identiques. On envoie src(0, i, j, ...) au proc 0,
 *   src(0, i, j, ...) au proc 1, etc... la valeur recue du processeur 0 et mise dans
 *   dest(0, i, j, ...) , processeur 1 dans dest(1, i, j, ...) , etc...
 *   Il est autorise d'appeler la fonction avec le meme tableau src et dest.
 *
 */
void envoyer_all_to_all(const DoubleTab& src, DoubleTab& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.dimension(0) == grp.nproc());
  assert(dest.dimension(0) == grp.nproc());
  assert(src.size_array() == dest.size_array());

  if (dest.addrForDevice() == src.addrForDevice())
    {
      DoubleTab tmp(src);
      envoyer_all_to_all(src, tmp);
      dest = tmp;
    }
  else
    {
      const int sz = src.size() / grp.nproc() * (int)sizeof(double);
      grp.all_to_all(src.addrForDevice(), dest.addrForDevice(), sz);
    }
}
template <typename T>
void envoyer_all_gather_(const TRUSTTab<T>& src, TRUSTTab<T>& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();

  if (dest.addr() == src.addr())
    {
      assert(src.dimension(0) == grp.nproc());
      assert(dest.dimension(0) == grp.nproc());
      const int sz = src.size() / grp.nproc() * (int)sizeof(T);
      TRUSTTab<T> tmp(src);
      grp.all_gather(src.addr(), tmp.addr(), sz);
      dest = tmp;
    }
  else
    {
      const int sz = src.size() * (int)sizeof(T);
      assert( dest.size_array() == grp.nproc()*src.size_array());
      grp.all_gather(src.addr(), dest.addr(), sz);
    }
}

template <typename T>
void envoyer_gather_(const TRUSTTab<T>& src, TRUSTTab<T>& dest, int root)
{
  const Comm_Group& grp = PE_Groups::current_group();

  if (dest.addr() == src.addr())
    {
      assert(src.dimension(0) == grp.nproc());
      assert(dest.dimension(0) == grp.nproc());
      const int sz = src.size() / grp.nproc() * (int)sizeof(T);
      TRUSTTab<T> tmp(src);
      grp.gather(src.addr(), tmp.addr(), sz, root);
      dest = tmp;
    }
  else
    {
      const int sz = src.size() * (int)sizeof(T);
      assert( dest.size_array() == grp.nproc()*src.size_array());
      grp.gather(src.addr(), dest.addr(), sz, root);
    }
}

void envoyer_gather(const DoubleTab& src, DoubleTab& dest, int root)
{
  envoyer_gather_(src,dest,root);
}

void envoyer_gather(const IntTab& src, IntTab& dest, int root)
{
  envoyer_gather_(src,dest,root);
}

void envoyer_all_gather(const DoubleTab& src, DoubleTab& dest)
{
  envoyer_all_gather_(src,dest);
}

void envoyer_all_gather(const IntTab& src, IntTab& dest)
{
  envoyer_all_gather_(src,dest);
}

void envoyer_all_gatherv(const DoubleTab& src, DoubleTab& dest, const IntTab& recv_size)
{
  const Comm_Group& grp = PE_Groups::current_group();

  assert(dest.size_array()==local_somme_vect(recv_size));
  assert(src.size_array()==recv_size[grp.me()]);

  const True_int nbprocs = (True_int)grp.nproc();

  std::vector<True_int> sz(nbprocs);
  for (True_int p=0; p<nbprocs; p++)
    sz[p] = recv_size[p] * (True_int)sizeof(double);

  True_int sz0 =  src.size() * (True_int)sizeof(double);

  std::vector<True_int> displs(nbprocs);
  displs[0] = 0;
  for (True_int p=1; p<nbprocs; p++)
    displs[p] = displs[p-1]+sz[p-1];

  grp.all_gatherv(src.addr(), dest.addr(), sz0 , sz.data(),displs.data());
}


/*! @brief renvoie le drapeau Comm_Group::check_enabled().
 *
 * Ce drapeau indique si le code tourne en mode DEBUG du parallele. Dans ce cas, des communications supplementaires ont lieu pour verifier la coherence du parallelisme et le mode de communication
 *   est force en mode synchrone. Cette methode permet de tester le drapeau, et s'il est mis de realiser des verifications supplementaires
 *
 */
int comm_check_enabled() { return Comm_Group::check_enabled(); }

/*! @brief renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs, sinon renvoie 0 (sur tous les procs)
 *
 */
int is_parallel_object(const int x)
{
  const int x1 = mp_min(x);
  const int x2 = mp_max(x);
  return x1 == x2;
}

/*! @brief renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs, sinon renvoie 0 (sur tous les procs)
 *
 */
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

/*! @brief renvoie 1 (sur tous les procs) si le parametre est identique sur tous les procs, sinon renvoie 0 (sur tous les procs)
 *
 */
int is_parallel_object(const Objet_U& obj)
{
  int erreur = 0;
  if (Process::je_suis_maitre())
    {
      DerObjU copie;
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
              if (compare<IntTab> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<DoubleTab> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<IntVect> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<DoubleVect> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<ArrOfInt> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<ArrOfDouble> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<Motcle> (obj, obj2, erreur)) { /* Do nothing */ }
              else if (compare<Nom> (obj, obj2, erreur)) { /* Do nothing */ }
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
      DerObjU copie(obj);
      envoyer(copie, 0, 67 /* tag */);
    }

  envoyer_broadcast(erreur, 0 /* pe source */);
  return !erreur;
}

/*! @brief en mode comm_check_enabled(), verifie que le parametre a la meme valeur sur tous les processeurs.
 *
 * Ne fonctionne pas pour tous les types ! Pour l'instant ok pour Noms, Motcles et tableaux.
 *
 */
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
