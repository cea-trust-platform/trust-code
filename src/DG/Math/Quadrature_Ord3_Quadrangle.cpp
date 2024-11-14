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

#include <Quadrature_Ord3_Quadrangle.h>
/****************************************************************/
/* Formule de quadrature 2D : Formule Gauss-Lobatto Aide-mémoire Éléments finis Ern p.220  */
/* Formule de quadrature 1D : Formule Gauss-Lobatto Aide-mémoire Éléments finis Ern p.220       */
/****************************************************************/
namespace
{
static constexpr double WEIGHTS[9] = {1./36., 1./36., 1./36., 1./36., 1./9., 1./9., 1./9., 1./9., 4./9.};
static constexpr double WEIGHTS_FACETS[3] = {1./6., 1./6., 2./3.};
static constexpr double LAMBDA[9][4] =
{
  {1.,0.,0.,0.},
  {0.,1.,0.,0.},
  {0.,0.,1.,0.},
  {0.,0.,0.,1.},
  {1./2.,1./2.,0.,0.},
  {0.,1./2.,1./2.,0.},
  {0.,0.,1./2.,1./2.},
  {1./2.,0.,0.,1./2.},
  {1./4.,1./4.,1./4.,1./4.}
}; // Barycentric coordinates coefficients of integration points in elem */
static constexpr double LAMBDA_FACETS[3][2] =
{
  {1., 0.},
  {0.,1.},
  {1. / 2., 1. / 2.}
}; // Barycentric coordinates coefficients of integration points on facets */
}


void Quadrature_Ord3_Quadrangle::compute_integ_points()
{
  assert(Objet_U::dimension == 2); // no quadrangle in 3D!

  const IntTab& vert_elems = dom_->domaine().les_elems();
  int nb_elem_tot = dom_->nb_elem_tot();
  int ndim = Objet_U::dimension;
  DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  // We ensure that sum(weights)=1 and sum(Lambda[i])=1
  DoubleTab lambda(nb_pts_integ_, ndim + 2);  // ndim+2 = number of vertices

  integ_points_.resize(nb_elem_tot*nb_pts_integ_, ndim);
  weights_.resize(nb_pts_integ_);

  weights_[nb_pts_integ_-1] = 1;
  for (int pts = 0; pts < nb_pts_integ_; pts++)
    {
      if (pts < nb_pts_integ_ - 1)
        {
          weights_(pts) = ::WEIGHTS[pts];
          weights_(nb_pts_integ_-1) -= weights_(pts);
        }
      lambda(pts, 0) = ::LAMBDA[pts][0];
      lambda(pts, 1) = ::LAMBDA[pts][1];
      lambda(pts, 2) = ::LAMBDA[pts][2];
      lambda(pts, 3) = 1 -lambda(pts, 2) - lambda(pts, 1) - lambda(pts, 0);
    }

  for (int e = 0; e < nb_elem_tot; e++)
    {
      for (int pts = 0; pts < nb_pts_integ_; pts++)
        {
          for (int dim = 0; dim < ndim; dim++)
            {
//              integ_points_(e*nb_pts_integ + pts, dim) = 0.;
              for (int loc_vert = 0; loc_vert < ndim + 2; loc_vert++)  // ndim+2 = number of vertices
                integ_points_(e*nb_pts_integ_ + pts, dim) += xs(vert_elems(e, loc_vert), dim) * lambda(pts, loc_vert);
            }
        }
    }
}

void Quadrature_Ord3_Quadrangle::compute_integ_points_on_facet()
{

  assert(Objet_U::dimension == 2); // no quadrangle in 3D!

  int nb_faces = dom_->nb_faces();
  int ndim = Objet_U::dimension;
  DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  IntTab& face_sommets = dom_->face_sommets();

  integ_points_facets_.resize(nb_faces, nb_pts_integ_facets_, Objet_U::dimension); // one point per facets, 2D -> 1*2 = 2 columns
  weights_facets_.resize(nb_pts_integ_facets_);

  // We ensure that sum(weights)=1 and sum(Lambda[i])=1
  DoubleTab lambda_facets(nb_pts_integ_facets_, ndim);
  weights_facets_[nb_pts_integ_facets_-1] = 1;
  for (int pts = 0; pts < nb_pts_integ_facets_; pts++)
    {
      if (pts < nb_pts_integ_facets_ - 1)
        {
          weights_facets_(pts) = ::WEIGHTS_FACETS[pts];
          weights_facets_(nb_pts_integ_facets_-1) -= weights_facets_(pts);
        }
      lambda_facets(pts, 0) = ::LAMBDA_FACETS[pts][0];
      lambda_facets(pts, 1) = 1 - lambda_facets(pts, 0);
    }

  for (int f = 0; f < nb_faces; f++)
    {
      for (int pts = 0; pts < nb_pts_integ_facets_; pts++)
        {
          for (int dim = 0; dim < ndim; dim++)
            {
              integ_points_facets_(f, pts, dim) = 0.;
              for (int loc_vert = 0; loc_vert < 2; loc_vert++)
                integ_points_facets_(f, pts, dim) += xs(face_sommets(f, loc_vert), dim) * lambda_facets(pts, loc_vert);
            }
        }
    }
}
