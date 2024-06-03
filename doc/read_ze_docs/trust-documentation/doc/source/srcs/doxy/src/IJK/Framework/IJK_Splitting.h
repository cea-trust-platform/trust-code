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

#ifndef IJK_Splitting_included
#define IJK_Splitting_included

#include <Linear_algebra_tools.h>
#include <IJK_Grid_Geometry.h>
#include <FixedVector.h>
#include <TRUSTArrays.h>
#include <TRUSTTab.h>

typedef FixedVector<int,3> Int3;

// This class describes the splitting of a mesh on processors
// It gives the offset and number of elements, nodes, faces... on the local processor
// (each processor contains a different subset of the global mesh)
class IJK_Splitting : public Objet_U
{
  Declare_instanciable_sans_constructeur(IJK_Splitting);
protected:
  typedef FixedVector<Int3,3> Int33;
  typedef FixedVector<Int3,2> Int23;
public:
  static double shear_x_time_ ;
  static double shear_x_DT_ ;
  static double Lx_for_shear_perio ;
  static int defilement_ ;
  enum Localisation { ELEM, NODES, FACES_I, FACES_J, FACES_K };
  static Localisation FacesDirToLocalisation(int dir)
  {
    Localisation toto[] = { FACES_I, FACES_J, FACES_K };
    assert(dir >= 0 && dir <= 2);
    return toto[dir];
  }

  IJK_Splitting();
  void nommer(const Nom& n) override { object_name_ = n; }
  const Nom& le_nom() const override { return object_name_; }
  void initialize(const IJK_Grid_Geometry& grid_geom,
                  int nproc_i, int nproc_j, int nproc_k,
                  int process_grouping_i = 1, int process_grouping_j = 1, int process_grouping_k = 1);
  void initialize(const IJK_Grid_Geometry& geom,
                  const ArrOfInt& slice_size_i,
                  const ArrOfInt& slice_size_j,
                  const ArrOfInt& slice_size_k,
                  const IntTab& processor_mapping);

  void init_subregion(const IJK_Splitting& src,
                      int ni, int nj, int nk,
                      int offset_i, int offset_j, int offset_k,
                      const Nom& subregion,
                      bool perio_x = false, bool perio_y = false, bool perio_z = false);

  const IJK_Grid_Geometry& get_grid_geometry() const
  {
    return grid_geom_;
  }
  // Returns the number of elements owned by this processor in the given direction (0,1 or 2)
  int get_nb_elem_local(int dir) const
  {
    return nb_elem_local_[dir];
  }
  // Returns the number of nodes owned by this processor (generally equal to nb_elem_local()
  //  because the last node of the last element is owned by the next processor)
  int get_nb_nodes_local(int dir) const
  {
    return
      nb_nodes_local_[dir];
  }
  // Returns the number, in direction dir, of faces that are oriented in direction "compo"
  //  (same remark that for the number of nodes)
  int get_nb_faces_local(int compo, int dir) const
  {
    return nb_faces_local_[compo][dir];
  }

  // Returns the number of local items (on this processor) for the given localisation in the given direction
  int get_nb_items_local(Localisation loc, int dir) const
  {
    switch(loc)
      {
      case ELEM:
        return get_nb_elem_local(dir);
      case NODES:
        return get_nb_nodes_local(dir);
      case FACES_I:
        return get_nb_faces_local(0, dir);
      case FACES_J:
        return get_nb_faces_local(1, dir);
      case FACES_K:
        return get_nb_faces_local(2, dir);
      }
    return -1;
  }
  int get_nb_items_global(Localisation loc, int dir) const;

  // Returns the global index (in the whole mesh) of the first real local element/node/face on this processor in specified direction
  int get_offset_local(int dir) const
  {
    return offset_[dir];
  }
  // Returns the position of the local subdomain in the requested direction
  int get_local_slice_index(int dir) const { return processor_position_[dir]; }

  // Returns the index of the requested neighbour processor (-1 if no neighbour)
  // previous_or_next = 0 => get processor at left (in the direction of smaller indices)
  // previous_or_next = 1 => get processor at right (in the direction of the larger indices)
  int get_neighbour_processor(int previous_or_next, int dir) const
  {
    return neighbour_processors_[previous_or_next][dir];
  }
  // Returns the number of slices in the given direction
  int get_nprocessor_per_direction(int dir) const
  {
    return nproc_per_direction_[dir];
  }

  // Fills the "delta" array with the size of the cells owned by the processor in the given direction.
  // "delta" is redimensionned with the specified number of ghost cells and filled.
  void get_local_mesh_delta(int dir, int ghost_cells, ArrOfDouble_with_ghost& delta) const;
  // Fills an array containing the mapping of processors: for 3 slice indices i,j,k, the processor
  //  that owns the intersection of these slices is mapping(i,j,k).
  void get_processor_mapping(IntTab& mapping) const;
  // Returns the indices of the first cell in direction "dir" of every slices in this direction
  // (array in redimensionned and filled).
  void get_slice_offsets(int dir, ArrOfInt&) const;
  // Returns the number of items of given location (elements, nodes, faces...) for alls slices in
  // the given direction.
  void get_slice_size(int dir, Localisation loc, ArrOfInt&) const;
  int get_processor_by_ijk(const Int3& slice) const
  {
    return mapping_(slice[0], slice[1], slice[2]);
  }
  int get_processor_by_ijk(int slice_i, int slice_j, int slice_k) const
  {
    return mapping_(slice_i, slice_j, slice_k);
  }
  inline Vecteur3 get_coords_of_dof(int i, int j, int k, Localisation loc) const;

  // Convert the ijk index to a cell number : (adapted from Maillage_FT_IJK.h)
  int convert_ijk_cell_to_packed(const Int3 ijk) const
  {
    return convert_ijk_cell_to_packed(ijk[0], ijk[1], ijk[2]);
  }

  int convert_ijk_cell_to_packed(int i, int j, int k) const
  {
    const int nbmailles_euler_i = get_nb_elem_local(0); //DIRECTION_I
    const int nbmailles_euler_j = get_nb_elem_local(1);//DIRECTION_J);
    // const int nbmailles_euler_k = get_nb_elem_local(2);//DIRECTION_K);
    assert((i < 0 && j < 0 && k < 0)
           || (i >= 0 && i < nbmailles_euler_i
               && j >= 0 && j < nbmailles_euler_j
               && k >= 0 && k < get_nb_elem_local(2)));  // nbmailles_euler_k));
    if (i < 0)
      return -1;
    else
      return (k * nbmailles_euler_j + j) * nbmailles_euler_i + i;
  }

  // Convert the cell number to the ijk index :  (adapted from Maillage_FT_IJK.h)
  Int3 convert_packed_to_ijk_cell(int index) const
  {
    const int nbmailles_euler_i = get_nb_elem_local(0); // DIRECTION_I
    const int nbmailles_euler_j = get_nb_elem_local(1); // DIRECTION_J;
    // const int nbmailles_euler_k = get_nb_elem_local(2); // DIRECTION_K;

    Int3 ijk;
    if (index < 0)
      {
        ijk[0] = ijk[1] = ijk[2] = -1;
      }
    else
      {
        ijk[0] = index % nbmailles_euler_i;
        index /= nbmailles_euler_i;
        ijk[1] = index % nbmailles_euler_j;
        index /= nbmailles_euler_j;
        ijk[2] = index;
        /*
         * TODO: M.G. Effacer
         */
        //        Cerr << "nbmailles_euler_i" << nbmailles_euler_i << finl;
        //        Cerr << "nbmailles_euler_j" << nbmailles_euler_j << finl;
        //        Cerr << "index" << index << finl;
        //     assert(index < nbmailles_euler_k);
        assert(index < get_nb_elem_local(2));
      }
    return ijk;
  }

  // ijk_global : the global coordinate of the cell
  // ijk_local is the local coordinate of the cell on the process ijk_processeur
  // ijk_me = [1,1,1] if the elem belongs to me... A sort of 3D flag.
  void search_elem(const double& x, const double& y, const double& z,
                   Int3& ijk_global, Int3& ijk_local, Int3& ijk_me) const
  {
    Vecteur3 coords(x,y,z);
    ijk_me[0] = 0;
    ijk_me[1] = 0;
    ijk_me[2] = 0;
    for (int i=0; i<3; i++)
      {
        const double d = grid_geom_.get_constant_delta(i);
        const double o = grid_geom_.get_origin(i);
        //const double l = grid_geom_.get_domain_length(i);
        //const int N = grid_geom_.get_nb_elem_tot(i);
        const double val = (coords[i] - o)/d;
        const int ival = (int)std::lrint(std::floor(val));
        ijk_global[i] = ival;
        assert((ival>=0) && (ival<grid_geom_.get_nb_elem_tot(i)));
        const int ioff = get_offset_local(i);
        const int n_loc = get_nb_elem_local(i);
        if ((ival>=ioff) && (ival<ioff+n_loc))
          ijk_me[i] = 1;
        ijk_local[i] = ival - ioff;
      }
  }

protected:
  // Global data (the following variables have the same value on all processors)
  // ---------------------------------------------------------------------------
  Nom object_name_;
  IJK_Grid_Geometry grid_geom_;
  // Number of processors in each direction
  Int3 nproc_per_direction_;
  // Global processor mapping: for each subdomain, which processor has it (indexed like this: mapping_(i, j, k))
  IntTab mapping_;
  // For each direction, offsets of all slices
  VECT(ArrOfInt) offsets_all_slices_;
  // For each direction, size of all slices
  VECT(ArrOfInt) sizes_all_slices_;

  // Local data (processor dependent)
  // --------------------------------
  // Where is this processor in the global domain (slice number in each direction, -1 if processor has no data)
  Int3 processor_position_;
  // If the current processor has an empty subdomain, the number of elements, nodes, faces is zero.
  Int3 nb_elem_local_;
  Int3 nb_nodes_local_;
  // indexing is nb_faces_local_[for orientation i][number of faces in direction j]
  Int33 nb_faces_local_;

  // Index in the global mesh of the first (non ghost) element on this processor, in each direction
  Int3 offset_;
  // MPI ranks of the processors that hold the neighbour domains
  // Indexing is neighbour_processors_[previous=0, next=1][direction]
  // Contains -1 if no neighbour
  // Wraps if periodic domain, if there is only one processor in a direction, the neighbour might be myself
  Int23 neighbour_processors_;
};

// dof = degree of freedom
inline Vecteur3 IJK_Splitting::get_coords_of_dof(int i, int j, int k, Localisation loc) const
{
  int gi = i + offset_[0];
  int gj = j + offset_[1];
  int gk = k + offset_[2];
  Vecteur3 xyz;
  xyz[0] = grid_geom_.get_node_coordinates(0)[gi];
  xyz[1] = grid_geom_.get_node_coordinates(1)[gj];
  xyz[2] = grid_geom_.get_node_coordinates(2)[gk];
  if (loc == ELEM || loc == FACES_J || loc == FACES_K)
    xyz[0] += grid_geom_.get_delta(0)[gi] * 0.5;
  if (loc == ELEM || loc == FACES_I || loc == FACES_K)
    xyz[1] += grid_geom_.get_delta(1)[gj] * 0.5;
  if (loc == ELEM || loc == FACES_I || loc == FACES_J)
    xyz[2] += grid_geom_.get_delta(2)[gk] * 0.5;
  return xyz;
}

#endif
