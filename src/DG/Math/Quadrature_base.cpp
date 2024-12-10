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

#include <Quadrature_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>


double Quadrature_base::compute_integral_on_elem(int num_elem, Parser_U& parser) const
{
  DoubleTab val_pts_integ(tab_nb_pts_integ_(num_elem));
  for (int pts = 0; pts < tab_nb_pts_integ_(num_elem); pts++)
    {
      double x = integ_points_(ind_pts_integ_(num_elem) + pts, 0),
             y = integ_points_(ind_pts_integ_(num_elem) + pts, 1);
      parser.setVar("x", x);
      parser.setVar("y", y);
      val_pts_integ(pts) = parser.eval();
    }
  return compute_integral_on_elem(num_elem, val_pts_integ);
}

double Quadrature_base::compute_integral_on_elem(int num_elem, DoubleTab& val_pts_integ) const
{
  double volume = dom_->volumes()[num_elem];
  double acc = 0.;
  for (int pts = 0; pts < tab_nb_pts_integ_(num_elem); pts++)
    {
      acc += val_pts_integ(pts) * weights_(ind_pts_integ_(num_elem)+pts);
    }
  return acc * volume;
}

/* DoubleTab Quadrature_base::compute_integral_on_elem(DoubleTab& val_pts_integ) const // pas sur que cela serve : je commente jusqu'a destruction
{
  int nb_elem_tot = dom_->nb_elem_tot();
  DoubleTab results(nb_elem_tot);
  DoubleTab val_pts_elem;
  for (int num_elem = 0; num_elem < nb_elem_tot; num_elem++)
    {
      val_pts_elem.ref_tab(val_pts_integ, num_elem,1);
      results[num_elem] = compute_integral_on_elem(num_elem, val_pts_elem);
    }
  return results;
} */

double Quadrature_base::compute_integral_on_facet(int num_facet, Parser_U& parser) const
{
  DoubleTab val_pts_integ(nb_pts_integ_facets_);
  for (int pts = 0; pts < nb_pts_integ_facets_; pts++)
    {
      double x = integ_points_facets_(num_facet, pts, 0),
             y = integ_points_facets_(num_facet, pts, 1);
      parser.setVar("x", x);
      parser.setVar("y", y);
      val_pts_integ(pts) = parser.eval();
    }
  return compute_integral_on_facet(num_facet, val_pts_integ);
}

double Quadrature_base::compute_integral_on_facet(int num_facet, DoubleTab& val_pts_integ) const
{
  double surface = dom_->face_surfaces(num_facet);
  double acc = 0.;
  for (int pts = 0; pts < nb_pts_integ_facets_; pts++)
    {
      double val_on_pt = val_pts_integ(pts);
      acc += val_on_pt * weights_facets_(pts);
    }
  return acc * surface;
}

double Quadrature_base::compute_integral(Parser_U& parser) const
{
  int nb_elem = dom_->nb_elem();
  double acc = 0.;
  for (int e = 0; e < nb_elem; e++)
    acc += Quadrature_base::compute_integral_on_elem(e, parser);
  Process::mp_sum(acc);
  return acc;
}

double Quadrature_base::compute_integral(DoubleTab& vals_pts_integ) const
{
  int nb_elem = dom_->nb_elem();
  double acc = 0.;
  double volume;
  for (int e = 0; e < nb_elem; e++)
    {
      volume = dom_->volumes(e);
      for (int pts = 0; pts < tab_nb_pts_integ_(e); pts++)
        acc += vals_pts_integ(ind_pts_integ_(e)+pts)*weights_(ind_pts_integ_(e)+pts)*volume;
    }
  Process::mp_sum(acc);
  return acc;
}
