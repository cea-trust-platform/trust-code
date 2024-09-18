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

#include <Quadrature_Ord5_Triangle.h>
/****************************************************************/
/* Formule de quadrature 2D : Hammer-Marlow-Stroud, Dautray-Lions, tome 6 p.781  */
/* Formule de quadrature 1D : Formule de Milne, Open Newton-Cotes formulas       */
/****************************************************************/
namespace
{

static constexpr int NB_PTS_INTEG = 7;
static constexpr int NB_PTS_INTEG_FACETS = 5;
static constexpr double WEIGHTS[7] = {0.225, 0.125939180544827, 0.125939180544827, 0.125939180544827, 0.132394152788506, 0.132394152788506, 0.132394152788506};
static constexpr double WEIGHTS_FACETS[5] = {7. / 90., 32. / 90., 12. / 90., 32. / 90., 7. / 90.};
static constexpr double LAMBDA[7][3] =
{
  {0.797426985353087, 0.101286507323456, 0.101286507323456},
  {0.101286507323456, 0.797426985353087, 0.101286507323456},
  {0.101286507323456, 0.101286507323456, 0.797426985353087},
  {0.059715871789770, 0.470142064105115, 0.470142064105115},
  {0.470142064105115, 0.059715871789770, 0.470142064105115},
  {0.470142064105115, 0.470142064105115, 0.059715871789770}
}; // Barycentric coordinates coefficients of integration points in elem */
static constexpr double LAMBDA_FACETS[5][2] =
{
  {1, 0},
  {3 / 4, 1 / 4},
  {1 / 2, 1 / 2},
  {1 / 4, 3 / 4},
  {0, 1}
}; // Barycentric coordinates coefficients of integration points on facets */
static constexpr int ORDER = 5;
}

Implemente_instanciable(Quadrature_Ord5_Triangle,"Quadrature_Ord5_Triangle",Quadrature_base);

Sortie& Quadrature_Ord5_Triangle::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Quadrature_Ord5_Triangle::readOn(Entree& s )
{
  return s;
}

void Quadrature_Ord5_Triangle::compute_integ_points()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  const IntTab& vert_elems = dom_->domaine().les_elems();
  int nb_elem = dom_->nb_elem();
  int nb_pts_integ = ::NB_PTS_INTEG;
  int ndim = Objet_U::dimension;
  DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  // We ensure that sum(weights)=1 and sum(Lambda[i])=1
  DoubleTab lambda(nb_pts_integ, ndim + 1);

  integ_points_.resize(nb_elem, nb_pts_integ, ndim);
  weights_.resize(nb_pts_integ);

  weights_[nb_pts_integ] = 1;
  for (int pts = 0; pts < nb_pts_integ; pts++)
    {
      if (pts < nb_pts_integ - 1)
        {
          weights_(pts) = ::WEIGHTS[pts];
          weights_(nb_pts_integ) -= weights_(pts);
        }
      lambda(pts, 1) = ::LAMBDA[pts][1];
      lambda(pts, 2) = ::LAMBDA[pts][2];
      lambda(pts, 3) = 1 - lambda(pts, 2) - lambda(pts, 1);
    }

  for (int e = 0; e < nb_elem; e++)
    {
      for (int pts = 0; pts < nb_pts_integ; pts++)
        {
          for (int dim = 0; dim < ndim; dim++)
            {
              integ_points_(e, pts, dim) = 0.;
              for (int loc_vert = 0; loc_vert < ndim + 1; loc_vert++)
                integ_points_(e, pts, dim) += xs(vert_elems(e, loc_vert), dim) * lambda(pts, loc_vert);
            }
        }
    }
}

void Quadrature_Ord5_Triangle::compute_integ_points_on_facet()
{

  assert(Objet_U::dimension == 2); // no triangle in 3D!

  int nb_faces = dom_->nb_faces(); // Q: Cette ligne renvoie t-elle bien le nombre de faces ? (nb_faces_tot)
  int ndim = Objet_U::dimension;
  DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  IntTab& face_sommets = dom_->face_sommets();
  int nb_pts_integ = NB_PTS_INTEG_FACETS;

  integ_points_facets_.resize(nb_faces, nb_pts_integ, Objet_U::dimension); // one point per facets, 2D -> 1*2 = 2 columns
  weights_facets_.resize(nb_pts_integ);

  // We ensure that sum(weights)=1 and sum(Lambda[i])=1
  DoubleTab lambda_facets(nb_pts_integ, ndim + 1);
  weights_facets_[nb_pts_integ] = 1;
  for (int pts = 0; pts < nb_pts_integ; pts++)
    {
      if (pts < nb_pts_integ - 1)
        {
          weights_facets_(pts) = ::WEIGHTS_FACETS[pts];
          weights_facets_(nb_pts_integ) -= weights_facets_(pts);
        }
      lambda_facets(pts, 1) = ::LAMBDA_FACETS[pts][1];
      lambda_facets(pts, 2) = ::LAMBDA_FACETS[pts][2];
      lambda_facets(pts, 3) = 1 - lambda_facets(pts, 2) - lambda_facets(pts, 1);
    }

  for (int f = 0; f < nb_faces; f++)
    {
      for (int pts = 0; pts < nb_pts_integ; pts++)
        {
          for (int dim = 0; dim < ndim; dim++)
            {
              integ_points_facets_(f, pts, dim) = 0.;
              for (int loc_vert = 0; loc_vert < ndim + 1; loc_vert++)
                integ_points_facets_(f, pts, dim) += xs(face_sommets(f, loc_vert), dim) * lambda_facets(pts, loc_vert);
            }
        }
    }
}
