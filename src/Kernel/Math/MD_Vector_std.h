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

#ifndef MD_Vector_std_included
#define MD_Vector_std_included

#include <Schema_Comm_Vecteurs.h>
#include <Echange_EV_Options.h>
#include <Static_Int_Lists.h>
#include <MD_Vector_base2.h>
#include <communications.h>
#include <TRUSTArrays.h>
#include <TRUSTVect.h>
#include <ArrOfBit.h>

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
  MD_Vector_std(int nb_items_tot, int nb_items_reels, const ArrOfInt& pe_voisins,
                const ArrsOfInt& items_to_send, const ArrsOfInt& items_to_recv, const ArrsOfInt& blocs_to_recv);

  inline void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, DoubleVect& v) const override { initialize_comm_template<double>(opt,sc,v); }
  inline void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, IntVect& v) const override { initialize_comm_template<int>(opt,sc,v); }
  inline void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, DoubleVect& v) const override { prepare_send_data_template<double>(opt,sc,v); }
  inline void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, IntVect& v) const override { prepare_send_data_template<int>(opt,sc,v); }
  inline void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, DoubleVect& v) const override { process_recv_data_template<double>(opt,sc,v); }
  inline void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& sc, IntVect& v) const override { process_recv_data_template<int>(opt,sc,v); }

  // Numeros des processeurs voisins avec qui j'echange des donnees (meme taille que les VECT suivants)
  ArrOfInt pe_voisins_;
  // Indices des items individuels a envoyer a chaque voisin (une liste par processeur voisin)
  //  Les listes contiennent a la fois les items communs a envoyer et les items "distants" qui seront
  //  stockes dans les espaces virtuels. Les premiers items de cette liste correspondent aux "items_to_recv_"
  //  sur le processeur voisin, les items suivants aux "blocs_to_recv_".
  Static_Int_Lists items_to_send_;
  // Parmi les items to send, combien, sur chaque processeur, seront recus dans des "items_to_recv_" ? (les suivants sont recus dans des blocs_to_recv_)
  ArrOfInt nb_items_to_items_;
  // Indices des items individuels a recevoir de chaque voisin (en principe, ce sont les items communs recus, rarement contigus dans les tableaux)
  Static_Int_Lists items_to_recv_;
  // Blocs d'items a recevoir
  // Chaque liste contient debut_bloc1, fin_bloc1, debut_bloc2, fin_bloc2, etc... attention, fin_bloc est l'indice du dernier element + 1
  // En principe ce sont les items virtuels, qui sont souvent contigus dans les tableaux.
  Static_Int_Lists blocs_to_recv_;
  // Pour accelerer le calcul des tailles de buffer, nombre total d'items mentionnes dans blocs_to_recv_ pour chaque processeur (egal a la somme des fin_bloc-debut_bloc)
  ArrOfInt blocs_items_count_;

private:
  template <typename _TYPE_>
  void initialize_comm_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TRUSTVect<_TYPE_>&) const;

  template <typename _TYPE_>
  void prepare_send_data_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TRUSTVect<_TYPE_>&) const;

  template <typename _TYPE_>
  void process_recv_data_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TRUSTVect<_TYPE_>&) const;
};

#include <MD_Vector_std_tools.tpp>

template<typename _TYPE_>
void MD_Vector_std::initialize_comm_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm_, TRUSTVect<_TYPE_>& v) const
{
  const int reverse = (opt.get_op() != Echange_EV_Options::SYNC);
  const int have_items_to_recv = (items_to_recv_.get_data().size_array() > 0);
  const int have_blocs_to_recv = (blocs_to_recv_.get_data().size_array() > 0);
  const int nprocs = pe_voisins_.size_array();
  for (int i = 0; i < nprocs; i++)
    {
      const int pe = pe_voisins_[i];
      int sz1 = items_to_send_.get_list_size(i), sz2 = 0;

      if (have_items_to_recv) sz2 = items_to_recv_.get_list_size(i);
      if (have_blocs_to_recv) sz2 += blocs_items_count_[i];
      if (reverse)
        {
          // Schema a l'envers: lecture dans les items to recv, ecriture dans les items to send
          int tmp = sz1;
          sz1 = sz2;
          sz2 = tmp;
        }
      comm_.add_send_area_template < _TYPE_ > (pe, sz1 * v.line_size());
      comm_.add_recv_area_template < _TYPE_ > (pe, sz2 * v.line_size());
    }
}

// Description: This is the first part of the echange_espace_virtuel() process.
//  We take the data that has to be sent to other processors in "vect" and put it in appropriate send buffers in "buffers". The data taken depends on "opt".
// Preconditon: The buffers must have been initialized to the appropriate size and neighbour processors.
template<typename _TYPE_>
void MD_Vector_std::prepare_send_data_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& buffers,  TRUSTVect<_TYPE_>& vect) const
{
  const int line_size = vect.line_size();
  switch(opt.get_op())
    {
    case Echange_EV_Options::SYNC:
      read_from_vect_items(line_size, pe_voisins_, items_to_send_, vect, buffers);
      break;
    case Echange_EV_Options::SUM:
    case Echange_EV_Options::MAX:
    case Echange_EV_Options::MINCOL1:
      if (items_to_recv_.get_data().size_array() > 0)
        read_from_vect_items(line_size, pe_voisins_, items_to_recv_, vect, buffers);
      if (blocs_to_recv_.get_data().size_array() > 0)
        read_from_vect_blocs(line_size, pe_voisins_, blocs_to_recv_, blocs_items_count_, vect, buffers);
      break;
    default:
      Cerr << "Error in MD_Vector_std.tpp prepare_send_data_template : operation not implemented" << finl;
      Process::exit();
    }
}

template<typename _TYPE_>
void MD_Vector_std::process_recv_data_template(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& buffers, TRUSTVect<_TYPE_>& vect) const
{
  const int line_size = vect.line_size();
  switch(opt.get_op())
    {
    case Echange_EV_Options::SYNC:
      if (items_to_recv_.get_data().size_array() > 0)
        write_to_vect_items(line_size, pe_voisins_, items_to_recv_, vect, buffers);
      if (blocs_to_recv_.get_data().size_array() > 0)
        write_to_vect_blocs(line_size, pe_voisins_, blocs_to_recv_, blocs_items_count_, vect, buffers);
      break;
    case Echange_EV_Options::SUM:
      add_to_vect_items(line_size, pe_voisins_, items_to_send_, vect, buffers);
      break;
    case Echange_EV_Options::MAX:
      max_to_vect_items(line_size, pe_voisins_, items_to_send_, vect, buffers);
      break;
    case Echange_EV_Options::MINCOL1:
      mincol1_to_vect_items(line_size, pe_voisins_, items_to_send_, vect, buffers);
      break;
    default:
      Cerr << "Error in MD_Vector_std.tpp process_recv_data_template : operation not implemented" << finl;
      Process::exit();
    }
}

#endif /* MD_Vector_std_included */
