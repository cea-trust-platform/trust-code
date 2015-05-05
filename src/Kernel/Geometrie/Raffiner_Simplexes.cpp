/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Raffiner_Simplexes.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Raffiner_Simplexes.h>
#include <Domaine.h>
#include <Nom.h>
#include <Scatter.h>
#include <Motcle.h>
#include <IntTab.h>
#include <Array_tools.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>
#include <Synonyme_info.h>

Implemente_instanciable(Raffiner_Simplexes,"Raffiner_Simplexes",Interprete_geometrique_base) ;
Add_synonym(Raffiner_Simplexes,"Raffiner_isotrope"); // Raffiner_Simplexes class replaces the obsolete Raffiner_isotrope class

Sortie& Raffiner_Simplexes::printOn(Sortie& os) const
{
  Interprete::printOn(os);
  return os;
}

Entree& Raffiner_Simplexes::readOn(Entree& is)
{
  Interprete::readOn(is);
  return is;
}

// Macros pour l'encodage de l'idx (meme fonction que build_idx mais pratique
//  pour ecrire le code de build_refine_...)
// Sommet fin situe sur le sommet "num" de l'element grossier:
#define NODE_ID(num) num
// Sommet fin situe au milieu de l'arete "num" de l'element grossier:
#define EDGE_ID(num) (-num-1)

static void fill_tab_2(const int *t, int n, int m, IntTab& tab)
{
  tab.resize(n, m);

  for (int i=0; i<n; ++i)
    {
      for (int j=0; j<m; ++j)
        {
          tab(i,j) = t[i*m+j];
        }
    }

  if (t[n*m] != -1)
    {
      Cerr << "Error in Raffiner_Simplexes 'filltab_2'" << finl;
      Process::exit();
    }
}
static void fill_tab_2_new(const int *t, int n, int m, IntTab& tab)
{
  tab.resize(n, m);
  tab=-1;
  int compt=0;
  for (int i=0; i<n; ++i)
    {
      int nb=t[compt++];

      for (int j=0; j<nb; ++j)
        {
          tab(i,j) = t[compt++];
        }
    }

  if (t[compt] != -1)
    {
      Cerr << "Error in Raffiner_Simplexes 'filltab_2_new'" << finl;
      Process::exit();
    }
}


static void fill_tab_3(const int *t, int n, int m, int p, IntTab& tab)
{
  tab.resize(n, m, p);

  for (int i=0; i<n; ++i)
    {
      for (int j=0; j<m; ++j)
        {
          for (int k=0; k<p; ++k)
            {
              tab(i,j,k) = t[(i*m+j)*p+k];
            }
        }
    }

  if (t[n*m*p] != -1)
    {
      Cerr << "Error in Raffiner_Simplexes 'filltab_3'" << finl;
      Process::exit();
    }
}


static void build_quadrangle_cell_refinement_pattern(IntTab& pattern)
{
  static const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(3), EDGE_ID(4), // triangle defini par sommet0 arete0 arete1
                            EDGE_ID(0), NODE_ID(1), EDGE_ID(4),EDGE_ID(1), // etc...
                            EDGE_ID(4), EDGE_ID(1), EDGE_ID(2), NODE_ID(3),
                            EDGE_ID(3), EDGE_ID(4), NODE_ID(2), EDGE_ID(2), -1
                         } ;
  fill_tab_2(t, 4, 4, pattern);
}

static void build_triangle_cell_refinement_pattern(IntTab& pattern)
{
  static const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(1), // triangle defini par sommet0 arete0 arete1
                            EDGE_ID(0), NODE_ID(1), EDGE_ID(2), // etc...
                            EDGE_ID(1), EDGE_ID(2), NODE_ID(2),
                            EDGE_ID(2), EDGE_ID(1), EDGE_ID(0), -1
                         } ;
  fill_tab_2(t, 4, 3, pattern);
}

static void build_tetrahedron_cell_refinement_pattern(IntTab& pattern)
{
  static const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(1), EDGE_ID(2) , // tetraedre defini par sommet0 arete0 arete1 arete2
                            EDGE_ID(0), NODE_ID(1), EDGE_ID(3), EDGE_ID(4) , // etc...
                            EDGE_ID(1), EDGE_ID(3), NODE_ID(2), EDGE_ID(5) ,
                            EDGE_ID(2), EDGE_ID(4), EDGE_ID(5), NODE_ID(3) ,
                            EDGE_ID(1), EDGE_ID(5), EDGE_ID(4), EDGE_ID(3) ,
                            EDGE_ID(5), EDGE_ID(4), EDGE_ID(2), EDGE_ID(1) ,
                            EDGE_ID(4), EDGE_ID(2), EDGE_ID(1), EDGE_ID(0) ,
                            EDGE_ID(3), EDGE_ID(1), EDGE_ID(0), EDGE_ID(4) , -1
                         };
  fill_tab_2(t, 8, 4, pattern);
}
// Convention de numerotation
//    sommets
//      6------7
//     /|     /|
//    4------5 |
//    | |    | |
//    | 2----|-3
//    |/     |/
//    0------1
static void build_hexa_cell_refinement_pattern(IntTab& pattern)
{
  static const int t[] =
  {
    NODE_ID(0),EDGE_ID(0),EDGE_ID(3),EDGE_ID(4),EDGE_ID(5),EDGE_ID(6),EDGE_ID(8),EDGE_ID(9), // tetraedre defini par sommet0 arete0 arete1 arete2

    EDGE_ID(0),NODE_ID(1),EDGE_ID(4),EDGE_ID(1),EDGE_ID(6),EDGE_ID(7),EDGE_ID(9),EDGE_ID(10),
    EDGE_ID(3),EDGE_ID(4),NODE_ID(2),EDGE_ID(2),EDGE_ID(8),EDGE_ID(9),EDGE_ID(11),EDGE_ID(12),
    EDGE_ID(4),EDGE_ID(1),EDGE_ID(2),NODE_ID(3),EDGE_ID(9),EDGE_ID(10),EDGE_ID(12),EDGE_ID(13),

    EDGE_ID(5),EDGE_ID(6),EDGE_ID(8),EDGE_ID(9),NODE_ID(4),EDGE_ID(14),EDGE_ID(17),EDGE_ID(18),
    EDGE_ID(6),EDGE_ID(7),EDGE_ID(9),EDGE_ID(10),EDGE_ID(14),NODE_ID(5),EDGE_ID(18),EDGE_ID(15),
    EDGE_ID(8),EDGE_ID(9),EDGE_ID(11),EDGE_ID(12),EDGE_ID(17),EDGE_ID(18),NODE_ID(6),EDGE_ID(16),
    EDGE_ID(9),EDGE_ID(10),EDGE_ID(12),EDGE_ID(13),EDGE_ID(18),EDGE_ID(15),EDGE_ID(16),NODE_ID(7),

    -1
  };
  fill_tab_2(t, 8, 8, pattern);
}

int type_elem(const Nom& cell_type)
{
  Motcles understood_keywords(6);
  understood_keywords[0] = "Triangle";
  understood_keywords[1] = "Tetraedre";
  understood_keywords[2] = "Rectangle";
  understood_keywords[3] = "Hexaedre";
  understood_keywords[4] = "Quadrangle";
  understood_keywords[5] = "Hexaedre_VEF";
  int rank = understood_keywords.search(cell_type);
  if (rank>3) rank-=2;
  return rank;
}

static void build_cell_refinement_pattern(IntTab& pattern, const Nom& cell_type)
{


  int rank = type_elem(cell_type);
  switch(rank)
    {
    case 0  :
      build_triangle_cell_refinement_pattern(pattern);
      break;
    case 1  :
      build_tetrahedron_cell_refinement_pattern(pattern);
      break;
    case 2  :
      build_quadrangle_cell_refinement_pattern(pattern);
      break;
    case 3  :
      build_hexa_cell_refinement_pattern(pattern);
      break;
    default :
      Cerr << "Error in Raffiner_Simplexes.cpp 'build_cell_refinement_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}


static void build_quadrangle_face_refinement_pattern(IntTab& pattern)
{
  static const int t[] = {   NODE_ID(0), EDGE_ID(0),
                             EDGE_ID(0), NODE_ID(1),

                             NODE_ID(3), EDGE_ID(1),
                             EDGE_ID(1), NODE_ID(1),

                             NODE_ID(3), EDGE_ID(2),
                             EDGE_ID(2), NODE_ID(2),

                             NODE_ID(0), EDGE_ID(3),
                             EDGE_ID(3), NODE_ID(2),-1
                         };
  fill_tab_3(t, 4, 2, 2, pattern);
}
static void build_triangle_face_refinement_pattern(IntTab& pattern)
{
  static const int t[] = {  NODE_ID(2), EDGE_ID(2),
                            EDGE_ID(2), NODE_ID(1),

                            NODE_ID(0), EDGE_ID(1),
                            EDGE_ID(1), NODE_ID(2),

                            NODE_ID(1), EDGE_ID(0),
                            EDGE_ID(0), NODE_ID(0), -1
                         };
  fill_tab_3(t, 3, 2, 2, pattern);
}

static void build_tetrahedron_face_refinement_pattern(IntTab& pattern)
{
  static const int t[] = { NODE_ID(1), EDGE_ID(3), EDGE_ID(4),
                           EDGE_ID(3), NODE_ID(2), EDGE_ID(5),
                           EDGE_ID(4), EDGE_ID(5), NODE_ID(3),
                           EDGE_ID(5), EDGE_ID(4), EDGE_ID(3),

                           NODE_ID(2), EDGE_ID(1), EDGE_ID(5),
                           EDGE_ID(1), NODE_ID(0), EDGE_ID(2),
                           EDGE_ID(5), EDGE_ID(2), NODE_ID(3),
                           EDGE_ID(2), EDGE_ID(5), EDGE_ID(1),

                           NODE_ID(3), EDGE_ID(2), EDGE_ID(4),
                           EDGE_ID(2), NODE_ID(0), EDGE_ID(0),
                           EDGE_ID(4), EDGE_ID(0), NODE_ID(1),
                           EDGE_ID(0), EDGE_ID(4), EDGE_ID(2),

                           NODE_ID(0), EDGE_ID(1), EDGE_ID(0),
                           EDGE_ID(1), NODE_ID(2), EDGE_ID(3),
                           EDGE_ID(0), EDGE_ID(3), NODE_ID(1),
                           EDGE_ID(3), EDGE_ID(0), EDGE_ID(1), -1
                         };

  fill_tab_3(t, 4, 4, 3, pattern);
}
static void build_hexa_face_refinement_pattern(IntTab& pattern)
{


  static const int t[] = { NODE_ID(0), EDGE_ID(3), EDGE_ID(5), EDGE_ID(8),           // 0 2 4 6
                           EDGE_ID(3), NODE_ID(2), EDGE_ID(8), EDGE_ID(11),
                           EDGE_ID(5), EDGE_ID(8), NODE_ID(4), EDGE_ID(17),
                           EDGE_ID(8), EDGE_ID(11), EDGE_ID(17), NODE_ID(6),

                           NODE_ID(0), EDGE_ID(0), EDGE_ID(5), EDGE_ID(6),           // 0, 1, 4, 5 ,
                           EDGE_ID(0), NODE_ID(1), EDGE_ID(6), EDGE_ID(7),
                           EDGE_ID(5), EDGE_ID(6), NODE_ID(4), EDGE_ID(14),
                           EDGE_ID(6), EDGE_ID(7), EDGE_ID(14), NODE_ID(5),


                           NODE_ID(0), EDGE_ID(0), EDGE_ID(3), EDGE_ID(4),           // 	0, 1, 2, 3 ,
                           EDGE_ID(0), NODE_ID(1), EDGE_ID(4), EDGE_ID(1),
                           EDGE_ID(3), EDGE_ID(4), NODE_ID(2), EDGE_ID(2),
                           EDGE_ID(4), EDGE_ID(1), EDGE_ID(2), NODE_ID(3),

                           NODE_ID(1), EDGE_ID(1), EDGE_ID(7), EDGE_ID(10),           // 	1, 3, 5, 7 ,
                           EDGE_ID(1), NODE_ID(3), EDGE_ID(10), EDGE_ID(13),
                           EDGE_ID(7), EDGE_ID(10), NODE_ID(5), EDGE_ID(15),
                           EDGE_ID(10), EDGE_ID(13), EDGE_ID(15), NODE_ID(7),

                           NODE_ID(2), EDGE_ID(2), EDGE_ID(11), EDGE_ID(12),           // 2, 3, 6, 7 ,
                           EDGE_ID(2), NODE_ID(3), EDGE_ID(12), EDGE_ID(13),
                           EDGE_ID(11), EDGE_ID(12), NODE_ID(6), EDGE_ID(16),
                           EDGE_ID(12), EDGE_ID(13), EDGE_ID(16), NODE_ID(7),

                           NODE_ID(4), EDGE_ID(14), EDGE_ID(17), EDGE_ID(18),           // 4, 5, 6, 7
                           EDGE_ID(14), NODE_ID(5), EDGE_ID(18), EDGE_ID(15),
                           EDGE_ID(17), EDGE_ID(18), NODE_ID(6), EDGE_ID(16),
                           EDGE_ID(18), EDGE_ID(15), EDGE_ID(16), NODE_ID(7),


                           -1
                         };

  fill_tab_3(t, 6, 4, 4, pattern);
}


static void build_face_refinement_pattern(IntTab& pattern, const Nom& cell_type)
{
  int rank = type_elem(cell_type);
  switch(rank)
    {
    case 0  :
      build_triangle_face_refinement_pattern(pattern);
      break;
    case 1  :
      build_tetrahedron_face_refinement_pattern(pattern);
      break;
    case 2  :
      build_quadrangle_face_refinement_pattern(pattern);
      break;
    case 3  :
      build_hexa_face_refinement_pattern(pattern);
      break;
    default :
      Cerr << "Error in Raffiner_Simplexes.cpp 'build_face_refinement_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}


static void build_quadrangle_edges_pattern(IntTab& pattern)
{
  static const int t[] = {  2,0, 1 ,
                            2,1, 3,
                            2,3, 2,
                            2,2, 0,
                            4,0, 3,1,2,  -1
                         };
  fill_tab_2_new(t, 5, 4, pattern);
}

static void build_triangle_edges_pattern(IntTab& pattern)
{
  static const int t[] = {  0, 1 ,
                            0, 2 ,
                            1, 2 , -1
                         };
  fill_tab_2(t, 3, 2, pattern);
}

static void build_tetrahedron_edges_pattern(IntTab& pattern)
{
  static const int t[] = {  0, 1 , // L'arete 0 est entre le sommet 0 et le sommet 1 de l'element
                            0, 2 , // L'arete 1 est entre 0 et 2
                            0, 3 , // etc...
                            1, 2 ,
                            1, 3 ,
                            2, 3 , -1
                         };
  fill_tab_2(t, 6, 2, pattern);
}

// plan haut
//      6-- 16'---7
//     /         /
//    17'  18'  15'
//   /         /
//  4---14'---5
//
// plan milieu
//      11'--12'---13'
//     /           /
//    8'    9'   10'
//   /          /
//  5'---6'---7'
//
// plan bas
//      2--2'---3
//     /       /
//    3'  4'  1'
//   /       /
//  0---0'---1
static void build_hexa_edges_pattern(IntTab& pattern)
{

  static const int t[] = {  2, 0, 1 , // L'arete 0 est entre le sommet 0 et le sommet 1 de l'element
                            2, 1, 3 , // etc...
                            2, 3, 2 ,
                            2, 2, 0 ,
                            4, 0, 3 , 1,2,
                            2, 0,4, 4,0,5,1,4 ,2, 1,5 , // 5' 6' 7'
                            4,0,6,2,4, 8,0,7,1,6,3,4,2,5, 4,1,7,3,5,
                            2,2,6, 4,2,7,3,6, 2,3,7, //11' 12' 13'
                            2,4,5, 2,5,7, 2,7,6, 2,6,4 , 4,4,7,5,6,
                            -1
                         };
  fill_tab_2_new(t, 19, 8, pattern);
}
static void build_edges_pattern(IntTab& pattern, const Nom& cell_type)
{

  int rank = type_elem(cell_type);
  switch(rank)
    {
    case 0  :
      build_triangle_edges_pattern(pattern);
      break;
    case 1  :
      build_tetrahedron_edges_pattern(pattern);
      break;
    case 2  :
      build_quadrangle_edges_pattern(pattern);
      break;
    case 3  :
      build_hexa_edges_pattern(pattern);
      break;
    default :
      Cerr << "Error in Raffiner_Simplexes.cpp 'build_edges_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

static void build_quadrangle_faces_pattern(IntTab& pattern)
{
  static const int t[] = {  0, 1 ,
                            1, 3,
                            3, 2 ,
                            2, 0 , -1
                         };
  fill_tab_2(t, 4, 2, pattern);
}
static void build_triangle_faces_pattern(IntTab& pattern)
{
  static const int t[] = {  2, 1 ,
                            0, 2,
                            1, 0 , -1
                         };
  fill_tab_2(t, 3, 2, pattern);
}

static void build_tetrahedron_faces_pattern(IntTab& pattern)
{
  static const int t[] = {  1, 2, 3,
                            2, 0, 3,
                            3, 0, 1,
                            0, 2, 1, -1
                         };
  fill_tab_2(t, 4, 3, pattern);
}
static void build_hexa_faces_pattern(IntTab& pattern)
{
  static const int t[] = {   0, 2, 4, 6 ,
                             0, 1, 4, 5 ,
                             0, 1, 2, 3 ,
                             1, 3, 5, 7 ,
                             2, 3, 6, 7 ,
                             4, 5, 6, 7 , -1
                         };
  fill_tab_2(t, 6, 4, pattern);
}



static void build_faces_pattern(IntTab& pattern, const Nom& cell_type)
{

  int rank = type_elem(cell_type);
  switch(rank)
    {
    case 0  :
      build_triangle_faces_pattern(pattern);
      break;
    case 1  :
      build_tetrahedron_faces_pattern(pattern);
      break;
    case 2  :
      build_quadrangle_faces_pattern(pattern);
      break;
    case 3  :
      build_hexa_faces_pattern(pattern);
      break;
    default :
      Cerr << "Error in Raffiner_Simplexes.cpp 'build_faces_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

static void build_edges(const IntTab& nodes_of_cells,
                        const IntTab& edges_pattern,
                        IntTab&        edges_of_cells,
                        IntTab&        nodes_of_edges)
{

  const int nb_cells          = nodes_of_cells.dimension(0);
  const int nb_edges_per_cell = edges_pattern.dimension(0);
  //  const int nb_nodes_per_edge = 2;
  const int nb_nodes_per_edge_max=edges_pattern.dimension(1);
  IntTab duplicated_edges(nb_cells * nb_edges_per_cell, nb_nodes_per_edge_max);
  int nb_duplicated_edges = 0;
  for (int cell=0; cell<nb_cells; ++cell)
    {
      for (int edge_in_cell=0; edge_in_cell<nb_edges_per_cell; ++edge_in_cell)
        {
          const int node_in_cell_0 = edges_pattern(edge_in_cell,0);
          const int node_in_cell_1 = edges_pattern(edge_in_cell,1);

          int node_0 = nodes_of_cells(cell,node_in_cell_0);
          int node_1 = nodes_of_cells(cell,node_in_cell_1);
          // En 3D le centre de gravite des faces est defini par les 2 diagonales
          // pour assurer l'unicite avec la numerotation on prend la diagonale avec l indice de noeuds le grand en premier
          int quad_2d=0;

          if (Objet_U::dimension==3)
            {
              int it=nb_nodes_per_edge_max;
              while ( edges_pattern(edge_in_cell,it-1)==-1) it--;
              if (it==4)
                quad_2d=1;
            }
          if (quad_2d)
            {
              const int node_in_cell_2 = edges_pattern(edge_in_cell,2);
              const int node_in_cell_3 = edges_pattern(edge_in_cell,3);

              int node_2 = nodes_of_cells(cell,node_in_cell_2);
              int node_3 = nodes_of_cells(cell,node_in_cell_3);
              if (node_0 <node_1)
                std::swap(node_0,node_1);
              if (node_2 <node_3)
                std::swap(node_2,node_3);
              if (node_2>node_0)
                {
                  duplicated_edges(nb_duplicated_edges,2)=node_0;
                  duplicated_edges(nb_duplicated_edges,3)=node_1;
                  node_0=node_2;
                  node_1=node_3;
                }
              else
                {
                  assert(node_2!=node_0);
                  duplicated_edges(nb_duplicated_edges,2)=node_2;
                  duplicated_edges(nb_duplicated_edges,3)=node_3;
                }
              for (int c=4; c<nb_nodes_per_edge_max; c++)
                duplicated_edges(nb_duplicated_edges,c)=-1;

            }
          duplicated_edges(nb_duplicated_edges,0) = (node_0 < node_1) ? node_0 : node_1;
          duplicated_edges(nb_duplicated_edges,1) = (node_0 < node_1) ? node_1 : node_0;
          if (!quad_2d)
            for (int c=2; c<nb_nodes_per_edge_max; c++)
              {
                int node=edges_pattern(edge_in_cell,c);
                if (node==-1)
                  duplicated_edges(nb_duplicated_edges,c)=-1;
                else
                  duplicated_edges(nb_duplicated_edges,c)=nodes_of_cells(cell,node);
              }
          ++nb_duplicated_edges;
        }
    }


  ArrOfInt edges_indices;
  tri_lexicographique_tableau_indirect(duplicated_edges,edges_indices);

  edges_of_cells.resize(nb_cells, nb_edges_per_cell);
  nodes_of_edges.resize(0, nb_nodes_per_edge_max);
  nodes_of_edges.set_smart_resize(1);

  ArrOfInt& edges_of_cells_array = edges_of_cells;
  int node_0,node_1,edges_counter;
  node_0 = node_1 = edges_counter = -1;

  for (int duplicated_edge=0; duplicated_edge<nb_duplicated_edges; ++duplicated_edge)
    {
      const int edge_index = edges_indices[duplicated_edge];
      const int new_node_0 = duplicated_edges(edge_index, 0);
      const int new_node_1 = duplicated_edges(edge_index, 1);
      if (node_0 != new_node_0 || node_1 != new_node_1)
        {
          edges_counter++;
          nodes_of_edges.resize(edges_counter+1, nb_nodes_per_edge_max);
          nodes_of_edges(edges_counter, 0) = new_node_0;
          nodes_of_edges(edges_counter, 1) = new_node_1;
          for (int c=2; c<nb_nodes_per_edge_max; c++)
            nodes_of_edges(edges_counter, c)=duplicated_edges(edge_index,c);
          node_0 = new_node_0;
          node_1 = new_node_1;
        }
      // la, ya un truc qui m'etonne : dans mon code, c'est plus complexe
      edges_of_cells_array[edge_index] = edges_counter;
    }

  nodes_of_edges.resize(edges_counter+2, nb_nodes_per_edge_max);
  nodes_of_edges.set_smart_resize(0);
  nodes_of_edges.resize(edges_counter+1, nb_nodes_per_edge_max);
}

static void build_nodes(const DoubleTab& nodes_src,
                        const IntTab&     nodes_of_edges_src,
                        DoubleTab&        nodes_dest)
{
  assert( &(nodes_src) != &(nodes_dest) );

  const int nb_nodes_src = nodes_src.dimension(0);
  const int nb_edges_src = nodes_of_edges_src.dimension(0);

  const int nb_nodes_dest = nb_nodes_src + nb_edges_src;

  nodes_dest = nodes_src;
  nodes_dest.resize(nb_nodes_dest,Objet_U::dimension);
  int nb_node_per_edge_max= nodes_of_edges_src.dimension(1);
  if (nb_node_per_edge_max==20)
    for (int edge=0; edge<nb_edges_src; ++edge)
      {
        const int node_0 = nodes_of_edges_src(edge, 0);
        const int node_1 = nodes_of_edges_src(edge, 1);
        for (int i=0; i<Objet_U::dimension; ++i)
          {
            const double x = (nodes_src(node_0, i) + nodes_src(node_1, i)) * 0.5;
            nodes_dest(nb_nodes_src + edge, i) = x;
          }
      }
  else
    for (int edge=0; edge<nb_edges_src; ++edge)
      {
        int nb=nb_node_per_edge_max;

        while(nodes_of_edges_src(edge,nb-1)==-1) nb--;

        double inv=1./nb;

        const int node_0 = nodes_of_edges_src(edge, 0);
        for (int i=0; i<Objet_U::dimension; ++i)
          {
            double x = nodes_src(node_0, i);
            for (int c=1; c<nb; c++)
              {
                const int node_1 = nodes_of_edges_src(edge, c);
                x+=nodes_src(node_1, i);
              }
            nodes_dest(nb_nodes_src + edge, i) = x*inv;
          }
      }
}

static int find_refined_node_index(const IntTab& nodes_of_cells_src,
                                   const IntTab& edges_of_cells_src,
                                   int         nb_nodes_src,
                                   int         cell_src,
                                   int         index)
{
  int node_index = -1;
  if (index >= 0)
    {
      const int idx = NODE_ID(index);
      node_index = nodes_of_cells_src(cell_src,idx);
    }
  else
    {
      const int idx = EDGE_ID(index);
      node_index = nb_nodes_src + edges_of_cells_src(cell_src,idx);
    }
  return node_index;
}

static void build_cells(const IntTab& nodes_of_cells_src,
                        const IntTab& edges_of_cells_src,
                        const IntTab& pattern,
                        int         nb_nodes_src,
                        IntTab&        nodes_of_cells_dest)
{
  assert( &(nodes_of_cells_src) != &(nodes_of_cells_dest) );

  const int nb_cells_src              = nodes_of_cells_src.dimension(0);
  const int nb_nodes_per_cell         = nodes_of_cells_src.dimension(1);
  const int nb_refined_cells_per_cell = pattern.dimension(0);

  nodes_of_cells_dest.resize(nb_cells_src * nb_refined_cells_per_cell, nb_nodes_per_cell);

  int nb_cells_dest = 0;
  for (int cell_src=0; cell_src<nb_cells_src; ++cell_src)
    {
      for (int refined_cell=0; refined_cell<nb_refined_cells_per_cell; ++refined_cell)
        {
          for (int node_in_cell=0; node_in_cell<nb_nodes_per_cell; ++node_in_cell)
            {
              const int index = pattern(refined_cell,node_in_cell);
              const int node  = find_refined_node_index(nodes_of_cells_src,edges_of_cells_src,nb_nodes_src,cell_src,index);
              nodes_of_cells_dest(nb_cells_dest,node_in_cell) = node;
            }
          ++nb_cells_dest;
        }
    }
}

static void build_frontier(const Frontiere&         src,
                           const Type_Face&         face_type,
                           const IntTab&            nodes_of_cells_src,
                           const Static_Int_Lists& cells_of_nodes_src,
                           const IntTab&            edges_of_cells_src,
                           const IntTab&            faces_pattern,
                           const IntTab&            faces_refinement_pattern,
                           const Zone&              zone_dest,
                           Frontiere&               dest)
{
  dest.associer_zone(zone_dest);
  dest.typer_faces(face_type);
  dest.nommer(src.le_nom());

  const IntTab& nodes_of_faces_src         = src.les_sommets_des_faces();
  int         nb_faces_src               = nodes_of_faces_src.dimension(0);
  int         nb_nodes_per_face          = nodes_of_faces_src.dimension(1);
  int         nb_faces_dest_per_face_src = faces_refinement_pattern.dimension(1);
  int         nb_nodes_src               = cells_of_nodes_src.get_nb_lists();

  IntTab nodes_of_faces_dest(nb_faces_src*nb_faces_dest_per_face_src,nb_nodes_per_face);
  ArrOfInt nodes_of_current_face(nb_nodes_per_face);
  ArrOfInt incident_cells;
  incident_cells.set_smart_resize(1);

  for (int face=0; face<nb_faces_src; ++face)
    {
      for (int node_in_face=0; node_in_face<nb_nodes_per_face; ++node_in_face)
        {
          nodes_of_current_face[node_in_face] = nodes_of_faces_src(face,node_in_face);
        }
      find_adjacent_elements(cells_of_nodes_src,nodes_of_current_face,incident_cells);
      if (incident_cells.size_array() != 1)
        {
          // Cette erreur n'en est pas une pour les "faces internes"...
          // si le cas se presente, faire un test en essayant d'ignorer l'erreur...
          Cerr << "Error in Raffiner_Simplexes.cpp 'build_frontier()'" << finl;
          Cerr << "  The boundary face " << face << " having nodes " << nodes_of_current_face << " has the following incident cells " << incident_cells << finl;
          Process::exit();
        }
      const int coarse_cell         = incident_cells[0];
      const int face_in_coarse_cell = Faces_builder::chercher_face_element(nodes_of_cells_src,
                                                                           faces_pattern,
                                                                           nodes_of_current_face,
                                                                           coarse_cell);
      if (face_in_coarse_cell < 0)
        {
          Cerr << "Error in Raffiner_Simplexes.cpp 'build_frontier()'" << finl;
          Cerr << "  Internal error in 'Faces_builder::chercher_face_element()'" << finl;
          Process::exit();
        }

      for (int fine_face=0; fine_face<nb_faces_dest_per_face_src; ++fine_face)
        {
          int face_dest = (face*nb_faces_dest_per_face_src)+fine_face;
          for (int node_in_face=0; node_in_face<nb_nodes_per_face; ++node_in_face)
            {
              const int index = faces_refinement_pattern(face_in_coarse_cell,fine_face,node_in_face);
              const int node_dest  = find_refined_node_index(nodes_of_cells_src,edges_of_cells_src,nb_nodes_src,coarse_cell,index);
              nodes_of_faces_dest(face_dest,node_in_face) = node_dest;
            }
        }
    }
  dest.les_sommets_des_faces() = nodes_of_faces_dest;
}

void Raffiner_Simplexes::refine_domain(const Domaine& src,
                                       Domaine&        dest)
{

  const Nom&        cell_type          = src.zone(0).type_elem().valeur().que_suis_je();
  const IntTab&     nodes_of_cells_src = src.zone(0).les_elems();
  const DoubleTab& nodes_src          = src.les_sommets();

  IntTab edges_pattern;
  build_edges_pattern(edges_pattern,cell_type);

  IntTab faces_pattern;
  build_faces_pattern(faces_pattern,cell_type);

  IntTab cell_refinement_pattern;
  build_cell_refinement_pattern(cell_refinement_pattern,cell_type);

  IntTab face_refinement_pattern;
  build_face_refinement_pattern(face_refinement_pattern,cell_type);

  Static_Int_Lists cells_of_nodes_src;
  construire_connectivite_som_elem(src.nb_som(), nodes_of_cells_src, cells_of_nodes_src,0);


  IntTab edges_of_cells_src;
  IntTab nodes_of_edges_src;
  build_edges(nodes_of_cells_src, edges_pattern, edges_of_cells_src, nodes_of_edges_src);

  DoubleTab& nodes_dest = dest.les_sommets();
  build_nodes(nodes_src,nodes_of_edges_src,nodes_dest);

  IntTab& nodes_of_cells_dest = dest.zone(0).les_elems();
  build_cells(nodes_of_cells_src,edges_of_cells_src,cell_refinement_pattern,nodes_src.dimension(0),nodes_of_cells_dest);

  {
    const Bords& boundaries_src  = src.zone(0).faces_bord();
    Bords&        boundaries_dest = dest.zone(0).faces_bord();
    boundaries_dest.vide();
    const int nb_boundaries = boundaries_src.size();
    for (int boundary=0; boundary<nb_boundaries; ++boundary)
      {
        boundaries_dest.add(Bord());
        const Type_Face& face_type = boundaries_src[boundary].faces().type_face();
        build_frontier(boundaries_src[boundary],
                       face_type,
                       nodes_of_cells_src,
                       cells_of_nodes_src,
                       edges_of_cells_src,
                       faces_pattern,
                       face_refinement_pattern,
                       dest.zone(0),
                       boundaries_dest[boundary]);
      }
  }

  {
    const Raccords& boundaries_src  = src.zone(0).faces_raccord();
    Raccords&        boundaries_dest = dest.zone(0).faces_raccord();
    boundaries_dest.vide();
    const int nb_boundaries = boundaries_src.size();
    for (int boundary=0; boundary<nb_boundaries; ++boundary)
      {
        boundaries_dest.add(Raccord());
        boundaries_dest[boundary].typer(boundaries_src[boundary].valeur().que_suis_je());
        const Type_Face& face_type = boundaries_src[boundary].valeur().faces().type_face();
        build_frontier(boundaries_src[boundary].valeur(),
                       face_type,
                       nodes_of_cells_src,
                       cells_of_nodes_src,
                       edges_of_cells_src,
                       faces_pattern,
                       face_refinement_pattern,
                       dest.zone(0),
                       boundaries_dest[boundary].valeur());
      }
  }

  {
    const Faces_Internes& boundaries_src  = src.zone(0).faces_int();
    Faces_Internes&        boundaries_dest = dest.zone(0).faces_int();
    boundaries_dest.vide();
    const int nb_boundaries = boundaries_src.size();
    for (int boundary=0; boundary<nb_boundaries; ++boundary)
      {
        boundaries_dest.add(Faces_Internes());
        const Type_Face& face_type = boundaries_src[boundary].faces().type_face();
        build_frontier(boundaries_src[boundary],
                       face_type,
                       nodes_of_cells_src,
                       cells_of_nodes_src,
                       edges_of_cells_src,
                       faces_pattern,
                       face_refinement_pattern,
                       dest.zone(0),
                       boundaries_dest[boundary]);
      }
  }
}

Entree& Raffiner_Simplexes::interpreter_(Entree& is)
{
  associer_domaine(is);

  Domaine& domain = domaine();
  Scatter::uninit_sequential_domain(domain);

  Domaine initial_domain(domain);
  refine_domain(initial_domain,domain);
  Scatter::init_sequential_domain(domain);

  return is;
}
