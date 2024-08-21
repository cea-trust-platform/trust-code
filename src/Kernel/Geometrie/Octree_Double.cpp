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

#include <Octree_Double.h>

template <typename _SIZE_>
void Octree_Double_32_64<_SIZE_>::reset()
{
  dim_ = 0;
  octree_int_.reset();
  origin_.reset();
  factor_.reset();
}

/*! @brief cherche les elements ou les points contenus dans l'octree_floor qui contient le point (x,y,z).
 *
 * Renvoie le nombre n de ces elements.
 *   Les indices des elements sont dans floor_elements()[index+i] pour 0 <= i < n
 *
 */
template <typename _SIZE_>
typename Octree_Double_32_64<_SIZE_>::int_t Octree_Double_32_64<_SIZE_>::search_elements(double x, double y, double z, int_t& index) const
{
  if (dim_ == 0)
    return 0; // octree vide
  int ix = 0, iy = 0, iz = 0;
  int ok = integer_position(x, 0, ix)
           && integer_position(y, 1, iy)
           && integer_position(z, 2, iz);
  return ok ? octree_int_.search_elements(ix, iy, iz, index) : 0;
}

/*! @brief construit un octree contenant les points de coordonnees coords.
 *
 * Si include_virtual=1, on stocke coords.dimension_tot(0) elements, sinon on en
 *   stocke coords.dimension(0)
 *   Si epsilon = 0, on construit un octree de points de taille nulle (chaque point
 *    se trouve dans un seul octree_floor)
 *   Sinon, on construit un octree d'elements cubiques centres sur les coords, de demie-largeur epsilon.
 *    Un point peut alors se trouver dans plusieurs octree_floor.
 *
 */
template <typename _SIZE_>
void Octree_Double_32_64<_SIZE_>::build_nodes(const DoubleTab_t& coords, const bool include_virtual, const double epsilon)
{
  octree_int_.reset();
  compute_origin_factors(coords, epsilon, include_virtual);
  const int_t nb_som = include_virtual ? coords.dimension_tot(0) : coords.dimension(0);
  if (nb_som == 0)
    return; // octree vide
  const int dim = coords.dimension_int(1);
  if (epsilon < 0.)
    {
      Cerr << "Internal error in Octree_Double_32_64<_SIZE_>::build_nodes: negative epsilon" << finl;
      Process::exit();
    }
  bool have_epsilon = (epsilon != 0.);
  IntTab_t elements_boxes;
  elements_boxes.resize(nb_som, have_epsilon ? (dim*2) : dim, RESIZE_OPTIONS::NOCOPY_NOINIT);

  for (int_t i = 0; i < nb_som; i++)
    for (int j = 0; j < dim; j++)
      {
        int pos1 = 0;
        const double x0 = coords(i, j);
        double x = x0 - epsilon;
        if (!integer_position(x, j, pos1))
          {
            Cerr << "Fatal error in octree : integer position outside octree" << finl;
            Process::exit();
          }
        elements_boxes(i, j) = pos1;
        if (have_epsilon)
          {
            pos1 = 0;
            double xbis = x0 + epsilon;
            if (!integer_position(xbis, j, pos1))
              {
                Cerr << "Fatal error in octree : integer position outside octree" << finl;
                Process::exit();
              }
            elements_boxes(i, dim+j) = pos1;
          }
      }
  octree_int_.build(dim, elements_boxes);
}

/*! @brief cherche tous les elements ou points ayant potentiellement une intersection non vide avec la boite donnee.
 *
 */
template <typename _SIZE_>
typename Octree_Double_32_64<_SIZE_>::int_t Octree_Double_32_64<_SIZE_>::search_elements_box(double xmin, double ymin, double zmin,
                                                                                             double xmax, double ymax, double zmax,
                                                                                             ArrOfInt_t& elements) const
{
  const int dim = dim_;
  if (dim == 0)
    {
      elements.resize_array(0);
      return 0;
    }
  int x0 = 0, x1 = 0, y0 = 0, y1 = 0, z0 = 0, z1 = 0;
  int ok = integer_position_clip(xmin, xmax, x0, x1, 0);
  if ((ok) && (dim >= 1))
    {
      ok = integer_position_clip(ymin, ymax, y0, y1, 1);
      if ((ok) && (dim >= 2))
        ok = integer_position_clip(zmin, zmax, z0, z1, 2);
    }
  if (ok)
    octree_int_.search_elements_box(x0, y0, z0, x1, y1, z1, elements);
  else
    elements.resize_array(0);
  return elements.size_array();
}

/*! @brief cherche tous les elements ou points ayant potentiellement une intersection non vide avec la boite donnee (centre + ou - radius dans chaque direction)
 *
 */
template <typename _SIZE_>
typename Octree_Double_32_64<_SIZE_>::int_t
Octree_Double_32_64<_SIZE_>::search_elements_box(const ArrOfDouble& center, const double radius,
                                                 ArrOfInt_t& elements) const
{
  int dim = center.size_array();
  double x = center[0];
  double y = (dim>=2) ? center[1] : 0.;
  double z = (dim>2) ? center[2] : 0.;
  int_t i = search_elements_box(x-radius, y-radius, z-radius,
                                x+radius, y+radius, z+radius,
                                elements);
  return i;
}

/*! @brief Methode hors classe Cherche parmi les sommets de la liste node_list ceux qui sont a une
 *
 *   distance inferieure a epsilon du point (x,y,z). node_list contient des indices de
 *   sommets dans le tableau coords. La liste des noeuds verifiant le critere est mise
 *   dans node_list. On renvoie l'indice dans le tableau coords du sommet le plus proche.
 *
 */
template <typename _SIZE_>
typename Octree_Double_32_64<_SIZE_>::int_t  Octree_Double_32_64<_SIZE_>::search_nodes_close_to(double x, double y, double z,
                                                                                                const DoubleTab_t& coords, ArrOfInt_t& node_list,
                                                                                                double epsilon)
{
  const int_t n = node_list.size_array();
  double eps2 = epsilon * epsilon;
  int_t count = 0;
  const int dim = coords.dimension_int(1);
  double dmin = eps2;
  int_t nearest = -1;
  for (int_t i = 0; i < n; i++)
    {
      const int_t som = node_list[i];
      double dx = x - coords(som, 0);
      double dy = (dim >= 2) ? y - coords(som, 1) : 0.;
      double dz = (dim >= 3) ? z - coords(som, 2) : 0.;
      double d2 = dx * dx + dy * dy + dz * dz;
      if (d2 < eps2)
        {
          node_list[count] = som;
          if (d2 < dmin)
            {
              dmin = d2;
              nearest = som;
            }
          count++;
        }
    }
  node_list.resize_array(count);
  return nearest;
}

/*! @brief Idem que search_nodes_close_to(double x, double y, double z, .
 *
 * ..)
 *
 */
template <typename _SIZE_>
typename Octree_Double_32_64<_SIZE_>::int_t  Octree_Double_32_64<_SIZE_>::search_nodes_close_to(const ArrOfDouble& point,
                                                                                                const DoubleTab_t& coords, ArrOfInt_t& node_list,
                                                                                                double epsilon)
{
  int dim = point.size_array();
  double x = point[0];
  double y = (dim>=2) ? point[1] : 0.;
  double z = (dim>2) ? point[2] : 0.;
  int_t i = search_nodes_close_to(x, y, z, coords, node_list, epsilon);
  return i;
}

template class Octree_Double_32_64<int>;
#if INT_is_64_ == 2
template class Octree_Double_32_64<trustIdType>;
#endif

