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

#ifndef IJK_Grid_Geometry_included
#define IJK_Grid_Geometry_included
#include <IJK_Field_forward.h>
#include <TRUSTArrays.h>

class Zone;

// This class stores the geometry of the entire ijk grid (number of cells, cell sizes, periodicity, etc).
// The content of the object is identical on all processors of the group.
class IJK_Grid_Geometry : public Objet_U
{
  Declare_instanciable_sans_constructeur(IJK_Grid_Geometry);
public:
  IJK_Grid_Geometry();
  void nommer(const Nom& n) override { object_name_ = n; }
  const Nom& le_nom() const override { return object_name_; }

  // Initializes the object by analysing the provided VDF zone (works for a distributed mesh)
  void initialize_from_unstructured(const Zone&,
                                    int direction_for_x,
                                    int direction_for_y,
                                    int direction_for_z,
                                    bool perio_x, bool perio_y, bool perio_z);

  // Initializes the object with the given parameters
  void initialize_origin_deltas(double x0, double y0, double z0,
                                const ArrOfDouble& delta_x, const ArrOfDouble& delta_y, const ArrOfDouble& delta_z,
                                bool perio_x, bool perio_y, bool perio_z);

  // Returns the answer of the test "is mesh periodic in requested direction ?"
  bool get_periodic_flag(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    return periodic_[direction];
  }

  // Returns the coordinate of the first node of the mesh in the requested direction
  double get_origin(int direction) const { return node_coordinates_xyz_[direction][0]; }

  // Returns the total number of mesh cells in requested direction
  int get_nb_elem_tot(int direction) const { return get_delta(direction).size_array(); }

  // Returns the array of mesh cell sizes in requested direction
  const ArrOfDouble& get_delta(int direction) const { return delta_xyz_[direction]; }

  // Returns the coordinates of all nodes in the mesh. Array size is equal to number of cells plus 1.
  const ArrOfDouble& get_node_coordinates(int direction) const { return node_coordinates_xyz_[direction]; }

  // If the mesh cells have a constant size in requested direction, returns the size, otherwise error.
  double get_constant_delta(int direction) const
  {
    assert(direction >= 0 && direction < 3);
    if (!is_uniform(direction))
      {
        Cerr << "Error in IJK_Grid_Geometry::get_constant_delta: grid is not uniform in direction " << direction << endl;
        Process::exit();
      }
    return delta_xyz_[direction][0];
  }

  bool is_uniform(int direction) const
  {
    assert(direction>=0 && direction<3);
    return uniform_[direction];
  }
  double get_domain_length(int direction) const
  {
    const int n = get_nb_elem_tot(direction);
    double x0 = get_origin(direction);
    double x1 = get_node_coordinates(direction)[n];
    return x1-x0;
  }
protected:
  // Grid geometry name when the object is named and read from a Trio_U data file
  Nom object_name_;
  // Coordinates of all nodes (when cell size is needed, take delta_xyz_ which is more accurate)
  // in directions i, j and k
  // We have: node_coordinates_xyz_.size() == 3  (3 directions in space)
  // node_coordinates_xyz_[DIRECTION_I/J/K] == number of cells in direction i,j,k plus one
  // The coordinate of the last node is the end coordinate of the mesh: if mesh is periodic, the last
  //  node coordinate is not equal to the first one.
  VECT(ArrOfDouble) node_coordinates_xyz_;
  // Mesh cell sizes for the entire mesh.
  // The size of each array is equal to the total number of cells in each direction.
  // If possible, this data is not computed from nodes (less truncation errors for uniform meshes)
  VECT(ArrOfDouble) delta_xyz_;
  // Is grid uniform in each direction ?
  bool uniform_[3];
  // Is grid periodic in each direction ?
  bool periodic_[3];
};
#endif
