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
#ifndef Quadrature_included
#define Quadrature_included

#include <TRUSTTabs_forward.h>
#include <Parser_U.h>
#include <Domaine_DG.h>

class Quadrature
{
public:
  Quadrature(const Domaine_DG& dom) : dom_(dom)
  { }
  virtual ~Quadrature() {}

  void register_quadrature();
  const DoubleTab& get_integ_points();
  const DoubleTab& get_integ_points_facets();

//  DoubleTab get_weights();
//  DoubleTab get_weights_facets();

  virtual void compute_integ_points() = 0;
  virtual void compute_integ_points_on_facet() = 0;
  virtual int order() = 0;

  /*! Compute the integral of a function on the whole domain
   */
  double compute_integral(Parser_U& parser) const;
  /*! Compute the integral of a function on the whole domain with its values on integration points
   */
  double compute_integral(DoubleTab& vals_pts_integ) const;
  /*! Compute the integral of a function on one triangle
   */
  double compute_integral_on_elem(int num_elem, Parser_U& parser) const ;

  /*! Compute the integral of a function on one triangle with its value on integration points
   */
  double compute_integral_on_elem(int num_elem, DoubleTab& val_pts_integ) const ;

  /*! Compute the integral of a function on one facet
   */
  double compute_integral_on_facet(int num_facet, Parser_U& parser) const ;
  /*! Compute the integral of a function on one facet with its value on integration points
   */
  double compute_integral_on_facet(int num_facet, DoubleTab& val_pts_integ) const ;

protected:
  REF(Domaine_DG) dom_;
  Parser_U parser_; // why not?

  DoubleTab integ_points_; // number of cols (line size) will vary according to order of the method and element type
  DoubleTab integ_points_facets_;
  DoubleTab weights_;
  DoubleTab weights_facets_;
};

#endif
