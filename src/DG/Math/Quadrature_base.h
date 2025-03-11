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
#ifndef Quadrature_base_included
#define Quadrature_base_included

#include <TRUSTTabs_forward.h>
#include <Parser_U.h>
#include <Domaine_DG.h>
#include <Objet_U.h>
#include <Matrice_Morse_Sym.h>

class Quadrature_base : public Objet_U
{
  Declare_base(Quadrature_base);
public:

  inline DoubleTab& get_integ_points() { return integ_points_; }
  inline const DoubleTab& get_integ_points() const { return integ_points_; }
  inline const DoubleTab& get_integ_points_facets() const { return integ_points_facets_; }

  inline const DoubleTab& get_weights() const { return weights_; }
  inline const DoubleTab& get_weights_facets() const { return weights_facets_; }

//  void register_quadrature();

  /*! Compute for the whole domain the exact location of integration points per element
   */
  virtual void compute_integ_points() = 0;

  /*! Compute for the whole domain the exact location of integration points per facet
   */
  virtual void compute_integ_points_on_facet() = 0;

  /*! Compute the mass matrix
   */

  virtual int order() const=0;
  virtual int nb_pts_integ() const=0;
  virtual int nb_pts_integ_facets() const=0;



  /*! Compute for a elem the projection of a function on the basis functions
   */
//  virtual DoubleTab compute_projection_on_elem(const DoubleTab& val_pts_integer) const = 0;

  void associer_domaine(const Domaine_DG& dom);
  void initialiser_weights_and_points();

  /*! Compute the integral of a function on the whole domain
   */
  double compute_integral(Parser_U& parser) const;
  /*! Compute the integral of a function on the whole domain with its values on integration points
   */
  double compute_integral(DoubleTab& vals_pts_integ) const;
  /*! Compute the integral of a function on one triangle
   */
  double compute_integral_on_elem(int num_elem, Parser_U& parser) const ;
  /*! Compute the integral of a function on each triangle
   */
  double compute_integral_on_elem(Parser_U& parser) const ;

  /*! Compute the integral of a function on one triangle with its value on integration points
   */
  double compute_integral_on_elem(int num_elem, DoubleTab& val_pts_integ) const ;
  /*! Compute the integral of a function on each triangle with its value on integration points
   */
  DoubleTab compute_integral_on_elem(DoubleTab& val_pts_integ) const ;

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

  Matrice_Morse_Sym mass_matrix_;
  Matrice_Morse_Sym inv_mass_matrix_;


};

#endif
