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

#include <VDF_to_IJK.h>
#include <Schema_Comm_Vecteurs.h>
#include <Schema_Comm.h>
#include <communications.h>
#include <Statistiques.h>
#include <Zone_VF.h>
#include <IJK_Field.h>
#include <TRUSTTabs.h>
#include <Array_tools.h>
#include <Zone.h>

// search non empty lists in src and fills non_empty and dest with non empty lists
static void pack_lists(const VECT(ArrOfInt) & src, ArrOfInt& non_empty, Static_Int_Lists& dest)
{
  // count non empty lists
  int n = 0;
  const int nmax = src.size();
  for (int i = 0; i < nmax; i++)
    if (src[i].size_array() > 0)
      n++;

  VECT(ArrOfInt) tmp(n);
  non_empty.resize_array(n);

  // copy non empty lists into tmp
  int j = 0;
  for (int i = 0; i < nmax; i++)
    {
      if (src[i].size_array() > 0)
        {
          non_empty[j] = i;
          tmp[j] = src[i];
          j++;
        }
    }

  // Built dest
  dest.set(tmp);
}

// Returns 0 if valeur < tab[1],
// returns tab.size_array() - 1 if valeur >= tab[size_array()-1]
// returns n if valeur >= tab[n] && valeur < tab[n+1]
//
static int bsearch_double(const ArrOfDouble& tab, double valeur)
{
  int i = 0;
  int j = tab.size_array() - 1;
  if (valeur >= tab[j])
    return j;
  while (j > i + 1)
    {
      // Le tableau doit etre trie
      assert(j == tab.size_array() || tab[i] <= tab[j]);
      const int milieu = (i + j) / 2;
      const double val = tab[milieu];
      if (val > valeur)
        j = milieu;
      else
        i = milieu;
    }
  return i;
}

static const DoubleTab& get_items_coords(const Zone_VF& zone_vf, IJK_Splitting::Localisation loc, int& nb_items)
{
  switch(loc)
    {
    case IJK_Splitting::ELEM:
      nb_items = zone_vf.zone().nb_elem();
      return zone_vf.xp();
    case IJK_Splitting::NODES:
      nb_items = zone_vf.zone().nb_som();
      return zone_vf.zone().les_sommets();
    default:
      nb_items = zone_vf.nb_faces();
      return zone_vf.xv();
    }
}

void VDF_to_IJK::initialize(const Zone_VF& zone_vf, const IJK_Splitting& splitting,
                            IJK_Splitting::Localisation localisation,
                            int direction_for_x,
                            int direction_for_y,
                            int direction_for_z)
{
  Cerr << "VDF_to_IJK::initialize localisation=" << (int)localisation << endl;
  const int np = Process::nproc();
  const int moi = Process::me();

  const ArrOfDouble& all_coord_i = splitting.get_grid_geometry().get_node_coordinates(0);
  const ArrOfDouble& all_coord_j = splitting.get_grid_geometry().get_node_coordinates(1);
  const ArrOfDouble& all_coord_k = splitting.get_grid_geometry().get_node_coordinates(2);

  // For each direction, global position of the first element of each slice
  VECT(ArrOfInt) slice_offsets(3);
  VECT(ArrOfInt) slice_size(3);
  // For each subdomain, on which mpi process is it ?
  IntTab mapping;
  splitting.get_processor_mapping(mapping);

  for (int i = 0; i < 3; i++)
    {
      splitting.get_slice_offsets(i, slice_offsets[i]);
      splitting.get_slice_size(i, localisation, slice_size[i]);
    }
  Schema_Comm sch;
  ArrOfInt all_pe;
  all_pe.set_smart_resize(1);

  {
    // Ajout pour limiter la porte de la variable !!!
    int pe;
    for (pe = 0; pe < np; pe++)
      if (pe != moi)
        all_pe.append_array(pe);
    sch.set_send_recv_pe_list(all_pe, all_pe, 1 /* me to me */);
  }

  VECT(IntTab) all_data(np);
  for (int pe = 0; pe < np; pe++)
    {
      all_data[pe].set_smart_resize(1);
      all_data[pe].resize(0,2);
    }

  int srccomponent_for_direction[3];
  srccomponent_for_direction[direction_for_x] = 0;
  srccomponent_for_direction[direction_for_y] = 1;
  srccomponent_for_direction[direction_for_z] = 2;

  // Coordinates of geometric items:
  int nb_items;
  // attention, coords.dimension(0) inclut les mailles virtuelles pour le tableau des elements, c'est pas bon,
  // il faut recuperer nb_elem reels...
  const DoubleTab& coords = get_items_coords(zone_vf, localisation, nb_items);
  const IntVect& orientation = zone_vf.orientation();

  sch.begin_comm();
  const double eps = Objet_U::precision_geom;
  for (int item_index = 0; item_index < nb_items; item_index++)
    {
      if (localisation == IJK_Splitting::FACES_I && orientation[item_index] != srccomponent_for_direction[0])
        continue;
      else if (localisation == IJK_Splitting::FACES_J && orientation[item_index] !=  srccomponent_for_direction[1])
        continue;
      else if (localisation == IJK_Splitting::FACES_K && orientation[item_index] != srccomponent_for_direction[2])
        continue;

      double coord[3];
      coord[direction_for_x] = coords(item_index, 0) + eps;
      coord[direction_for_y] = coords(item_index, 1) + eps;
      coord[direction_for_z] = coords(item_index, 2) + eps;

      int ijk[3];
      ijk[0] = bsearch_double(all_coord_i, coord[0]);
      ijk[1] = bsearch_double(all_coord_j, coord[1]);
      ijk[2] = bsearch_double(all_coord_k, coord[2]);


      int slice_index[3]; // index of the subdomain than contains the current element, in each direction
      int local_ijk_size[3];  // size of the ijk subdomain that contains the current element
      int local_ijk_index[3]; // ijk index of the element in the subdomain
      for (int i = 0; i < 3; i++)
        {
          // Wrap for periodic items:
          if (ijk[i] >= splitting.get_nb_items_global(localisation, i))
            ijk[i] = 0;
          int idx = slice_offsets[i].size_array()-1;
          while (slice_offsets[i][idx] > ijk[i])
            idx--;
          slice_index[i] = idx;
          local_ijk_size[i] = slice_size[i][idx];
          local_ijk_index[i] = ijk[i] - slice_offsets[i][idx];
        }
      // which pe has this element in the ijk mesh?
      int pe = mapping(slice_index[0], slice_index[1], slice_index[2]);
      // local index of this element in the ijk mesh (linear index computed without ghost cells)
      int local_index = (local_ijk_index[2] * local_ijk_size[1] + local_ijk_index[1]) * local_ijk_size[0] + local_ijk_index[0];

      all_data[pe].append_line(local_index, item_index);
    }

  VECT(ArrOfInt) to_send(np);
  for (int pe = 0; pe < np; pe++)
    {
      to_send[pe].set_smart_resize(1);
    }

  for (int pe = 0; pe < np; pe++)
    {
      tri_lexicographique_tableau(all_data[pe]);
      const int n = all_data[pe].dimension(0);
      if (n > 0)
        Cerr << "VDF_to_IJK::initialize localisation=" << (int)localisation
             << " found " << n << " items to send to processor " << pe << finl;
      Sortie& buf = sch.send_buffer(pe);
      for (int i = 0; i < n; i++)
        {
          buf << all_data[pe](i,0);
          to_send[pe].append_array(all_data[pe](i,1));
        }
    }
  sch.echange_taille_et_messages();
  VECT(ArrOfInt) to_recv(np);
  for (int pe = 0; pe < np; pe++)
    {
      to_recv[pe].set_smart_resize(1);
    }

  for (int pe = 0; pe < np; pe++)
    {
      to_recv[pe].set_smart_resize(1);
      Entree& buf = sch.recv_buffer(pe);
      for (;;)
        {
          int val;
          buf >> val;
          if (buf.eof())
            break;
          to_recv[pe].append_array(val);
        }
      if (to_recv[pe].size_array() > 0)
        Cerr << "VDF_to_IJK::initialize localisation=" << (int)localisation
             << " found " << to_recv[pe].size_array() << " items to receive from processor " << pe << finl;
    }
  sch.end_comm();
  pack_lists(to_send, pe_send_, pe_send_data_);
  pack_lists(to_recv, pe_recv_, pe_recv_data_);

  ijk_ni_ = splitting.get_nb_elem_local(0);
  ijk_nj_ = splitting.get_nb_elem_local(1);
}
