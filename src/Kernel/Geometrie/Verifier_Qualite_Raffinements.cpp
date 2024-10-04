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

#include <Verifier_Qualite_Raffinements.h>
#include <Domaine.h>
#include <TRUSTArray.h>
#include <SFichier.h>

Implemente_instanciable_32_64(Verifier_Qualite_Raffinements_32_64,"Verifier_Qualite_Raffinements",Interprete_geometrique_base_32_64<_T_>) ;
// XD verifier_qualite_raffinements interprete verifier_qualite_raffinements 0 not_set
// XD  attr domain_names vect_nom domain_names 0 not_set

template <typename _SIZE_>
Sortie& Verifier_Qualite_Raffinements_32_64<_SIZE_>::printOn(Sortie& os) const
{
  Interprete_geometrique_base_32_64<_SIZE_>::printOn(os);
  return os;
}

template <typename _SIZE_>
Entree& Verifier_Qualite_Raffinements_32_64<_SIZE_>::readOn(Entree& is)
{
  Interprete_geometrique_base_32_64<_SIZE_>::readOn(is);
  return is;
}

static void compute_triangle_side_lengths(const ArrOfDouble& x,const ArrOfDouble& y,ArrOfDouble& l)
{
  for (int i=0; i<3; ++i)
    {
      const double dx = x[(i+1)%3] - x[i];
      const double dy = y[(i+1)%3] - y[i];
      l[i] = sqrt(dx*dx + dy*dy);
    }
}

template <typename _SIZE_>
static void compute_cell_qualities_for_triangle(const Domaine_32_64<_SIZE_>& domain, ArrOfDouble_T<_SIZE_>& quality)
{
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  const IntTab_t&    cells = domain.les_elems();
  const DoubleTab_t& nodes = domain.les_sommets();

  const int_t nb_cells = cells.dimension(0);
  quality.resize_array(nb_cells);


  ArrOfDouble x(3);
  ArrOfDouble y(3);
  ArrOfDouble l(3);

  for (int_t cell=0; cell<nb_cells; ++cell)
    {

      for (int i=0; i<3; ++i)
        {
          x[i] = nodes(cells(cell,i),0);
          y[i] = nodes(cells(cell,i),1);
        }

      compute_triangle_side_lengths(x,y,l);

      const double p = l[0]+l[1]+l[2];
      const double r = sqrt(p*(p-l[0])*(p-l[1])*(p-l[2]))/p;
      const double h = (l[0] > l[1]) ? ( (l[0]>l[2]) ? l[0] : l[2]) : ( (l[1]>l[2]) ? l[1] : l[2] );

      quality[cell] = ( h / r );
    }
}

static double compute_tetrahedron_diameter(const ArrOfDouble& x,const ArrOfDouble& y,const ArrOfDouble& z)
{
  double diameter = 0.;
  for (int i=0; i<4; ++i)
    {
      for (int j=i+1; j<4; ++j)
        {
          const double dx = x[j]-x[i];
          const double dy = y[j]-y[i];
          const double dz = z[j]-z[i];
          const double l = dx*dx + dy*dy + dz*dz;
          diameter = (diameter < l) ? l : diameter;
        }
    }
  return sqrt(diameter);
}

static double compute_tetrahedron_volume(const ArrOfDouble& x,const ArrOfDouble& y,const ArrOfDouble& z)
{
  double volume = (x[1]-x[0]) * (y[2]-y[0]) * (z[3]-z[0]) +
                  (x[2]-x[0]) * (y[3]-y[0]) * (z[1]-z[0]) +
                  (x[3]-x[0]) * (y[1]-y[0]) * (z[2]-z[0]) -
                  (z[1]-z[0]) * (y[2]-y[0]) * (x[3]-x[0]) -
                  (x[2]-x[0]) * (z[3]-z[0]) * (y[1]-y[0]) -
                  (y[3]-y[0]) * (x[1]-x[0]) * (z[2]-z[0]);

  // GF est ce le lieu pour faire l'assert....
  if (volume<0.) volume=-volume;
  assert( volume > 0. );

  return (volume / 6.);
}


static double compute_tetrahedron_surface(const ArrOfDouble& x,const ArrOfDouble& y,const ArrOfDouble& z)
{
  ArrOfDouble u(3);
  ArrOfDouble v(3);

  double surface = 0.;
  u[0] = x[2] - x[1];
  v[0] = x[3] - x[1];
  u[1] = y[2] - y[1];
  v[1] = y[3] - y[1];
  u[2] = z[2] - z[1];
  v[2] = z[3] - z[1];

  surface += 0.5 * sqrt( ( ( (u[1]*v[2]) - (u[2]*v[1]) ) * ( (u[1]*v[2]) - (u[2]*v[1]) ) ) +
                         ( ( (u[2]*v[0]) - (u[0]*v[2]) ) * ( (u[2]*v[0]) - (u[0]*v[2]) ) ) +
                         ( ( (u[0]*v[1]) - (u[1]*v[0]) ) * ( (u[0]*v[1]) - (u[1]*v[0]) ) ) );

  u[0] = x[3] - x[2];
  v[0] = x[0] - x[2];
  u[1] = y[3] - y[2];
  v[1] = y[0] - y[2];
  u[2] = z[3] - z[2];
  v[2] = z[0] - z[2];

  surface += 0.5 * sqrt( ( ( (u[1]*v[2]) - (u[2]*v[1]) ) * ( (u[1]*v[2]) - (u[2]*v[1]) ) ) +
                         ( ( (u[2]*v[0]) - (u[0]*v[2]) ) * ( (u[2]*v[0]) - (u[0]*v[2]) ) ) +
                         ( ( (u[0]*v[1]) - (u[1]*v[0]) ) * ( (u[0]*v[1]) - (u[1]*v[0]) ) ) );

  u[0] = x[0] - x[3];
  v[0] = x[1] - x[3];
  u[1] = y[0] - y[3];
  v[1] = y[1] - y[3];
  u[2] = z[0] - z[3];
  v[2] = z[1] - z[3];

  surface += 0.5 * sqrt( ( ( (u[1]*v[2]) - (u[2]*v[1]) ) * ( (u[1]*v[2]) - (u[2]*v[1]) ) ) +
                         ( ( (u[2]*v[0]) - (u[0]*v[2]) ) * ( (u[2]*v[0]) - (u[0]*v[2]) ) ) +
                         ( ( (u[0]*v[1]) - (u[1]*v[0]) ) * ( (u[0]*v[1]) - (u[1]*v[0]) ) ) );


  u[0] = x[1] - x[0];
  v[0] = x[2] - x[0];
  u[1] = y[1] - y[0];
  v[1] = y[2] - y[0];
  u[2] = z[1] - z[0];
  v[2] = z[2] - z[0];

  surface += 0.5 * sqrt( ( ( (u[1]*v[2]) - (u[2]*v[1]) ) * ( (u[1]*v[2]) - (u[2]*v[1]) ) ) +
                         ( ( (u[2]*v[0]) - (u[0]*v[2]) ) * ( (u[2]*v[0]) - (u[0]*v[2]) ) ) +
                         ( ( (u[0]*v[1]) - (u[1]*v[0]) ) * ( (u[0]*v[1]) - (u[1]*v[0]) ) ) );

  return surface;
}

template <typename _SIZE_>
static void compute_cell_qualities_for_tetrahedron(const Domaine_32_64<_SIZE_>& domain, ArrOfDouble_T<_SIZE_>& quality)
{
  using int_t = _SIZE_;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  const IntTab_t&    cells = domain.les_elems();
  const DoubleTab_t& nodes = domain.les_sommets();

  const int_t nb_cells = cells.dimension(0);
  quality.resize_array(nb_cells);

  ArrOfDouble x(4);
  ArrOfDouble y(4);
  ArrOfDouble z(4);

  for (int_t cell=0; cell<nb_cells; ++cell)
    {

      for (int i=0; i<4; ++i)
        {
          x[i] = nodes(cells(cell,i),0);
          y[i] = nodes(cells(cell,i),1);
          z[i] = nodes(cells(cell,i),2);
        }

      const double h = compute_tetrahedron_diameter(x,y,z);
      const double v = compute_tetrahedron_volume(x,y,z);
      const double s = compute_tetrahedron_surface(x,y,z);

      quality[cell] = ( (h*s) / v );
    }
}

template <typename _SIZE_>
static void compute_cell_qualities(const Domaine_32_64<_SIZE_>& domain, ArrOfDouble_T<_SIZE_>& quality)
{
  const Nom& cell_type = domain.type_elem()->que_suis_je();

  Motcles understood_keywords(2);
  understood_keywords[0] = "Triangle";
  understood_keywords[1] = "Tetraedre";

  int rank = understood_keywords.search(cell_type);
  switch(rank)
    {
    case 0  :
      compute_cell_qualities_for_triangle(domain,quality);
      break;
    case 1  :
      compute_cell_qualities_for_tetrahedron(domain,quality);
      break;
    default :
      Cerr << "Error in Verifier_Qualite_Raffinements.cpp 'compute_cell_qualities()'" << finl;
      Cerr << "  Unknown cell type : " << cell_type << finl;
      Cerr << "  Verifier_Qualite_Raffinements can only check Triangles and Tetrahedrons" << finl;
      Process::exit();
    }
}

template <typename _SIZE_>
void Verifier_Qualite_Raffinements_32_64<_SIZE_>::verifier_qualite_raffinements()
{
  const int nb_domains = this->domaines().size() ;
  assert( nb_domains > 1 );

  for (int i=0; i<nb_domains; ++i)
    {
      ArrOfDouble_t qualities;
      compute_cell_qualities(this->domaine(i),qualities);

      Nom filename("quality_");
      filename += Nom(i);
      filename += Nom(".gnu");

      SFichier os(filename);
      const int_t nb_cells = qualities.size_array();
      for (int_t cell=0; cell<nb_cells; ++cell)
        {
          os << qualities[cell] << finl;
        }
      os.close();
    }
}

template <typename _SIZE_>
Entree& Verifier_Qualite_Raffinements_32_64<_SIZE_>::interpreter_(Entree& is)
{
  int nb_refinements;
  is >> nb_refinements;

  for (int i=0; i<nb_refinements; ++i)
    {
      this->associer_domaine(is);
    }
  verifier_qualite_raffinements();
  return is;
}

template class Verifier_Qualite_Raffinements_32_64<int>;
#if INT_is_64_ == 2
template class Verifier_Qualite_Raffinements_32_64<trustIdType>;
#endif
