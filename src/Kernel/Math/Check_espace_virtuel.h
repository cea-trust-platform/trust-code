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

#ifndef Check_espace_virtuel_H
#define Check_espace_virtuel_H

#include <Comm_Group.h>
#include <TRUSTVect.h>

// Renvoie 1 si l'espace virtuel de v est a jour, 0 sinon
int check_espace_virtuel_vect(const DoubleVect& v);
int check_espace_virtuel_vect(const IntVect& v);
void assert_invalide_items_non_calcules(DoubleVect& v, double valeur = 0.);

template <typename _TYPE_>
inline void remplir_items_non_calcules_(TRUSTVect<_TYPE_>& v, _TYPE_ valeur)
{
  if (v.get_md_vector().non_nul())
    {
      const ArrOfInt& blocs = v.get_md_vector().valeur().get_items_to_compute();
      const int sz = blocs.size_array() / 2, line_size = v.line_size();
      int j = 0;
      // Ne pas passer par operator[], sinon plantage si la valeur actuelle est invalide
      _TYPE_ *ptr = v.addr();
      for (int i = 0; i < sz; i++)
        {
          // remplir les elements jusqu'au debut du bloc:
          const int j_fin = blocs[i*2] * line_size;
          assert(j >= 0 && j_fin <= v.size_array());
          for (; j < j_fin; j++) ptr[j] = valeur;
          // Sauter a la fin du bloc
          j = blocs[i*2+1] * line_size;
        }
      // Remplir les elements entre la fin du dernier bloc et la fin du vecteur
      const int j_fin = v.size_array();
      for (; j < j_fin; j++) ptr[j] = valeur;
    }
}

// Description: Remplit les "items non calcules" du tableau avec une valeur invalide. Ce sont tous les items qui ne sont pas repertories
//  dans v.get_md_vector().valeurs().get_items_to_compute(). (items non calcules par les operations par defaut sur les vecteurs, en general ce sont les items virtuels)
//  Il est conseille d'appliquer cette methode a la fin des fonctions qui ne renvoient pas un espace virtuel a jour avec declare_espace_virtuel_invalide(...))
//  de sorte a provoquer une erreur si l'espace virtuel est utilise.
template<typename _TYPE_>
inline void remplir_items_non_calcules(TRUSTVect<_TYPE_>& v, _TYPE_ valeur = 0)
{
  remplir_items_non_calcules_(v, valeur);
}

// Description: en mode comm_check_enabled(), verifie si l'espace virtuel du vecteur est a jour, si ce n'est pas le cas, exit().
// Ce test n'est fait qu'en mode comm_check_enabled() car il necessite des communications.
template<typename _TYPE_>
inline void assert_espace_virtuel_vect(const TRUSTVect<_TYPE_>& v)
{
  if (Comm_Group::check_enabled())
    {
      if (! check_espace_virtuel_vect(v))
        {
          Cerr << "Fatal error in assert_espace_virtuel_vect: virtual space of this vector is not up to date." << finl;
          Process::barrier();
          Process::exit();
        }
    }
}

// Description: When compiled without NDEBUG or when running with check_enabled flag,
//  fills all "not computed" items with an invalid value (ie, items that are not VECT_REAL_ITEMS, usually, these are the virtual items).
//  You should call this method whenever you compute some field values but you don't compute or update the virtual space.
template<typename _TYPE_>
inline void declare_espace_virtuel_invalide(TRUSTVect<_TYPE_>& v)
{
  const _TYPE_ valeur = (std::is_same<_TYPE_,double>::value) ? -98765.4321 : -1999999999;
#ifdef NDEBUG
  if (Comm_Group::check_enabled())
    remplir_items_non_calcules(v, valeur);
#else
  remplir_items_non_calcules(v, valeur);
#endif
}

#endif /* Check_espace_virtuel_H */
