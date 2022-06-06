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

#ifndef communications_included
#define communications_included

#include <communications_array.h>
#include <TRUSTTabs_forward.h>
#include <type_traits>
#include <Vect.h>
#include <vector>

// Pour Objet_U ... on buffer !!
int envoyer(const Objet_U& t, int source, int cible, int canal);
int envoyer(const Objet_U& t, int cible, int canal);
int recevoir(Objet_U& t, int source, int cible, int canal);
int recevoir(Objet_U& t, int source, int canal);
int envoyer_broadcast(Objet_U& t, int source);

void mpsum_multiple(double& x1, double& x2);
int mppartial_sum(int i);
int mp_max(int i);
int mp_min(int i);

void assert_parallel(const Objet_U& obj);
int is_parallel_object(const Objet_U& obj);
int is_parallel_object(const double x);
int is_parallel_object(const int x);

int envoyer_all_to_all(const TRUST_Vector<TRUSTTab,double>& src, TRUST_Vector<TRUSTTab,double>& dest);
int envoyer_all_to_all(const TRUST_Vector<TRUSTArray,int>& src, TRUST_Vector<TRUSTArray,int>& dest);
int envoyer_all_to_all(std::vector<long long>& src, std::vector<long long>& dest);
void envoyer_all_to_all(const DoubleTab& src, DoubleTab& dest);

int reverse_send_recv_pe_list(const ArrOfInt& src_list, ArrOfInt& dest_list);
int comm_check_enabled();

/* ******************************************************************************************************** *
 * FUNCTION TEMPLATE IMPLEMENTATIONS with SFINAE to avoid substitution failure because now IT IS AN ERROR ! *
 * ******************************************************************************************************** */

// Pour les types simples, on passe par envoyer_array_ qui n'utilise pas un buffer mais envoie directement les valeurs. Plus rapide.
// TYPES SIMPLES (std::is_arithmetic) => PAS Objet_U => SFINAE
template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline envoyer(const _TYPE_ t, int source, int cible, int canal)
{
  return envoyer_array<_TYPE_>(&t, 1, source, cible, canal);
}

#ifndef INT_is_64_
inline int envoyer(const long t, int source, int cible, int canal) { return envoyer_array<long>(&t, 1, source, cible, canal); }
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline envoyer(const _TYPE_ t, int cible, int canal)
{
  return envoyer_array<_TYPE_>(&t, 1, Process::me(), cible, canal);
}

#ifndef INT_is_64_
inline int envoyer(const long t, int cible, int canal) { return envoyer_array<long>(&t, 1, Process::me(), cible, canal); }
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline recevoir(_TYPE_& t, int source, int cible, int canal)
{
  return recevoir_array<_TYPE_>(&t, 1, source, cible, canal);
}

#ifndef INT_is_64_
inline int recevoir(long& t, int source, int cible, int canal) { return recevoir_array<long>(&t, 1, source, cible, canal); }
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline recevoir(_TYPE_& t, int source, int canal)
{
  return recevoir_array<_TYPE_>(&t, 1, source, Process::me(), canal);
}

#ifndef INT_is_64_
inline int recevoir(long& t, int source, int canal) { return recevoir_array<long>(&t, 1, source, Process::me(), canal); }
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline envoyer_broadcast(_TYPE_& t, int source)
{
  return envoyer_broadcast_array<_TYPE_>(&t, 1, source);
}

#ifndef INT_is_64_
inline int envoyer_broadcast(long& t, int source) { return envoyer_broadcast_array<long>(&t, 1, source); }
#endif

// Description: en mode comm_check_enabled(), verifie que le parametre a la meme valeur sur tous les processeurs
template<typename _TYPE_>
typename std::enable_if<(std::is_same<_TYPE_, int>::value || std::is_same<_TYPE_, double>::value),void >::type
inline assert_parallel(const _TYPE_ x)
{
  if (!Comm_Group::check_enabled())
    return;
  if (!is_parallel_object(x))
    {
      Cerr << "Fatal error in template<typename _TYPE_>assert_parallel(const _TYPE_ x). See log files" << finl;
      Process::Journal() << "Error in template<typename _TYPE_>assert_parallel(const _TYPE_ x)" << "This processor has this value: " << x << finl;
      Process::barrier();
      Process::exit();
    }
}

template<typename _TYPE_>
inline void mp_collective_op(TRUSTArray<_TYPE_>& x, Comm_Group::Collective_Op op)
{
  int sz = x.size_array();
  assert_parallel<_TYPE_>(sz);
  if (sz > 0)
    {
      _TYPE_ *data = x.addr();
      _TYPE_ *tmp = new _TYPE_[sz];
      const Comm_Group& grp = PE_Groups::current_group();
      grp.mp_collective_op(data, tmp, sz, op);
      memcpy(data, tmp, sz * sizeof(_TYPE_));
      delete[] tmp;
    }
}

template<typename _TYPE_>
inline void mp_sum_for_each_item(TRUSTArray<_TYPE_>& x) { mp_collective_op(x, Comm_Group::COLL_SUM); }

template<typename _TYPE_>
inline void mp_max_for_each_item(TRUSTArray<_TYPE_>& x) { mp_collective_op(x, Comm_Group::COLL_MAX); }

template<typename _TYPE_>
inline void mp_min_for_each_item(TRUSTArray<_TYPE_>& x) { mp_collective_op(x, Comm_Group::COLL_MIN); }

// Description: On suppose que les tableaux en entree et en sortie sont de taille nproc() . On envoie src[0] au proc 0,
//  src[1] au proc 1, etc... la valeur recue du processeur 0 et mise dans dest[0], processeur 1 dans dest[1], etc...
//  Il est autorise d'appeler la fonction avec le meme tableau src et dest.
template<typename _TYPE_>
inline int envoyer_all_to_all(const TRUSTArray<_TYPE_>& src, TRUSTArray<_TYPE_>& dest)
{
  const Comm_Group& grp = PE_Groups::current_group();
  assert(src.size_array() == grp.nproc());
  assert(dest.size_array() == grp.nproc());
  if (src.addr() == dest.addr())
    {
      TRUSTArray<_TYPE_> tmp;
      tmp.resize_array(grp.nproc(), Array_base::NOCOPY_NOINIT);
      grp.all_to_all(src.addr(), tmp.addr(), sizeof(_TYPE_));
      dest.inject_array(tmp);
    }
  else
    grp.all_to_all(src.addr(), dest.addr(), sizeof(_TYPE_));
  return 1;
}

#endif /* communications_included */
