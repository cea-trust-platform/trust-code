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

#include <Quadrature_Ord1_Polygone.h>
/****************************************************************/
/* Formule de quadrature 2D : Formule Gauss-Lobatto Aide-memoire elements finis Ern p.220  */
/* Formule de quadrature 1D : Formule Gauss-Lobatto Aide-memoire elements finis Ern p.220       */
/****************************************************************/
namespace
{
static constexpr int nb_pts_integ_tri = {1};
// static constexpr int nb_pts_integ_quad = {4};
static constexpr int nb_pts_integ_facet = {1};

// static constexpr double WEIGHTS_QUAD[4] = {1./4.,1./4.,1./4.,1./4.};
// static constexpr double WEIGHTS_QUAD_POLY[4] = {1./4.,1./4.,1./4.,1./4.};
static constexpr double WEIGHTS_FACETS[1] = {1.};
/*static constexpr double LAMBDA_QUAD[4][4] =
{
  {1./2,1./2,0.,0.},
  {0.,1./2.,0.,1./2.},
  {0.,0.,1./2.,1./2.},
  {1./2.,0.,1./2.,0.}
}; // Barycentric coordinates coefficients of integration points in elem */
/*static constexpr double LAMBDA_QUAD_POLY[4][4] =
{
  {1./2,1./2,0.,0.},
  {0.,1./2.,1./2.,0},
  {0.,0.,1./2.,1./2.},
  {1./2.,0.,0.,1./2.}
}; // Barycentric coordinates coefficients of integration points in elem */
static constexpr double LAMBDA_FACETS[1][2] =
{
  {1. / 2., 1. / 2.},
}; // Barycentric coordinates coefficients of integration points on facets */
static constexpr double WEIGHTS_TRI[1] = {1.0};

static constexpr double LAMBDA_TRI[1][3] =
{
  {1. / 3., 1. / 3., 1. / 3.}
}; // Barycentric coordinates coefficients of integration points in elem */
}
static constexpr int N_TRI_IN_QUAD = {2};
static constexpr int TRI_IN_QUAD[2][3] =
{
  {0, 1, 2},
  {0, 2, 3}
}; // List of vertices that decomposes quad in tri */*

static constexpr int TRI_IN_CART[2][3] =
{
  {0, 1, 2},
  {1, 2, 3}
}; // List of vertices that decomposes quad in tri */

void Quadrature_Ord1_Polygone::compute_integ_points()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  // Get infos of the mesh
  const IntTab& vert_elems = dom_->domaine().les_elems();
  int nb_elem_tot = dom_->nb_elem_tot();
  int ndim = Objet_U::dimension;
  const DoubleTab& xs = dom_->domaine().les_sommets(); // facets barycentre
  DoubleVect& volumes = dom_->volumes();
  const IntTab& nfaces_elem = dom_->get_nfaces_elem(); // IntTab that indicate the number of facet of each elem
  const IntTab& elem_faces = dom_->elem_faces();       // IntTab connectivity between elem and facet
  const IntTab& face_sommets = dom_->face_sommets();   // IntTab connectivity between facet and vertices
  const DoubleTab& xp = dom_->xp();                    // barycentre elem

  // Filling the table Tab_nb_pts_integ
  int cumul = 0;
  tab_nb_pts_integ_.resize(dom_->nb_elem_tot());
  ind_pts_integ_.resize(dom_->nb_elem_tot());
  DoubleTab lambda_tri; // tesselation
  IntTab tri_in_quad;   // tesselation

  nb_pts_integ_max_ = 0;
  int nb_pts_quad;
  // only for quad
  tri_in_quad.resize(::N_TRI_IN_QUAD, 3);
  if (dom_->get_type_elem()->que_suis_je() == "Quadri_poly")
    {
      for (int n_tri = 0; n_tri < ::N_TRI_IN_QUAD; n_tri++)
        for (int i = 0; i < 3; i++)
          tri_in_quad(n_tri, i) = ::TRI_IN_CART[n_tri][i];
    }
  else
    {
      for (int n_tri = 0; n_tri < ::N_TRI_IN_QUAD; n_tri++)
        for (int i = 0; i < 3; i++)
          tri_in_quad(n_tri, i) = ::TRI_IN_QUAD[n_tri][i];
    }

  for (int e = 0; e < dom_->nb_elem_tot(); e++)
    {
      int nsom = nfaces_elem(e); // Récupération du nombre de faces de l'élément
      switch (nsom)
        {
        case 3: // triangle
          tab_nb_pts_integ_(e) = ::nb_pts_integ_tri;
          ind_pts_integ_(e) = cumul;
          cumul += ::nb_pts_integ_tri;
          nb_pts_integ_max_ = std::max(nb_pts_integ_max_, ::nb_pts_integ_tri);
          break;
        case 4:                                 // quadrangle
          nb_pts_quad = 2 * ::nb_pts_integ_tri; // tesselation with 2 triangle S1S4S3 and S1S2S3
          tab_nb_pts_integ_(e) = nb_pts_quad;
          ind_pts_integ_(e) = cumul;
          cumul += nb_pts_quad;
          nb_pts_integ_max_ = std::max(nb_pts_integ_max_, nb_pts_quad);
          break;
        default: // other
          int nb_pts_integ_e = nsom * ::nb_pts_integ_tri;
          tab_nb_pts_integ_(e) = nb_pts_integ_e;
          ind_pts_integ_(e) = cumul;
          cumul += nb_pts_integ_e;
          nb_pts_integ_max_ = std::max(nb_pts_integ_max_, nb_pts_integ_e);
          break;
        }
    }
  // Adjustment of weights for quadrature of triangle
  weights_tri_.resize(::nb_pts_integ_tri);
  weights_tri_[::nb_pts_integ_tri - 1] = 1.;
  lambda_tri.resize(::nb_pts_integ_tri, 3); // tesselation
  for (int pts = 0; pts < ::nb_pts_integ_tri; pts++)
    {
      if (pts < nb_pts_integ_tri - 1)
        {
          weights_tri_(pts) = ::WEIGHTS_TRI[pts];
          weights_tri_(nb_pts_integ_tri - 1) -= weights_tri_(pts);
        }
      lambda_tri(pts, 0) = ::LAMBDA_TRI[pts][0];
      lambda_tri(pts, 1) = ::LAMBDA_TRI[pts][1];
      lambda_tri(pts, 2) = 1. - lambda_tri(pts, 1) - lambda_tri(pts, 0);
    }

  nb_pts_integ_max_ = Process::mp_max(nb_pts_integ_max_);

  // We ensure that sum(weights)=1 and sum(Lambda[i])=1

  integ_points_.resize(cumul, ndim); // cumul : number total of integ points
  weights_.resize(cumul);            // Each weight with global numerotation: Linked by the tables ind_elem and tab_nb_elem

  for (int e = 0; e < nb_elem_tot; e++)
    {
      int ind_elem_e = ind_pts_integ_(e); // It may be faster to recalculate this with GPU
      int nsom = nfaces_elem(e);          // Récupération du nombre de faces de l'élément
      switch (nsom)
        {
        case 3: // triangle
          for (int pts = 0; pts < ::nb_pts_integ_tri; pts++)
            {
              for (int dim = 0; dim < ndim; dim++)
                for (int loc_vert = 0; loc_vert < 3; loc_vert++) // ndim+2 = number of vertices
                  integ_points_(ind_elem_e + pts, dim) += xs(vert_elems(e, loc_vert), dim) * lambda_tri(pts, loc_vert);
              weights_(ind_elem_e + pts) = weights_tri_(pts);
            }
          break;
        case 4: // quadrangle
          for (int n_tri = 0; n_tri < ::N_TRI_IN_QUAD; n_tri++)
            {
              double weight_scale = calculateWeightScale(vert_elems, xs, volumes, e, tri_in_quad(n_tri, 0), tri_in_quad(n_tri, 1), tri_in_quad(n_tri, 2));
              for (int pts = 0; pts < ::nb_pts_integ_tri; pts++)
                {
                  for (int dim = 0; dim < ndim; dim++)
                    for (int loc_vert = 0; loc_vert < 3; loc_vert++)
                      integ_points_(ind_elem_e + pts + n_tri * ::nb_pts_integ_tri, dim) += xs(vert_elems(e, tri_in_quad(n_tri, loc_vert)), dim) * lambda_tri(pts, loc_vert);
                  weights_(ind_elem_e + pts + n_tri * ::nb_pts_integ_tri) = weight_scale * weights_tri_(pts);
                }
            }
          break;
        default: // other
          for (int n_tri = 0; n_tri < nsom; n_tri++)
            {
              int f = elem_faces(e, n_tri);
              double weight_scale = calculateWeightScale(volumes(e), xs(face_sommets(f, 0), 0), xs(face_sommets(f, 0), 1), xs(face_sommets(f, 1), 0), xs(face_sommets(f, 1), 1), xp(e, 0), xp(e, 1));
              for (int pts = 0; pts < ::nb_pts_integ_tri; pts++)
                {
                  for (int dim = 0; dim < ndim; dim++)
                    {
                      for (int loc_vert = 0; loc_vert < 2; loc_vert++)
                        integ_points_(ind_elem_e + pts + n_tri * ::nb_pts_integ_tri, dim) += xs(face_sommets(f, loc_vert), dim) * lambda_tri(pts, loc_vert);
                      integ_points_(ind_elem_e + pts + n_tri * ::nb_pts_integ_tri, dim) += xp(e, dim) * lambda_tri(pts, 2);
                    }
                  weights_(ind_elem_e + pts + n_tri * ::nb_pts_integ_tri) = weight_scale * weights_tri_(pts);
                }
            }
          break;
        }
    }
}

void Quadrature_Ord1_Polygone::compute_integ_points_on_facet()
{
  nb_pts_integ_facets_ = ::nb_pts_integ_facet;
  assert(Objet_U::dimension == 2); // no quadrangle in 3D!

  int nb_faces = dom_->nb_faces();
  int ndim = Objet_U::dimension;
  DoubleTab& xs = dom_->domaine().les_sommets(); // vertices
  IntTab& face_sommets = dom_->face_sommets();

  integ_points_facets_.resize(nb_faces, nb_pts_integ_facets_, Objet_U::dimension); // one point per facets, 2D -> 1*2 = 2 columns
  weights_facets_.resize(nb_pts_integ_facets_);

  // We ensure that sum(weights)=1 and sum(Lambda[i])=1
  DoubleTab lambda_facets(nb_pts_integ_facets_, ndim);
  weights_facets_[nb_pts_integ_facets_ - 1] = 1.;
  for (int pts = 0; pts < nb_pts_integ_facets_; pts++)
    {
      if (pts < nb_pts_integ_facets_ - 1)
        {
          weights_facets_(pts) = ::WEIGHTS_FACETS[pts];
          weights_facets_(nb_pts_integ_facets_ - 1) -= weights_facets_(pts);
        }
      lambda_facets(pts, 0) = ::LAMBDA_FACETS[pts][0];
      lambda_facets(pts, 1) = 1. - lambda_facets(pts, 0);
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
