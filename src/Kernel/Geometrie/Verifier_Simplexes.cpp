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

#include <Verifier_Simplexes.h>
#include <Domaine.h>
#include <Motcle.h>

Implemente_instanciable_32_64(Verifier_Simplexes_32_64,"Verifier_Simplexes",Interprete_geometrique_base_32_64<_T_>) ;
// XD verifier_simplexes interprete verifier_simplexes -1 Keyword to raffine a simplexes
// XD  attr domain_name ref_domaine domain_name 0 Name of domain.

template <typename _SIZE_>
Sortie& Verifier_Simplexes_32_64<_SIZE_>::printOn(Sortie& os) const
{
  Interprete_geometrique_base_32_64<_SIZE_>::printOn(os);
  return os;
}

template <typename _SIZE_>
Entree& Verifier_Simplexes_32_64<_SIZE_>::readOn(Entree& is)
{
  Interprete_geometrique_base_32_64<_SIZE_>::readOn(is);
  return is;
}

// Helper implementation class that will only live in this file:
template <typename _SIZE_>
class Impl_32_64
{
public:
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;

  static void check_internal_diagonal_for_triangle(Domaine_t& domain) { }
  static void check_internal_diagonal_for_tetrahedron(Domaine_t& domain);
  static void check_internal_diagonal(Domaine_t& domain);
  static void check_positive_volumes_for_triangle(Domaine_t& domain);
  static void check_positive_volumes_for_tetrahedron(Domaine_t& domain);
  static void check_positive_volumes(Domaine_t& domain);
};


template <typename _SIZE_>
void Impl_32_64<_SIZE_>::check_internal_diagonal_for_tetrahedron(Domaine_t& domain)
{
  const DoubleTab_t& nodes = domain.les_sommets();
  IntTab_t& cells = domain.les_elems();

  ArrOfDouble lengths(3);

  const int_t nb_cells = cells.dimension(0);
  int_t err_count = 0;
  for (int_t cell=0; cell<nb_cells; ++cell)
    {
      const int_t s0 = cells(cell,0),
                  s1 = cells(cell,1),
                  s2 = cells(cell,2),
                  s3 = cells(cell,3);

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

      const int_t id = imin_array(lengths);

      if (id == 0)
        {
          if (lengths[0] <  lengths[1])
            {
              if (err_count == 0)
                {
                  Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
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
                  Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
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
              Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_internal_diagonal_for_tetrahedron()'" << finl;
              Cerr << " The following cells have a bad internal diagonal :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }

  if (err_count>0)
    Process::exit();
}

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::check_internal_diagonal(Domaine_t& domain)
{
  const Nom& cell_type = domain.type_elem()->que_suis_je();

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
      Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_internal_diagonal()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Verifier_Simplexes_32_64 can only check Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::check_positive_volumes_for_triangle(Domaine_t& domain)
{
  const DoubleTab_t& nodes = domain.les_sommets();
  IntTab_t& cells = domain.les_elems();

  const int_t nb_cells = cells.dimension(0);
  int_t err_count = 0;
  for (int_t cell=0; cell<nb_cells; ++cell)
    {
      const int_t s0 = cells(cell,0),
                  s1 = cells(cell,1),
                  s2 = cells(cell,2);

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
              Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_positive_volumes_for_triangle()'" << finl;
              Cerr << " The following cells have a negative orientation :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }
  if (err_count>0)
    Process::exit();
}

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::check_positive_volumes_for_tetrahedron(Domaine_t& domain)
{
  const DoubleTab_t& nodes = domain.les_sommets();
  IntTab_t& cells = domain.les_elems();

  const int_t nb_cells = cells.dimension(0);
  int_t err_count = 0;
  for (int_t cell=0; cell<nb_cells; ++cell)
    {
      const int_t s0 = cells(cell,0),
                  s1 = cells(cell,1),
                  s2 = cells(cell,2),
                  s3 = cells(cell,3);

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
              Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_positive_volumes_for_tetrahedron()'" << finl;
              Cerr << " The following cells have a negative orientation :" << finl;
            }
          Cerr << cell << finl;
          ++err_count;
        }
    }
  if (err_count>0)
    Process::exit();
}

template <typename _SIZE_>
void Impl_32_64<_SIZE_>::check_positive_volumes(Domaine_t& domain)
{
  const Nom& cell_type = domain.type_elem()->que_suis_je();

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
      Cerr << "Error in Verifier_Simplexes_32_64.cpp 'check_positive_volumes()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Verifier_Simplexes_32_64 can only check Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

template <typename _SIZE_>
void Verifier_Simplexes_32_64<_SIZE_>::check_domain(Domaine_t& domain)
{
  using Impl_ = Impl_32_64<_SIZE_>;

  Impl_::check_internal_diagonal(domain);
  Impl_::check_positive_volumes(domain);
}

template <typename _SIZE_>
Entree& Verifier_Simplexes_32_64<_SIZE_>::interpreter_(Entree& is)
{
  this->associer_domaine(is);

  Domaine_t& domain = this->domaine();
  check_domain(domain);

  return is;
}


template class Verifier_Simplexes_32_64<int>;
#if INT_is_64_ == 2
template class Verifier_Simplexes_32_64<trustIdType>;
#endif

