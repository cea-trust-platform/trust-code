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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Refine_Mesh.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Refine_Mesh_included
#define Refine_Mesh_included

#include <Interprete_geometrique_base.h>
#include <TRUSTTabs.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Refine_Mesh
// .HEADER      : TRUST_Kernel_Addons TRUST_Kernel_Addons/src/Mesh_Refinement
// .LIBRARY     : libTRUST_Kernel_Addons
// .DESCRIPTION : class Refine_Mesh
//
// <Description of class Refine_Mesh>
//
/////////////////////////////////////////////////////////////////////////////

class Static_Int_Lists;

class Refine_Mesh : public Interprete_geometrique_base
{

  Declare_instanciable(Refine_Mesh) ;

public :
  Entree& interpreter_(Entree& is) override;

protected :
  void apply(void);

  void check_dimension(void) const;
  void check_nb_zones(void) const;
  void check_cell_type(void) const;

  void apply_2D(void);
  void apply_3D(void);

  void build_edges_2D(IntTab& nodes_of_edges, IntTab& edges_of_cells) const;
  void build_edges_3D(IntTab& nodes_of_edges, IntTab& edges_of_cells) const;

  void build_edges(IntTab&       nodes_of_edges,
                   IntTab&       edges_of_cells,
                   int        nb_edges_per_cell,
                   int        nb_nodes_per_edge,
                   const IntTab& local_nodes_of_edges) const;

  void build_new_nodes(DoubleTab& new_nodes, const IntTab& nodes_of_edges) const;

  void build_new_cells_2D(IntTab& new_cells, const IntTab& edges_of_cells) const;
  void build_new_cells_3D(IntTab& new_cells, const IntTab& edges_of_cells) const;

  void build_new_sub_zones_descriptions(Noms& new_sub_zones_descriptions) const;

  void build_incidence_from_node_to_edges(int            nb_nodes,
                                          const IntTab&     nodes_of_edges,
                                          Static_Int_Lists& incidence) const;

  void build_new_boundary_faces_2D(IntTabs&           new_nodes_of_boundary_faces,
                                   IntTabs&           new_cells_of_boundary_faces,
                                   const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_boundary_faces_3D(IntTabs&           new_nodes_of_boundary_faces,
                                   IntTabs&           new_cells_of_boundary_faces,
                                   const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_connector_faces_2D(IntTabs&           new_nodes_of_connector_faces,
                                    IntTabs&           new_cells_of_connector_faces,
                                    const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_connector_faces_3D(IntTabs&           new_nodes_of_connector_faces,
                                    IntTabs&           new_cells_of_connector_faces,
                                    const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_internal_frontier_faces_2D(IntTabs&           new_nodes_of_internal_frontier_faces,
                                            IntTabs&           new_cells_of_internal_frontier_faces,
                                            const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_internal_frontier_faces_3D(IntTabs&           new_nodes_of_internal_frontier_faces,
                                            IntTabs&           new_cells_of_internal_frontier_faces,
                                            const Static_Int_Lists& incidence_from_node_to_edges) const;

  void build_new_frontier_faces_2D(IntTab&                 new_nodes_of_frontier_faces,
                                   IntTab&                 new_cells_of_frontier_faces,
                                   const Static_Int_Lists& incidence_from_node_to_edges,
                                   const Faces&            old_frontier_faces) const;

  void build_new_frontier_faces_3D(IntTab&                 new_nodes_of_frontier_faces,
                                   IntTab&                 new_cells_of_frontier_faces,
                                   const Static_Int_Lists& incidence_from_node_to_edges,
                                   const Faces&            old_frontier_faces) const;

  int find_edge(const Static_Int_Lists& incidence_from_node_to_edges, int node0, int node1) const;

  void update_domain(const Nom&          cell_type,
                     const Type_Face&    face_type,
                     const DoubleTab&    new_nodes,
                     const IntTab&       new_cells,
                     const Noms&         new_sub_zones_descriptions,
                     const IntTabs& new_nodes_of_boundary_faces,
                     const IntTabs& new_cells_of_boundary_faces,
                     const IntTabs& new_nodes_of_connector_faces,
                     const IntTabs& new_cells_of_connector_faces,
                     const IntTabs& new_nodes_of_internal_frontier_faces,
                     const IntTabs& new_cells_of_internal_frontier_faces);

  void update_nodes(const DoubleTab& new_nodes);

  void update_cells(const IntTab& new_cells);

  void update_octree(const Nom& cell_type);

  void update_sub_zones(const Noms& new_sub_zones_descriptions);

  void update_boundary_faces(const Type_Face&    face_type,
                             const IntTabs& new_nodes_of_boundary_faces,
                             const IntTabs& new_cells_of_boundary_faces);

  void update_connector_faces(const Type_Face&    face_type,
                              const IntTabs& new_nodes_of_connector_faces,
                              const IntTabs& new_cells_of_connector_faces);

  void update_internal_frontier_faces(const Type_Face&    face_type,
                                      const IntTabs& new_nodes_of_internal_frontier_faces,
                                      const IntTabs& new_cells_of_internal_frontier_faces);

};

#endif /* Refine_Mesh_included */
