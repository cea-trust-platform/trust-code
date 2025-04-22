/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <EFichier.h>
#include <Domaine_IJK.h>
#include <Hexaedre.h>
#include <IJK_tools.h>

#define print_vect(x) (Nom("[") + Nom(x[0]) + Nom(" ") + Nom(x[1]) + Nom(" ") + Nom(x[2]) + Nom("]"))


// XD Domaine_IJK Domaine_base domaine_ijk -1 domain for IJK simulation (used in TrioCFD)
Implemente_instanciable_sans_constructeur(Domaine_IJK, "Domaine_IJK", Domaine_base);

// XD attr nbelem listentierf nbelem 0 Number of elements in each direction  (integers, 2 or 3 values depending on dimension)
// XD attr size_dom listf size_dom 0 Domain size in each direction  (floats, 2 or 3 values depending on dimension)
// XD attr perio listentierf perio 0 Is the direction periodic ? (0 or 1, 2 or 3 values depending on dimension)
// XD attr nproc listentierf nproc 0 Number of procs in each direction (integers, 2 or 3 values depending on dimension)

Domaine_IJK::Domaine_IJK()
{
  volume_elem_status_ = DEFAULT;
  delta_xyz_.dimensionner(3);
  node_coordinates_xyz_.dimensionner(3);
  offsets_all_slices_.dimensionner(3);
  sizes_all_slices_.dimensionner(3);
  volume_elem_ = 0.;
  mapping_ = 0;
  for(int i = 0; i < 3; ++i)
    {
      nb_elem_local_[i] = 0;
      nb_nodes_local_[i] = 0;
      uniform_[i] = false;
      periodic_[i] = false;
      offset_[i] = 0;
      nproc_per_direction_[i] = 0;
      for(int j = 0; j < 3; ++j)
        nb_faces_local_[i][j] = 0;
      for(int j = 0; j < 2; ++j)
        neighbour_processors_[j][i] = -1;
    }
}

Sortie& Domaine_IJK::printOn(Sortie& os) const
{
  Cerr << "Domain loaded for this subclass" << finl;
  return os;
}

Entree& Domaine_IJK::readOn(Entree& is)
{
  static const int dim = Objet_U::dimension;

  Cerr << "Reading Domaine_IJK (" << le_nom() << ")" << finl;

  if(dim < 1)
    Process::exit("Dimension must be set before declaring the domain.");

  ArrOfDouble origin(dim);            // Defaults to 0.
  ArrOfInt perio_flags(dim);          // Defaults to not periodic.
  ArrOfInt ndir(dim);                 // Number of elements per direction.
  ndir = -1;                          // Default means "unspecified".
  ArrOfDouble size_dom(dim);          // Constant element size in each direction.
  Noms file_coords(dim);              // File(s) with the coord of each node in a direction.
  size_dom = -1.;                     // Default means "unspecified".
  IntTab groups(dim);
  groups = 1;

  // Number of procs per direction
  IntTab nprocs(dim);
  nprocs = -1;

  Param param(que_suis_je());
  perio_flags = 0;

  Motcle motlu;
  int rang;
  is >> motlu;

  if(motlu != "{")
    Process::exit(" The symbol { was expected after 'read|lire'");

  Motcles les_mots(50);
  {
    les_mots[0] = "nbelem";
    les_mots[1] = "size_dom";
    les_mots[2] = "file_coords";
    les_mots[3] = "origin";
    les_mots[4] = "perio";
    les_mots[5] = "nproc";
    les_mots[6] = "process_grouping";
    les_mots[7] = "ijk_splitting_ft_extension";
  }

  is >> motlu;
  while(motlu != "}")
    {
      rang = les_mots.search(motlu);
      switch(rang)
        {
        case 0:
          for(int i = 0; i < dim; ++i)
            is >> ndir[i];
          break;
        case 1:
          for(int i = 0; i < dim; ++i)
            is >> size_dom[i];
          break;
        case 2:
          for(int i = 0; i < dim; ++i)
            is >> file_coords[i];
          break;
        case 3:
          for(int i = 0; i < dim; ++i)
            is >> origin[i];
          break;
        case 4:
          for(int i = 0; i < dim; ++i)
            is >> perio_flags[i];
          break;
        case 5:
          for(int i = 0; i < dim; ++i)
            is >> nprocs[i];
          break;
        case 6:
          for(int i = 0; i < dim; ++i)
            is >> groups[i];
          break;
        case 7:
          is >> ft_extension_;
          break;
        default:
          Cerr << "Keyword : " << motlu <<" not understood by Domaine_IJK::readOn. Either update if needed or change the keyword." << finl;
          Process::exit();
          break;
        }
      is >> motlu;
    }

  // Initializing the sizes of eulerian elements
  VECT(ArrOfDouble) delta_dir(3);
  for (int i = 0; i < 3; ++i)
    {
      if (file_coords[i] != "??")
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "Reading coordinates for direction " << i << " in file " << file_coords[i] << finl;
              EFichier EFcoord(file_coords[i]);
              // Skipping the origin. If the origin was written in the file, it may lead to errors.
              double previous_x = 0.;
              int n = 0;
              while(1)
                {
                  double next_x;
                  EFcoord >> next_x;
                  if (EFcoord.eof())
                    break;
                  if (n == 0)
                    origin[i] = next_x;
                  else
                    delta_dir[i].append_array(next_x - previous_x);
                  n++;
                  previous_x = next_x;
                }
              if (delta_dir[i].size_array() < 1)
                Cerr << "Error in Domaine_IJK::readOn: input file contains less than 2 values" << finl;
            }
          envoyer_broadcast(delta_dir[i], 0);
        }
      else if (ndir[i] != -1) // Then it's uniform
        {
          if (ndir[i] < 1 || size_dom[i] <= 0.)
            {
              Cerr << "Error in Domaine_IJK::readOn: wrong value of nbelem or uniform_domain_size for direction " << i
                   << "\n : nbelem = " << ndir[i] << " domain_size = " << size_dom[i] << finl;
              Process::exit();
            }
          delta_dir[i].resize_array(ndir[i]);
          delta_dir[i] = size_dom[i] / ndir[i];
        }
      else // The dataset wasn't filled properly. nbelem takes 3 parameters.
        {
          Cerr << "Error in Domaine_IJK::readOn: you must provide (nbelem x y z)\n"
               << "   or file_coords for each direction i, j and k" << finl;
          Process::exit();
        }
      double x = 0;
      for (int j = 0; j < delta_dir[i].size_array(); j++)
        {
          if (delta_dir[i][j] <= 0.)
            {
              Cerr << "Error in Domaine_IJK::readOn: size of element " << j << "  in direction " << i << " is not positive" << finl;
              Process::exit();
            }
          x += delta_dir[i][j];
        }
      Cerr << "Direction " << i << " has " << delta_dir[i].size_array() << " elements. Total domain size = " << x << finl;
    }

  int tot_proc = 1;
  for (int j = 0; j < dim; j++)
    {
      if(nprocs[j] < 1)
        Process::exit("Proc number in every direction must be strictly positive! Did you forget 'nprocs'?");
      tot_proc *= nprocs[j];
    }
  if (tot_proc != Process::nproc())
    {
      Cerr << "!! ERROR: Domaine_IJK is built with a total number of " << tot_proc << " procs, but TRUST/Trio was launched with " << Process::nproc() << " procs!!" << finl;
      Process::exit();
    }

  Cerr << "nproc in i, j, k directions = " << nprocs[0] << " " << nprocs[1] << " " << nprocs[2] << finl;
  Cerr << "grouping processes in i, j, k directions (node topology) = "
       << groups[0] << " " << groups[1] << " " << groups[2] << finl;

  initialize_origin_deltas(origin[0], origin[1], origin[2], delta_dir[0], delta_dir[1],
                           delta_dir[2], perio_flags[0], perio_flags[1], perio_flags[2]);

  initialize_splitting(*this, nprocs[0], nprocs[1], nprocs[2], groups[0], groups[1], groups[2]);
  update_volume_elem();
  return is;
}

static void retirer_doublons(ArrOfDouble& tab, double epsilon)
{
  const int n = tab.size_array();
  if (n == 0)
    return;
  double last_value = tab[0];
  int dest = 1;
  for (int i = 1; i < n; i++)
    {
      double x = tab[i];
      if (x > last_value + epsilon)
        {
          tab[dest++] = x;
          last_value = x;
        }
    }
  tab.resize_array(dest);
}

static void find_unique_coord(const DoubleTab& src, int column, ArrOfDouble& result)
{
  const int n = src.dimension(0);
  ArrOfDouble tmp(n);
  int i;
  for (i = 0; i < n; i++)
    tmp[i] = src(i, column);
  tmp.ordonne_array();
  retirer_doublons(tmp, Objet_U::precision_geom);


  if (Process::me() != 0)
    {
      envoyer(tmp, 0, 53);
    }
  else
    {
      result = tmp;
      int np = Process::nproc();
      for (i = 1; i < np; i++)
        {
          recevoir(tmp, i, 53 /* canal */);
          int m1Loc = tmp.size_array();
          int m2Loc = result.size_array();
          result.resize_array(m2Loc+m1Loc);
          result.inject_array(tmp, m1Loc /* nbelem */, m2Loc /* dest index */, 0 /* src index */);
        }
      result.ordonne_array();
      retirer_doublons(result, Objet_U::precision_geom);
    }

  envoyer_broadcast(result, 0);
}



// Extracts the mesh origin, dimensions and cell sizes from a distributed VDF mesh.
//
void Domaine_IJK::initialize_from_unstructured(const Domaine& domaine,
                                               int direction_for_x,
                                               int direction_for_y,
                                               int direction_for_z,
                                               bool perio_x, bool perio_y, bool perio_z)
{
  if (!sub_type(Hexaedre, domaine.type_elem().valeur()))
    {
      Cerr << "Error in IJK_Grid_Geometry::initialize_from_unstructured:\n"
           << " the provided domaine does not have Hexaedre element type" << finl;
      exit();
    }
  periodic_[0] = perio_x;
  periodic_[1] = perio_y;
  periodic_[2] = perio_z;
  // Find all coordinates in the unstructured mesh
  // swap directions
  const DoubleTab& coord_som = domaine.les_sommets();
  Cout << "IJK_Grid_Geometry::initialize_from_unstructured maps x->" <<  direction_for_x
       << " y->" << direction_for_y << " z->" << direction_for_z << finl;

  find_unique_coord(coord_som, 0 /* coordonnees y */, node_coordinates_xyz_[direction_for_x]);
  find_unique_coord(coord_som, 1 /* coordonnees y */, node_coordinates_xyz_[direction_for_y]);
  find_unique_coord(coord_som, 2 /* coordonnees z (swap y et z) */, node_coordinates_xyz_[direction_for_z]);
  const double eps = Objet_U::precision_geom;

  for (int dir = 0; dir < 3; dir++)
    {
      const ArrOfDouble& coord = node_coordinates_xyz_[dir];
      ArrOfDouble& delta = delta_xyz_[dir];
      const int nelem = coord.size_array() - 1;
      Cout << " Direction " << dir << " has " << nelem << " elements. coord[0]="
           << coord[0] << " coord[" << nelem << "]=" << coord[nelem];
      delta.resize_array(nelem);
      double mindelta = 1e30;
      double maxdelta = 0.;
      for (int i = 0; i < nelem; i++)
        {
          const double d = coord[i+1] - coord[i];
          delta[i] = d;
          mindelta = std::min(d, mindelta);
          maxdelta = std::max(d, maxdelta);
        }
      if (maxdelta - mindelta < eps)
        {
          // The mesh is uniform: recompute a constant delta with a better accuracy:
          double d = (coord[nelem] - coord[0]) / (double) nelem;
          delta = d; // affect the exact same value to all cells
          Cout << " Uniform, delta=" << d << finl;
          uniform_[dir] = true;
        }
      else
        {
          Cout << "IJK_Grid_Geometry::initialize_from_unstructured direction " << dir
               << " Not uniform, min delta=" << mindelta
               << " max delta=" << maxdelta << finl;
          uniform_[dir] = false;
        }
    }
}

/*! @brief Buils a splitting of the given deometry on the requested number of processors
 *         in each direction.
 *
 *
 *         Process_grouping allows to rearrange process ranks by packets of ni*nj*nk processes
 *         to matc the topology of the cluster/node. ex: 8 cores node/machine => use groups
 *         of size 2x2x2 to minimize extra-node messages.
 *
 *  @param geom another domaine
 *  @param nproc_i Number of processors in i direction.
 *  @param nproc_j Number of processors in j direction.
 *  @param nproc_k Number of processors in k direction.
 *  @param process_grouping_i 1 by default. Number of processors per subdomain in i direction.
 *  @param process_grouping_j 1 by default. Number of processors per subdomain in j direction.
 *  @param process_grouping_k 1 by default. Number of processors per subdomain in k direction.
 */
void Domaine_IJK::initialize_splitting(Domaine_IJK& geom,
                                       int nproc_i, int nproc_j, int nproc_k,
                                       int process_grouping_i,
                                       int process_grouping_j,
                                       int process_grouping_k)
{
  assert(nproc_i % process_grouping_i == 0 && "While this will still work, may not bring expected results. Try having a process grouping number that divides the number of proc allocated to this direction.");
  assert(nproc_j % process_grouping_j == 0 && "While this will still work, may not bring expected results. Try having a process grouping number that divides the number of proc allocated to this direction.");
  assert(nproc_k % process_grouping_k == 0 && "While this will still work, may not bring expected results. Try having a process grouping number that divides the number of proc allocated to this direction.");

  const int available_nproc = Process::nproc();
  if (nproc_i * nproc_j * nproc_k > available_nproc)
    {
      Cerr << "Error in Domaine_IJK::initialize_splitting(nproc_i =" << nproc_i
           << ", nproc_j =" << nproc_j
           << ", nproc_k =" << nproc_k
           << "): requested splitting larger than available processor number (" << available_nproc << ")" << endl;
      Process::exit();
    }

  if (nproc_i > geom.get_nb_elem_tot(0)
      || nproc_j > geom.get_nb_elem_tot(1)
      || nproc_k > geom.get_nb_elem_tot(2))
    {
      Cerr << "Error in Domaine_IJK::initialize_splitting(nproc_i = " << nproc_i
           << ", nproc_j = " << nproc_j
           << ", nproc_k = " << nproc_k
           << "): requested splitting larger than total number of cells in one direction";
      Cerr << "\n (number of cells: "
           << geom.get_nb_elem_tot(0) << " "
           << geom.get_nb_elem_tot(1) << " "
           << geom.get_nb_elem_tot(2) << ")" << endl;
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
      FixedVector<int, 3> process_grouping;
      process_grouping[0] = process_grouping_i;
      process_grouping[1] = process_grouping_j;
      process_grouping[2] = process_grouping_k;
      FixedVector<int, 3> nprocdir;
      nprocdir[0] = nproc_i;
      nprocdir[1] = nproc_j;
      nprocdir[2] = nproc_k;
      FixedVector<int, 3> ngroups;
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
          const int ne = geom.get_nb_elem_tot(i);
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
  initialize_mapping(geom, sizes_all_slices[0], sizes_all_slices[1], sizes_all_slices[2], mapping);
}

/*! @brief Creates a splitting of the domain by specifying the slice
 *         sizes and the processor mapping.
 *
 *
 *         The total cell number in directions i, j, and k must match the
 *         total number of cells in the whole geometry for each direction.
 *         The number of slices in each direction must match each corresponding
 *         dimensions of the mapping array.
 *         All processors do not have to be used!
 *
 *  @param slice_size_i Contains for each slice in the i direction, the number of cells this slice.
 *  @param slice_size_j Contains for each slice in the j direction, the number of cells this slice.
 *  @param slice_size_k Contains for each slice in the k direction, the number of cells this slice.
 *  @param processor_mapping Provides the rank of the mpi process that will own this subdomain.
 */
void Domaine_IJK::initialize_mapping(Domaine_IJK& geom, const ArrOfInt& slice_size_i,
                                     const ArrOfInt& slice_size_j,
                                     const ArrOfInt& slice_size_k,
                                     const IntTab& processor_mapping)
{
  assert(slice_size_i.size_array() == processor_mapping.dimension(0));
  assert(slice_size_j.size_array() == processor_mapping.dimension(1));
  assert(slice_size_k.size_array() == processor_mapping.dimension(2));

  // Copy all geometrical information:
  *this = geom;

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
      for (int j = 1; j < n; ++j)
        offsets_all_slices_[i][j] = offsets_all_slices_[i][j - 1] + sizes_all_slices_[i][j - 1];
      assert(offsets_all_slices_[i][n - 1] + sizes_all_slices_[i][n - 1] == geom.get_nb_elem_tot(i));
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
                int numproc = mapping_(pos_i, pos_j, pos_k);
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
      for (int i = 0; i < 3; ++i)
        {
          nb_elem_local_[i] = 0;
          nb_nodes_local_[i] = 0;
          for (int j = 0; j < 3; ++j)
            nb_faces_local_[i][j] = 0;
          offset_[i] = 0;
          neighbour_processors_[0][i] = -1;
          neighbour_processors_[1][i] = -1;
        }
    }
  else
    {
      for (int i = 0; i < 3; ++i)
        {
          nb_elem_local_[i] = sizes_all_slices_[i][processor_position_[i]];
          nb_nodes_local_[i] = nb_elem_local_[i];
          if (!geom.get_periodic_flag(i) && processor_position_[i] == nproc_per_direction_[i] -1)
            {
              // We have one more node on this processor only if we are the last processor in this direction
              // and if the domain is not periodic (otherwise, last node is owned by the next processor)
              nb_nodes_local_[i]++;
            }
          for (int j = 0; j < 3; ++j)
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
      for (int i = 0; i < 3; ++i)
        {
          for (int previous_or_next = 0; previous_or_next < 2; previous_or_next++)
            {
              FixedVector<int, 3> other_pos(processor_position_); // ijk index of neighbour processor
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
                    other_pos[i] = -1;
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
  Journal() << "Domaine_IJK::initialize_mapping:\n"
            << " processor_position =" << print_vect(processor_position_) << "\n"
            << " offset             =" << print_vect(offset_) << "\n"
            << " nb_elem_local      =" << print_vect(nb_elem_local_) << "\n";
  Journal() << " neighbour_procs    = left:" << print_vect(neighbour_processors_[0])
            << " right:" << print_vect(neighbour_processors_[1]) << finl;
}

/*! @brief Initializes class elements given dataset's parameters.
 *
 *
 *  @param x0 Origin of the whole domain on the x axis.
 *  @param y0 Origin of the whole domain on the y axis.
 *  @param z0 Origin of the whole domain on the z axis.
 *  @param delta_x Array with the sizes of the elements on the x axis.
 *  @param delta_y Array with the sizes of the elements on the y axis.
 *  @param delta_z Array with the sizes of the elements on the z axis.
 *  @param perio_x Periodic flag along x axis.
 *  @param perio_y Periodic flag along y axis.
 *  @param perio_z Periodic flag along z axis.
 */
void Domaine_IJK::initialize_origin_deltas(double x0, double y0, double z0,
                                           const ArrOfDouble& delta_x,
                                           const ArrOfDouble& delta_y,
                                           const ArrOfDouble& delta_z,
                                           bool perio_x, bool perio_y, bool perio_z)
{
  periodic_[0] = perio_x;
  periodic_[1] = perio_y;
  periodic_[2] = perio_z;

  static const double eps = Objet_U::precision_geom;

  delta_xyz_[0] = delta_x;
  delta_xyz_[1] = delta_y;
  delta_xyz_[2] = delta_z;
  for (int dir = 0; dir < 3; ++dir)
    {
      int n = delta_xyz_[dir].size_array();
      node_coordinates_xyz_[dir].resize_array(n+1);
      node_coordinates_xyz_[dir][0] = (dir == 0) ? x0 : ((dir == 1) ? y0 : z0);
      double mindelta = 1e30;
      double maxdelta = 0.;
      for (int i = 0; i < n; i++)
        {
          const double d = delta_xyz_[dir][i];
          node_coordinates_xyz_[dir][i+1] = node_coordinates_xyz_[dir][i] + d;
          mindelta = std::min(d, mindelta);
          maxdelta = std::max(d, maxdelta);
        }
      if (maxdelta - mindelta < eps)
        {
          uniform_[dir] = true;
        }
      else
        {
          uniform_[dir] = false;
        }
    }
}

/*! @brief Builds the geometry, parallel splitting and DOF correspondance
 *         between a "father" region and a "son" region which is a subpart
 *         of the father region.
 *
 *
 *         Only conformal subregion is supported for now, with ELEMENT types.
 *         Missing features: be able to build a subregion which is the boundary
 *         of another, eg: father is "3D elements", son is "2D faces".
 *
 *  @param ni Number of elements in direction(0)
 *  @param nj Number of elements in direction(1)
 *  @param nk Number of elements in direction(2)
 *  @param offset_i Offset along x axis for the "son" subregion
 *  @param offset_j Offset along x axis for the "son" subregion
 *  @param offset_k Offset along x axis for the "son" subregion
 *  @param subregion_name Name of the "son" subregion
 *  @param perio_x Whether if domain is periodic along x axis
 *  @param perio_y Whether if domain is periodic along y axis
 *  @param perio_z Whether if domain is periodic along z axis
 */
void Domaine_IJK::init_subregion(int ni, int nj, int nk,
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
  assert(nproc_per_direction_[0] == 1 || (ni == get_nb_elem_tot(0) && offset_i == 0));
  assert(nproc_per_direction_[1] == 1 || (nj == get_nb_elem_tot(1) && offset_j == 0));
  assert(nproc_per_direction_[2] == 1 || (nk == get_nb_elem_tot(2) && offset_k == 0));

  // La construction de la geometrie ne necessite pas les prerequis: c'est general
  double x0 = get_node_coordinates(0)[offset_i];
  double y0 = get_node_coordinates(1)[offset_j];
  double z0 = get_node_coordinates(2)[offset_k];
  ArrOfDouble dx0 = get_delta(0);
  ArrOfDouble dy0 = get_delta(1);
  ArrOfDouble dz0 = get_delta(2);
  ArrOfDouble dx, dy, dz;
  dx.ref_array(dx0, offset_i, ni);
  dy.ref_array(dy0, offset_j, nj);
  dz.ref_array(dz0, offset_k, nk);
  initialize_origin_deltas(x0, y0, z0, dx, dy, dz, perio_x, perio_y, perio_z);

  nommer(subregion_name);
  // Pour le decoupage, on recopie:

  int nproc_i = nproc_per_direction_[0];
  int nproc_j = nproc_per_direction_[1];
  int nproc_k = nproc_per_direction_[2];
  initialize_splitting(*this, nproc_i, nproc_j, nproc_k);
}

/*! @brief Creates a splitting of the domain by specifying the mapping.
 *
 *
 *         The total cell number in directions i,j,k must match the total
 *         number of cells in each direction.
 *         The number of slices in direction i, j , k must match dimensions
 *         0,1 and 2 of the processor_mapping() array.
 *
 *  @param slice_size_i Contains, for each slice in the x direction, the number of cells in this slice.
 *  @param slice_size_j Contains, for each slice in the y direction, the number of cells in this slice.
 *  @param slice_size_k Contains, for each slice in the z direction, the number of cells in this slice.
 *  @param processor_mapping Provides the rank of the mpi process that will have this subdomain.
 */
void Domaine_IJK::initialize_with_mapping(const ArrOfInt& slice_size_i,
                                          const ArrOfInt& slice_size_j,
                                          const ArrOfInt& slice_size_k,
                                          const IntTab& processor_mapping)
{
  assert(slice_size_i.size_array() == processor_mapping.dimension(0));
  assert(slice_size_j.size_array() == processor_mapping.dimension(1));
  assert(slice_size_k.size_array() == processor_mapping.dimension(2));
  // assert_parallel(slice_size_i); // check that same value on all processors
  // assert_parallel(slice_size_j);
  // assert_parallel(slice_size_k);
  // assert_parallel(processor_mapping);

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
      assert(offsets_all_slices_[i][n-1] + sizes_all_slices_[i][n-1] == get_nb_elem_tot(i));
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
          if (!get_periodic_flag(i) && processor_position_[i] == nproc_per_direction_[i]-1)
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
                  if (get_periodic_flag(i))
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
  Journal() << "Domaine_IJK::initialize_with_mapping:\n"
            << " processor_position=" << print_vect(processor_position_) << "\n"
            << " offset            =" << print_vect(offset_) << "\n"
            << " nb_elem_local     =" << print_vect(nb_elem_local_) << "\n";
  Journal() << " neighbour_procs   = left:" << print_vect(neighbour_processors_[0])
            << " right:" << print_vect(neighbour_processors_[1]) << finl;
}

/*! @brief renvoie new(Faces) ! elle est surchargee par Domaine_VDF par ex.
 */
Faces* Domaine_IJK::creer_faces()
{
  Faces* les_faces = new(Faces);
  return les_faces;
}

/*! @brief Returns the number of local items (on this processor) for the given localisation in the requested direction
 *
 *
 *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
 *  @param direction In IJK, x(0), y(1) or z(2)
 *  @return Number of requested items
 */
int Domaine_IJK::get_nb_items_local(Localisation loc, int direction) const
{
  assert(loc == ELEM || loc == NODES || loc == FACES_I ||
         loc == FACES_J || loc == FACES_K);
  assert(direction >= 0 && direction < 3);
  switch(loc)
    {
    case ELEM:
      return get_nb_elem_local(direction);
    case NODES:
      return get_nb_nodes_local(direction);
    case FACES_I:
      return get_nb_faces_local(0, direction);
    case FACES_J:
      return get_nb_faces_local(1, direction);
    case FACES_K:
      return get_nb_faces_local(2, direction);
    }
  return -1;
}

/*! @brief Returns the length of the entire domain in requested direction
 *  @param direction In IJK, x(0), y(1) or z(2).
 *  @return Global length of the domain
 */
double Domaine_IJK::get_domain_length(int direction) const
{
  assert(direction >= 0 && direction < 3);
  const int n = get_nb_elem_tot(direction);
  const double x0 = get_origin(direction);
  const double x1 = get_node_coordinates(direction)[n];
  return x1 - x0;
}

/*! @brief Fills the "delta" array with the size of the cells owned by the processor in the requested direction.
 *
 *
 *  "delta" is redimensionned with the specified number of ghost cells and filled.
 *  If domain is periodic, takes periodic mesh size in ghost cells on first and last subdomain,
 *  if domain is not periodic, copy the size of the first and last cell into ghost cells in the walls.
 *
 *  @param direction In IJK, x(0), y(1) or z(2).
 *  @param ghost_cells Size of the ghost cells
 *  @param delta Size of cells in each direction
 */
void Domaine_IJK::get_local_mesh_delta(int direction, int ghost_cells,
                                       ArrOfDouble_with_ghost& delta) const
{
  const int ntot = get_nb_elem_tot(direction);
  const int offset = get_offset_local(direction);
  const ArrOfDouble& global_delta = get_delta(direction);
  const int nlocal = get_nb_elem_local(direction);
  delta.resize(nlocal, ghost_cells);

  if (ghost_cells > ntot)
    {
      Cerr << "Error in Domaine_IJK::get_local_mesh_delta(dir = "<<direction<<",ghost_cells = "<<ghost_cells<<")\n"
           << "Number of ghost cells larger than number of nodes (" << ntot << ") in the domain !" << endl;
      Process::exit();
    }

  for (int ilocal = -ghost_cells; ilocal < nlocal + ghost_cells; ilocal++)
    {
      const int iglobal = ilocal + offset;
      double d;
      if (iglobal < 0)
        {
          if (get_periodic_flag(direction))
            d = global_delta[iglobal + ntot];
          else
            d = global_delta[0];
        }
      else if (iglobal >= ntot)
        {
          if (get_periodic_flag(direction))
            d = global_delta[iglobal - ntot];
          else
            d = global_delta[ntot - 1];
        }
      else
        d = global_delta[iglobal];

      delta[ilocal] = d;
    }
}

/*! @brief Returns the number of local items (on this processor) for the given localisation in the requested direction
 *
 *  If periodic along requested direction, need to add the last item for nodes and faces.
 *
 *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
 *  @param direction In IJK, x(0), y(1) or z(2)
 *  @return Number of requested items
 */
int Domaine_IJK::get_nb_items_global(Localisation loc, int direction) const
{
  assert(loc == ELEM || loc == NODES || loc == FACES_I ||
         loc == FACES_J || loc == FACES_K);
  assert(direction >= 0 && direction < 3);
  int n = get_nb_elem_tot(direction);
  int no_perio = 0;
  if (!periodic_[direction])
    no_perio = 1;

  switch(loc)
    {
    case ELEM:
      break;
    case NODES:
      n += no_perio;
      break;
    case FACES_I:
      if (direction == 0)
        n += no_perio;
      break;
    case FACES_J:
      if (direction == 1)
        n += no_perio;
      break;
    case FACES_K:
      if (direction == 2)
        n += no_perio;
      break;
    }
  return n;
}

/*! @brief Returns the number of items of given location (elements, nodes, faces...)
 *         for all slices in the requested direction.
 *
 *  @param direction In IJK, x(0), y(1) or z(2).
 *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
 *  @param tab Array in which we'll store the number of slices in given direction
 */
void Domaine_IJK::get_slice_size(int direction, Localisation loc, ArrOfInt& tab) const
{
  assert(loc == ELEM || loc == NODES || loc == FACES_I ||
         loc == FACES_J || loc == FACES_K);
  assert(direction >= 0 && direction < 3);
  tab = sizes_all_slices_[direction];
  const int n = tab.size_array() - 1;
  if (!periodic_[direction])
    {
      switch(loc)
        {
        case ELEM:
          break;
        case NODES:
          tab[n]++;
          break;
        case FACES_I:
          if (direction == 0) tab[n]++;
          break;
        case FACES_J:
          if (direction == 1) tab[n]++;
          break;
        case FACES_K:
          if (direction == 2) tab[n]++;
          break;
        default:
          break;
        }
    }
}

/*! @brief Determines the dof of an element along a localisation
 *
 *  TODO: Not sure about the brief?
 *
 *  @param i Local index of an element along x axis.
 *  @param j Local index of an element along y axis.
 *  @param k Local index of an element along z axis.
 *  @param In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K.
 *
 *  @return A vector with the coordinates of dof
 */
Vecteur3 Domaine_IJK::get_coords_of_dof(int i, int j, int k, Localisation loc) const
{
  assert(loc == ELEM || loc == NODES || loc == FACES_I ||
         loc == FACES_J || loc == FACES_K);
  int gi = i + offset_[0];
  int gj = j + offset_[1];
  int gk = k + offset_[2];
  Vecteur3 xyz;
  xyz[0] = get_node_coordinates(0)[gi];
  xyz[1] = get_node_coordinates(1)[gj];
  xyz[2] = get_node_coordinates(2)[gk];
  if (loc == ELEM || loc == FACES_J || loc == FACES_K)
    xyz[0] += get_delta(0)[gi] * 0.5;
  if (loc == ELEM || loc == FACES_I || loc == FACES_K)
    xyz[1] += get_delta(1)[gj] * 0.5;
  if (loc == ELEM || loc == FACES_I || loc == FACES_J)
    xyz[2] += get_delta(2)[gk] * 0.5;
  return xyz;
}

/*! @brief With three indices, find the local index of an element
 *
 *  @param i Local index of an element along x axis.
 *  @param j Local index of an element along y axis.
 *  @param k Local index of an element along z axis.
 *
 *  @return The LOCAL index of an element.
 */
int Domaine_IJK::convert_ijk_cell_to_packed(int i, int j, int k) const
{
  const int nbmailles_euler_i = get_nb_elem_local(0);
  const int nbmailles_euler_j = get_nb_elem_local(1);

  if (i < 0)
    return -1;
  else
    return (k * nbmailles_euler_j + j) * nbmailles_euler_i + i;
}

/*! @brief Convert the local index of an element to a vector with IJK indices.
 *
 *  Adapted from Maillage_FT_IJK.h
 *
 *  @param index Local index of the element.
 *  @return Vector with IJK coordinates.
 */
FixedVector<int, 3> Domaine_IJK::convert_packed_to_ijk_cell(int index) const
{
  const int nbmailles_euler_i = get_nb_elem_local(0);
  const int nbmailles_euler_j = get_nb_elem_local(1);

  FixedVector<int, 3> ijk;
  if (index < 0)
    ijk[0] = ijk[1] = ijk[2] = -1;
  else
    {
      ijk[0] = index % nbmailles_euler_i;
      index /= nbmailles_euler_i;
      ijk[1] = index % nbmailles_euler_j;
      index /= nbmailles_euler_j;
      ijk[2] = index;
    }
  return ijk;
}

/*! @brief Find the element which contains the item's coodirnates.
 *
 *
 *  The element's coordinates can be outside of this processor's subdomain.
 *
 *
 *  @param x First coordinate of an item in the mesh.
 *  @param y Second coordinate of an item in the mesh.
 *  @param z Third coordinate of an item in the mesh.
 *  @param ijk_global The global coordinates of the cell.
 *  @param ijk_local The local coordinates of the cell.
 *  @param ijk_me A sort of 3D flag. Will be [1,1,1] if the element belongs to me.
 */
void Domaine_IJK::search_elem(const double& x, const double& y, const double& z,
                              FixedVector<int, 3>& ijk_global,
                              FixedVector<int, 3>& ijk_local,
                              FixedVector<int, 3>& ijk_me) const
{
  Vecteur3 coords(x, y, z);
  ijk_me[0] = 0;
  ijk_me[1] = 0;
  ijk_me[2] = 0;
  for (int dir = 0; dir < 3; ++dir)
    {
      const double d = get_constant_delta(dir);
      const double o = get_origin(dir);
      const double val = (coords[dir] - o) / d;
      const int ival = (int)std::lrint(std::floor(val));
      ijk_global[dir] = ival;
      assert((ival >= 0) && (ival < get_nb_elem_tot(dir)));
      const int ioff = get_offset_local(dir);
      const int n_loc = get_nb_elem_local(dir);
      if ((ival >= ioff) && (ival < ioff + n_loc))
        // It's within my subdomain along this direction
        ijk_me[dir] = 1;
      ijk_local[dir] = ival - ioff;
    }
}

/*! @brief Updates volume_elem_
 *  / ! \ If the grid changes, this needs to be called again!
 */
void Domaine_IJK::update_volume_elem()
{
  // volume_elem is already up to date
  if(volume_elem_status_ == DONE)
    return;

  // This method only makes sense for 3D
  // resize of volume_elem_
  const int nx = get_nb_elem_local(0);
  assert(nx > 0);
  const int ny = get_nb_elem_local(1);
  assert(ny > 0);
  const int nz = get_nb_elem_local(2);
  assert(nz > 0);
  const int nsize = nx * ny * nz;
  volume_elem_.resize(nsize);

  // Fill with the volume of each elem
  double size_elem = 0.;
  // Nb of axis with uniform size
  int nb_uniform_axis = 0;
  int not_uniform = -1;
  for(int dir = 0; dir < 3; ++dir)
    if(is_uniform(dir))
      nb_uniform_axis++;

  const double size_x = is_uniform(0) ? get_constant_delta(0) : throw;
  const double size_y = is_uniform(1) ? get_constant_delta(1): throw;
  const double size_z = is_uniform(2) ? get_constant_delta(2) : -123.;
  const ArrOfDouble& sizes_x = get_delta(0);
  const ArrOfDouble& sizes_y = get_delta(1);
  const ArrOfDouble& sizes_z = get_delta(2);
  switch(nb_uniform_axis)
    {
    case 3: // Constant on all axis
      size_elem = size_x * size_y * size_z;
      volume_elem_ = size_elem;
      volume_elem_status_ = DONE;
      return;
    case 2: // Not constant on 1 axis
      for(int dir = 0; dir < 3; ++dir)
        if(!is_uniform(dir))
          not_uniform = dir;
      if(not_uniform == 0) // Not constant along x axis
        {
          assert(nx == sizes_x.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = sizes_x[idx] * size_y * size_z;
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else if(not_uniform == 1) // Not constant along y axis
        {
          assert(ny == sizes_y.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = size_x * sizes_y[idy] * size_z;
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else if(not_uniform == 2) // Not constant along z axis
        {
          assert(nz == sizes_z.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = size_x * size_y * sizes_z[idz];
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else
        {
          Cerr << "Error in Maillage_Ft_IJK::update_volume_elem !!! No direction selected?!" <<finl;
          assert(0);
          Process::exit();
        }
      volume_elem_status_ = DONE;
      return;
    case 1: // constant along one axis
      if(is_uniform(0))
        not_uniform = 23;
      else if(is_uniform(1))
        not_uniform = 13;
      else if(is_uniform(2))
        not_uniform = 12;
      if(not_uniform == 23) // constant along x axis
        {
          assert(ny == sizes_y.size_array());
          assert(nz == sizes_z.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = size_x * sizes_y[idy] * sizes_z[idz];
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else if(not_uniform == 13) // constant along y axis
        {
          assert(nx == sizes_x.size_array());
          assert(nz == sizes_z.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = sizes_x[idx] * size_y * sizes_z[idz];
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else if(not_uniform == 12) // constant along z axis
        {
          assert(ny == sizes_y.size_array());
          assert(nx == sizes_x.size_array());
          for(int idz = 0; idz < nz; ++idz)
            for(int idy = 0; idy < ny; ++idy)
              for(int idx = 0; idx < nx; ++idx)
                {
                  size_elem = sizes_x[idx] * sizes_y[idy] * size_z;
                  volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
                }
        }
      else
        {
          Cerr << "Error in Maillage_Ft_IJK::update_volume_elem !!! No direction selected?!" <<finl;
          assert(0);
          Process::exit();
        }
      volume_elem_status_ = DONE;
      return;
    case 0: // Varies on all axis
      // Checking if everything makes sense
      assert(nx == sizes_x.size_array());
      assert(ny == sizes_y.size_array());
      assert(nz == sizes_z.size_array());
      for(int idz = 0; idz < nz; ++idz)
        for(int idy = 0; idy < ny; ++idy)
          for(int idx = 0; idx < nx; ++idx)
            {
              size_elem = sizes_x[idx] * sizes_y[idy] * sizes_z[idz];
              volume_elem_[(idz * ny * nx) +(idy * nx) + idx] = size_elem;
            }
      volume_elem_status_ = DONE;
      return;
    default:
      Cerr << "Error Maillage_FT_IJK::update_volume_elem" << finl;
      assert(0);
      Process::exit();
      return;
    }
}

void Domaine_IJK::set_extension_from_bulle_param(double vol_bulle, double diam_bulle)
{
  int ijk_splitting_ft_extension_from_diameter = 0;
  for (int c=0; c<3; c++)
    {
      const double delta = get_constant_delta(c);
      ijk_splitting_ft_extension_from_diameter = std::max(ijk_splitting_ft_extension_from_diameter, (int) ceil(diam_bulle/delta));
    }
  ft_extension_ = (ijk_splitting_ft_extension_from_diameter > ft_extension_) ? ijk_splitting_ft_extension_from_diameter : ft_extension_;
}



int Domaine_IJK::periodic_get_processor_by_ijk(int slice_i, int slice_j, int slice_k) const
{
  int periodic_slice_i = slice_i;
  int periodic_slice_j = slice_j;
  int periodic_slice_k = slice_k;

  // Correction du processeur a chercher dans le cas periodique,
  // si le slice est plus petit que zero ou plus grand que le nombre maximal.
  if (get_periodic_flag(0) && (slice_i < 0))
    periodic_slice_i += get_nprocessor_per_direction(0);
  else if (get_periodic_flag(0) && (slice_i >= get_nprocessor_per_direction(0)))
    periodic_slice_i -= get_nprocessor_per_direction(0);

  if (get_periodic_flag(1) && (slice_j < 0))
    periodic_slice_j += get_nprocessor_per_direction(1);
  else if (get_periodic_flag(1) && (slice_j >= get_nprocessor_per_direction(1)))
    periodic_slice_j -= get_nprocessor_per_direction(1);

  if (get_periodic_flag(2) && (slice_k < 0))
    periodic_slice_k += get_nprocessor_per_direction(2);
  else if (get_periodic_flag(2) && (slice_k >= get_nprocessor_per_direction(2)))
    periodic_slice_k -= get_nprocessor_per_direction(2);

  // Si on est pas periodique dans une direction, on retourne -1 pour indiquer l'absence de processeur
  if ((!get_periodic_flag(0)) && (slice_i < 0))
    return -1;
  else if ((!get_periodic_flag(0)) && (slice_i >= get_nprocessor_per_direction(0)))
    return -1;

  if ((!get_periodic_flag(1)) && (slice_j < 0))
    return -1;
  else if ((!get_periodic_flag(1)) && (slice_j >= get_nprocessor_per_direction(1)))
    return -1;

  if ((!get_periodic_flag(2)) && (slice_k < 0))
    return -1;
  else if ((!get_periodic_flag(2)) && (slice_k >= get_nprocessor_per_direction(2)))
    return -1;
  else
    // Sinon, on retourne le numero du processeur
    return get_processor_by_ijk(periodic_slice_i, periodic_slice_j, periodic_slice_k);
}


/*! independent_index adds a ghost_size to the packed index.
 * It is similar to the linear_index defined in IJK_Field_local_template, but with
 * a universal, predefined ghost_size of 256 instead of a field-dependent ghost_size.
 * Since the ghost_size_ value is larger than any ghost_size expected to be used in
 * practice, any virtual cell can be represented by the independent index.
 */
int Domaine_IJK::get_independent_index(int i, int j, int k) const
{
  int ghost_size = 256;
  const int ni = get_nb_elem_local(0);
  const int nj = get_nb_elem_local(1);

  int offset = ghost_size + (ni + 2*ghost_size)*ghost_size + (ni + 2*ghost_size)*(nj + 2*ghost_size)*ghost_size;
  int independent_index = offset + i + (ni + 2*ghost_size)*j + (ni + 2*ghost_size)*(nj + 2*ghost_size)*k;

  return independent_index;
}

Int3 Domaine_IJK::get_ijk_from_independent_index(int independent_index) const
{
  int ghost_size = 256;
  const int ni = get_nb_elem_local(0);
  const int nj = get_nb_elem_local(1);

  // Computation of the indices disregarding the offset (i goes from 0 to ni + 2*ghost_size)
  int k = (independent_index)/((ni + 2*ghost_size)*(nj + 2*ghost_size));
  int j = ((independent_index) - (ni + 2*ghost_size)*(nj + 2*ghost_size)*k)/(ni + 2*ghost_size);
  int i = (independent_index) - (ni + 2*ghost_size)*(nj + 2*ghost_size)*k - (ni + 2*ghost_size)*j;

  // Computation of the indices with the offset (i goes from -ghost_size_ to ni + ghost_size)
  k -= ghost_size;
  j -= ghost_size;
  i -= ghost_size;

  Int3 ijk = {i, j, k};
  return ijk;
}

/*! signed_independent_index: encodes in the sign the phase of the cell in a
 * two-phase flow: positive sign for phase 0, and negative sign for phase 1.
 * With a cut-cell method, this can be used to disambiguate the sub-cell.
 */
int Domaine_IJK::get_signed_independent_index(int phase, int i, int j, int k) const
{
  int signed_independent_index = (phase == 1) ? -1 - get_independent_index(i,j,k) : get_independent_index(i,j,k);
  return signed_independent_index;
}

int Domaine_IJK::get_independent_index_from_signed_independent_index(int signed_independent_index) const
{
  int independent_index = (signed_independent_index < 0) ? -1 - signed_independent_index : signed_independent_index;
  return independent_index;
}

int Domaine_IJK::get_phase_from_signed_independent_index(int signed_independent_index) const
{
  int phase = (signed_independent_index < 0) ? 1 : 0;
  return phase;
}

/*! Check whether the cell (i,j,k) is contained within the specified ghost along any direction.
 */
bool Domaine_IJK::within_ghost(int i, int j, int k, int negative_ghost_size, int positive_ghost_size) const
{
  bool i_within_ghost = ((i >= -negative_ghost_size) && (i < get_nb_elem_local(0) + positive_ghost_size));
  bool j_within_ghost = ((j >= -negative_ghost_size) && (j < get_nb_elem_local(1) + positive_ghost_size));
  bool k_within_ghost = ((k >= -negative_ghost_size) && (k < get_nb_elem_local(2) + positive_ghost_size));

  return (i_within_ghost && j_within_ghost && k_within_ghost);
}

/*! Check whether the cell (i,j,k) is contained within the specified ghost along a specific direction.
 */
bool Domaine_IJK::within_ghost_along_dir(int dir, int i, int j, int k, int negative_ghost_size, int positive_ghost_size) const
{
  assert(dir >= 0 && dir < 3);

  bool i_local = ((i >= 0) && (i < get_nb_elem_local(0)));
  bool j_local = ((j >= 0) && (j < get_nb_elem_local(1)));
  bool k_local = ((k >= 0) && (k < get_nb_elem_local(2)));

  bool i_within_ghost = ((i >= -negative_ghost_size) && (i < get_nb_elem_local(0) + positive_ghost_size));
  bool j_within_ghost = ((j >= -negative_ghost_size) && (j < get_nb_elem_local(1) + positive_ghost_size));
  bool k_within_ghost = ((k >= -negative_ghost_size) && (k < get_nb_elem_local(2) + positive_ghost_size));

  bool case_i_outside = (i_within_ghost && j_local && k_local);
  bool case_j_outside = (i_local && j_within_ghost && k_local);
  bool case_k_outside = (i_local && j_local && k_within_ghost);

  return select_dir(dir, case_i_outside, case_j_outside, case_k_outside);
}

int Domaine_IJK::correct_perio_i_local(int direction, int i) const
{
  int n = get_nb_elem_local(direction);
  int ng = get_nb_elem_tot(direction);

  int index = -1;
  if (is_uniform(direction))
    {
      index = i;

      if (get_periodic_flag(direction))
        {
          if ((index < 0) && (index + ng - n + 1 < -index))
            {
              index += ng;
            }
          else if ((index >= n) && (-(index - ng) < index - n + 1))
            {
              index -= ng;
            }
        }
    }
  else
    {
      Cerr << "Error: In correct_perio_i_local(), the case of a non-uniform mesh along direction " << direction << " is not implemented." << finl;
      Process::exit();
    }

  return index;
}

int Domaine_IJK::get_i_along_dir_no_perio(int direction, double coord_dir, Domaine_IJK::Localisation loc) const
{
  bool loc_equal_dir = (((loc == Domaine_IJK::FACES_I) && (direction == 0)) || ((loc == Domaine_IJK::FACES_J) && (direction == 1)) || ((loc == Domaine_IJK::FACES_K) && (direction == 2)));

  const double d = get_constant_delta(direction);

  const int offset_dir = get_offset_local(direction);
  double origin_dir = get_origin(direction) - .5*d*loc_equal_dir;

  int index = -1;
  if (is_uniform(direction))
    {
      index = (int)(std::floor((coord_dir - origin_dir)/d)) - offset_dir;
    }
  else
    {
      Cerr << "Error: get_i_along_dir_no_perio(), the case of a non-uniform mesh along direction " << direction << " is not implemented." << finl;
      Process::exit();
    }

  return index;
}

int Domaine_IJK::get_i_along_dir_perio(int direction, double coord_dir, Domaine_IJK::Localisation loc) const
{
  int index_no_perio = get_i_along_dir_no_perio(direction, coord_dir, loc);
  int index_perio = correct_perio_i_local(direction, index_no_perio);
  return index_perio;
}

Int3 Domaine_IJK::get_ijk_from_coord(double coord_x, double coord_y, double coord_z, Domaine_IJK::Localisation loc) const
{
  int i = get_i_along_dir_perio(0, coord_x, loc);
  int j = get_i_along_dir_perio(1, coord_y, loc);
  int k = get_i_along_dir_perio(2, coord_z, loc);

  Int3 ijk = {i, j, k};
  return ijk;
}


