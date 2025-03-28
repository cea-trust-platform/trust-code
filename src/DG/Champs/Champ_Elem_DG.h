/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Champ_Elem_DG_included
#define Champ_Elem_DG_included

#include <Champ_Inc_P0_base.h>
#include <Operateur.h>
#include <Quadrature_base.h>
#include <Matrice_Dense.h>

// Champ correspondant a une inconnue scalaire (type temperature ou pression)
// Degres de libertes : valeur aux elements + flux aux faces
class Champ_Elem_DG: public Champ_Inc_P0_base
{
  Declare_instanciable(Champ_Elem_DG);
public:
  Champ_base& affecter_(const Champ_base& ch) override;
  int imprime(Sortie&, int) const override;

  int fixer_nb_valeurs_nodales(int n) override;

  void associer_domaine_dis_base(const Domaine_dis_base&) override;

  inline const int& get_order() const { return order_; }
  inline const IntTab& indices_glob_elem() const { return indices_glob_elem_; }

  inline const int& nb_bfunc() const { return nb_bfunc_; }

  //Evaluation of the basis functions on integration points for elements and facets
  void eval_bfunc(const Quadrature_base& quad, const int& nelem, DoubleTab& fbasis) const;
  void eval_bfunc_on_facets(const Quadrature_base& quad, const int& nelem, const int& num_face, DoubleTab& grad_fbasis) const;
  void eval_bfunc(const DoubleTab& coord, const int& nelem, DoubleTab& fbasis) const;

  //Evaluation of the gradient of the basis functions on integration points for elements and facets
  void eval_grad_bfunc(const Quadrature_base& quad, const int& nelem, DoubleTab& fbasis) const;
  void eval_grad_bfunc_on_facets(const Quadrature_base& quad, const int& nelem, const int& num_face, DoubleTab& grad_fbasis) const;


  inline const Matrice_Morse& get_mass_matrix() const { return mass_matrix_; }
//  inline const Matrice_Morse& get_inv_mass_matrix() const { return inv_mass_matrix_; }

  inline const DoubleTab& get_eta_elem() const { return eta_elem; }
  inline const DoubleTab& get_eta_facet() const { return eta_facet; }

  const Matrice_Dense eval_invMassMatrix(const Quadrature_base& quad, const int& nelem) const;
  const Matrice_Dense build_local_mass_matrix(const Quadrature_base& quad, const int nelem) const;

  /* fonctions pour reconstruire la valeur du champ selon la localisation */
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;
  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const override;
  DoubleTab& eval_elem(DoubleTab& valeurs) const override;

protected:
  /*! Compute the mass matrix
   */
  void allocate_mass_matrix();
  void allocate_transition_matrix();
  void compute_stab_param();

  void build_mass_matrix();
  void build_transition_matrix();
  void orthonormalize(const int& nelem, DoubleTab& fbasis) const;


  void gramSchmidt(DoubleTab& fbase, const Quadrature_base& quad, const int& num_elem, const int& current_indice, const int& nb_pts_integ, const double& volume, int index);

//  void build_inv_mass_matrix();

  int order_ = -1;
  int nb_bfunc_ = -1;

  bool is_orthonormalized_ = false;

  IntTab indices_glob_elem_;

  Matrice_Morse mass_matrix_;
//  Matrice_Morse inv_mass_matrix_;
  Matrice_Morse transition_matrix_;

  DoubleTab eta_elem;
  DoubleTab eta_facet;

};


#endif /* Champ_Elem_DG_included */
