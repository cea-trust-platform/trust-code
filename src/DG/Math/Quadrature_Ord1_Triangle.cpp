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

#include <Quadrature_Ord1_Triangle.h>

namespace
{
static constexpr double WEIGHTS[1] = {1.0};
static constexpr double WEIGHTS_FACETS[1] = {1.0};
static constexpr int ORDER = 1;
}

Implemente_instanciable(Quadrature_Ord1_Triangle,"Quadrature_Ord1_Triangle",Quadrature_base);

Sortie& Quadrature_Ord1_Triangle::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Quadrature_Ord1_Triangle::readOn(Entree& s )
{
  return s;
}


void Quadrature_Ord1_Triangle::compute_integ_points()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  int nb_elem_tot = dom_->nb_elem_tot();
  int nb_pts_integ = ::ORDER; // one point per element, 2D -> 1*2 = 2 columns
  int ndim = Objet_U::dimension;

  integ_points_.resize(nb_elem_tot, nb_pts_integ, ndim);
  weights_.resize(nb_pts_integ);
  const DoubleTab& xp = dom_->xp(); // barycentre elem
  for (int pts = 0; pts < nb_pts_integ; pts++)
    {
      for (int e = 0; e < nb_elem_tot; e++) // TODO : Utiliser plutôt la fonction copy
        {
          for (int dim = 0; dim < ndim; dim++)
            integ_points_(e, pts, dim) = xp(e, dim);
        }
    }
  // We ensure that sum(weights)=1;
  weights_(nb_pts_integ-1)=1;
  for (int pts = 0; pts < nb_pts_integ-1; pts++)
    {
      weights_(pts) = ::WEIGHTS[pts];
      weights_(nb_pts_integ-1)-=weights_(pts);
    }
}

void Quadrature_Ord1_Triangle::compute_integ_points_on_facet()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  int nb_faces = dom_->nb_faces();
  DoubleTab& xv = dom_->xv(); // facets barycentre
  int nb_pts_integ = ::ORDER;

  weights_facets_.resize(nb_pts_integ);
  integ_points_facets_.resize(nb_faces, ::ORDER, 2); // one point per facets, 2D -> 1*2 = 2 columns
  for (int pts = 0; pts < nb_pts_integ; pts++)
    {
      for (int f = 0; f < nb_faces; f++) // TODO : Utiliser plutôt la fonction copy
        {
          for (int dim = 0; dim < 2; dim++)
            integ_points_facets_(f, pts, dim) = xv(f, dim);
        }
      weights_facets_(pts) = ::WEIGHTS_FACETS[pts];
    }
  weights_facets_(nb_pts_integ-1)=1;
  for (int pts = 0; pts < nb_pts_integ-1; pts++)
    {
      weights_facets_(nb_pts_integ-1)-=weights_facets_(pts);
    }
}
