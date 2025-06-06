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

#ifndef MD_Vector_tools_included
#define MD_Vector_tools_included

#include <TRUSTTabs_forward.h>
#include <Array_base.h>
#include <MD_Vector.h>
#include <ArrOfBit.h>
#include <array>
#include <map>

class MD_Vector_renumber;
class Domaine_VF;
class Echange_EV_Options;

//format de structure pour demander d'agrandir un MD_Vector : (proc, item sur le proc) -> item distant sur Process::me()
typedef std::map<std::array<int, 2>, int> extra_item_t;

class MD_Vector_tools
{
public:
  // ECHANGE_EV: echange traditionnel
  // EV_SOMME: items sequentiels = somme sur tous les procs qui partagent l'item
  // EV_SOMME_ECHANGE: idem, suivi d'un ECHANGE_EV pour mettre a jour les items virtuels
  // EV_MIN: items sequentiel = min sur tous les procs
  // EV_MIN_COLONNE1: pour chaque ligne partagee du tableau, prend le processeur qui a la plus petite valeur
  //   dans la colonne1, envoie toute la ligne au processeur qui possede la ligne.
  enum Operations_echange { ECHANGE_EV, EV_SOMME, EV_SOMME_ECHANGE, EV_MAX, EV_MINCOL1 };

  static void creer_tableau_distribue(const MD_Vector&, Array_base&, RESIZE_OPTIONS opt=RESIZE_OPTIONS::COPY_INIT);

  static void echange_espace_virtuel(IntVect&, Operations_echange opt=ECHANGE_EV);
#if INT_is_64_ == 2
  static void echange_espace_virtuel(TIDVect& v, Operations_echange opt=ECHANGE_EV);
#endif
  static void echange_espace_virtuel(DoubleVect&, Operations_echange opt=ECHANGE_EV);
  static void echange_espace_virtuel(FloatVect&, Operations_echange opt=ECHANGE_EV);

  // valeur de retour: nombre d'items sequentiels sur ce proc (nombre de flags a un dans le tableau)
  static void compute_sequential_items_index(const MD_Vector&, MD_Vector_renumber&, int line_size = 1);
  static void creer_md_vect_renum(const IntVect& renum, MD_Vector& md_vect);
  static void creer_md_vect_renum_auto(IntVect& flags_renum, MD_Vector& md_vect);

  static void dump_vector_with_md(const DoubleVect&, Sortie&);
  static void restore_vector_with_md(DoubleVect&, Entree&);

  static void CleanMyStatics() { last_md.detach(); }

private:
  template <typename _TYPE_>
  static void echange_espace_virtuel_(const MD_Vector& md, TRUSTVect<_TYPE_>& v, const Echange_EV_Options& opt);
  template<typename _TYPE_>
  static void echange_espace_virtuel1_(const MD_Vector& md, TRUSTVect<_TYPE_>& v, MD_Vector_tools::Operations_echange opt);
  template<typename _TYPE_>
  static void call_echange_espace_virtuel(TRUSTVect<_TYPE_>& v, MD_Vector_tools::Operations_echange opt);

  static Schema_Comm_Vecteurs comm;
  static MD_Vector last_md;
  static int last_isdouble;
  static int last_linesize;
  static Echange_EV_Options last_opt;
};

#endif /* MD_Vector_tools_included */
