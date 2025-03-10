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

#ifndef Domaine_IJK_included
#define Domaine_IJK_included

#include <FixedVector.h>
#include <Vecteur3.h>
#include <TRUSTArrays.h>
#include <TRUSTTab.h>
#include <Domaine_base.h>
#include <Front_VF.h>
#include <IJK_Field_forward.h>
#include <Domaine_forward.h>
#include <Param.h>
#include <Faces.h>
#include <Joint.h>
#include <Static_Int_Lists.h>
#include <Faces_builder.h>
#include <Probleme_base.h>
#include <Interprete_bloc.h>
#include <Linear_algebra_tools.h>
#include <IJKArray_with_ghost.h>
#include <Analyse_Angle.h>
#include <Connectivite_som_elem.h>
#include <Scatter.h>

using Int3 = FixedVector<int, 3>;

/*! @brief This class encapsulates all the information related to the eulerian mesh for TrioIJK
 *
 * Multiples methods regarding how to navigates through it, volume of the each cells, etc...
 */
class Domaine_IJK : public Domaine_base
{
  Declare_instanciable_sans_constructeur(Domaine_IJK);

public:
  /*! @brief Localisation sub class
   */
  enum Localisation
  {
    ELEM,
    NODES,
    FACES_I,
    FACES_J,
    FACES_K
  };

  /*! @brief status sub class to not compute the same structure twitce when not needed
   */
  enum grid_status
  {
    DEFAULT,
    INITIALIZED,
    DONE
  };
  /*! @brief Returns the face according to the direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return A face
   */
  static Localisation Faces_Dir_To_Localisation(int direction)
  {
    Localisation faces[] = {FACES_I, FACES_J, FACES_K};
    assert(direction >= 0 && direction <= 2);
    return faces[direction];
  }

  Domaine_IJK();

  // Initializes the object by analysing the provided VDF domaine (works for a distributed mesh)
  void initialize_from_unstructured(const Domaine&,
                                    int direction_for_x,
                                    int direction_for_y,
                                    int direction_for_z,
                                    bool perio_x, bool perio_y, bool perio_z);

  /*! @brief Buils a splitting of the given deometry on the requested number of processors
   *         in each direction.
   *
   *         Process_grouping allows to rearrange process ranks by packets of ni*nj*nk processes
   *         to matc the topology of the cluster/node. ex: 8 cores node/machine => use groups
   *         of size 2x2x2 to minimize extra-node messages.
   *
   *  @param nproc_i Number of processors in i direction.
   *  @param nproc_j Number of processors in j direction.
   *  @param nproc_k Number of processors in k direction.
   *  @param process_grouping_i 1 by default. Number of processors per subdomain in i direction.
   *  @param process_grouping_j 1 by default. Number of processors per subdomain in j direction.
   *  @param process_grouping_k 1 by default. Number of processors per subdomain in k direction.
   */
  void initialize_splitting(Domaine_IJK& dom,
                            int nproc_i, int nproc_j, int nproc_k,
                            int process_grouping_i = 1, int process_grouping_j = 1,
                            int process_grouping_k = 1);

  /*! @brief Creates a splitting of the domain by specifying the slice
   *         sizes and the processor mapping.
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
  void initialize_mapping(Domaine_IJK& dom, const ArrOfInt& slice_size_i,
                          const ArrOfInt& slice_size_j,
                          const ArrOfInt& slice_size_k,
                          const IntTab& processor_mapping);

  /*! @brief Initializes class elements given dataset's parameters.
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
  void initialize_origin_deltas(double x0, double y0, double z0,
                                const ArrOfDouble& delta_x,
                                const ArrOfDouble& delta_y,
                                const ArrOfDouble& delta_z,
                                bool perio_x, bool perio_y, bool perio_z);

  /*! @brief Builds the geometry, parallel splitting and DOF correspondance
   *         between a "father" region and a "son" region which is a subpart
   *         of the father region.
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
  void init_subregion(const Domaine_IJK& src, int ni, int nj, int nk,
                      int offset_i, int offset_j, int offset_k,
                      const Nom& subregion,
                      bool perio_x = false, bool perio_y = false, bool perio_z = false);

  /*! @brief Creates a splitting of the domain by specifying the mapping.
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
  void initialize_with_mapping(const ArrOfInt& slice_size_i,
                               const ArrOfInt& slice_size_j,
                               const ArrOfInt& slice_size_k,
                               const IntTab& processor_mapping);

  /*! @brief
   *
   */
  //  void discretiser();

  /*! @brief renvoie new(Faces) ! elle est surchargee par Domaine_VDF par ex.
   */
  Faces *creer_faces();

  /*! @brief Returns the number of elements owned by this processor in the given direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return nb_elem_local_[direction]
   */
  inline int get_nb_elem_local(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return nb_elem_local_[direction];
  }

  /*! @brief Returns the number of element owned by this processor.
   */
  inline int get_nb_elem_local() const
  {
    assert(Objet_U::dimension == 3);
    assert(nb_elem_local_[0] > 0);
    assert(nb_elem_local_[1] > 0);
    assert(nb_elem_local_[2] > 0);
    return nb_elem_local_[0] * nb_elem_local_[1] * nb_elem_local_[2];
  }

  /*! @brief Returns the number of nodes owned by this processor (generally equal to nb_elem_local())
   *
   *  The last node of the last element is owned by the next processor.
   *
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return nb_nodes_local_[dir]
   */
  inline int get_nb_nodes_local(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return nb_nodes_local_[direction];
  }

  /*! @brief Returns the number, in requested direction, of faces that are oriented in direction of "compo"
   *
   *  The last face of the last element is owned by the next processor.
   *
   *  @param compo direction In IJK, horizontal(0), vertical(1) or z-axis depth(2)
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return nb_faces_local_[compo][direction]
   */
  inline int get_nb_faces_local(int compo, int direction) const
  {
    assert(compo >= 0 && compo < 3);
    assert(direction >= 0 && direction < 3);
    return nb_faces_local_[compo][direction];
  }

  /*! @brief Returns the number of local items (on this processor) for the given localisation in the requested direction
   *
   *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return Number of requested items
   */
  int get_nb_items_local(Localisation loc, int direction) const;

  /*! @brief Returns the total (global) number of mesh cells in requested direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return Number of elements
   */
  inline int get_nb_elem_tot(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return get_delta(direction).size_array();
  }

  /*! @brief Returns the total (global) number of mesh cells.
   */
  inline int get_nb_elem_tot() const
  {
    assert(Objet_U::dimension == 3);
    return get_delta(0).size_array() * get_delta(1).size_array() * get_delta(2).size_array();
  }

  /*! @brief Returns the length of the entire domain in requested direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return Global length of the domain
   */
  double get_domain_length(int direction) const;

  /*! @brief Returns the number of local items (on this processor) for the given localisation in the requested direction
   *
   *  If periodic along requested direction, need to add the last item for nodes and faces.
   *
   *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return Number of requested items
   */
  int get_nb_items_global(Localisation loc, int direction) const;

  /*! @brief Returns the local offset in requested direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return offset_[direction]
   */
  inline int get_offset_local(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return offset_[direction];
  }

  /*! @brief Returns the position of the local subdomain in the requested direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2)
   *  @return processor_position_[direction]
   */
  inline int get_local_slice_index(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return processor_position_[direction];
  }

  /*! @brief Returns the index of the requested neighbour processor (-1 if no neighbour).
   *
   *  previous_or_next = 0 => get processor at left (in the direction of smaller indices).
   *  previous_or_next = 1 => get processor at right (in the direction of the larger indices).
   *
   *  @param preivous_or_next 0 or 1, previous or next one respectively.
   *  @param direction In IJK, x(0), y(1) or z(2).
   *
   *  @return neighbour_processors_[previous_or_next][dir]
   */
  inline int get_neighbour_processor(int previous_or_next, int direction) const
  {
    assert(direction >= 0 && direction < 3);
    assert(previous_or_next == 0 || previous_or_next == 1);
    return neighbour_processors_[previous_or_next][direction];
  }

  /*! @brief Returns the number of slices in the given direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return nproc_per_direction_[direction]
   */
  inline int get_nprocessor_per_direction(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return nproc_per_direction_[direction];
  }

  /*! @brief Returns an array with the coordinates of all nodes in the mesh in requested direction.
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return node_coordinates_xyz_[direction]
   */
  inline const ArrOfDouble& get_node_coordinates(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return node_coordinates_xyz_[direction];
  }

  /*! @brief Returns the coordinate of the first node (global) of the mesh in the requested direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return coordinate.
   */
  inline double get_origin(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return node_coordinates_xyz_[direction][0];
  }

  /*! @brief Returns the array of mesh cell sizes in requested direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return Array with the sizes.
   */
  inline const ArrOfDouble& get_delta(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return delta_xyz_[direction];
  }

  /*! @brief Returns the size of cells in a direction
   *
   * This requires the size of all cells in this direction to be the same,
   * Which means is_uniform(direction) has to be true.
   *
   * @param direction In IJK, x(0), y(1) or z(2).
   * @return length of cells in this direction.
   */
  inline double get_constant_delta(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    if (!is_uniform(direction))
      {
        Cerr << "Error in Domaine_IJK::get_constant_delta: grid is not uniform in direction : " << direction << endl;
        assert(0);
        Process::exit();
      }
    return delta_xyz_[direction][0];
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
  void get_local_mesh_delta(int direction, int ghost_cells,
                            ArrOfDouble_with_ghost& delta) const;

  /*! @brief Fills an array containing the mapping of processors
   *
   *
   *  For 3 slices indices i,j,k, the processor that owns
   *  the intersection of these slices is mapping(i,j,k).
   *
   *  @param mapping Table in which we'll copy the mapping
   */
  inline void get_processor_mapping(IntTab& mapping) const { mapping = mapping_; }

  /*! @brief Returns the indices of the first cell in requested direction
   *         of every slices in this direction
   *
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @param tab Array in which we'll store the offsets in that direction
   */
  inline void get_slice_offsets(int direction, ArrOfInt& tab) const
  {
    assert(direction >= 0 && direction < 3);
    tab = offsets_all_slices_[direction];
  }

  /*! @brief Returns the number of items of given location (elements, nodes, faces...)
   *         for all slices in the requested direction.
   *
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @param loc In IJK, ELEM, NODES, FACES_I, FACES_J or FACES_K
   *  @param tab Array in which we'll store the number of slices in given direction
   */
  void get_slice_size(int direction, Localisation loc, ArrOfInt& tab) const;

  /*! @brief Return the global index of the processor according to its position.
   *
   *  @param slice Vector with the x, y, and z coordinate of the processor.
   *  @return mapping_(slice[0], slice[1], slice[2])
   */
  inline int get_processor_by_ijk(const FixedVector<int, 3>& slice) const { return mapping_(slice[0], slice[1], slice[2]); }

  /*! @brief Return the global index of the processor according to its position.
   *
   *  @param slice_i First index of the processor in the global mapping
   *  @param slice_j Second index of the processor in the global mapping
   *  @param slice_k Third index of the processor in the global mapping
   *  @return return mapping_(slice_i, slice_j, slice_k)
   */
  inline int get_processor_by_ijk(int slice_i, int slice_j, int slice_k) const
  {
    return mapping_(slice_i, slice_j, slice_k);
  }

  /*! Returns the processor associated with slice indices i,j,k  accounting for periodicity
   * e.g. a slice index -1 can refer to the last slice along a periodic direction.
   */
  int periodic_get_processor_by_ijk(int slice_i, int slice_j, int slice_k) const;

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
  Vecteur3 get_coords_of_dof(int i, int j, int k, Localisation loc) const;

  double get_coord_of_dof_along_dir(int dir, int i, Localisation loc) const;

  /*! independent_index adds a ghost_size to the packed index.
   * It is similar to the linear_index defined in IJK_Field_local_template, but with
   * a universal, predefined ghost_size of 256 instead of a field-dependent ghost_size.
   * Since the ghost_size_ value is larger than any ghost_size expected to be used in
   * practice, any virtual cell can be represented by the independent index.
   */
  int get_independent_index(int i, int j, int k) const;
  Int3 get_ijk_from_independent_index(int independent_index) const;

  /*! signed_independent_index: encodes in the sign the phase of the cell in a
   * two-phase flow: positive sign for phase 0, and negative sign for phase 1.
   * With a cut-cell method, this can be used to disambiguate the sub-cell.
   */
  int get_signed_independent_index(int phase, int i, int j, int k) const;
  int get_independent_index_from_signed_independent_index(int signed_independent_index) const;
  int get_phase_from_signed_independent_index(int signed_independent_index) const;

  /*! Check whether the cell (i,j,k) is contained within the specified ghost along any direction.
   */
  bool within_ghost(int i, int j, int k, int negative_ghost_size, int positive_ghost_size) const;

  /*! Check whether the cell (i,j,k) is contained within the specified ghost along a specific direction.
   */
  bool within_ghost_along_dir(int dir, int i, int j, int k, int negative_ghost_size, int positive_ghost_size) const;

  template <int _DIR_>
  bool within_ghost_(int i, int j, int k, int negative_ghost_size, int positive_ghost_size) const
  {
    int dir = static_cast<int>(_DIR_);
    return within_ghost_along_dir(dir, i, j, k, negative_ghost_size, positive_ghost_size);
  }

  int correct_perio_i_local(int direction, int i) const;
  int get_i_along_dir_no_perio(int direction, double coord_dir, Localisation loc) const;
  int get_i_along_dir_perio(int direction, double coord_dir, Localisation loc) const;

  Int3 get_ijk_from_coord(double coord_x, double coord_y, double coord_z, Localisation loc) const;

  /*! @brief Converts the ijk index of an element to a cell index.
   *
   *  Adapted from Maillage_FT_IJK.h
   *
   *  @param ijk Vector with the x, y, and z coordinates.
   *  @return The LOCAL index of an element.
   */
  inline int convert_ijk_cell_to_packed(const FixedVector<int, 3> ijk) const
  {
    return convert_ijk_cell_to_packed(ijk[0], ijk[1], ijk[2]);
  }

  /*! @brief With three indices, find the local index of an element
   *
   *  @param i Local index of an element along x axis.
   *  @param j Local index of an element along y axis.
   *  @param k Local index of an element along z axis.
   *
   *  @return The LOCAL index of an element.
   */
  int convert_ijk_cell_to_packed(int i, int j, int k) const;

  /*! @brief Convert the local index of an element to a vector with IJK indices.
   *
   *  Adapted from Maillage_FT_IJK.h
   *
   *  @param index Local index of the element.
   *  @return Vector with IJK coordinates.
   */
  FixedVector<int, 3> convert_packed_to_ijk_cell(int index) const;

  /*! @brief Find the element which contains the item's coodirnates.
   *
   *  The element's coordinates can be outside of this processor's subdomain.
   *
   *  @param x First coordinate of an item in the mesh.
   *  @param y Second coordinate of an item in the mesh.
   *  @param z Third coordinate of an item in the mesh.
   *  @param ijk_global The global coordinates of the cell.
   *  @param ijk_local The local coordinates of the cell.
   *  @param ijk_me A sort of 3D flag. Will be [1,1,1] if the element belongs to me.
   */
  void search_elem(const double& x, const double& y, const double& z,
                   FixedVector<int, 3>& ijk_global,
                   FixedVector<int, 3>& ijk_local,
                   FixedVector<int, 3>& ijk_me) const;

  /*! @brief Method returns true if periodic in this direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return true or false
   */
  inline bool get_periodic_flag(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return periodic_[direction];
  }

  /*! @brief Method returns true if uniform in this direction
   *  @param direction In IJK, x(0), y(1) or z(2).
   *  @return true or false
   */
  inline bool is_uniform(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return uniform_[direction];
  }

  /*! @brief Updates volume_elem_
   *  / ! \ If the grid changes, this needs to be called again!
   */
  void update_volume_elem();

  /*! @brief Returns volume_elem_ */
  inline const DoubleVect& get_volume_elem() const
  {
    assert(volume_elem_.size_array() > 0 && "volume_elem_ is empty. Update before trying to read it.");
    assert(volume_elem_.size_array() == get_nb_elem_local() && "volume_elem_ is not up to date. Update before trying to read it.");
    assert(volume_elem_status_ == DONE);
    return volume_elem_;
  }

  inline int ft_extension() const { return ft_extension_; }

  void set_extension_from_bulle_param(double vol_bulle, double diam_bulle);

private:
  /*! @brief  Coordinates of all nodes (when cell size is needed, take delta_xyz_ which is more accurate) in directions i, j and k.
   *
   * We have: node_coordinates_xyz_.size() == 3  (3 directions in space)
   * node_coordinates_xyz_[DIRECTION_I/J/K] == number of cells in direction i,j,k plus one
   * The coordinate of the last node is the end coordinate of the mesh: if mesh is periodic, the last
   * node coordinate is not equal to the first one.
   */
  VECT(ArrOfDouble)
  node_coordinates_xyz_;
  /*! @brief Mesh cell sizes for the entire mesh.
   *
   *  The size of each array is equal to the total number of cells in each direction.
   *  If possible, this data is not computed from nodes (less truncation errors for uniform meshes)
   */
  VECT(ArrOfDouble)
  delta_xyz_;
  /*! Number of processors in each direction */
  FixedVector<int, 3> nproc_per_direction_;
  /*! @brief Global processor mapping: for each subdomain, which processor has it (indexed like this: mapping_(i, j, k)) */
  IntTab mapping_;
  /*! For each direction, offsets of all slices */
  VECT(ArrOfInt)
  offsets_all_slices_; ///< F
  /*! @brief For each direction, size of all slices */
  VECT(ArrOfInt)
  sizes_all_slices_;
  /*! @brief Stores the uniform flag for each direction */
  bool uniform_[3];
  /*! @brief Stores the periodic flag for each direction */
  bool periodic_[3];

  // Local data (processor dependent)
  // --------------------------------

  /*! @brief Where is this processor in the global domain (slice number in each direction, -1 if processor has no data) */
  FixedVector<int, 3> processor_position_;
  /*! @brief Number of element in requested direction.
   *   If the current processor has an empty subdomain, the number of elements, nodes, faces is zero.
   */
  FixedVector<int, 3> nb_elem_local_;
  /*! @brief Number of nodes in requested direction.
   *   If the current processor has an empty subdomain, the number of elements, nodes, faces is zero.
   */
  FixedVector<int, 3> nb_nodes_local_;
  /*! indexing is nb_faces_local_[for orientation i][number of faces in direction j] */
  FixedVector<FixedVector<int, 3>, 3> nb_faces_local_;
  /*! Index in the global mesh of the first (non ghost) element on this processor, in each direction */
  FixedVector<int, 3> offset_;
  /*! @brief MPI ranks of the processors that hold the neighbour domains.
   *   Indexing is neighbour_processors_[previous=0, next=1][direction].
   *   Contains -1 if no neighbour.
   *   Wraps if periodic domain, if there is only one processor in a direction, the neighbour might be myself.
   */
  FixedVector<FixedVector<int, 3>, 2> neighbour_processors_;
  /*! Volume of each element on this processor */
  DoubleVect volume_elem_;
  /*! State of volume_elem_ on this processor */
  grid_status volume_elem_status_;
};

inline double Domaine_IJK::get_coord_of_dof_along_dir(int dir, int i, Localisation loc) const
{
  int gi = i + offset_[dir];
  double x = get_node_coordinates(dir)[gi];

  bool loc_equal_dir = (((loc == FACES_I) && (dir == 0)) || ((loc == FACES_J) && (dir == 1)) || ((loc == FACES_K) && (dir == 2)));
  bool loc_equal_dir_or_nodes = loc_equal_dir || (loc == NODES);

  if (!loc_equal_dir_or_nodes)
    x += get_delta(dir)[gi] * 0.5;

  return x;
}

#endif
