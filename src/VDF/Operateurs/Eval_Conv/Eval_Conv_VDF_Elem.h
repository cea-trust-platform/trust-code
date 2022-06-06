/****************************************************************************
 * Copyright (c) 2021, CEA
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

#ifndef Eval_Conv_VDF_Elem_included
#define Eval_Conv_VDF_Elem_included

#include <CL_Types_include.h>
#include <Eval_VDF_Elem.h>

template <typename DERIVED_T>
class Eval_Conv_VDF_Elem : public Eval_VDF_Elem
{
public:
  static constexpr bool IS_AMONT = false, IS_CENTRE = false, IS_CENTRE4 = false, IS_QUICK = false;
  // Overload Eval_VDF_Elem
  static constexpr bool CALC_FLUX_FACES_ECH_EXT_IMP = false, CALC_FLUX_FACES_ECH_GLOB_IMP = false, CALC_FLUX_FACES_PAR = false,
                        CALC_FLUX_FACES_PAR_FIXE = false, CALC_FLUX_FACES_SORTIE_LIB = true;

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void flux_face(const DoubleTab&, const int, const BC&, const int, Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Dirichlet_entree_fluide&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Neumann_sortie_libre&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const Periodique&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void flux_faces_interne(const DoubleTab&, const int, Type_Double& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void coeffs_face(const int, const int, const BC&, Type_Double& , Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Dirichlet_entree_fluide&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Neumann_sortie_libre&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const Periodique&, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face(const int, const int, const int, const int, const Echange_externe_impose&, Type_Double&, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void coeffs_faces_interne(const int, Type_Double&, Type_Double& ) const;

  // contribution de la derivee en vitesse d'une equation scalaire
  template <typename BC, typename Type_Double> // Generic return
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, const int, const BC&, const int, Type_Double& ) const { /* Do nothing */ }

  // Type_Double
  template <typename Type_Double> inline void coeffs_face_bloc_vitesse(const DoubleTab&, const int, const Dirichlet_entree_fluide&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face_bloc_vitesse(const DoubleTab&, const int, const Neumann_sortie_libre&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face_bloc_vitesse(const DoubleTab&, const int, const Periodique&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face_bloc_vitesse(const DoubleTab&, const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void coeffs_faces_interne_bloc_vitesse(const DoubleTab&, const int, Type_Double& ) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void secmem_face(const int, const BC&, const int, Type_Double& ) const { /* Do nothing */ }

  // To overload
  template <typename Type_Double> inline void secmem_face(const int, const Dirichlet_entree_fluide&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const Neumann_sortie_libre&, const int, Type_Double& ) const;
  template <typename Type_Double> inline void secmem_face(const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void secmem_faces_interne(const int, Type_Double& ) const { /* Do nothing */ }

private:
  template <typename Type_Double> inline void coeffs_face_common(const int, Type_Double&, Type_Double& ) const;
  template <typename Type_Double> inline void coeffs_face_bloc_vitesse_common(const DoubleTab&, const int, Type_Double& ) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation : This is magic !
  inline int elem_(const int i, const int j) const { return static_cast<const DERIVED_T *>(this)->get_elem(i,j); }
  inline int amont_amont_(const int face, const int i) const { return static_cast<const DERIVED_T *>(this)->amont_amont(face,i); }
  inline double dt_vitesse(const int face) const { return static_cast<const DERIVED_T *>(this)->get_dt_vitesse(face); }
  inline double surface_porosite(const int face) const { return static_cast<const DERIVED_T *>(this)->get_surface_porosite(face); }

  template <typename Type_Double> inline void quick_fram_(const double psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face, const DoubleTab& transporte, Type_Double& flux) const
  { static_cast<const DERIVED_T *>(this)->template quick_fram<Type_Double>(psc, num0, num1, num0_0, num1_1, face, transporte, flux); }

  template <typename Type_Double> inline void qcentre_(const double psc, const int num0, const int num1, const int num0_0, const int num1_1, const int face, const DoubleTab& transporte, Type_Double& flux) const
  { static_cast<const DERIVED_T *>(this)->template qcentre<Type_Double>(psc,num0,num1,num0_0,num1_1,face,transporte,flux); }
};

#include <Eval_Conv_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* Eval_Conv_VDF_Elem_included */
