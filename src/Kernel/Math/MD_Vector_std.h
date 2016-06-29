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
// File:        MD_Vector_std.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MD_Vector_std_included
#define MD_Vector_std_included
#include <MD_Vector_base.h>
#include <ArrOfBit.h>
#include <Static_Int_Lists.h>
#include <Vect_ArrOfInt.h>
#include <MD_Vector_base2.h>

// .DESCRIPTION
//  C'est le plus simple des descripteurs, utilise pour les tableaux de valeurs aux sommets, elements,
//  faces, aretes, faces de bord, etc...
//  Il supporte la notion d'items "communs" (sommets du maillage partages entre plusieurs processeurs)
//  et d'items "distants" et "virtuels" (sommets, elements dans l'epaisseur de joint).
//  Voir aussi MD_Vector_composite (descripteur compose d'une reunion de plusieurs descripteurs, pour le P1Bulle
//  par exemple)
class MD_Vector_std : public MD_Vector_base2
{
  Declare_instanciable_sans_constructeur(MD_Vector_std);
public:
  MD_Vector_std();
  MD_Vector_std(int nb_items_tot,
                int nb_items_reels,
                const ArrOfInt& pe_voisins,
                const VECT(ArrOfInt) & items_to_send,
                const VECT(ArrOfInt) & items_to_recv,
                const VECT(ArrOfInt) & blocs_to_recv);

  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;

  //static void compute_blocs_count(const Static_Int_Lists blocs, ArrOfInt & items_counts);
  //void fill_count_items_to_send_to_items();

  // Numeros des processeurs voisins avec qui j'echange des donnees (meme taille que les VECT suivants)
  ArrOfInt pe_voisins_;
  // Indices des items individuels a envoyer a chaque voisin (une liste par processeur voisin)
  //  Les listes contiennent a la fois les items communs a envoyer et les items "distants" qui seront
  //  stockes dans les espaces virtuels. Les premiers items de cette liste correspondent aux "items_to_recv_"
  //  sur le processeur voisin, les items suivants aux "blocs_to_recv_".
  Static_Int_Lists items_to_send_;
  // Parmi les items to send, combien, sur chaque processeur, seront recus dans des "items_to_recv_" ?
  //  (les suivants sont recus dans des blocs_to_recv_)
  ArrOfInt nb_items_to_items_;
  // Indices des items individuels a recevoir de chaque voisin (en principe, ce sont les items communs recus,
  //  rarement contigus dans les tableaux)
  Static_Int_Lists items_to_recv_;
  // Blocs d'items a recevoir
  // Chaque liste contient debut_bloc1, fin_bloc1, debut_bloc2, fin_bloc2, etc...
  //  attention, fin_bloc est l'indice du dernier element + 1
  // En principe ce sont les items virtuels, qui sont souvent contigus dans les tableaux.
  Static_Int_Lists blocs_to_recv_;
  // Pour accelerer le calcul des tailles de buffer, nombre total d'items mentionnes dans blocs_to_recv_
  //  pour chaque processeur (egal a la somme des fin_bloc-debut_bloc)
  ArrOfInt blocs_items_count_;
};

#endif
