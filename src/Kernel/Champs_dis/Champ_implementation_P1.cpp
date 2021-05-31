/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Champ_implementation_P1.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_implementation_P1.h>
#include <Domaine.h>
#include <Octree_Double.h>

double Champ_implementation_P1::form_function(const ArrOfDouble& position, const IntTab& les_elems, const DoubleTab& nodes, ArrOfInt& index, int cell, int ddl) const
{
  int nb_nodes_per_cell = les_elems.dimension(1);
  assert(ddl < nb_nodes_per_cell);

  for (int i=0; i<nb_nodes_per_cell; i++)
    index[i] = les_elems(cell,(i+ddl)%nb_nodes_per_cell);

  if (nb_nodes_per_cell==2)
    {
      double num=0.;
      double den=0.;
      for (int d=0; d<Objet_U::dimension; d++)
        {
          num+=(position[d]-nodes(index[0],d))*(position[d]-nodes(index[0],d));
          den+=(nodes(index[1],d)-nodes(index[0],d))*(nodes(index[1],d)-nodes(index[0],d));
        }
      double res= 1.-sqrt(num/den);
      // Cerr<<"ii "<<res<<" "<<index[0]<<" "<<ddl<<" "<<(ddl)%nb_nodes_per_cell<<finl;
      return res;
    }
  double num = 0.;
  double den = 0.;

  if (Objet_U::dimension == 2)
    {
      den = (nodes(index[2],0) - nodes(index[1],0)) * (nodes(index[0],1) - nodes(index[1],1))
            - (nodes(index[2],1) - nodes(index[1],1)) * (nodes(index[0],0) - nodes(index[1],0));

      num = (nodes(index[2],0) - nodes(index[1],0)) * (position[1] - nodes(index[1],1))
            - (nodes(index[2],1) - nodes(index[1],1)) * (position[0] - nodes(index[1],0));
    }
  else if (Objet_U::dimension == 3)
    {
      double xp = (nodes(index[2],1) - nodes(index[1],1)) * (nodes(index[0],2) - nodes(index[1],2))
                  - (nodes(index[2],2) - nodes(index[1],2)) * (nodes(index[0],1) - nodes(index[1],1));

      double yp = (nodes(index[2],2) - nodes(index[1],2)) * (nodes(index[0],0) - nodes(index[1],0))
                  - (nodes(index[2],0) - nodes(index[1],0)) * (nodes(index[0],2) - nodes(index[1],2));

      double zp = (nodes(index[2],0) - nodes(index[1],0)) * (nodes(index[0],1) - nodes(index[1],1))
                  - (nodes(index[2],1) - nodes(index[1],1)) * (nodes(index[0],0) - nodes(index[1],0));

      den = xp * (nodes(index[3],0) - nodes(index[1],0))
            + yp * (nodes(index[3],1) - nodes(index[1],1))
            + zp * (nodes(index[3],2) - nodes(index[1],2));

      xp = (nodes(index[2],1) - nodes(index[1],1)) * (position[2] - nodes(index[1],2))
           - (nodes(index[2],2) - nodes(index[1],2)) * (position[1] - nodes(index[1],1));

      yp = (nodes(index[2],2) - nodes(index[1],2)) * (position[0] - nodes(index[1],0))
           - (nodes(index[2],0) - nodes(index[1],0)) * (position[2] - nodes(index[1],2));

      zp = (nodes(index[2],0) - nodes(index[1],0)) * (position[1] - nodes(index[1],1))
           - (nodes(index[2],1) - nodes(index[1],1)) * (position[0] - nodes(index[1],0));

      num = xp * (nodes(index[3],0) - nodes(index[1],0))
            + yp * (nodes(index[3],1) - nodes(index[1],1))
            + zp * (nodes(index[3],2) - nodes(index[1],2));
    }
  else
    {
      Cerr << "Error in Champ_implementation_P1::form_function : Invalid dimension" << finl;
      Process::exit();
    }

  assert(den != 0.);
  double result = num/den;

  if ((result < -Objet_U::precision_geom) || (result > 1.+Objet_U::precision_geom))
    {
      Cerr << "WARNING: The barycentric coordinate of point :" << finl;
      Cerr << "x= " << position[0] << " y=" << position[1];
      if (Objet_U::dimension == 3)
        {
          Cerr << " z=" << position[3];
        }
      Cerr << finl;
      Cerr << "is not between 0 and 1 : " << result << finl;
      Cerr << "On the element " << cell << " of the processor " << Process::me() << finl;

#ifndef NDEBUG
      Process::exit();
#endif

    }

  return result;
}
void Champ_implementation_P1::value_interpolation(const DoubleTab& positions, const ArrOfInt& cells, const DoubleTab& values, DoubleTab& resu, int ncomp) const
{
  const Zone&      zone              = get_zone_geom();
  const IntTab&    les_elems         = zone.les_elems();
  const DoubleTab& nodes = zone.domaine().les_sommets();
  const int nb_nodes_per_cell = zone.nb_som_elem(), N = resu.line_size();
  ArrOfInt index(nb_nodes_per_cell);
  ArrOfDouble position(Objet_U::dimension);
  resu = 0;
  for (int ic=0; ic<cells.size_array(); ic++)
    {
      int cell = cells(ic);
      if (cell<0) continue;
      for (int k=0; k<Objet_U::dimension; k++)
        position(k) = positions(ic,k);

      assert(cell >= 0);
      assert(cell < les_elems.dimension_tot(0));
      if (ncomp != -1)
        {
          for (int j = 0; j < nb_nodes_per_cell; j++)
            {
              int node = les_elems(cell, j);
              resu(ic) += values(node, ncomp) * form_function(position,les_elems,nodes,index,cell,j);
            }
        }
      else
        {

          assert(values.line_size() == N);
          for (int j = 0; j < nb_nodes_per_cell; j++)
            {
              double weight = form_function(position,les_elems,nodes,index,cell,j);
              int node = les_elems(cell, j);
              for (int k = 0; k < N; k++)
                resu(ic, k) += values(node, k) * weight;
            }
        }
    }
}

// Description:
//  Initialise le tableau de valeurs aux sommets du domaine dom a partir de valeurs lues dans "input".
//  (on dimensionne, associe la structure parallele et remplit les valeurs reelles et virtuelles)
//  Le fichier doit avoir le format suivant (n est le nombre de valeurs nodales
//  stockees, x, y, z sont les coordonnees des sommets, compo1... sont les valeurs
//  des composantes.
//  n  (int)
//  x y [z] compo1 [compo2 [compo3 ... ]]   (type double)
void Champ_implementation_P1::init_from_file(DoubleTab& val, const Domaine& dom,
                                             int nb_comp, double tolerance, Entree& input)
{
  val.resize(0, nb_comp);
  dom.creer_tableau_sommets(val, Array_base::NOCOPY_NOINIT);

  // Construction d'un octree avec les sommets du domaine:
  const DoubleTab& coord = dom.coord_sommets();
  Octree_Double octree;
  octree.build_nodes(coord, 0 /* do not include virtual nodes */);

  // Lecture des valeurs dans input
  int nb_val_lues;
  input >> nb_val_lues;
  const int dim = coord.dimension(1);
  const int tmp_size = dim + nb_comp;
  ArrOfDouble tmp(tmp_size);
  ArrOfDouble node_coord; // points to the dim first elements of "tmp" (coordinates of the node)
  node_coord.ref_array(tmp, 0 /* start index */, dim /* size */);
  ArrOfInt node_list;
  node_list.set_smart_resize(1);

  ArrOfInt count(dom.nb_som()); // number of times this coordinate has been found

  for (int i_val = 0; i_val < nb_val_lues; i_val++)
    {
      input.get(tmp.addr(), tmp_size);
      // first call returns more points (some might be at a larger distance)
      octree.search_elements_box(node_coord, tolerance, node_list);
      octree.search_nodes_close_to(node_coord, coord, node_list, tolerance);
      const int n = node_list.size_array();
      if (n > 1)
        {
          Cerr << "Error in Champ_som_lu::readOn: point " << node_coord
               << " corresponds to " << node_list.size_array()
               << " nodes in the geometry within the specified tolerance" << finl;
          Process::exit();
        }
      if (n == 1)
        {
          const int node_index = node_list[0];
          count[node_index]++;
          for (int i = 0; i < nb_comp; i++)
            val(node_index, i) = tmp[dim + i];
        }
      else
        {
          // Ce sommet n'est pas sur ce processeur...
        }
    }

  if (dom.nb_som() > 0 && min_array(count) == 0)
    {
      Cerr << "Error in Champ_som_lu::readOn: some coordinates were not found in the file" << finl;
      Process::exit();
    }
  val.echange_espace_virtuel();
}

// Description:
//    Calcule la coordonnee barycentrique d'un point (x,y) par
//    rapport au sommet specifie d'un triangle ou d'un rectange (un element)
//    Ce calcul concerne un point 2D.
// Precondition:
// Parametre: IntTab& polys
//    Signification: tableau contenant les numeros des elements
//                   par rapport auxquels on veut calculer une
//                   coordonnee barycentrique.
//                   polys(i,0) est l'indice du sommet 0 de l'element
//                   i dans le tableau des coordonnees (coord).
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& coord
//    Signification: les coordonnees des sommets par auxquels on veut
//                   calculer les coordonnees barycentriques.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: double x
//    Signification: la premiere coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double y
//    Signification: la deuxieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int le_poly
//    Signification: le numero de l'element (dans le tableau polys) par
//                   rapport auquel on calculera la coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entre
// Parametre: int i
//    Signification: le numero du sommet par rapport auquel on veut la
//                   coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes: Triangle  0 <= i <= 2
//    Contraintes: Rectangle 0 <= i <= 3
//    Acces: entree
// Retour: double
//    Signification: la coordonnee barycentrique du point (x,y) par rapport
//                   au sommet specifie (i) dans l'element specifie (le_poly)
//    Contraintes: 0 <= valeur <= 1
// Excpetion: un triangle n'a pas plus de 3 sommets
// Excpetion: un rectangle n'a pas plus de 4 sommets
// Exception: erreur arithmetique, denominateur nul
// Exception: erreur de calcul, coordonnee barycentrique invalide
// Effets de bord:
// Postcondition:
// Not inlined (this method has been splitted in coord_barycentrique_P1_triangle and coord_barycentrique_P1_rectangle to be inlined elsewhere in the code)
double coord_barycentrique_P1(const IntTab& polys,
                              const DoubleTab& coord,
                              double x, double y, int le_poly, int i)
{
  int nb_som_elem = polys.dimension(1);
  //Distinction du calcul de la coordonnee barycentrique en fonction du type de l element
  //Cas Triangle
  if (nb_som_elem==3)
    return coord_barycentrique_P1_triangle(polys,coord,x,y,le_poly,i);
  //Cas Rectangle
  else if (nb_som_elem==4)
    return coord_barycentrique_P1_rectangle(polys,coord,x,y,le_poly,i);
  Cerr<<"The number of nodes by element " <<nb_som_elem<<" does not correspond to a treated situation in the coord_barycentrique_P1 function."<<finl;
  Process::exit();
  return 0.;
}


// Description:
//    Calcule la coordonnee barycentrique d'un point (x,y,z) par
//    rapport au sommet specifie d'un tetraedre ou d'un hexaedre (un element)
//    Ce calcul concerne un point 3D.
// Precondition:
// Parametre: IntTab& polys
//    Signification: tableau contenant les numeros des elements
//                   par rapport auxquels on veut calculer une
//                   coordonnee barycentrique.
//                   polys(i,0) est l'indice du sommet 0 de l'element
//                   i dans le tableau des coordonnees (coord).
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& coord
//    Signification: les coordonnees des sommets par auxquels on veut
//                   calculer les coordonnees barycentriques.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: double x
//    Signification: la premiere coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double y
//    Signification: la deuxieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: double z
//    Signification: la troisieme coordonnee cartesienne du point dont
//                   on veut calculer les coordonnees barycentriques
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: int le_poly
//    Signification: le numero de l'element (dans le tableau polys) par
//                   rapport auquel on calculera la coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entre
// Parametre: int i
//    Signification: le numero du sommet par rapport auquel on veut la
//                   coordonnee barycentrique.
//    Valeurs par defaut:
//    Contraintes: Tetraedre 0 <= i <= 3
//    Contraintes: Hexaedre  0 <= i <= 7
//    Acces: entree
// Retour: double
//    Signification: la coordonnee barycentrique du point (x,y,z) par rapport
//                   au sommet specifie (i) dans l'element specifie (le_poly)
//    Contraintes: 0 <= valeur <= 1
// Exception: un tetraedre n'a pas plus de 4 sommets
// Exception: un hexaedre n'a pas plus de 8 sommets
// Exception: erreur arithmetique, denominateur nul
// Exception: erreur de calcul, coordonnee barycentrique invalide
// Effets de bord:
// Postcondition:
// Do NOT inline (use call to coord_barycentrique_P1_tetraedre or coord_barycentrique_P1_hexaedre instead)
double coord_barycentrique_P1(const IntTab& polys,
                              const DoubleTab& coord,
                              double x, double y, double z,
                              int le_poly, int i)
{
  int nb_som_elem = polys.dimension(1);
  //Distinction du calcul de la coordonnee barycentrique en fonction du type de l element
  //Cas Tetraedre
  if (nb_som_elem==4)
    return coord_barycentrique_P1_tetraedre(polys, coord, x, y, z, le_poly, i);
  else if (nb_som_elem==8)
    return coord_barycentrique_P1_hexaedre(polys, coord, x, y, z, le_poly, i);
  Cerr<<"The number of nodes by element " <<nb_som_elem<<" does not correspond to a treated situation in the coord_barycentrique_P1 function."<<finl;
  Process::exit();
  return 0.;
}
