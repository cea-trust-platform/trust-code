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

#include <Verifier_Simplexes.h>
#include <Domaine.h>
#include <Motcle.h>

Implemente_instanciable(Verifier_Simplexes,"Verifier_Simplexes",Interprete_geometrique_base) ;

Sortie& Verifier_Simplexes::printOn(Sortie& os) const
{
  Interprete_geometrique_base::printOn(os);
  return os;
}

Entree& Verifier_Simplexes::readOn(Entree& is)
{
  Interprete_geometrique_base::readOn(is);
  return is;
}

static void check_internal_diagonal_for_triangle(Domaine& domain)
{
  // nothing to do
}

static void check_internal_diagonal_for_tetrahedron(Domaine& domain)
{
  const DoubleTab& nodes = domain.les_sommets();
  IntTab&           cells = domain.zone(0).les_elems();

  ArrOfDouble lengths(3);

  const int nb_cells = cells.dimension(0);
  int err_count = 0;
  for (int cell=0; cell<nb_cells; ++cell)
    {
      const int s0 = cells(cell,0);
      const int s1 = cells(cell,1);
      const int s2 = cells(cell,2);
      const int s3 = cells(cell,3);

      const double x0 = nodes(s0,0);
      const double x1 = nodes(s1,0);
      const double x2 = nodes(s2,0);
      const double x3 = nodes(s3,0);

      const double y0 = nodes(s0,1);
      const double y1 = nodes(s1,1);
      const double y2 = nodes(s2,1);
      const double y3 = nodes(s3,1);

      const double z0 = nodes(s0,2);
      const double z1 = nodes(s1,2);
      const double z2 = nodes(s2,2);
      const double z3 = nodes(s3,2);

      const double x01 = 0.5 * (x0+x1);
      const double x02 = 0.5 * (x0+x2);
      const double x03 = 0.5 * (x0+x3);
      const double x12 = 0.5 * (x1+x2);
      const double x13 = 0.5 * (x1+x3);
      const double x23 = 0.5 * (x2+x3);

      const double y01 = 0.5 * (y0+y1);
      const double y02 = 0.5 * (y0+y2);
      const double y03 = 0.5 * (y0+y3);
      const double y12 = 0.5 * (y1+y2);
      const double y13 = 0.5 * (y1+y3);
      const double y23 = 0.5 * (y2+y3);

      const double z01 = 0.5 * (z0+z1);
      const double z02 = 0.5 * (z0+z2);
      const double z03 = 0.5 * (z0+z3);
      const double z12 = 0.5 * (z1+z2);
      const double z13 = 0.5 * (z1+z3);
      const double z23 = 0.5 * (z2+z3);

      lengths[0] = (x23-x01)*(x23-x01) + (y23-y01)*(y23-y01) + (z23-z01)*(z23-z01);
      lengths[1] = (x13-x02)*(x13-x02) + (y13-y02)*(y13-y02) + (z13-z02)*(z13-z02);
      lengths[2] = (x12-x03)*(x12-x03) + (y12-y03)*(y12-y03) + (z12-z03)*(z12-z03);

      const int id = imin_array(lengths);

      if (id == 0)
        {
          if (lengths[0] <  lengths[1])
            {
              if (err_count == 0)
                {
                  Cerr << "Error in Verifier_Simplexes.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
                  Cerr << " The following cells have a bad internal diagonal :" << finl;
                }
              Cerr << cell << finl;
              ++err_count;
            }
        }
      else if (id == 1)
        {
          // rien a faire
        }
      else if (id == 2)
        {
          if (lengths[2] < lengths[1])
            {
              if (err_count == 0)
                {
                  Cerr << "Error in Verifier_Simplexes.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
                  Cerr << " The following cells have a bad internal diagonal :" << finl;
                }
              Cerr << cell << finl;
              ++err_count;
            }
        }
      else
        {
          if (err_count == 0)
            {
              Cerr << "Error in Verifier_Simplexes.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
              Cerr << " The following cells have a bad internal diagonal :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }

  if (err_count>0)
    {
      Process::exit();
    }
}

static void check_internal_diagonal(Domaine& domain)
{
  const Nom& cell_type = domain.zone(0).type_elem().valeur().que_suis_je();

  Motcles understood_keywords(2);
  understood_keywords[0] = "Triangle";
  understood_keywords[1] = "Tetraedre";

  int rank = understood_keywords.search(cell_type);
  switch(rank)
    {
    case 0  :
      check_internal_diagonal_for_triangle(domain);
      break;
    case 1  :
      check_internal_diagonal_for_tetrahedron(domain);
      break;
    default :
      Cerr << "Error in Verifier_Simplexes.cpp 'check_internal_diagonal()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Verifier_Simplexes can only check Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

static void check_positive_volumes_for_triangle(Domaine& domain)
{
  const DoubleTab& nodes = domain.les_sommets();
  IntTab&           cells = domain.zone(0).les_elems();

  const int nb_cells = cells.dimension(0);
  int err_count = 0;
  for (int cell=0; cell<nb_cells; ++cell)
    {
      const int s0 = cells(cell,0);
      const int s1 = cells(cell,1);
      const int s2 = cells(cell,2);

      const double x0 = nodes(s0,0);
      const double x1 = nodes(s1,0);
      const double x2 = nodes(s2,0);

      const double y0 = nodes(s0,1);
      const double y1 = nodes(s1,1);
      const double y2 = nodes(s2,1);

      const double area = 0.5 * ((x1-x0)*(y2-y0) - (x2-x0)*(y1-y0));
      if ( area < 0.)
        {
          if (err_count == 0)
            {
              Cerr << "Error in Verifier_Simplexes.cpp 'check_positive_volumes_for_triangle()'" << finl;
              Cerr << " The following cells have a negative orientation :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }
  if (err_count>0)
    {
      Process::exit();
    }
}

static void check_positive_volumes_for_tetrahedron(Domaine& domain)
{
  const DoubleTab& nodes = domain.les_sommets();
  IntTab&           cells = domain.zone(0).les_elems();

  const int nb_cells = cells.dimension(0);
  int err_count = 0;
  for (int cell=0; cell<nb_cells; ++cell)
    {
      const int s0 = cells(cell,0);
      const int s1 = cells(cell,1);
      const int s2 = cells(cell,2);
      const int s3 = cells(cell,3);

      const double x0 = nodes(s0,0);
      const double x1 = nodes(s1,0);
      const double x2 = nodes(s2,0);
      const double x3 = nodes(s3,0);

      const double y0 = nodes(s0,1);
      const double y1 = nodes(s1,1);
      const double y2 = nodes(s2,1);
      const double y3 = nodes(s3,1);

      const double z0 = nodes(s0,2);
      const double z1 = nodes(s1,2);
      const double z2 = nodes(s2,2);
      const double z3 = nodes(s3,2);

      const double volume = (1./6.) * ( (x1-x0) * (y2-y0) * (z3-z0) +
                                        (x2-x0) * (y3-y0) * (z1-z0) +
                                        (x3-x0) * (y1-y0) * (z2-z0) -
                                        (z1-z0) * (y2-y0) * (x3-x0) -
                                        (x2-x0) * (z3-z0) * (y1-y0) -
                                        (y3-y0) * (x1-x0) * (z2-z0)   );

      if (volume < 0.)
        {
          if (err_count == 0)
            {
              Cerr << "Error in Verifier_Simplexes.cpp 'check_positive_volumes_for_tetrahedron()'" << finl;
              Cerr << " The following cells have a negative orientation :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }
  if (err_count>0)
    {
      Process::exit();
    }
}


static void check_positive_volumes(Domaine& domain)
{
  const Nom& cell_type = domain.zone(0).type_elem().valeur().que_suis_je();

  Motcles understood_keywords(2);
  understood_keywords[0] = "Triangle";
  understood_keywords[1] = "Tetraedre";

  int rank = understood_keywords.search(cell_type);
  switch(rank)
    {
    case 0  :
      check_positive_volumes_for_triangle(domain);
      break;
    case 1  :
      check_positive_volumes_for_tetrahedron(domain);
      break;
    default :
      Cerr << "Error in Verifier_Simplexes.cpp 'check_positive_volumes()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Verifier_Simplexes can only check Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

void Verifier_Simplexes::check_domain(Domaine& domain)
{
  check_internal_diagonal(domain);
  check_positive_volumes(domain);
}

Entree& Verifier_Simplexes::interpreter_(Entree& is)
{
  associer_domaine(is);

  Domaine& domain = domaine();
  check_domain(domain);

  return is;
}

