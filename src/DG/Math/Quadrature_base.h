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
#include <Matrice_Morse_Sym.h>

class Quadrature_base
{
public:
  Quadrature_base(const Domaine_DG& dom) : dom_(dom),  nb_pts_integ_quad_(-1), nb_pts_integ_tri_(-1), nb_pts_integ_facets_(-1), nb_pts_integ_max_(-1)
  { }

  virtual ~Quadrature_base() {}

  inline const DoubleTab& get_integ_points() const { return integ_points_; }
  inline const DoubleTab& get_integ_points_facets() const { return integ_points_facets_; }

  inline const DoubleTab& get_weights() const { return weights_; }
  inline const DoubleTab& get_weights_facets() const { return weights_facets_; }

  inline const IntTab& get_tab_nb_pts_integ() const { return tab_nb_pts_integ_; }
  inline const IntTab& get_ind_pts_integ() const { return ind_pts_integ_; }
//  void register_quadrature();

  /*! Compute for the whole domain the exact location of integration points per element
   */
  virtual void compute_integ_points() = 0;

  /*! Compute for the whole domain the exact location of integration points per facet
   */
  virtual void compute_integ_points_on_facet() = 0;


  inline int nb_pts_integ_max() const { return nb_pts_integ_max_; } // TODO: max du nombre de pts de la quadrature
  inline int nb_pts_integ(int e ) const { return tab_nb_pts_integ_(e); }
  inline int nb_pts_integ_facets() const { return nb_pts_integ_facets_; }
  inline int ind_pts_integ(int e ) const { return ind_pts_integ_(e); }

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
  // DoubleTab compute_integral_on_elem(DoubleTab& val_pts_integ) const ; // pas sur que cela serve : je commente jusqu'a destruction

  /*! Compute the integral of a function on one facet
   */
  double compute_integral_on_facet(int num_facet, Parser_U& parser) const ;
  /*! Compute the integral of a function on one facet with its value on integration points
   */
  double compute_integral_on_facet(int num_facet, DoubleTab& val_pts_integ) const ;


protected:
  OBS_PTR(Domaine_DG) dom_;

  int nb_pts_integ_quad_; // local numbers of integ points for quadrangles
  int nb_pts_integ_tri_; // // local numbers of integ points for triangles
  int nb_pts_integ_facets_; // local numbers of integ points for facets
  int nb_pts_integ_max_;

  IntTab tab_nb_pts_integ_; // numbers of integ points cumulated

  IntTab ind_pts_integ_; // numero of the first integ points

  Parser_U parser_; // why not?

  DoubleTab integ_points_; // Integ points for quadrature formula
  DoubleTab integ_points_facets_;
  DoubleTab weights_;      //global weights
  DoubleTab weights_quad_; // local numbers
  DoubleTab weights_tri_; // local numbers
  DoubleTab weights_facets_; // local numbers

  // Computes the area of a triangle from vertex coordinates
  double triangleArea(double x1, double y1, double x2, double y2, double x3, double y3);

  // Computes a weight scale based on tessellation triangle and element volume
  double calculateWeightScale(const IntTab& vert_elems, const DoubleTab& xs, DoubleVect& volumes, int e, int s1, int s2, int s3);
  double calculateWeightScale(double ve, double s1x, double s1y, double s2x, double s2y, double s3x, double s3y);



};

#endif
