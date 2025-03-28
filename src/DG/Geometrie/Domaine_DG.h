
/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Domaine_DG_included
#define Domaine_DG_included

#include <Domaine_Poly_base.h>
#include <Option_DG.h>

class Quadrature_base;

class Domaine_DG : public Domaine_Poly_base
{
  Declare_instanciable(Domaine_DG);
public :
  void discretiser() override;
  void init_equiv() const override;

  inline const DoubleTab& get_dia() const { return dia_; }
  inline const DoubleTab& get_invdia() const { return invdia_; }
  inline const DoubleTab& get_per() const { return per_; }
  inline const DoubleTab& get_rho() const { return rho_; }
  inline const DoubleTab& get_sig() const { return sig_; }
  inline const IntTab& get_nfaces_elem() const { return nfaces_elem_; }
  inline const TRUST_Deriv<Elem_poly_base> get_type_elem() const { return type_elem_; }

  inline const Quadrature_base& get_quadrature(int order) const;
  inline const Quadrature_base& get_quadrature() const;

  inline const IntTab& get_stencil_sorted() const { return stencil_sorted_;}


  void set_default_order(int order);
  void get_position(DoubleTab& positions) const override;
  double compute_L1_norm(const DoubleVect& val_source) const override;
  double compute_L2_norm(const DoubleVect& val_source) const override;
  void get_nb_integ_points(IntTab& nb_integ_points) const override;
  void get_ind_integ_points(IntTab& ind_integ_points) const override;

  void calculer_h_carre() override;

  inline const bool& gram_schmidt() const { return gram_schmidt_;}

protected:

  std::shared_ptr<Quadrature_base> quad1_;   // Key: quadrature order, value: DoubleTab representing the quadrature barycenters for that order
  std::shared_ptr<Quadrature_base> quad3_;   // Key: quadrature order, value: DoubleTab representing the quadrature barycenters for that order
  std::shared_ptr<Quadrature_base> quad5_;   // Key: quadrature order, value: DoubleTab representing the quadrature barycenters for that order

  DoubleTab dia_;
  DoubleTab invdia_;
  DoubleTab per_;
  DoubleTab rho_;
  DoubleTab sig_;
  IntTab nfaces_elem_;
  int order_quad_=-1;
  bool gram_schmidt_;
// DoubleVect h_, sigma;

  IntTab stencil_sorted_; //table of stencil sorted for each elements

  void compute_mesh_param(); // Compute the stabilization parameters
  bool build_nfaces_elem_();
};

const Quadrature_base& Domaine_DG::get_quadrature(int order) const
{
  switch(order)
    {
    case 1:
      return *quad1_;
    case 3:
      return *quad3_;
    case 5:
      return *quad5_;
    default:
      {
        Cerr << "not implemented quadrature order " << finl;
        Process::exit();
      }
    }

  return *quad1_;
}

const Quadrature_base& Domaine_DG::get_quadrature() const // overloaded to give the defaut order, depending of the order of the method
{
  return get_quadrature(order_quad_);
}


#endif /* Domaine_DG_included */
