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
#include <Quadrature_Ord2_Triangle.h>

/****************************************************************/
/* Formule de quadrature 2D : Ern, Finite Elements II, 2021, p 71  */
/* Formule de quadrature 1D : Formule des Trapèzes                 */
/****************************************************************/
namespace
{
static constexpr double WEIGHTS[3] = {1 / 3, 1 / 3, 1 / 3};
static constexpr double WEIGHTS_FACETS[2] = {0.5, 0.5};
/*static constexpr double LAMBDA[3][3] = {
      {1/2, 1/2, 0},
      {0, 1/2, 1/2},
      {1/2, 0, 1/2}
  }; // Barycentric coordinates coefficients of integration points in elem */
/*static constexpr double LAMBDA_FACETS[2][2] = {
      {1, 0},
      {0, 1}
  }; // Barycentric coordinates coefficients of integration points on facets */
static constexpr int ORDER = 2;
}

void Quadrature_Ord2_Triangle::compute_integ_points()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  const IntTab& elems = dom_->domaine().les_elems();
  int nb_elem = elems.dimension(0);
  int nb_pts_integ = 3;
  int ndim = Objet_U::dimension;
  const DoubleTab& xv = dom_->xv(); // facets barycentre
  const IntTab& elem_faces = dom_->elem_faces();

  integ_points_.resize(nb_elem, nb_pts_integ, ndim); // three point per element, 2D -> 1*2 = 2 columns
  weights_.resize(nb_pts_integ);

  for (int e = 0; e < nb_elem; e++)
    {
      for (int pts = 0; pts < nb_pts_integ; pts++)
        {
          for (int dim = 0; dim < ndim; dim++)
            integ_points_(e, pts, dim) = xv(elem_faces(e, pts), dim);
        }
    }
  // We ensure that sum(weights)=1;
  weights_[nb_pts_integ-1] = 1;
  for (int pts = 0; pts < nb_pts_integ - 1; pts++)
    {
      weights_(pts) = ::WEIGHTS[pts];
      weights_(nb_pts_integ-1) -= weights_(pts);
    }
}

void Quadrature_Ord2_Triangle::compute_integ_points_on_facet()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!


  int nb_faces = dom_->nb_faces(); // Q: Cette ligne renvoie t-elle bien le nombre de faces ? (nb_faces_tot)
  DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  IntTab& face_sommets = dom_->face_sommets();
  int nb_pts_integ = 2;

  integ_points_facets_.resize(nb_faces, nb_pts_integ, Objet_U::dimension); // one point per facets, 2D -> 1*2 = 2 columns
  weights_facets_.resize(nb_pts_integ);

  for (int pts = 0; pts < nb_pts_integ; pts++)
    {
      for (int f = 0; f < nb_faces; f++) // TODO : Utiliser plutôt la fonction copy
        {
          for (int dim = 0; dim < Objet_U::dimension; dim++)
            integ_points_facets_(f, pts, dim) = xs(face_sommets(f, pts), dim);
        }
    }
  weights_facets_[nb_pts_integ-1] = 1;
  for (int pts = 0; pts < nb_pts_integ - 1; pts++)
    {
      weights_facets_[pts] = ::WEIGHTS_FACETS[pts];
      weights_facets_[nb_pts_integ-1] -= weights_facets_[pts];
    }
}
