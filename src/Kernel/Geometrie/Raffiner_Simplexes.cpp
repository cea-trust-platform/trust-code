/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Raffiner_Simplexes.h>
#include <Domaine.h>
#include <Nom.h>
#include <Scatter.h>
#include <Motcle.h>
#include <TRUSTTab.h>
#include <Array_tools.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>
#include <Synonyme_info.h>

Implemente_instanciable_32_64(Raffiner_Simplexes_32_64,"Raffiner_Simplexes",Interprete_geometrique_base_32_64<_T_>) ;
Add_synonym(Raffiner_Simplexes,"Raffiner_isotrope"); // Raffiner_Simplexes class replaces the obsolete Raffiner_isotrope class

// XD raffiner_isotrope interprete raffiner_isotrope -1 For VDF and VEF discretizations, allows to cut triangles/quadrangles or tetrahedral/hexaedras elements respectively in 4 or 8 new ones by defining new summits located at the middle of edges (and center of faces and elements for quadrangles and hexaedra). Such a cut preserves the shape of original elements (isotropic). For 2D elements: \includepng{{raffinerisotrirect.jpeg}}{{6}} NL2 For 3D elements: \includepng{{raffinerisotetra.jpeg}}{{6}} NL2 \includepng{{raffinerisohexa.jpeg}}{{5}}.
// XD attr domain_name ref_domaine domain_name 0 Name of domain.

template<typename _SIZE_>
Sortie& Raffiner_Simplexes_32_64<_SIZE_>::printOn(Sortie& os) const
{
  Interprete::printOn(os);
  return os;
}

template<typename _SIZE_>
Entree& Raffiner_Simplexes_32_64<_SIZE_>::readOn(Entree& is)
{
  Interprete::readOn(is);
  return is;
}

/////////// Helper implementation class
template<typename _SIZE_>
class Impl_32_64
{
public:
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Sous_Domaine_t = Sous_Domaine_32_64<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;
  using Groupe_Faces_t = Groupe_Faces_32_64<_SIZE_>;
  using Bord_Interne_t = Bord_Interne_32_64<_SIZE_>;

  static void build_edges(const IntTab_t& nodes_of_cells, const IntTab& edges_pattern,
                          IntTab_t& edges_of_cells, IntTab_t& nodes_of_edges);
  static void build_nodes(const DoubleTab_t& nodes_src, const IntTab_t& nodes_of_edges_src, DoubleTab_t& nodes_dest);
  static int_t find_refined_node_index(const IntTab_t& nodes_of_cells_src, const IntTab_t& edges_of_cells_src,
                                       int_t nb_nodes_src, int_t cell_src, int index);
  static void build_cells(const IntTab_t& nodes_of_cells_src, const IntTab_t& edges_of_cells_src, const IntTab& pattern,
                          int_t nb_nodes_src, IntTab_t&  nodes_of_cells_dest);
  static void build_frontier(const Frontiere_t& src,
                             const Type_Face&         face_type,
                             const IntTab_t&            nodes_of_cells_src,
                             const Static_Int_Lists_32_64<_SIZE_>& cells_of_nodes_src,
                             const IntTab_t&            edges_of_cells_src,
                             const IntTab&            faces_pattern,
                             const IntTab&            faces_refinement_pattern,
                             const Domaine_t&              domaine_dest,
                             Frontiere_t&               dest);
};


// Macros pour l'encodage de l'idx (meme fonction que build_idx mais pratique
//  pour ecrire le code de build_refine_...)
// Sommet fin situe sur le sommet "num" de l'element grossier:
#define NODE_ID(num) num
// Sommet fin situe au milieu de l'arete "num" de l'element grossier:
#define EDGE_ID(num) (-num-1)

namespace
{

void fill_tab_2(const int *t, int n, int m, IntTab& tab)
{
  tab.resize(n, m);

  for (int i=0; i<n; ++i)
    for (int j=0; j<m; ++j)
      tab(i,j) = t[i*m+j];

  if (t[n*m] != -1)
    {
      Cerr << "Error in Raffiner_Simplexes_32_64 'filltab_2'" << finl;
      Process::exit();
    }
}

void fill_tab_2_new(const int *t, int n, int m, IntTab& tab)
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
      Cerr << "Error in Raffiner_Simplexes_32_64 'filltab_2_new'" << finl;
      Process::exit();
    }
}


void fill_tab_3(const int *t, int n, int m, int p, IntTab& tab)
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
      Cerr << "Error in Raffiner_Simplexes_32_64 'filltab_3'" << finl;
      Process::exit();
    }
}


void build_quadrangle_cell_refinement_pattern(IntTab& pattern)
{
  const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(3), EDGE_ID(4), // triangle defini par sommet0 arete0 arete1
                     EDGE_ID(0), NODE_ID(1), EDGE_ID(4),EDGE_ID(1), // etc...
                     EDGE_ID(4), EDGE_ID(1), EDGE_ID(2), NODE_ID(3),
                     EDGE_ID(3), EDGE_ID(4), NODE_ID(2), EDGE_ID(2), -1
                  } ;
  fill_tab_2(t, 4, 4, pattern);
}

void build_triangle_cell_refinement_pattern(IntTab& pattern)
{
  const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(1), // triangle defini par sommet0 arete0 arete1
                     EDGE_ID(0), NODE_ID(1), EDGE_ID(2), // etc...
                     EDGE_ID(1), EDGE_ID(2), NODE_ID(2),
                     EDGE_ID(2), EDGE_ID(1), EDGE_ID(0), -1
                  } ;
  fill_tab_2(t, 4, 3, pattern);
}

void build_tetrahedron_cell_refinement_pattern(IntTab& pattern)
{
  const int t[] = {  NODE_ID(0), EDGE_ID(0), EDGE_ID(1), EDGE_ID(2) , // tetraedre defini par sommet0 arete0 arete1 arete2
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
void build_hexa_cell_refinement_pattern(IntTab& pattern)
{
  const int t[] =
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

void build_cell_refinement_pattern(IntTab& pattern, const Nom& cell_type)
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
      Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_cell_refinement_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes_32_64 can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}


void build_quadrangle_face_refinement_pattern(IntTab& pattern)
{
  const int t[] = {   NODE_ID(0), EDGE_ID(0),
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

void build_triangle_face_refinement_pattern(IntTab& pattern)
{
  const int t[] = {  NODE_ID(2), EDGE_ID(2),
                     EDGE_ID(2), NODE_ID(1),

                     NODE_ID(0), EDGE_ID(1),
                     EDGE_ID(1), NODE_ID(2),

                     NODE_ID(1), EDGE_ID(0),
                     EDGE_ID(0), NODE_ID(0), -1
                  };
  fill_tab_3(t, 3, 2, 2, pattern);
}

void build_tetrahedron_face_refinement_pattern(IntTab& pattern)
{
  const int t[] = { NODE_ID(1), EDGE_ID(3), EDGE_ID(4),
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

void build_hexa_face_refinement_pattern(IntTab& pattern)
{
  const int t[] = { NODE_ID(0), EDGE_ID(3), EDGE_ID(5), EDGE_ID(8),           // 0 2 4 6
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


void build_face_refinement_pattern(IntTab& pattern, const Nom& cell_type)
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
      Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_face_refinement_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes_32_64 can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}


void build_quadrangle_edges_pattern(IntTab& pattern)
{
  const int t[] = {  2,0, 1 ,
                     2,1, 3,
                     2, 2, 3,
                     2, 0, 2,
                     4,0,3,1,2,
                     -1
                  };
  fill_tab_2_new(t, 5, 4, pattern);
}

void build_triangle_edges_pattern(IntTab& pattern)
{
  const int t[] = {  0, 1 ,
                     0, 2 ,
                     1, 2 , -1
                  };
  fill_tab_2(t, 3, 2, pattern);
}

void build_tetrahedron_edges_pattern(IntTab& pattern)
{
  const int t[] = {  0, 1 , // L'arete 0 est entre le sommet 0 et le sommet 1 de l'element
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
void build_hexa_edges_pattern(IntTab& pattern)
{

  const int t[] = {  2, 0, 1 , // L'arete 0 est entre le sommet 0 et le sommet 1 de l'element
                     2, 1, 3 , // etc...
                     2, 2, 3 ,
                     2, 0, 2 ,
                     4, 0, 3 , 1,2,
                     2, 0,4, 4,0,5,1,4 ,2, 1,5 , // 5' 6' 7'
                     4,0,6,2,4, 8,0,7,1,6,3,4,2,5, 4,1,7,3,5,
                     2,2,6, 4,2,7,3,6, 2,3,7, //11' 12' 13'
                     2,4,5, 2,5,7, 2,6,7, 2,4,6 , 4,4,7,5,6,
                     -1
                  };
  fill_tab_2_new(t, 19, 8, pattern);
}

void build_edges_pattern(IntTab& pattern, const Nom& cell_type)
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
      Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_edges_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes_32_64 can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

void build_quadrangle_faces_pattern(IntTab& pattern)
{
  const int t[] = {  0, 1 ,
                     1, 3,
                     3, 2 ,
                     2, 0 , -1
                  };
  fill_tab_2(t, 4, 2, pattern);
}

void build_triangle_faces_pattern(IntTab& pattern)
{
  const int t[] = {  2, 1 ,
                     0, 2,
                     1, 0 , -1
                  };
  fill_tab_2(t, 3, 2, pattern);
}

void build_tetrahedron_faces_pattern(IntTab& pattern)
{
  const int t[] = {  1, 2, 3,
                     2, 0, 3,
                     3, 0, 1,
                     0, 2, 1, -1
                  };
  fill_tab_2(t, 4, 3, pattern);
}

void build_hexa_faces_pattern(IntTab& pattern)
{
  const int t[] = {   0, 2, 4, 6 ,
                      0, 1, 4, 5 ,
                      0, 1, 2, 3 ,
                      1, 3, 5, 7 ,
                      2, 3, 6, 7 ,
                      4, 5, 6, 7 , -1
                  };
  fill_tab_2(t, 6, 4, pattern);
}


void build_faces_pattern(IntTab& pattern, const Nom& cell_type)
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
      Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_faces_pattern()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Raffiner_Simplexes_32_64 can only refine Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

} // end anonymous namespace

///////////////////////////////////////////////////////////////

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::build_edges(const IntTab_t& nodes_of_cells, const IntTab& edges_pattern,
                                     IntTab_t& edges_of_cells, IntTab_t& nodes_of_edges)
{
  const int_t nb_cells          = nodes_of_cells.dimension(0);
  const int nb_edges_per_cell = edges_pattern.dimension(0);
  //  const int nb_nodes_per_edge = 2;
  const int nb_nodes_per_edge_max = edges_pattern.dimension(1);
  IntTab_t duplicated_edges(nb_cells * nb_edges_per_cell, nb_nodes_per_edge_max);
  int_t nb_duplicated_edges = 0;
  for (int_t cell=0; cell<nb_cells; ++cell)
    {
      for (int edge_in_cell=0; edge_in_cell<nb_edges_per_cell; ++edge_in_cell)
        {
          const int node_in_cell_0 = edges_pattern(edge_in_cell,0);
          const int node_in_cell_1 = edges_pattern(edge_in_cell,1);

          int_t node_0 = nodes_of_cells(cell,node_in_cell_0);
          int_t node_1 = nodes_of_cells(cell,node_in_cell_1);
          // En 3D le centre de gravite des faces est defini par les 2 diagonales
          // pour assurer l'unicite avec la numerotation on prend la diagonale avec l indice de noeuds le grand en premier
          bool quad_2d=false;

          if (Objet_U::dimension==3)
            {
              int it=nb_nodes_per_edge_max;
              while ( edges_pattern(edge_in_cell,it-1)==-1) it--;
              if (it==4)
                quad_2d=true;
            }
          if (quad_2d)
            {
              const int node_in_cell_2 = edges_pattern(edge_in_cell,2);
              const int node_in_cell_3 = edges_pattern(edge_in_cell,3);

              int_t node_2 = nodes_of_cells(cell,node_in_cell_2);
              int_t node_3 = nodes_of_cells(cell,node_in_cell_3);
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


  ArrOfInt_t edges_indices;
  tri_lexicographique_tableau_indirect(duplicated_edges,edges_indices);

  edges_of_cells.resize(nb_cells, nb_edges_per_cell);
  nodes_of_edges.resize(0, nb_nodes_per_edge_max);


  ArrOfInt_t& edges_of_cells_array = edges_of_cells;
  int_t node_0,node_1,edges_counter;
  node_0 = node_1 = edges_counter = -1;

  for (int_t duplicated_edge=0; duplicated_edge<nb_duplicated_edges; ++duplicated_edge)
    {
      const int_t edge_index = edges_indices[duplicated_edge];
      const int_t new_node_0 = duplicated_edges(edge_index, 0);
      const int_t new_node_1 = duplicated_edges(edge_index, 1);
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

  nodes_of_edges.resize(edges_counter+1, nb_nodes_per_edge_max);
}

template<typename _SIZE_>
void Impl_32_64<_SIZE_>::build_nodes(const DoubleTab_t& nodes_src, const IntTab_t& nodes_of_edges_src, DoubleTab_t& nodes_dest)
{
  assert( &(nodes_src) != &(nodes_dest) );

  const int_t nb_nodes_src = nodes_src.dimension(0);
  const int_t nb_edges_src = nodes_of_edges_src.dimension(0);

  const int_t nb_nodes_dest = nb_nodes_src + nb_edges_src;

  nodes_dest = nodes_src;
  nodes_dest.resize(nb_nodes_dest,Objet_U::dimension);
  int nb_node_per_edge_max= nodes_of_edges_src.dimension_int(1);
  if (nb_node_per_edge_max==20)
    for (int_t edge=0; edge<nb_edges_src; ++edge)
      {
        const int_t node_0 = nodes_of_edges_src(edge, 0);
        const int_t node_1 = nodes_of_edges_src(edge, 1);
        for (int i=0; i<Objet_U::dimension; ++i)
          {
            const double x = (nodes_src(node_0, i) + nodes_src(node_1, i)) * 0.5;
            nodes_dest(nb_nodes_src + edge, i) = x;
          }
      }
  else
    for (int_t edge=0; edge<nb_edges_src; ++edge)
      {
        int nb=nb_node_per_edge_max;

        while(nodes_of_edges_src(edge,nb-1)==-1) nb--;

        double inv=1./nb;

        const int_t node_0 = nodes_of_edges_src(edge, 0);
        for (int i=0; i<Objet_U::dimension; ++i)
          {
            double x = nodes_src(node_0, i);
            for (int c=1; c<nb; c++)
              {
                const int_t node_1 = nodes_of_edges_src(edge, c);
                x+=nodes_src(node_1, i);
              }
            nodes_dest(nb_nodes_src + edge, i) = x*inv;
          }
      }
}

template<typename _SIZE_>
typename Impl_32_64<_SIZE_>::int_t Impl_32_64<_SIZE_>::find_refined_node_index(const IntTab_t& nodes_of_cells_src, const IntTab_t& edges_of_cells_src,
                                                                               int_t nb_nodes_src, int_t cell_src, int index)
{
  int_t node_index = -1;
  if (index >= 0)
    node_index = nodes_of_cells_src(cell_src,NODE_ID(index));
  else
    node_index = nb_nodes_src + edges_of_cells_src(cell_src,EDGE_ID(index));
  return node_index;
}

template<typename _SIZE_>
void Impl_32_64<_SIZE_>:: build_cells(const IntTab_t& nodes_of_cells_src, const IntTab_t& edges_of_cells_src, const IntTab& pattern,
                                      int_t nb_nodes_src, IntTab_t&  nodes_of_cells_dest)
{
  assert( &(nodes_of_cells_src) != &(nodes_of_cells_dest) );

  const int_t nb_cells_src              = nodes_of_cells_src.dimension(0);
  const int nb_nodes_per_cell         = nodes_of_cells_src.dimension_int(1);
  const int nb_refined_cells_per_cell = pattern.dimension(0);

  nodes_of_cells_dest.resize(nb_cells_src * nb_refined_cells_per_cell, nb_nodes_per_cell);

  int_t nb_cells_dest = 0;
  for (int_t cell_src=0; cell_src<nb_cells_src; ++cell_src)
    {
      for (int refined_cell=0; refined_cell<nb_refined_cells_per_cell; ++refined_cell)
        {
          for (int node_in_cell=0; node_in_cell<nb_nodes_per_cell; ++node_in_cell)
            {
              const int index = pattern(refined_cell,node_in_cell);
              const int_t node  = find_refined_node_index(nodes_of_cells_src,edges_of_cells_src,nb_nodes_src,cell_src,index);
              nodes_of_cells_dest(nb_cells_dest,node_in_cell) = node;
            }
          ++nb_cells_dest;
        }
    }
}


template<typename _SIZE_>
void Impl_32_64<_SIZE_>::build_frontier(const Frontiere_t& src,
                                        const Type_Face&         face_type,
                                        const IntTab_t&            nodes_of_cells_src,
                                        const Static_Int_Lists_32_64<_SIZE_>& cells_of_nodes_src,
                                        const IntTab_t&            edges_of_cells_src,
                                        const IntTab&            faces_pattern,
                                        const IntTab&            faces_refinement_pattern,
                                        const Domaine_t&              domaine_dest,
                                        Frontiere_t&               dest)
{
  dest.associer_domaine(domaine_dest);
  dest.typer_faces(face_type);
  dest.nommer(src.le_nom());

  const IntTab_t& nodes_of_faces_src         = src.les_sommets_des_faces();
  int_t         nb_faces_src               = nodes_of_faces_src.dimension(0);
  int         nb_nodes_per_face          =  nodes_of_faces_src.dimension_int(1);
  int         nb_faces_dest_per_face_src = faces_refinement_pattern.dimension_int(1);
  int_t         nb_nodes_src               = cells_of_nodes_src.get_nb_lists();

  IntTab_t nodes_of_faces_dest(nb_faces_src*nb_faces_dest_per_face_src,nb_nodes_per_face);
  SmallArrOfTID_t nodes_of_current_face(nb_nodes_per_face);
  SmallArrOfTID_t incident_cells;
  bool is_internal_faces = (sub_type(Groupe_Faces_t, src) || sub_type(Joint, src) || sub_type(Bord_Interne_t, src)) ;

  for (int_t face=0; face<nb_faces_src; ++face)
    {
      for (int node_in_face=0; node_in_face<nb_nodes_per_face; ++node_in_face)
        nodes_of_current_face[node_in_face] = nodes_of_faces_src(face,node_in_face);

      find_adjacent_elements(cells_of_nodes_src,nodes_of_current_face,incident_cells);
      if ((incident_cells.size_array() != 1) && (!is_internal_faces))
        {
          // Cette erreur n'en est pas une pour les "bords internes, groupe_faces ou joints"...
          // si le cas se presente, faire un test en essayant d'ignorer l'erreur...
          Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_frontier()'" << finl;
          Cerr << "  The boundary face " << face << " having nodes " << nodes_of_current_face << " has the following incident cells " << incident_cells << finl;
          Process::exit();
        }
      const int_t coarse_cell         = incident_cells[0];
      const int face_in_coarse_cell = Faces_builder::chercher_face_element<_SIZE_>(nodes_of_cells_src,
                                                                                   faces_pattern,
                                                                                   nodes_of_current_face,
                                                                                   coarse_cell);
      if (face_in_coarse_cell < 0)
        {
          Cerr << "Error in Raffiner_Simplexes_32_64.cpp 'build_frontier()'" << finl;
          Cerr << "  Internal error in 'Faces_builder::chercher_face_element()'" << finl;
          Process::exit();
        }

      for (int fine_face=0; fine_face<nb_faces_dest_per_face_src; ++fine_face)
        {
          int_t face_dest = (face*nb_faces_dest_per_face_src)+fine_face;
          for (int node_in_face=0; node_in_face<nb_nodes_per_face; ++node_in_face)
            {
              const int index = faces_refinement_pattern(face_in_coarse_cell,fine_face,node_in_face);
              const int_t node_dest  = find_refined_node_index(nodes_of_cells_src,edges_of_cells_src,nb_nodes_src,coarse_cell,index);
              nodes_of_faces_dest(face_dest,node_in_face) = node_dest;
            }
        }
    }
  dest.les_sommets_des_faces() = nodes_of_faces_dest;
}



template<typename _SIZE_>
void Raffiner_Simplexes_32_64<_SIZE_>::refine_domain(const Domaine_t& src, Domaine_t& dest)
{
  using Impl_ = Impl_32_64<_SIZE_>;

  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;

  using Sous_Domaine_t = Sous_Domaine_32_64<_SIZE_>;
  using Bords_t = Bords_32_64<_SIZE_>;
  using Bord_t = Bord_32_64<_SIZE_>;
  using Groupes_Faces_t = Groupes_Faces_32_64<_SIZE_>;
  using Groupe_Faces_t = Groupe_Faces_32_64<_SIZE_>;
  using Bords_Internes_t = Bords_Internes_32_64<_SIZE_>;
  using Bord_Interne_t = Bord_Interne_32_64<_SIZE_>;
  using Joints_t = Joints_32_64<_SIZE_>;
  using Joint_t = Joint_32_64<_SIZE_>;
  using Raccords_t = Raccords_32_64<_SIZE_>;
  using Raccord_t = OWN_PTR(Raccord_base_32_64<_SIZE_>);


  const Nom&   cell_type          = src.type_elem()->que_suis_je();
  const IntTab_t& nodes_of_cells_src = src.les_elems();
  const DoubleTab_t& nodes_src          = src.les_sommets();

  IntTab edges_pattern;
  ::build_edges_pattern(edges_pattern,cell_type);

  IntTab faces_pattern;
  ::build_faces_pattern(faces_pattern,cell_type);

  IntTab cell_refinement_pattern;
  ::build_cell_refinement_pattern(cell_refinement_pattern,cell_type);

  IntTab face_refinement_pattern;
  ::build_face_refinement_pattern(face_refinement_pattern,cell_type);

  Static_Int_Lists_32_64<_SIZE_> cells_of_nodes_src;
  ::construire_connectivite_som_elem(src.nb_som(), nodes_of_cells_src, cells_of_nodes_src,0);

  IntTab_t edges_of_cells_src;
  IntTab_t nodes_of_edges_src;
  Impl_::build_edges(nodes_of_cells_src, edges_pattern, edges_of_cells_src, nodes_of_edges_src);

  DoubleTab_t& nodes_dest = dest.les_sommets();
  Impl_::build_nodes(nodes_src,nodes_of_edges_src,nodes_dest);

  IntTab_t& nodes_of_cells_dest = dest.les_elems();
  Impl_::build_cells(nodes_of_cells_src,edges_of_cells_src,cell_refinement_pattern,nodes_src.dimension(0),nodes_of_cells_dest);

  // Rebuild sub-domains:
  const int nb_refined_cells_per_cell = cell_refinement_pattern.dimension(0);
  for (int i=0; i<dest.nb_ss_domaines(); i++)
    {
      Sous_Domaine_t& ssz = dest.ss_domaine(i);
      Cerr << "Refining sub-domain " << ssz.le_nom() << finl;
      IntVect_t  old_polys = ssz.les_elems();
      IntVect_t& les_polys = ssz.les_elems();
      int_t old_size = old_polys.size_array();
      les_polys.resize(old_size * nb_refined_cells_per_cell);
      int_t new_poly=0;
      for (auto& old_poly : old_polys)
        {
          for (int refined_cell = 0; refined_cell < nb_refined_cells_per_cell; ++refined_cell)
            {
              les_polys(new_poly) = old_poly * nb_refined_cells_per_cell + refined_cell;
              new_poly++;
            }
        }
    }

  auto build_front_lambda_rac = [&] (const auto& boundaries_src_arg, auto& boundaries_dest_arg, auto&& new_obj)
  {
    boundaries_dest_arg.vide();
    const int nb_boundaries = boundaries_src_arg.size();
    for (int boundary=0; boundary<nb_boundaries; ++boundary)
      {
        boundaries_dest_arg.add(new_obj);
        boundaries_dest_arg[boundary].typer(boundaries_src_arg[boundary]->que_suis_je());
        const Type_Face& face_type = boundaries_src_arg[boundary]->faces().type_face();
        Impl_::build_frontier(boundaries_src_arg[boundary].valeur(),
                              face_type,
                              nodes_of_cells_src, cells_of_nodes_src, edges_of_cells_src,
                              faces_pattern, face_refinement_pattern,
                              dest,
                              boundaries_dest_arg[boundary].valeur());
      }
  };

  // Almost the same, without .valeur():
  auto build_front_lambda = [&] (const auto& boundaries_src_arg, auto& boundaries_dest_arg, auto&& new_obj)
  {
    boundaries_dest_arg.vide();
    const int nb_boundaries_loc = boundaries_src_arg.size();
    for (int boundary=0; boundary<nb_boundaries_loc; ++boundary)
      {
        boundaries_dest_arg.add(new_obj);
        const Type_Face& face_type = boundaries_src_arg[boundary].faces().type_face();
        Impl_::build_frontier(boundaries_src_arg[boundary],
                              face_type,
                              nodes_of_cells_src, cells_of_nodes_src, edges_of_cells_src,
                              faces_pattern, face_refinement_pattern,
                              dest,
                              boundaries_dest_arg[boundary]);
      }
  };


  {
    const Bords_t& boundaries_src  = src.faces_bord();
    Bords_t&        boundaries_dest = dest.faces_bord();
    build_front_lambda(boundaries_src, boundaries_dest, Bord_t());
  }

  {
    const Raccords_t& boundaries_src  = src.faces_raccord();
    Raccords_t&        boundaries_dest = dest.faces_raccord();
    build_front_lambda_rac(boundaries_src, boundaries_dest, Raccord_t());
  }

  {
    const Bords_Internes_t& boundaries_src  = src.bords_int();
    Bords_Internes_t&        boundaries_dest = dest.bords_int();
    build_front_lambda(boundaries_src, boundaries_dest, Bord_Interne_t());
  }

  {
    const Groupes_Faces_t& boundaries_src  = src.groupes_faces();
    Groupes_Faces_t&        boundaries_dest = dest.groupes_faces();
    build_front_lambda(boundaries_src, boundaries_dest, Groupe_Faces_t());
  }

  {
    const Joints_t& boundaries_src  = src.faces_joint();
    const int nb_boundaries = boundaries_src.size();
    Joints_t&       boundaries_dest = dest.faces_joint();
    build_front_lambda(boundaries_src, boundaries_dest, Joint_t());

    for (int boundary=0; boundary<nb_boundaries; ++boundary)
      {
        Joint_t& joint_dest=boundaries_dest[boundary];
        joint_dest.affecte_PEvoisin(boundaries_src[boundary].PEvoisin());
        joint_dest.affecte_epaisseur(boundaries_src[boundary].epaisseur());

        // creation des SOMMETS communs
        ArrOfInt_t& liste_sommets = joint_dest.set_joint_item(JOINT_ITEM::SOMMET).set_items_communs();

        // On prend tous les sommets des faces de joint:
        const IntTab_t& som_faces = joint_dest.faces().les_sommets();
        liste_sommets = som_faces;
        // on ajoute les sommets du joint d'origine pour
        // les sommets isoles
        const ArrOfInt_t& som_isoles = boundaries_src[boundary].joint_item(JOINT_ITEM::SOMMET).items_communs();
        const int_t n = som_isoles.size_array();
        array_trier_retirer_doublons(liste_sommets);
        ArrOfInt_t liste_sommets_old(liste_sommets);
        for (int_t i = 0; i < n; i++)
          liste_sommets.append_array(som_isoles[i]);
        // Retirer les doublons de la liste
        array_trier_retirer_doublons(liste_sommets);
        int_t n1=liste_sommets.size_array();

        //if (n0!=liste_sommets.size_array())
        {

          SmallArrOfTID_t sommets_to_find(2);
          SmallArrOfTID_t incident_cells;

          // on doit trouver tous les sommets oublies, c'est ceux en plus.
          ArrOfInt_t oublie;// (n1-n0);
          const ArrOfInt_t& liste_sommets_org= som_isoles;
          int_t norg=som_isoles.size_array();
          // les liste sont tries
          oublie=liste_sommets;

          // pour chaque sommet oblie on regarde si on a une arrete commune en regardant tous les sommets de la liste
          for (int_t nio=0; nio<n1; nio++)
            {
              int_t io=oublie[nio];
              sommets_to_find[0]=io;
              for (int_t ns=nio+1; ns<norg; ns++)
                {
                  int_t s=liste_sommets_org[ns];
                  if (s==io)
                    continue;
                  sommets_to_find[1]=s;
                  find_adjacent_elements(cells_of_nodes_src,sommets_to_find,incident_cells);
                  int_t         nb_nodes_src               = cells_of_nodes_src.get_nb_lists();
                  if (incident_cells.size_array()>0)
                    {
                      for (int ne=0; ne<incident_cells.size_array(); ne++)
                        {
                          int_t elem=incident_cells[ne];
                          int nb_nodes_per_cells = nodes_of_cells_src.dimension_int(1);
                          int_t local0=-1,local1=-1;
                          for (int i=0; i<nb_nodes_per_cells; i++)
                            {
                              if (nodes_of_cells_src(elem,i)==io)
                                local0=i;
                              if (nodes_of_cells_src(elem,i)==s)
                                local1=i;
                            }
                          if ((local0==-1)||(local1==-1))
                            {
                              Cerr<<" pB arrete ?"<<finl;
                              Process::exit();
                            }
                          if (local1<local0)
                            std::swap(local0,local1);

                          int nb_edges_pattern=edges_pattern.dimension(0);
                          int nb_nodes_per_edge_max = edges_pattern.dimension(1);
                          int ok=-1;
                          for (int e=0; e<nb_edges_pattern; e++)
                            for (int c=0; c<nb_nodes_per_edge_max-1; c++)
                              {
                                if ((local0==edges_pattern(e,c))&&(local1==edges_pattern(e,c+1)))
                                  {
                                    ok=e;
                                    break;
                                  }
                              }
                          if (ok==-1)
                            {
                              Cerr<<" pB edge ?"<<finl;
                              Process::exit();
                            }

                          const int_t node  = Impl_::find_refined_node_index(nodes_of_cells_src,edges_of_cells_src,nb_nodes_src,elem,-(ok+1));
                          for (int dir=0; dir<Objet_U::dimension; dir++)
                            {
                              double test=nodes_dest(node,dir)-(nodes_dest(io,dir)+nodes_dest(s,dir))*0.5;
                              if (std::fabs(test)>1e-7)
                                {
                                  Cerr<<" pB position ?"<<test <<finl;
                                  Process::exit();
                                }
                            }

                          liste_sommets.append_array(node);
                        }
                    }
                }

            }
          array_trier_retirer_doublons(liste_sommets);
        }
      }
  }
}

template<typename _SIZE_>
Entree& Raffiner_Simplexes_32_64<_SIZE_>::interpreter_(Entree& is)
{
  this->associer_domaine(is);

  Domaine_t& domain = this->domaine();
  Scatter::uninit_sequential_domain(domain);

  Domaine_t initial_domain(domain);
  refine_domain(initial_domain,domain);
  Scatter::init_sequential_domain(domain);

  return is;
}


template class Raffiner_Simplexes_32_64<int>;
#if INT_is_64_ == 2
template class Raffiner_Simplexes_32_64<trustIdType>;
#endif


