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
#ifndef Partitionneur_base_included
#define Partitionneur_base_included

#include <TRUSTTabs_forward.h>
#include <Noms.h>

class Static_Int_Lists;
class Domaine;
class Param;
class Domaine;

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
class Partitionneur_base : public Objet_U
{
  Declare_base(Partitionneur_base);
public:
  virtual void set_param(Param& param)=0;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual void associer_domaine(const Domaine& domaine) = 0;
  virtual void declarer_bords_periodiques(const Noms& noms_bords_periodiques);
  virtual void construire_partition(IntVect& elem_part, int& nb_parts_tot) const = 0;

  static void corriger_elem0_sur_proc0(IntVect& elem_part);
  static int calculer_graphe_connexions_periodiques(const Domaine& domaine, const Noms& liste_bords_periodiques, const Static_Int_Lists& som_elem, const int my_offset, Static_Int_Lists& graph);
  static int corriger_bords_avec_graphe(const Static_Int_Lists& graph_elements_perio, const Static_Int_Lists& som_elem, const Domaine& domaine, const Noms& liste_bords_perio, IntVect& elem_part);
  static void corriger_bords_avec_liste(const Domaine& dom, const Noms& liste_bords_periodiques, const int my_offset, IntVect& elem_part);
  static int corriger_sommets_bord(const Domaine& domaine, const Noms& liste_bords_perio, const ArrOfInt& renum_som_perio, const Static_Int_Lists& som_elem, IntVect& elem_part);
  static int corriger_multiperiodique(const Domaine& domaine, const Noms& liste_bords_perio, const ArrOfInt& renum_som_perio, const Static_Int_Lists& som_elem, IntVect& elem_part);

protected:
  Noms liste_bords_periodiques_;
};

#endif
