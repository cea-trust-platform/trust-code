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
// File:        Rectify_Mesh.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Rectify_Mesh.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Synonyme_info.h>


Implemente_instanciable(Rectify_Mesh,"Rectify_Mesh",Interprete_geometrique_base) ;
Add_synonym(Rectify_Mesh,"Orienter_Simplexes");

Sortie& Rectify_Mesh::printOn(Sortie& os) const
{
  Interprete::printOn(os);
  return os;
}

Entree& Rectify_Mesh::readOn(Entree& is)
{
  Interprete::readOn(is);
  return is;
}

Entree& Rectify_Mesh::interpreter_(Entree& is)
{
  associer_domaine(is);
  apply();
  return is;
}

void Rectify_Mesh::apply(void)
{
  Cerr << "Rectifying domain " << domaine().le_nom() << finl;

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

  Cerr << "Rectification... OK" << finl;
}

void Rectify_Mesh::check_dimension(void) const
{
  if ( ! ( (Objet_U::dimension == 2) || (Objet_U::dimension == 3) ) )
    {
      Cerr << "Error in 'Rectify_Mesh::check_dimension()':" << finl;
      Cerr << "  Invalid dimension: " << Objet_U::dimension << finl;
      Cerr << "  Rectify_Mesh can only deal 2D or 3D domains" << finl;
      Process::exit();
    }
}

void Rectify_Mesh::check_nb_zones(void) const
{
  if ( domaine().nb_zones() != 1 )
    {
      Cerr << "Error in 'Rectify_Mesh::check_nb_zones()':" << finl;
      Cerr << "  Invalid number of zones: " << domaine().nb_zones() << finl;
      Cerr << "  Rectify_Mesh cannot deal with domains having more than one zone" << finl;
      Process::exit();
    }
}

void Rectify_Mesh::check_cell_type(void) const
{
  const Nom& cell_type = domaine().zone(0).type_elem().valeur().que_suis_je();

  if ( ! ( (cell_type == Motcle("Triangle")) || (cell_type == Motcle("Tetraedre")) ) )
    {
      Cerr << "Error in 'Rectify_Mesh::check_cell_type()':" << finl;
      Cerr << "  Invalid cell type: " << cell_type << finl;
      Cerr << "  Rectify_Mesh can only deal with triangles and tetrahedrons" << finl;
      Process::exit();
    }
}

void Rectify_Mesh::apply_2D(void)
{
  Domaine& domain = domaine();
  Scatter::uninit_sequential_domain(domain);

  assert( domain.zone(0).type_elem().valeur().que_suis_je() == Motcle("Triangle") );

  check_cell_orientation_2D();

  Scatter::init_sequential_domain(domain);
}


void Rectify_Mesh::apply_3D(void)
{
  Domaine& domain = domaine();
  Scatter::uninit_sequential_domain(domain);

  assert( domain.zone(0).type_elem().valeur().que_suis_je() == Motcle("Tetraedre") );

  check_cell_orientation_3D();
  check_cell_enumeration_3D();

  Scatter::init_sequential_domain(domain);
}

void Rectify_Mesh::check_cell_orientation_2D(void)
{
  const DoubleTab& nodes = domaine().les_sommets();
  IntTab&          cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);

  int nb_swaps = 0;

  for (int c=0; c<nb_cells; ++c)
    {
      const int n0 = cells(c,0);
      const int n1 = cells(c,1);
      const int n2 = cells(c,2);

      const double x0 = nodes(n0,0);
      const double x1 = nodes(n1,0);
      const double x2 = nodes(n2,0);

      const double y0 = nodes(n0,1);
      const double y1 = nodes(n1,1);
      const double y2 = nodes(n2,1);

      const double area = (x1-x0)*(y2-y0) - (x2-x0)*(y1-y0);
      if ( area < 0.)
        {
          swap_nodes(cells,c,1,2);
          ++nb_swaps;
        }
    }

  Cerr << "************************************************" << finl;
  Cerr << "Rectify_Mesh::check_cell_orientation_2D report :" << finl;
  Cerr << "  " << nb_swaps << " swap" << ((nb_swaps ==1) ? "" : "s") << " were performed" << finl;
  Cerr << "************************************************" << finl;
}

void Rectify_Mesh::check_cell_orientation_3D(void)
{
  const DoubleTab& nodes = domaine().les_sommets();
  IntTab&          cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);

  int nb_swaps = 0;

  for (int c=0; c<nb_cells; ++c)
    {
      const int n0 = cells(c,0);
      const int n1 = cells(c,1);
      const int n2 = cells(c,2);
      const int n3 = cells(c,3);

      const double x0 = nodes(n0,0);
      const double x1 = nodes(n1,0);
      const double x2 = nodes(n2,0);
      const double x3 = nodes(n3,0);

      const double y0 = nodes(n0,1);
      const double y1 = nodes(n1,1);
      const double y2 = nodes(n2,1);
      const double y3 = nodes(n3,1);

      const double z0 = nodes(n0,2);
      const double z1 = nodes(n1,2);
      const double z2 = nodes(n2,2);
      const double z3 = nodes(n3,2);

      const double volume =   (x1-x0) * (y2-y0) * (z3-z0)
                              + (x2-x0) * (y3-y0) * (z1-z0)
                              + (x3-x0) * (y1-y0) * (z2-z0)
                              - (z1-z0) * (y2-y0) * (x3-x0)
                              - (x2-x0) * (z3-z0) * (y1-y0)
                              - (y3-y0) * (x1-x0) * (z2-z0);
      if (volume < 0.)
        {
          swap_nodes(cells,c,2,3);
          ++nb_swaps;
        }
    }

  Cerr << "************************************************" << finl;
  Cerr << "Rectify_Mesh::check_cell_orientation_3D report :" << finl;
  Cerr << "  " << nb_swaps << " swap" << ((nb_swaps ==1) ? "" : "s") << " were performed" << finl;
  Cerr << "************************************************" << finl;

}

void Rectify_Mesh::check_cell_enumeration_3D(void)
{
  const DoubleTab& nodes = domaine().les_sommets();
  IntTab&          cells = domaine().zone(0).les_elems();

  const int nb_cells = cells.dimension(0);

  ArrOfDouble lengths(3);

  int nb_swaps = 0;

  for (int c=0; c<nb_cells; ++c)
    {
      const int n0 = cells(c,0);
      const int n1 = cells(c,1);
      const int n2 = cells(c,2);
      const int n3 = cells(c,3);

      const double x0 = nodes(n0,0);
      const double x1 = nodes(n1,0);
      const double x2 = nodes(n2,0);
      const double x3 = nodes(n3,0);

      const double y0 = nodes(n0,1);
      const double y1 = nodes(n1,1);
      const double y2 = nodes(n2,1);
      const double y3 = nodes(n3,1);

      const double z0 = nodes(n0,2);
      const double z1 = nodes(n1,2);
      const double z2 = nodes(n2,2);
      const double z3 = nodes(n3,2);

      lengths[0] =   ((x0+x1)-(x2+x3))*((x0+x1)-(x2+x3))
                     + ((y0+y1)-(y2+y3))*((y0+y1)-(y2+y3))
                     + ((z0+z1)-(z2+z3))*((z0+z1)-(z2+z3));

      lengths[1] =   ((x0+x2)-(x1+x3))*((x0+x2)-(x1+x3))
                     + ((y0+y2)-(y1+y3))*((y0+y2)-(y1+y3))
                     + ((z0+z2)-(z1+z3))*((z0+z2)-(z1+z3));

      lengths[2] =   ((x0+x3)-(x1+x2))*((x0+x3)-(x1+x2))
                     + ((y0+y3)-(y1+y2))*((y0+y3)-(y1+y2))
                     + ((z0+z3)-(z1+z2))*((z0+z3)-(z1+z2));

      const int id = imin_array(lengths);
      //  const int id = imax_array(lengths);

      if ( (id == 0) && (lengths[0] < lengths[1]) )
        {
          swap_nodes(cells,c,1,2);
          swap_nodes(cells,c,1,3);
          ++nb_swaps;
        }

      if (id == 1)
        {
          // nothing to do
        }

      if ( (id == 2) && (lengths[2] < lengths[1]) )
        {
          swap_nodes(cells,c,2,3);
          swap_nodes(cells,c,1,3);
          ++nb_swaps;
        }
    }

  Cerr << "************************************************" << finl;
  Cerr << "Rectify_Mesh::check_cell_enumeration_3D report :" << finl;
  Cerr << "  " << nb_swaps << " swap" << ((nb_swaps ==1) ? "" : "s") << " were performed" << finl;
  Cerr << "************************************************" << finl;

}

void Rectify_Mesh::swap_nodes(IntTab& cells, int cell, int node0, int node1)
{
  const int tmp  = cells(cell,node0);
  cells(cell,node0) = cells(cell,node1);
  cells(cell,node1) = tmp;
}
