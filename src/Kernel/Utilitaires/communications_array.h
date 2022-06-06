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

#ifndef communications_array_included
#define communications_array_included

#include <Comm_Group.h>
#include <PE_Groups.h>
#include <arch.h>

// Ces methodes sont volontairement separees de communications.h car plus dangereuses a l'usage.
// Les tableaux passes en parametres doivent absolument avoir au moins n cases et n doit etre
// identique sur tous les processeurs qui communiquent entre eux.

// Attention: le template ne marche que pour les types simples (pas Objet_U !)
// On ne passe pas par un buffer Entree / Sortie mais on envoie directement le tableau sous sa forme binaire.
// Un seul message envoye, sauf en mode check() ou on envoie aussi la taille pour verifier.
template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline envoyer_array(const _TYPE_ *objet, int n, int source, int cible, int canal)
{
  const Comm_Group& grp = PE_Groups::current_group();
  const int moi = grp.rank();
  if (moi == cible)
    return 0;

  if (source != moi && source != -1)
    return 0;

  const char *data = (const char*) objet;
  const int sz = sizeof(_TYPE_) * n;
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

#ifndef INT_is_64_
inline int envoyer_array(const long *t, int n, int source, int cible, int canal)
{
  return envoyer_array<long>(t,n,source,cible,canal);
}
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline recevoir_array(const _TYPE_ *objet, int n, int source, int cible, int canal)
{
  const Comm_Group& grp = PE_Groups::current_group();
  const int moi = grp.rank();
  if (moi == source)
    return 0;

  if (cible != moi && cible != -1)
    return 0;

  const int sz = sizeof(_TYPE_) * n;
  if (grp.check_enabled())
    {
      int sz_check;
      grp.recv(source, &sz_check, sizeof(int), canal);
      if (sz_check != sz)
        {
          Cerr << "Fatal error in template<typename _TYPE_> int recevoir_array : incorrect size\n" << " sent=" << sz_check << " expected=" << sz << finl;
          Process::exit();
        }
    }
  char *data = (char*) objet;
  if (sz > 0)
    grp.recv(source, data, sz, canal);
  return 1;
}

#ifndef INT_is_64_
inline int recevoir_array(const long *t, int n, int source, int cible, int canal)
{
  return recevoir_array<long>(t, n, source, cible, canal);
}
#endif

template<typename _TYPE_>
typename std::enable_if<std::is_arithmetic<_TYPE_>::value,int >::type
inline envoyer_broadcast_array(_TYPE_ *objet, int n, int source)
{
  const Comm_Group& grp = PE_Groups::current_group();
  if (grp.check_enabled())
    {
      int copie_n = n;
      grp.broadcast(&copie_n, sizeof(int), source);
      if (copie_n != n)
        {
          Cerr << "Error in template<typename _TYPE_> envoyer_broadcast_array !" << finl;
          Process::exit();
        }
    }
  grp.broadcast(objet, sizeof(_TYPE_) * n, source);
  return 1;
}

#ifndef INT_is_64_
inline int envoyer_broadcast_array(long *t, int n, int source)
{
  return envoyer_broadcast_array<long>(t, n, source);
}
#endif

#endif /* communications_array_included */
