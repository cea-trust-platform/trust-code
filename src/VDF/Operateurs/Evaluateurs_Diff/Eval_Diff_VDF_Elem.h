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
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_Elem_included
#define Eval_Diff_VDF_Elem_included

#include <CL_Types_include.h>
#include <Evaluateur_VDF.h>
#include <Eval_VDF_Elem.h>

// .DESCRIPTION class Eval_Diff_VDF_Elem
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_P0_VDF
// .SECTION voir aussi Eval_VDF_Elem, Evaluateur_VDF
template <typename DERIVED_T>
class Eval_Diff_VDF_Elem : public Eval_VDF_Elem, public Evaluateur_VDF
{
public:

  /*
   * Notes about that MACRO's predefined in the previous version of TRUST
   * (just to understand when,where and why (not sure of why :) )
   *
   * - MULTD :
   *    TRUST : predefined for Diff/Dift const_Elem / var_Elem / var_Elem_aniso (not Multi-Inco)
   *    TrioCFD : predefined for Diff/Dift K_Eps_var_VDF_Elem & K_Eps_Bas_Re_VDF_var_Elem
   *
   * - D_AXI :
   *    TRUST : predefined for Diff/Dift Axi operators
   *    TrioCFD : predefined for Diff/Dift Axi operators K_Eps
   *
   * - DEQUIV
   *    TRUST : predefined for Dift (not Diff) Multi_inco_const_Elem / Multi_inco_var_Elem &&
   *        Axi operators const_Elem_Axi / var_Elem_Axi / Multi_inco_var_Elem_Axi / Multi_inco_const_Elem_Axi
   *
   * - MODIF_DEQ
   *    TRUST : predefined for Dift (not Diff) const_Elem / var_Elem (no Axi, no Multi_Inco)
   *
   * - ISQUASI
   *    TrioCFD : predefined Diff_K_Eps_QC_VDF_Elem && Diff_K_Eps_QC_var_VDF_Elem
   *
   * =======================================================================================
   *
   * These static constexpr bools & Methods that will generalize the implementation almost for all operator classes
   * The previous MACRO methodology do not exist anymore
   * See their generic declaration later & override them in the derived class if needed to stay coherent with the previous macros
   */

  static constexpr bool IS_MULTD = true, IS_AXI = false, IS_DEQUIV = false, IS_MODIF_DEQ = false, IS_QUASI = false, IS_ANISO = false;

  inline double Dist_norm_bord (int face) const;
  inline double Dist_face_elem0(int face, int n0) const;
  inline double Dist_face_elem1(int face, int n1) const;
  inline double Dist_norm_bord_externe_VEC(int boundary_index, int global_face, int local_face) const;
  inline double Dist_norm_bord_externe_(int global_face) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline double nu_1(int i, int compo=0) const;
  inline double nu_2(int i, int compo=0) const;
  inline double compute_heq(double, int, double, int, int compo=0) const;
  inline int ind_Fluctu_Term() const;
  inline double equivalent_distance (int boundary_index, int local_face) const;

  //************************
  // CAS SCALAIRE
  //************************

  /*
   * XXX XXX XXX : VERY IMPORTANT
   * Partial specialization is not supported in C++ (otherwise I miss some information)
   * For example template<typename DERIVED_T>template<> will not compile due to
   * invalid explicit specialization before '>' token
   * Complete specialization works ie: template<>template<>
   *
   * So, we need to overload some functions for specific BC's outside the function template
   */

  // Fonctions qui servent a calculer le flux de grandeurs scalaires : Elles sont de type double et renvoient le flux

  // Generic return
  template<typename BC>
  inline double flux_face(const DoubleTab&, int , const BC&, int ) const { return 0; }

  // To overload
  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs scalaires.

  // Generic return
  template<typename BC>
  inline void coeffs_face(int,int, const BC&, double& aii, double& ajj ) const { /* Do nothing */ }

  // To overload
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const;
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites au second membre pour l"implicite pour les grandeurs scalaires.

  // Generic return
  template<typename BC>
  inline double secmem_face(int, const BC&, int ) const { return 0; }

  // To overload
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Neumann_paroi&, int ) const;
  inline double secmem_face(int, const Echange_global_impose&, int ) const;
  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const;
  inline double secmem_faces_interne(int ) const;

  //************************
  // CAS VECTORIEL
  //************************

  /*
   * XXX XXX XXX : VERY IMPORTANT
   * Partial specialization is not supported in C++ (otherwise I miss some information)
   * For example template<typename DERIVED_T>template<> will not compile due to
   * invalid explicit specialization before '>' token
   * Complete specialization works ie: template<>template<>
   *
   * So, we need to overload some functions for specific BC's outside the function template
   */

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles : Elles sont de type void et remplissent le tableau flux

  // Generic return
  template <typename BC>
  inline void flux_face(const DoubleTab&,int , const BC&,int, DoubleVect& flux) const { /* Do nothing */ }
  // To overload

  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,   int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,      int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Echange_global_impose&,     int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi&,             int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,  int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Periodique&,            int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&,   int, DoubleVect& flux) const;
  inline void flux_faces_interne(const DoubleTab&, int , DoubleVect& flux) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs vectorielles.

  // Generic return
  template <typename BC>
  inline void coeffs_face(int, int,const BC&, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }

  // To overload
  inline void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int, int,const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites au second membre pour l"implicite pour les grandeurs vectorielles.

  // Generic return
  template <typename BC>
  inline void secmem_face(int, const BC&, int, DoubleVect& ) const { /* Do nothing */ }

  // To overload
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const;
  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const;
  inline void secmem_faces_interne(int, DoubleVect& ) const;
};

#include <Eval_Diff_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* Eval_Diff_VDF_Elem_included */
