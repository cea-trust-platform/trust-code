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

#include <IJK_Splitting.h>
#include <IJK_Field.h>
#include <TRUSTTab.h>
#include <IJK_Field.h>
#include <Param.h>
#include <Interprete_bloc.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur(IJK_Splitting, "IJK_Splitting", Objet_U);
Implemente_ref(IJK_Splitting);

// XD IJK_Splitting objet_u IJK_Splitting -1 Object to specify how the domain will be divided between processors in IJK discretization

IJK_Splitting::IJK_Splitting()
{
  for (int i = 0; i < 3; i++)
    {
      nb_elem_local_[i] = 0;
      nb_nodes_local_[i] = 0;
      for (int j = 0; j < 3; j++)
        nb_faces_local_[i][j] = 0;
      offset_[i] = 0;
      for (int j = 0; j < 2; j++)
        neighbour_processors_[j][i] = -1;
      nproc_per_direction_[i] = 0;
    }
  offsets_all_slices_.dimensionner(3);
  sizes_all_slices_.dimensionner(3);
}

Sortie& IJK_Splitting::printOn(Sortie& os) const
{
  return os;
}

Entree& IJK_Splitting::readOn(Entree& is)
{
  Cerr << "Reading IJK_Splitting parameters (" << le_nom() << ")" << finl;
  Param param(que_suis_je());
  Nom geom_name;
  int ni, nj, nk;
  int group_i = 1, group_j = 1, group_k = 1;
  param.ajouter("ijk_grid_geometry", &geom_name, Param::REQUIRED); // XD_ADD_P ref_IJK_Grid_Geometry the grid that will be splitted
  param.ajouter("nproc_i", &ni, Param::REQUIRED); // XD_ADD_P entier the number of processors into which we will divide the grid following the I direction
  param.ajouter("nproc_j", &nj, Param::REQUIRED); // XD_ADD_P entier the number of processors into which we will divide the grid following the J direction
  param.ajouter("nproc_k", &nk, Param::REQUIRED); // XD_ADD_P entier the number of processors into which we will divide the grid following the K direction
  param.ajouter("process_grouping_i", &group_i);
  param.ajouter("process_grouping_j", &group_j);
  param.ajouter("process_grouping_k", &group_k);
  param.lire_avec_accolades(is);

  Cerr << " nproc in i, j, k directions = " << ni << " " << nj << " " << nk << finl;
  Cerr << " grouping processes in i, j, k directions (node topology) = "
       << group_i << " " << group_j << " " << group_k << finl;
  // Fetch the geometry object from its name in the list of instanciated objects:
  const IJK_Grid_Geometry& geom = ref_cast(IJK_Grid_Geometry, Interprete_bloc::objet_global(geom_name));

  initialize(geom, ni, nj, nk, group_i, group_j, group_k);
  return is;
}


void IJK_Splitting::get_processor_mapping(IntTab& mapping) const
{
  mapping = mapping_;
}

void IJK_Splitting::get_slice_offsets(int dir, ArrOfInt& tab) const
{
  tab = offsets_all_slices_[dir];
}

void IJK_Splitting::get_slice_size(int dir, Localisation loc, ArrOfInt& tab) const
{
  tab = sizes_all_slices_[dir];
  const int n = tab.size_array() - 1;
  if (! get_grid_geometry().get_periodic_flag(dir))
    {
      switch(loc)
        {
        case NODES:
          tab[n]++;
          break;
        case FACES_I:
          if (dir==0) tab[n]++;
          break;
        case FACES_J:
          if (dir==1) tab[n]++;
          break;
        case FACES_K:
          if (dir==2) tab[n]++;
          break;
        default:
          break;
        }
    }
}



// Builds a splitting of the given geometry on the requested number of processors in each direction
// process_grouping allows to rearrange process ranks by packets of ni*nj*nk processes to match the
// topology of the cluster/node (eg, on a 8 cores/node machine, use groups of size 2x2x2 to minimize
// extra-node messages)
void IJK_Splitting::initialize(const IJK_Grid_Geometry& grid_geom,
                               int nproc_i, int nproc_j, int nproc_k,
                               int process_grouping_i, int process_grouping_j, int process_grouping_k)
{
  const int available_nproc = Process::nproc();
  if (nproc_i * nproc_j * nproc_k > available_nproc)
    {
      Cerr << "Error in IJK_Splitting::initialize(nproc_i=" << nproc_i
           << ", nproc_j=" << nproc_j
           << ", nproc_k=" << nproc_k
           << "): requested splitting larger than available processor number (" << available_nproc << ")" << endl;
      Process::exit();
    }

  if (nproc_i > grid_geom.get_nb_elem_tot(0)
      || nproc_j > grid_geom.get_nb_elem_tot(1)
      || nproc_k > grid_geom.get_nb_elem_tot(2))
    {
      Cerr << "Error in IJK_Splitting::initialize(nproc_i=" << nproc_i
           << ", nproc_j=" << nproc_j
           << ", nproc_k=" << nproc_k
           << "): requested splitting larger than total number of cells in one direction";
      Cerr << "\n (number of cells: "
           << grid_geom.get_nb_elem_tot(0) << " "
           << grid_geom.get_nb_elem_tot(1) << " "
           << grid_geom.get_nb_elem_tot(2) << ")" << endl;
      Process::exit();
    }

  IntTab mapping;
  VECT(ArrOfInt) sizes_all_slices(3);
  // Compute processor mapping
  mapping.resize(nproc_i, nproc_j, nproc_k);

  // master processor computes splitting:
  if (Process::je_suis_maitre())
    {
      // Try to group subdomains by n in each direction on consecutive mpi processes:
      Int3 process_grouping;
      process_grouping[0] = process_grouping_i;
      process_grouping[1] = process_grouping_j;
      process_grouping[2] = process_grouping_k;
      Int3 nprocdir;
      nprocdir[0] = nproc_i;
      nprocdir[1] = nproc_j;
      nprocdir[2] = nproc_k;
      Int3 ngroups;
      for (int i = 0; i < 3; i++)
        {
          while (nprocdir[i] % process_grouping[i] != 0)
            process_grouping[i] /= 2;
          ngroups[i] = nprocdir[i] / process_grouping[i];
        }

      // Build processor mapping
      // Loop on groups of processes (idea is that one group maps to one computer node)
      for (int k = 0; k < ngroups[2]; k++)
        {
          for (int j = 0; j < ngroups[1]; j++)
            {
              for (int i = 0; i < ngroups[0]; i++)
                {
                  // Loop on processes in the group
                  for (int k2 = 0; k2 < process_grouping[2]; k2++)
                    {
                      for (int j2 = 0; j2 < process_grouping[1]; j2++)
                        {
                          for (int i2 = 0; i2 < process_grouping[0]; i2++)
                            {
                              int p = k;
                              p = (p * ngroups[1]) + j;
                              p = (p * ngroups[0]) + i;
                              p = (p * process_grouping[2]) + k2;
                              p = (p * process_grouping[1]) + j2;
                              p = (p * process_grouping[0]) + i2;
                              int iposition = i * process_grouping[0] + i2;
                              int jposition = j * process_grouping[1] + j2;
                              int kposition = k * process_grouping[2] + k2;
                              mapping(iposition, jposition, kposition) = p;
                            }
                        }
                    }
                }
            }
        }

      // Compute mesh slicing
      for (int i = 0; i < 3; i++)
        {
          const int n = mapping.dimension(i);
          const int ne = grid_geom.get_nb_elem_tot(i);
          sizes_all_slices[i].resize_array(n);
          for (int j = 0; j < n; j++)
            {
              sizes_all_slices[i][j] = (ne * (j+1) / n) - (ne * j / n);
            }
        }
    }
  // broadcast to all mpi processes:
  envoyer_broadcast(mapping, 0);
  envoyer_broadcast(sizes_all_slices, 0);

  // Initialize object with these data:
  initialize(grid_geom, sizes_all_slices[0], sizes_all_slices[1], sizes_all_slices[2], mapping);
}

#define print_vect(x) (Nom("[") + Nom(x[0]) + Nom(" ") + Nom(x[1]) + Nom(" ") + Nom(x[2]) + Nom("]"))

// Create a splitting of an IJK geometry by specifying the slice sizes and the processor mapping
// slice_size_i contains, for each slice in the i direction, the number of cells in this slice.
// The total cell number in directions i,j,k must match the total number of cells in "geom" in each direction.
// The number of slices in direction i, j , k must match dimensions 0,1 and 2 of the mapping() array.
// processor_mapping(i,j,k) provides the rank of the mpi process that will have this subdomain.
//  (all processors do not have to be used).
void IJK_Splitting::initialize(const IJK_Grid_Geometry& geom, const ArrOfInt& slice_size_i, const ArrOfInt& slice_size_j, const ArrOfInt& slice_size_k, const IntTab& processor_mapping)
{
  assert(slice_size_i.size_array() == processor_mapping.dimension(0));
  assert(slice_size_j.size_array() == processor_mapping.dimension(1));
  assert(slice_size_k.size_array() == processor_mapping.dimension(2));
// assert_parallel(slice_size_i); // check that same value on all processors
// assert_parallel(slice_size_j);
// assert_parallel(slice_size_k);
// assert_parallel(processor_mapping);

  grid_geom_ = geom;
  mapping_ = processor_mapping;
  sizes_all_slices_[0] = slice_size_i;
  sizes_all_slices_[1] = slice_size_j;
  sizes_all_slices_[2] = slice_size_k;
  for (int i = 0; i < 3; i++)
    {
      const int n = sizes_all_slices_[i].size_array();
      nproc_per_direction_[i] = n;
      offsets_all_slices_[i].resize_array(n);
      offsets_all_slices_[i][0] = 0;
      for (int j = 1; j < n; j++)
        {
          offsets_all_slices_[i][j] = offsets_all_slices_[i][j-1] + sizes_all_slices_[i][j-1];
        }
      assert(offsets_all_slices_[i][n-1] + sizes_all_slices_[i][n-1] == geom.get_nb_elem_tot(i));
    }
  // Find my rank in the processor mapping, fill processor_position_
  bool ok = false;
  processor_position_[0] = processor_position_[1] = processor_position_[2] = -1; // default, if I'm not in the mapping.
  {
    int pos_i = -1, pos_j = -1, pos_k = -1;
    const int myrank = Process::me();
    for (pos_k = 0; pos_k < nproc_per_direction_[2]; pos_k++)
      {
        for (pos_j = 0; pos_j < nproc_per_direction_[1]; pos_j++)
          {
            for (pos_i = 0; pos_i < nproc_per_direction_[0]; pos_i++)
              {
                int numproc = mapping_(pos_i,pos_j,pos_k);
                assert(numproc >= 0 && numproc < Process::nproc());
                if (numproc == myrank)
                  {
                    assert(!ok); // check that processor has not yet been visited
                    ok = true;
                    processor_position_[0] = pos_i;
                    processor_position_[1] = pos_j;
                    processor_position_[2] = pos_k;
                    // don't break, finish browsing the array to search to check for inconsistencies
                  }
              }
          }
      }
  }
  if (!ok)
    {
      // this processor has no part of the domain
      for (int i = 0; i < 3; i++)
        {
          nb_elem_local_[i] = 0;
          nb_nodes_local_[i] = 0;
          for (int j = 0; j < 3; j++)
            nb_faces_local_[i][j] = 0;
          offset_[i] = 0;
          neighbour_processors_[0][i] = -1;
          neighbour_processors_[1][i] = -1;
        }
    }
  else
    {
      for (int i = 0; i < 3; i++)
        {
          nb_elem_local_[i] = sizes_all_slices_[i][processor_position_[i]];
          nb_nodes_local_[i] = nb_elem_local_[i];
          if (!geom.get_periodic_flag(i) && processor_position_[i] == nproc_per_direction_[i]-1)
            {
              // We have one more node on this processor only if we are the last processor in this direction
              // and if the domain is not periodic (otherwise, last node is owned by the next processor)
              nb_nodes_local_[i]++;
            }
          for (int j = 0; j < 3; j++)
            {
              // Compute local number of faces on this processor in each direction.
              // In general nb_faces will be equal to nb_elements except if at the end of
              // the domain and in the direction of the face normal and if not periodic:
              if (i == j)
                nb_faces_local_[j][i] = nb_nodes_local_[i];
              else
                nb_faces_local_[j][i] = nb_elem_local_[i];
            }
          offset_[i] = offsets_all_slices_[i][processor_position_[i]];
        }
      // Compute neighbour processors, taking into account periodicity:
      for (int i = 0; i < 3; i++)
        {
          for (int previous_or_next = 0; previous_or_next < 2; previous_or_next++)
            {
              Int3 other_pos(processor_position_); // ijk index of neighbour processor
              if (previous_or_next == 0)
                other_pos[i]--;
              else
                other_pos[i]++;
              if (other_pos[i] < 0 || other_pos[i] >= nproc_per_direction_[i])
                {
                  if (geom.get_periodic_flag(i))
                    {
                      // wrap to processor at other end
                      other_pos[i] = nproc_per_direction_[i] - 1 - processor_position_[i];
                    }
                  else
                    {
                      other_pos[i] = -1;
                    }
                }
              if (other_pos[i] >= 0)
                {
                  neighbour_processors_[previous_or_next][i] = mapping_(other_pos[0], other_pos[1], other_pos[2]);
                }
              else
                {
                  // at boundary of domain:
                  neighbour_processors_[previous_or_next][i] = -1;
                }
            }
        }
    }
  Journal() << "IJK_Splitting::initialize:\n"
            << " processor_position=" << print_vect(processor_position_) << "\n"
            << " offset            =" << print_vect(offset_) << "\n"
            << " nb_elem_local     =" << print_vect(nb_elem_local_) << "\n";
  Journal() << " neighbour_procs   = left:" << print_vect(neighbour_processors_[0])
            << " right:" << print_vect(neighbour_processors_[1]) << finl;
}

// Builds geometry, parallel splitting and DOF correspondance between a "father" region
// and a "son" region which is a subpart of the father region.
// Only conformal subregion is supported for now, with ELEMENT types.
// Missing features: be able to build a subregion which is the boundary of another
// eg: father is "3D elements", son is "2D faces"
void IJK_Splitting::init_subregion(const IJK_Splitting& src,
                                   int ni, int nj, int nk,
                                   int offset_i, int offset_j, int offset_k,
                                   const Nom& subregion_name,
                                   bool perio_x, bool perio_y, bool perio_z )
{

  /* methode difficile a ecrire pour etre generale
     traiter proprement les differentes localisation, par exemple les faces de bord est un casse-tete
     Pour l'instant, en dur on suppose que la sous region ne peut etre partiel que dans la direction K
     (on garde les plans entiers en i et j)
     et que le decoupage en processeurs ne peut etre fait qu'en I et J, et que la localisation est toujours
     ELEM
   */
  assert(src.nproc_per_direction_[0] == 1 || (ni==src.get_grid_geometry().get_nb_elem_tot(0) && offset_i==0));
  assert(src.nproc_per_direction_[1] == 1 || (nj==src.get_grid_geometry().get_nb_elem_tot(1) && offset_j==0));
  assert(src.nproc_per_direction_[2] == 1 || (nk==src.get_grid_geometry().get_nb_elem_tot(2) && offset_k==0));

  // La construction de la geometrie ne necessite pas les prerequis: c'est general
  double x0 = src.get_grid_geometry().get_node_coordinates(0)[offset_i];
  double y0 = src.get_grid_geometry().get_node_coordinates(1)[offset_j];
  double z0 = src.get_grid_geometry().get_node_coordinates(2)[offset_k];
  ArrOfDouble dx0 = src.get_grid_geometry().get_delta(0);
  ArrOfDouble dy0 = src.get_grid_geometry().get_delta(1);
  ArrOfDouble dz0 = src.get_grid_geometry().get_delta(2);
  ArrOfDouble dx, dy, dz;
  dx.ref_array(dx0, offset_i, ni);
  dy.ref_array(dy0, offset_j, nj);
  dz.ref_array(dz0, offset_k, nk);
  IJK_Grid_Geometry grid_geom;
  grid_geom.initialize_origin_deltas(x0,y0,z0,dx,dy,dz,perio_x,perio_y,perio_z);

  grid_geom.nommer( subregion_name );
  // Pour le decoupage, on recopie:

  int nproc_i=src.nproc_per_direction_[0];
  int nproc_j=src.nproc_per_direction_[1];
  int nproc_k=src.nproc_per_direction_[2];
  initialize(grid_geom, nproc_i,nproc_j,nproc_k);
}

// Resizes delta with the local number of mesh cells in direction dir
// with required number of ghost cells, and fills the array with the size
// of the cells in this direction.
// If domain is periodic, takes periodic mesh size in ghost cells on first and last subdomain,
// if domain is not periodic, copy the size of the first and last cell into ghost cells in the walls.
void IJK_Splitting::get_local_mesh_delta(int dir, int ghost_cells, ArrOfDouble_with_ghost& delta) const
{
  const int ntot = grid_geom_.get_nb_elem_tot(dir);
  const int offset = get_offset_local(dir);
  const ArrOfDouble& global_delta = grid_geom_.get_delta(dir);
  const int nlocal = get_nb_elem_local(dir);
  delta.resize(nlocal, ghost_cells);

  if (ghost_cells > ntot)
    {
      Cerr << "Error in IJK_Splitting::get_local_mesh_delta(dir="<<dir<<",ghost_cells="<<ghost_cells<<")\n"
           << " Number of ghost cells larger than number of nodes (" << ntot << ") in the domain !" << endl;
      Process::exit();
    }

  for (int ilocal = -ghost_cells; ilocal < nlocal + ghost_cells; ilocal++)
    {
      const int iglobal = ilocal + offset;
      double d;
      if (iglobal < 0)
        {
          if (grid_geom_.get_periodic_flag(dir))
            {
              d = global_delta[iglobal + ntot];
            }
          else
            {
              d = global_delta[0];
            }
        }
      else if (iglobal >= ntot)
        {
          if (grid_geom_.get_periodic_flag(dir))
            {
              d = global_delta[iglobal - ntot];
            }
          else
            {
              d = global_delta[ntot - 1];
            }
        }
      else
        {
          d = global_delta[iglobal];
        }
      delta[ilocal] = d;
    }
}

// Returns the total number of items in the given direction (global mesh)
int IJK_Splitting::get_nb_items_global(Localisation loc, int dir) const
{
  int n = grid_geom_.get_nb_elem_tot(dir);
  int no_perio = 0;
  if (!grid_geom_.get_periodic_flag(dir))
    no_perio = 1;

  switch(loc)
    {
    case ELEM:
      break;
    case NODES:
      n += no_perio;
      break;
    case FACES_I:
      if (dir == 0)
        n += no_perio;
      break;
    case FACES_J:
      if (dir == 1)
        n += no_perio;
      break;
    case FACES_K:
      if (dir == 2)
        n += no_perio;
      break;
    }
  return n;
}
