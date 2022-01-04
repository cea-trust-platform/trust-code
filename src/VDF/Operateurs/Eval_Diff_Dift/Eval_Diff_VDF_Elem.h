/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Diff_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Diff_Dift
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Elem_included
#define Eval_Diff_VDF_Elem_included

#include <CL_Types_include.h>
#include <Evaluateur_VDF.h>
#include <Eval_VDF_Elem.h>

// .DESCRIPTION class Eval_Diff_VDF_Elem
// Evaluateur VDF pour la diffusion : Le champ diffuse est un Champ_P0_VDF
// .SECTION voir aussi Eval_VDF_Elem, Evaluateur_VDF
template <typename DERIVED_T>
class Eval_Diff_VDF_Elem : public Eval_VDF_Elem, public Evaluateur_VDF
{
public:

  /*
   * Notes about that MACRO's predefined in the previous version of TRUST (just to understand when,where and why (not sure of why :) )
   *
   * - MULTD --TRUST : predefined for Diff/Dift const_Elem / var_Elem / var_Elem_aniso (not Multi-Inco) --TrioCFD : predefined for Diff/Dift K_Eps_var_VDF_Elem & K_Eps_Bas_Re_VDF_var_Elem
   *
   * - D_AXI --TRUST : predefined for Diff/Dift Axi operators --TrioCFD : predefined for Diff/Dift Axi operators K_Eps
   *
   * - DEQUIV --TRUST : predefined for Dift (not Diff) Multi_inco_const_Elem / Multi_inco_var_Elem && Axi operators const_Elem_Axi / var_Elem_Axi / Multi_inco_var_Elem_Axi / Multi_inco_const_Elem_Axi
   *
   * - MODIF_DEQ --TRUST : predefined for Dift (not Diff) const_Elem / var_Elem (no Axi, no Multi_Inco)
   *
   * - ISQUASI --TrioCFD : predefined Diff_K_Eps_QC_VDF_Elem && Diff_K_Eps_QC_var_VDF_Elem
   *
   * These static constexpr bools & Methods that will generalize the implementation almost for all operator classes. The previous MACRO methodology do not exist anymore
   * See their generic declaration later & override them in the derived class if needed to stay coherent with the previous macros
   */
  static constexpr bool IS_MULTD = true, IS_AXI = false, IS_DEQUIV = false, IS_MODIF_DEQ = false, IS_QUASI = false, IS_ANISO = false;

  /*
   * XXX XXX XXX : VERY IMPORTANT
   * Partial specialization is not supported in C++ (otherwise I miss some information)
   * For example template<typename DERIVED_T>template<> will not compile due to invalid explicit specialization before '>' token
   * Complete specialization works ie: template<>template<>
   *
   * So, we need to overload some functions for specific BC's outside the function template (for F5 for example ...)
   */

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void flux_face(const DoubleTab&, const int, const BC&, int, Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Dirichlet_entree_fluide&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Neumann_paroi&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Periodique&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Echange_global_impose&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Dirichlet_paroi_fixe&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_faces_interne(const DoubleTab&, const int, Type_Double& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void coeffs_face(const int, const int, const BC&, Type_Double&, Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Dirichlet_entree_fluide&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Neumann_paroi&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Periodique&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Echange_global_impose&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Dirichlet_paroi_fixe&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const int, const int, const Echange_externe_impose&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_faces_interne(const int, Type_Double&, Type_Double& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void secmem_face(const int, const BC&, const int, Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void secmem_face(const int, const Dirichlet_entree_fluide&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const Neumann_paroi&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const Echange_global_impose&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const Dirichlet_paroi_fixe&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_faces_interne(const int, Type_Double& ) const;

private:
  inline double Dist_face_elem0(const int face, const int n0) const { return DERIVED_T::IS_AXI ? la_zone->dist_face_elem0_axi(face,n0) : la_zone->dist_face_elem0(face,n0); }
  inline double Dist_face_elem1(const int face, const int n1) const { return DERIVED_T::IS_AXI ? la_zone->dist_face_elem1_axi(face,n1) : la_zone->dist_face_elem1(face,n1); }
  inline double Dist_norm_bord (const int face) const
  {
    double val = DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(face) : la_zone->dist_norm_bord(face);
    return DERIVED_T::IS_MULTD ? val : 2*val;
  }

  inline double Dist_norm_bord_externe_VEC(const int boundary_index, const int global_face, const int local_face) const
  {
    if (DERIVED_T::IS_DEQUIV) return equivalent_distance(boundary_index,local_face);
    else return DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
  }

  inline double Dist_norm_bord_externe_(const int global_face) const
  {
    assert (!DERIVED_T::IS_DEQUIV);
    return DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
  }

  inline void not_implemented_k_eps(const char * nom_funct) const
  {
    std::cerr << "Error in : " << nom_funct << " ! Implement when Diff_K_Eps_QC follows the template architecture !" << std::endl;
    throw;
  }

  // CRTP pattern to static_cast the appropriate class and get the implementation : This is magic !
  inline int ind_Fluctu_Term() const { return static_cast<const DERIVED_T *>(this)->get_ind_Fluctu_Term(); } // See generic impl in the class Eval_Diff_VDF. They will be overloaded for Dift ops
  inline double nu_1(const int i, int compo = 0) const { return static_cast<const DERIVED_T *>(this)->nu_1_impl(i,compo); }
  inline double nu_2(const int i, int compo = 0) const { return static_cast<const DERIVED_T *>(this)->nu_2_impl(i,compo); }
  inline double compute_heq(const double d0, const int i0, const double d1, const int i1, int compo = 0) const { return static_cast<const DERIVED_T *>(this)->compute_heq_impl(d0,i0,d1,i1,compo); }
  inline double equivalent_distance (const int boundary_index, const int local_face) const { return static_cast<const DERIVED_T *>(this)->get_equivalent_distance(boundary_index,local_face); }
};

#include <Eval_Diff_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* Eval_Diff_VDF_Elem_included */
