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
// File:        Refine_Mesh.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Refine_Mesh.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Static_Int_Lists.h>
#include <Array_tools.h>
#include <Sous_Zone.h>
#include <SChaine.h>
#include <EChaine.h>

Implemente_instanciable(Refine_Mesh,"Refine_Mesh",Interprete_geometrique_base) ;

Sortie& Refine_Mesh::printOn(Sortie& os) const
{
  Interprete::printOn(os);
  return os;
}

Entree& Refine_Mesh::readOn(Entree& is)
{
  Interprete::readOn(is);
  return is;
}
// XD refine_mesh interprete refine_mesh 0 not_set
// XD attr domaine ref_domaine domaine 0 not_set
Entree& Refine_Mesh::interpreter_(Entree& is)
{
  associer_domaine(is);
  apply();
  return is;
}

void Refine_Mesh::apply(void)
{
  Cerr << "Refining domain " << domaine().le_nom() << finl;

  check_dimension();
  check_nb_zones();
  check_cell_type();

  if ( Objet_U::dimension == 2 )
    {
      apply_2D();
    }
  else
    {
      apply_3D();
    }

  Cerr << "Refinement... OK" << finl;
}

void Refine_Mesh::check_dimension(void) const
{
  if ( ! ( (Objet_U::dimension == 2) || (Objet_U::dimension == 3) ) )
    {
      Cerr << "Error in 'Refine_Mesh::check_dimension()':" << finl;
      Cerr << "  Invalid dimension: " << Objet_U::dimension << finl;
      Cerr << "  Refine_Mesh can only deal 2D or 3D domains" << finl;
      Process::exit();
    }
}

void Refine_Mesh::check_nb_zones(void) const
{
  if ( domaine().nb_zones() != 1 )
    {
      Cerr << "Error in 'Refine_Mesh::check_nb_zones()':" << finl;
      Cerr << "  Invalid number of zones: " << domaine().nb_zones() << finl;
      Cerr << "  Refine_Mesh cannot deal with domains having more than one zone" << finl;
      Process::exit();
    }
}

void Refine_Mesh::check_cell_type(void) const
{
  const Nom& cell_type = domaine().zone(0).type_elem().valeur().que_suis_je();

  if ( ! ( (cell_type == Motcle("Triangle")) || (cell_type == Motcle("Tetraedre")) ) )
    {
      Cerr << "Error in 'Refine_Mesh::check_cell_type()':" << finl;
      Cerr << "  Invalid cell type: " << cell_type << finl;
      Cerr << "  Refine_Mesh can only deal with triangles and tetrahedrons" << finl;
      Process::exit();
    }
}


void Refine_Mesh::apply_2D(void)
{
  Domaine& domain = domaine();
  Scatter::uninit_sequential_domain(domain);

  assert( domain.zone(0).type_elem().valeur().que_suis_je() == Motcle("Triangle") );

  IntTab nodes_of_edges;
  IntTab edges_of_cells;
  build_edges_2D(nodes_of_edges, edges_of_cells);

  DoubleTab new_nodes;
  build_new_nodes(new_nodes, nodes_of_edges);

  IntTab new_cells;
  build_new_cells_2D(new_cells, edges_of_cells);

  Noms new_sub_zones_descriptions;
  build_new_sub_zones_descriptions(new_sub_zones_descriptions);

  Static_Int_Lists incidence_from_node_to_edges;
  build_incidence_from_node_to_edges(new_nodes.dimension(0),nodes_of_edges,incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_boundary_faces;
  VECT(IntTab) new_cells_of_boundary_faces;
  build_new_boundary_faces_2D(new_nodes_of_boundary_faces, new_cells_of_boundary_faces, incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_connector_faces;
  VECT(IntTab) new_cells_of_connector_faces;
  build_new_connector_faces_2D(new_nodes_of_connector_faces, new_cells_of_connector_faces, incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_internal_frontier_faces;
  VECT(IntTab) new_cells_of_internal_frontier_faces;
  build_new_internal_frontier_faces_2D(new_nodes_of_internal_frontier_faces, new_cells_of_internal_frontier_faces, incidence_from_node_to_edges);

  update_domain(Nom("Triangle"),
                segment_2D,
                new_nodes,
                new_cells,
                new_sub_zones_descriptions,
                new_nodes_of_boundary_faces,
                new_cells_of_boundary_faces,
                new_nodes_of_connector_faces,
                new_cells_of_connector_faces,
                new_nodes_of_internal_frontier_faces,
                new_cells_of_internal_frontier_faces);

  Scatter::init_sequential_domain(domain);
}

void Refine_Mesh::apply_3D(void)
{
  Domaine& domain = domaine();
  Scatter::uninit_sequential_domain(domain);

  assert( domain.zone(0).type_elem().valeur().que_suis_je() == Motcle("Tetraedre") );

  IntTab nodes_of_edges;
  IntTab edges_of_cells;
  build_edges_3D(nodes_of_edges, edges_of_cells);

  DoubleTab new_nodes;
  build_new_nodes(new_nodes, nodes_of_edges);

  IntTab new_cells;
  build_new_cells_3D(new_cells, edges_of_cells);

  Noms new_sub_zones_descriptions;
  build_new_sub_zones_descriptions(new_sub_zones_descriptions);

  Static_Int_Lists incidence_from_node_to_edges;
  build_incidence_from_node_to_edges(new_nodes.dimension(0),nodes_of_edges,incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_boundary_faces;
  VECT(IntTab) new_cells_of_boundary_faces;
  build_new_boundary_faces_3D(new_nodes_of_boundary_faces, new_cells_of_boundary_faces, incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_connector_faces;
  VECT(IntTab) new_cells_of_connector_faces;
  build_new_connector_faces_3D(new_nodes_of_connector_faces, new_cells_of_connector_faces, incidence_from_node_to_edges);

  VECT(IntTab) new_nodes_of_internal_frontier_faces;
  VECT(IntTab) new_cells_of_internal_frontier_faces;
  build_new_internal_frontier_faces_3D(new_nodes_of_internal_frontier_faces, new_cells_of_internal_frontier_faces, incidence_from_node_to_edges);

  update_domain(Nom("Tetraedre"),
                triangle_3D,
                new_nodes,
                new_cells,
                new_sub_zones_descriptions,
                new_nodes_of_boundary_faces,
                new_cells_of_boundary_faces,
                new_nodes_of_connector_faces,
                new_cells_of_connector_faces,
                new_nodes_of_internal_frontier_faces,
                new_cells_of_internal_frontier_faces);

  Scatter::init_sequential_domain(domain);
}

void Refine_Mesh::build_edges_2D(IntTab& nodes_of_edges, IntTab& edges_of_cells) const
{
  const int nb_edges_per_cell = 3;
  const int nb_nodes_per_edge = 2;

  IntTab local_nodes_of_edges(nb_edges_per_cell,nb_nodes_per_edge);
  local_nodes_of_edges(0,0) = 0;
  local_nodes_of_edges(0,1) = 1;
  local_nodes_of_edges(1,0) = 0;
  local_nodes_of_edges(1,1) = 2;
  local_nodes_of_edges(2,0) = 1;
  local_nodes_of_edges(2,1) = 2;

  build_edges(nodes_of_edges, edges_of_cells, nb_edges_per_cell, nb_nodes_per_edge, local_nodes_of_edges);
}

void Refine_Mesh::build_edges_3D(IntTab& nodes_of_edges, IntTab& edges_of_cells) const
{
  const int nb_edges_per_cell = 6;
  const int nb_nodes_per_edge = 2;

  IntTab local_nodes_of_edges(nb_edges_per_cell,nb_nodes_per_edge);
  local_nodes_of_edges(0,0) = 0;
  local_nodes_of_edges(0,1) = 1;
  local_nodes_of_edges(1,0) = 0;
  local_nodes_of_edges(1,1) = 2;
  local_nodes_of_edges(2,0) = 0;
  local_nodes_of_edges(2,1) = 3;
  local_nodes_of_edges(3,0) = 1;
  local_nodes_of_edges(3,1) = 2;
  local_nodes_of_edges(4,0) = 1;
  local_nodes_of_edges(4,1) = 3;
  local_nodes_of_edges(5,0) = 2;
  local_nodes_of_edges(5,1) = 3;

  build_edges(nodes_of_edges, edges_of_cells, nb_edges_per_cell, nb_nodes_per_edge, local_nodes_of_edges);
}

void Refine_Mesh::build_edges(IntTab& nodes_of_edges, IntTab& edges_of_cells, int nb_edges_per_cell, int nb_nodes_per_edge, const IntTab& local_nodes_of_edges) const
{
  const IntTab& cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);

  int size = nb_cells * nb_edges_per_cell;
  IntTab nodes_of_edges_tmp(size,nb_nodes_per_edge);
  edges_of_cells.resize(nb_cells, nb_edges_per_cell);
  ArrOfInt global_nodes_of_edge(nb_nodes_per_edge);

  for (int c=0; c<nb_cells; ++c)
    {
      for (int e=0; e<nb_edges_per_cell; ++e)
        {
          const int idx = c * nb_edges_per_cell + e;

          for (int n=0; n<nb_nodes_per_edge; ++n)
            {
              global_nodes_of_edge[n] = cells(c,local_nodes_of_edges(e,n));
            }

          nodes_of_edges_tmp(idx,0) = (global_nodes_of_edge[0]<global_nodes_of_edge[1]) ? global_nodes_of_edge[0] : global_nodes_of_edge[1];
          nodes_of_edges_tmp(idx,1) = (global_nodes_of_edge[0]<global_nodes_of_edge[1]) ? global_nodes_of_edge[1] : global_nodes_of_edge[0];

          edges_of_cells(c,e) = idx;
        }
    }

  ArrOfInt renum;
  ArrOfInt renum_inverse;
  calculer_renum_sans_doublons(nodes_of_edges_tmp,renum,renum_inverse);

  const int nb_edges = renum_inverse.size_array();
  nodes_of_edges.resize(nb_edges,nb_nodes_per_edge);
  for (int e=0; e<nb_edges; ++e)
    {
      for (int n=0; n<nb_nodes_per_edge; ++n)
        {
          nodes_of_edges(e,n) = nodes_of_edges_tmp(renum_inverse[e],n);
        }
    }

  for (int c=0; c<nb_cells; ++c)
    {
      for (int e=0; e<nb_edges_per_cell; ++e)
        {
          edges_of_cells(c,e) = renum[edges_of_cells(c,e)];
        }
    }
}

void Refine_Mesh::build_new_nodes(DoubleTab& new_nodes, const IntTab& nodes_of_edges) const
{
  const DoubleTab& nodes = domaine().les_sommets();

  const int nb_nodes = nodes.dimension(0);
  const int nb_edges = nodes_of_edges.dimension(0);

  new_nodes.resize(nb_nodes+nb_edges,Objet_U::dimension);
  for (int n=0; n<nb_nodes; ++n)
    {
      for (int i=0; i<Objet_U::dimension; ++i)
        {
          new_nodes(n,i) = nodes(n,i);
        }
    }

  for (int e=0; e<nb_edges; ++e)
    {
      for (int i=0; i<Objet_U::dimension; ++i)
        {
          new_nodes(e+nb_nodes,i) = 0.5 * (nodes(nodes_of_edges(e,0),i) + nodes(nodes_of_edges(e,1),i));
        }
    }
}

void Refine_Mesh::build_new_cells_2D(IntTab& new_cells, const IntTab& edges_of_cells) const
{
  const DoubleTab& nodes = domaine().les_sommets();
  const IntTab&    cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);
  const int nb_nodes = nodes.dimension(0);

  new_cells.resize(4*nb_cells,3);

  int n0, n1, n2, n01, n02, n12;
  for (int c=0; c<nb_cells; ++c)
    {
      n0  = cells(c,0);
      n1  = cells(c,1);
      n2  = cells(c,2);
      n01 = nb_nodes + edges_of_cells(c,0);
      n02 = nb_nodes + edges_of_cells(c,1);
      n12 = nb_nodes + edges_of_cells(c,2);

      new_cells(4*c+0,0) = n0;
      new_cells(4*c+0,1) = n01;
      new_cells(4*c+0,2) = n02;
      new_cells(4*c+1,0) = n01;
      new_cells(4*c+1,1) = n1;
      new_cells(4*c+1,2) = n12;
      new_cells(4*c+2,0) = n02;
      new_cells(4*c+2,1) = n12;
      new_cells(4*c+2,2) = n2;
      new_cells(4*c+3,0) = n12;
      new_cells(4*c+3,1) = n02;
      new_cells(4*c+3,2) = n01;
    }
}

void Refine_Mesh::build_new_cells_3D(IntTab& new_cells, const IntTab& edges_of_cells) const
{
  const DoubleTab& nodes = domaine().les_sommets();
  const IntTab&    cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);
  const int nb_nodes = nodes.dimension(0);

  new_cells.resize(8*nb_cells,4);

  int n0, n1, n2, n3, n01, n02, n03, n12, n13, n23;
  for (int c=0; c<nb_cells; ++c)
    {
      n0  = cells(c,0);
      n1  = cells(c,1);
      n2  = cells(c,2);
      n3  = cells(c,3);
      n01 = nb_nodes + edges_of_cells(c,0);
      n02 = nb_nodes + edges_of_cells(c,1);
      n03 = nb_nodes + edges_of_cells(c,2);
      n12 = nb_nodes + edges_of_cells(c,3);
      n13 = nb_nodes + edges_of_cells(c,4);
      n23 = nb_nodes + edges_of_cells(c,5);


      new_cells(8*c+0,0) = n0;
      new_cells(8*c+0,1) = n01;
      new_cells(8*c+0,2) = n02;
      new_cells(8*c+0,3) = n03;
      new_cells(8*c+1,0) = n01;
      new_cells(8*c+1,1) = n1;
      new_cells(8*c+1,2) = n12;
      new_cells(8*c+1,3) = n13;
      new_cells(8*c+2,0) = n02;
      new_cells(8*c+2,1) = n12;
      new_cells(8*c+2,2) = n2;
      new_cells(8*c+2,3) = n23;
      new_cells(8*c+3,0) = n03;
      new_cells(8*c+3,1) = n13;
      new_cells(8*c+3,2) = n23;
      new_cells(8*c+3,3) = n3;
      new_cells(8*c+4,0) = n02;
      new_cells(8*c+4,1) = n23;
      new_cells(8*c+4,2) = n13;
      new_cells(8*c+4,3) = n12;
      new_cells(8*c+5,0) = n23;
      new_cells(8*c+5,1) = n13;
      new_cells(8*c+5,2) = n03;
      new_cells(8*c+5,3) = n02;
      new_cells(8*c+6,0) = n13;
      new_cells(8*c+6,1) = n03;
      new_cells(8*c+6,2) = n02;
      new_cells(8*c+6,3) = n01;
      new_cells(8*c+7,0) = n12;
      new_cells(8*c+7,1) = n02;
      new_cells(8*c+7,2) = n01;
      new_cells(8*c+7,3) = n13;
    }
}

void Refine_Mesh::build_new_sub_zones_descriptions(Noms& sub_zones_descriptions) const
{
  const int nb_sub_zones = domaine().nb_ss_zones();
  sub_zones_descriptions.dimensionner(nb_sub_zones);

  const int nb_new_cells_per_old_cell = (Objet_U::dimension == 2) ? 4 : 8;

  for (int i=0; i<nb_sub_zones; ++i)
    {
      const Sous_Zone& sub_zone = domaine().ss_zone(i);
      const int old_nb_cells_in_sub_zone = sub_zone.nb_elem_tot();
      const int new_nb_cells_in_sub_zone = old_nb_cells_in_sub_zone * nb_new_cells_per_old_cell;
      IntVect new_sub_zone_cells(new_nb_cells_in_sub_zone);
      int idx = 0;
      for (int c=0; c<old_nb_cells_in_sub_zone; ++c)
        {
          const int old_cell = sub_zone[c];
          for (int j=0; j<nb_new_cells_per_old_cell; ++j)
            {
              new_sub_zone_cells[idx] = old_cell*nb_new_cells_per_old_cell+j;
              ++idx;
            }
        }

      SChaine os;
      os << " { Liste " << new_sub_zone_cells << " } ";
      sub_zones_descriptions[i] = Nom(os.get_str());
    }
}

void Refine_Mesh::build_incidence_from_node_to_edges(int            nb_nodes,
                                                     const IntTab&     nodes_of_edges,
                                                     Static_Int_Lists& incidence) const
{
  const int nb_edges          = nodes_of_edges.dimension(0);
  const int nb_nodes_per_edge = nodes_of_edges.dimension(1);

  ArrOfInt nb_incident_edges(nb_nodes);

  for (int edge=0; edge<nb_edges; ++edge)
    {
      for (int n=0; n<nb_nodes_per_edge; ++n)
        {
          const int node = nodes_of_edges(edge,n);
          ++nb_incident_edges[node];
        }
    }

  incidence.set_list_sizes(nb_incident_edges);

  nb_incident_edges = 0;
  for (int edge=0; edge<nb_edges; ++edge)
    {
      for (int n=0; n<nb_nodes_per_edge; ++n)
        {
          const int node  = nodes_of_edges(edge,n);
          const int index = nb_incident_edges[node];
          incidence.set_value(node, index, edge);
          ++nb_incident_edges[node];
        }
    }

  incidence.trier_liste(-1);
}

void Refine_Mesh::build_new_boundary_faces_2D(VECT(IntTab)&           new_nodes_of_boundary_faces,
                                              VECT(IntTab)&           new_cells_of_boundary_faces,
                                              const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_boundaries = domaine().zone(0).nb_bords();
  new_nodes_of_boundary_faces.dimensionner(nb_boundaries);
  new_cells_of_boundary_faces.dimensionner(nb_boundaries);

  int boundary = 0;
  CONST_LIST_CURSEUR(Bord) cursor(domaine().zone(0).faces_bord());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_boundary_faces[boundary];
      IntTab& cells = new_cells_of_boundary_faces[boundary];
      build_new_frontier_faces_2D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().faces());
      ++cursor;
      ++boundary;
    }
}

void Refine_Mesh::build_new_boundary_faces_3D(VECT(IntTab)&           new_nodes_of_boundary_faces,
                                              VECT(IntTab)&           new_cells_of_boundary_faces,
                                              const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_boundaries = domaine().zone(0).nb_bords();
  new_nodes_of_boundary_faces.dimensionner(nb_boundaries);
  new_cells_of_boundary_faces.dimensionner(nb_boundaries);

  int boundary = 0;
  CONST_LIST_CURSEUR(Bord) cursor(domaine().zone(0).faces_bord());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_boundary_faces[boundary];
      IntTab& cells = new_cells_of_boundary_faces[boundary];
      build_new_frontier_faces_3D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().faces());
      ++cursor;
      ++boundary;
    }
}

void Refine_Mesh::build_new_connector_faces_2D(VECT(IntTab)&           new_nodes_of_connector_faces,
                                               VECT(IntTab)&           new_cells_of_connector_faces,
                                               const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_connectors = domaine().zone(0).nb_raccords();
  new_nodes_of_connector_faces.dimensionner(nb_connectors);
  new_cells_of_connector_faces.dimensionner(nb_connectors);

  int connector = 0;
  CONST_LIST_CURSEUR(Raccord) cursor(domaine().zone(0).faces_raccord());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_connector_faces[connector];
      IntTab& cells = new_cells_of_connector_faces[connector];
      build_new_frontier_faces_2D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().valeur().faces());
      ++cursor;
      ++connector;
    }
}

void Refine_Mesh::build_new_connector_faces_3D(VECT(IntTab)&           new_nodes_of_connector_faces,
                                               VECT(IntTab)&           new_cells_of_connector_faces,
                                               const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_connectors = domaine().zone(0).nb_raccords();
  new_nodes_of_connector_faces.dimensionner(nb_connectors);
  new_cells_of_connector_faces.dimensionner(nb_connectors);

  int connector = 0;
  CONST_LIST_CURSEUR(Raccord) cursor(domaine().zone(0).faces_raccord());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_connector_faces[connector];
      IntTab& cells = new_cells_of_connector_faces[connector];
      build_new_frontier_faces_3D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().valeur().faces());
      ++cursor;
      ++connector;
    }
}


void Refine_Mesh::build_new_internal_frontier_faces_2D(VECT(IntTab)&           new_nodes_of_internal_frontier_faces,
                                                       VECT(IntTab)&           new_cells_of_internal_frontier_faces,
                                                       const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_internal_frontier = domaine().zone(0).nb_frontieres_internes();
  new_nodes_of_internal_frontier_faces.dimensionner(nb_internal_frontier);
  new_cells_of_internal_frontier_faces.dimensionner(nb_internal_frontier);

  int internal_frontier = 0;
  CONST_LIST_CURSEUR(Faces_Interne) cursor(domaine().zone(0).faces_int());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_internal_frontier_faces[internal_frontier];
      IntTab& cells = new_cells_of_internal_frontier_faces[internal_frontier];
      build_new_frontier_faces_2D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().faces());
      ++cursor;
      ++internal_frontier;
    }
}


void Refine_Mesh::build_new_internal_frontier_faces_3D(VECT(IntTab)&           new_nodes_of_internal_frontier_faces,
                                                       VECT(IntTab)&           new_cells_of_internal_frontier_faces,
                                                       const Static_Int_Lists& incidence_from_node_to_edges) const
{
  const int nb_internal_frontier = domaine().zone(0).nb_frontieres_internes();
  new_nodes_of_internal_frontier_faces.dimensionner(nb_internal_frontier);
  new_cells_of_internal_frontier_faces.dimensionner(nb_internal_frontier);

  int internal_frontier = 0;
  CONST_LIST_CURSEUR(Faces_Interne) cursor(domaine().zone(0).faces_int());
  while (cursor)
    {
      IntTab& nodes = new_nodes_of_internal_frontier_faces[internal_frontier];
      IntTab& cells = new_cells_of_internal_frontier_faces[internal_frontier];
      build_new_frontier_faces_3D(nodes,cells,incidence_from_node_to_edges,cursor.valeur().faces());
      ++cursor;
      ++internal_frontier;
    }
}

void Refine_Mesh::build_new_frontier_faces_2D(IntTab&                 new_nodes_of_frontier_faces,
                                              IntTab&                 new_cells_of_frontier_faces,
                                              const Static_Int_Lists& incidence_from_node_to_edges,
                                              const Faces&            old_frontier_faces) const
{
  const int nb_nodes = domaine().les_sommets().dimension(0);

  const int old_nb_faces = old_frontier_faces.nb_faces();
  const int new_nb_faces = old_nb_faces * 2;

  const IntTab& old_nodes_of_frontier_faces = old_frontier_faces.les_sommets();

  new_nodes_of_frontier_faces.resize(new_nb_faces,2);
  for (int f=0; f<old_nb_faces; ++f)
    {
      const int node0 = old_nodes_of_frontier_faces(f,0);
      const int node1 = old_nodes_of_frontier_faces(f,1);
      const int edge  = find_edge(incidence_from_node_to_edges,node0,node1);

      new_nodes_of_frontier_faces(2*f+0,0) = node0;
      new_nodes_of_frontier_faces(2*f+0,1) = nb_nodes + edge;

      new_nodes_of_frontier_faces(2*f+1,0) = nb_nodes + edge;
      new_nodes_of_frontier_faces(2*f+1,1) = node1;
    }

  new_cells_of_frontier_faces.resize(new_nb_faces,2);
  new_cells_of_frontier_faces = -1;
}

void Refine_Mesh::build_new_frontier_faces_3D(IntTab&                 new_nodes_of_frontier_faces,
                                              IntTab&                 new_cells_of_frontier_faces,
                                              const Static_Int_Lists& incidence_from_node_to_edges,
                                              const Faces&            old_frontier_faces) const
{
  const int nb_nodes = domaine().les_sommets().dimension(0);

  const int old_nb_faces = old_frontier_faces.nb_faces();
  const int new_nb_faces = old_nb_faces * 4;

  const IntTab old_nodes_of_frontier_faces = old_frontier_faces.les_sommets();

  new_nodes_of_frontier_faces.resize(new_nb_faces,3);
  for (int f=0; f<old_nb_faces; ++f)
    {
      const int node0 = old_nodes_of_frontier_faces(f,0);
      const int node1 = old_nodes_of_frontier_faces(f,1);
      const int node2 = old_nodes_of_frontier_faces(f,2);

      const int edge0  = find_edge(incidence_from_node_to_edges,node1,node2);
      const int edge1  = find_edge(incidence_from_node_to_edges,node2,node0);
      const int edge2  = find_edge(incidence_from_node_to_edges,node0,node1);

      new_nodes_of_frontier_faces(4*f+0,0) = node0;
      new_nodes_of_frontier_faces(4*f+0,1) = nb_nodes + edge2;
      new_nodes_of_frontier_faces(4*f+0,2) = nb_nodes + edge1;

      new_nodes_of_frontier_faces(4*f+1,0) = nb_nodes + edge2;
      new_nodes_of_frontier_faces(4*f+1,1) = node1;
      new_nodes_of_frontier_faces(4*f+1,2) = nb_nodes + edge0;

      new_nodes_of_frontier_faces(4*f+2,0) = nb_nodes + edge1;
      new_nodes_of_frontier_faces(4*f+2,1) = nb_nodes + edge0;
      new_nodes_of_frontier_faces(4*f+2,2) = node2;

      new_nodes_of_frontier_faces(4*f+3,0) = nb_nodes +edge0;
      new_nodes_of_frontier_faces(4*f+3,1) = nb_nodes +edge1;
      new_nodes_of_frontier_faces(4*f+3,2) = nb_nodes +edge2;
    }

  new_cells_of_frontier_faces.resize(new_nb_faces,2);
  new_cells_of_frontier_faces = -1;
}

int Refine_Mesh::find_edge(const Static_Int_Lists& incidence_from_node_to_edges, int node0, int node1) const
{
  ArrOfInt edges0;
  incidence_from_node_to_edges.copy_list_to_array(node0, edges0);

  ArrOfInt edges1;
  incidence_from_node_to_edges.copy_list_to_array(node1, edges1);

  array_calculer_intersection(edges0,edges1);

  assert( edges0.size_array() == 1 );
  return edges0[0];
}

void Refine_Mesh::update_domain(const Nom&          cell_type,
                                const Type_Face&    face_type,
                                const DoubleTab&    new_nodes,
                                const IntTab&       new_cells,
                                const Noms&         new_sub_zones_descriptions,
                                const VECT(IntTab)& new_nodes_of_boundary_faces,
                                const VECT(IntTab)& new_cells_of_boundary_faces,
                                const VECT(IntTab)& new_nodes_of_connector_faces,
                                const VECT(IntTab)& new_cells_of_connector_faces,
                                const VECT(IntTab)& new_nodes_of_internal_frontier_faces,
                                const VECT(IntTab)& new_cells_of_internal_frontier_faces)
{
  update_nodes(new_nodes);
  update_cells(new_cells);
  update_octree(cell_type);
  update_sub_zones(new_sub_zones_descriptions);
  update_boundary_faces(face_type,new_nodes_of_boundary_faces,new_cells_of_boundary_faces);
  update_connector_faces(face_type,new_nodes_of_connector_faces,new_cells_of_connector_faces);
  update_internal_frontier_faces(face_type,new_nodes_of_internal_frontier_faces,new_cells_of_internal_frontier_faces);
}

void Refine_Mesh::update_nodes(const DoubleTab& new_nodes)
{
  domaine().les_sommets().ref(new_nodes);
}

void Refine_Mesh::update_cells(const IntTab& new_cells)
{
  domaine().zone(0).les_elems().ref(new_cells);
}

void Refine_Mesh::update_octree(const Nom& cell_type)
{
  domaine().zone(0).invalide_octree();
  domaine().zone(0).typer(cell_type);
  domaine().zone(0).construit_octree();
}

void Refine_Mesh::update_sub_zones(const Noms& new_sub_zones_descriptions)
{
  const int nb_sub_zones = domaine().nb_ss_zones();
  for (int i=0; i<nb_sub_zones; ++i)
    {
      Sous_Zone& sub_zone    = domaine().ss_zone(i);
      const Nom& description = new_sub_zones_descriptions[i];
      EChaine is(description.getChar());
      is >> sub_zone;
    }
}

void Refine_Mesh::update_boundary_faces(const Type_Face&    face_type,
                                        const VECT(IntTab)& new_nodes_of_boundary_faces,
                                        const VECT(IntTab)& new_cells_of_boundary_faces)
{
  int boundary = 0;
  LIST_CURSEUR(Bord) cursor(domaine().zone(0).faces_bord());
  while (cursor)
    {
      Faces& faces = cursor.valeur().faces();
      faces.typer(face_type);
      faces.les_sommets().ref(new_nodes_of_boundary_faces[boundary]);
      faces.voisins().ref(new_cells_of_boundary_faces[boundary]);
      ++cursor;
      ++boundary;
    }
}

void Refine_Mesh::update_connector_faces(const Type_Face&    face_type,
                                         const VECT(IntTab)& new_nodes_of_connector_faces,
                                         const VECT(IntTab)& new_cells_of_connector_faces)
{
  int connector = 0;
  LIST_CURSEUR(Raccord) cursor(domaine().zone(0).faces_raccord());
  while (cursor)
    {
      Faces& faces = cursor.valeur().valeur().faces();
      faces.typer(face_type);
      faces.les_sommets().ref(new_nodes_of_connector_faces[connector]);
      faces.voisins().ref(new_cells_of_connector_faces[connector]);
      ++cursor;
      ++connector;
    }
}

void Refine_Mesh::update_internal_frontier_faces(const Type_Face&    face_type,
                                                 const VECT(IntTab)& new_nodes_of_internal_frontier_faces,
                                                 const VECT(IntTab)& new_cells_of_internal_frontier_faces)
{
  int internal_frontier = 0;
  LIST_CURSEUR(Faces_Interne) cursor(domaine().zone(0).faces_int());
  while (cursor)
    {
      Faces& faces = cursor.valeur().faces();
      faces.typer(face_type);
      faces.les_sommets().ref(new_nodes_of_internal_frontier_faces[internal_frontier]);
      faces.voisins().ref(new_cells_of_internal_frontier_faces[internal_frontier]);
      ++cursor;
      ++internal_frontier;
    }
}
