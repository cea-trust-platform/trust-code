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
#ifndef Partitionneur_base_included
#define Partitionneur_base_included

#include <TRUSTTabs_forward.h>
#include <Noms.h>
#include <Static_Int_Lists.h>

#include <Domaine_forward.h>
class Param;
#include <Domaine_forward.h>

/*! @brief Classe de base des partitionneurs de domaine (pour decouper un maillage avant un calcul parallele).
 *
 *   Cette classe decrit l'interface commune a tous les partitionneurs:
 *   Exemple :
 *
 *    // Creation d'un instance de la classe:
 *    Partitionneur_xxx part;
 *    // Association du domaine de calcul a decouper
 *    part.associer_domaine(domaine);
 *    // Initialisation des parametres specifiques a la classe derivee!
 *        fichier_data >> part;       // Lecture des parametres dans le jdd
 *    ou
 *        part.initialiser(nb_parties); // Initialisation directe (depend de la classe)
 *    // Si besoin :
 *    part.declarer_bords_periodiques(liste_noms_bords_perio);
 *    // Construction du tableau de decoupage:
 *    ArrOfInt elem_part;
 *    part.construire_partition(elem_part);
 *
 *   ATTENTION: en principe, l'implementation de la methode construire_partition()
 *    doit assurer que le decoupage est valide (application des corrections pour les bords
 *    periodiques et autres...)
 *
 */
template <typename _SIZE_>
class Partitionneur_base_32_64 : public Objet_U
{
  Declare_base_32_64(Partitionneur_base_32_64);

public:
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Static_Int_Lists_t = Static_Int_Lists_32_64<_SIZE_>;
  using Bord_t = Bord_32_64<_SIZE_>;

  using BigArrOfInt_ = TRUSTArray<int, _SIZE_>;  // always int as value type, will hold proc/partition number.
  using BigIntVect_ = TRUSTVect<int, _SIZE_>;

  virtual void set_param(Param& param)=0;
  int lire_motcle_non_standard(const Motcle&, Entree&) override { return -1; }
  virtual void associer_domaine(const Domaine_t& domaine) = 0;
  virtual void declarer_bords_periodiques(const Noms& noms_bords_periodiques) { liste_bords_periodiques_ = noms_bords_periodiques; }
  virtual void construire_partition(BigIntVect_& elem_part, int& nb_parts_tot) const = 0;

  static void corriger_elem0_sur_proc0(BigIntVect_& elem_part);

  static int_t calculer_graphe_connexions_periodiques(const Domaine_t& domaine, const Noms& liste_bords_periodiques,
                                                      const Static_Int_Lists_t& som_elem, const int_t my_offset,
                                                      Static_Int_Lists_t& graph);

  static int_t corriger_bords_avec_graphe(const Static_Int_Lists_t& graph_elements_perio, const Static_Int_Lists_t& som_elem,
                                          const Domaine_t& domaine, const Noms& liste_bords_perio, BigIntVect_& elem_part);
  static void corriger_bords_avec_liste(const Domaine_t& dom, const Noms& liste_bords_periodiques, const int_t my_offset, BigIntVect_& elem_part);
  static int_t corriger_sommets_bord(const Domaine_t& domaine, const Noms& liste_bords_perio, const ArrOfInt_t& renum_som_perio,
                                     const Static_Int_Lists_t& som_elem, BigIntVect_& elem_part);
  static int_t corriger_multiperiodique(const Domaine_t& domaine, const Noms& liste_bords_perio, const ArrOfInt_t& renum_som_perio,
                                        const Static_Int_Lists_t& som_elem, BigIntVect_& elem_part);

protected:
  Noms liste_bords_periodiques_;
};

using Partitionneur_base = Partitionneur_base_32_64<int>;
using Partitionneur_base_64 = Partitionneur_base_32_64<trustIdType>;

#endif
