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

#ifndef Eval_Conv_VDF_Face_included
#define Eval_Conv_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <Eval_VDF_Face.h>

template <typename DERIVED_T>
class Eval_Conv_VDF_Face : public Eval_VDF_Face
{
public:
  static constexpr bool IS_AMONT = false, IS_CENTRE = false, IS_CENTRE4 = false, IS_QUICK = false, IS_AXI = false;
  static constexpr bool CALC_FA7_SORTIE_LIB = true, CALC_ARR_PAR = false, CALC_ARR_SYMM_PAR = false;

  /* ****************************************************************************** *
   * YES, we do magic ! Its all about : Substition Failure Is Not An Error (SFINAE) *
   * ****************************************************************************** */

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  flux_fa7(const DoubleTab&, int , const Neumann_sortie_libre&, int, Type_Double& ) const;

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  flux_fa7(const DoubleTab&, int, int, int, Type_Double& ) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double& ) const ;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double& ) const ;

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI), void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double&, Type_Double& ) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double&, Type_Double& ) const ;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, Type_Double&, Type_Double&) const;

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  coeffs_fa7(int , const Neumann_sortie_libre&, Type_Double& , Type_Double& ) const;

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  coeffs_fa7(int, int, int, Type_Double& , Type_Double& ) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  coeffs_arete(int, int, int, int, Type_Double& , Type_Double& ) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  coeffs_arete(int, int, int, int, Type_Double& , Type_Double& , Type_Double& ) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline
  enable_if_t<(Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI), void>
  coeffs_arete(int, int, int, int, Type_Double& , Type_Double& , Type_Double& ) const { /* do nothing */ }

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  secmem_fa7(int , const Neumann_sortie_libre&, int, Type_Double& ) const;

  template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  secmem_fa7(int, int, int, Type_Double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PAROI ||
                      Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI), void>
  secmem_arete(int, int, int, int, Type_Double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type, typename Type_Double>
  inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  secmem_arete(int, int, int, int, Type_Double&, Type_Double&) const;

  template<Type_Flux_Arete Arete_Type, typename Type_Double> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  secmem_arete(int, int, int, int, Type_Double&, Type_Double&) const { /* do nothing */ }

private:
  template <typename Type_Double>
  inline void fill_coeffs_proto(const double , const double, Type_Double& , Type_Double& ) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation: This is magic !
  inline int premiere_face_bord() const { return static_cast<const DERIVED_T *>(this)->get_premiere_face_bord(); }
  inline int orientation(int face) const { return static_cast<const DERIVED_T *>(this)->get_orientation(face); }
  inline int elem_(int i, int j) const { return static_cast<const DERIVED_T *>(this)->get_elem(i,j); }
  inline int face_amont_princ_(int num_face, int i) const { return static_cast<const DERIVED_T *>(this)->face_amont_princ(num_face,i); }
  inline int face_amont_conj_(int num_face,int i, int k) const { return static_cast<const DERIVED_T *>(this)->face_amont_conj(num_face,i,k); }
  inline double dt_vitesse(int face) const { return static_cast<const DERIVED_T *>(this)->get_dt_vitesse(face); }
  inline double surface_porosite(int face) const { return static_cast<const DERIVED_T *>(this)->get_surface_porosite(face); }
  inline double surface(int face) const { return static_cast<const DERIVED_T *>(this)->get_surface(face); }
  inline double porosite(int face) const { return static_cast<const DERIVED_T *>(this)->get_porosite(face); }
  inline double dim_face_(int n1,int k) const { return static_cast<const DERIVED_T *>(this)->dim_face(n1,k); }
  inline double dim_elem_(int n1,int k) const { return static_cast<const DERIVED_T *>(this)->dim_elem(n1,k); }
  inline double dist_face_(int n1,int n2,int k) const { return static_cast<const DERIVED_T *>(this)->dist_face(n1,n2,k); }
  inline double dist_face_period_(int n1,int n2,int k) const { return static_cast<const DERIVED_T *>(this)->dist_face_period(n1,n2,k); }
  inline double dist_elem_period_(int n1, int n2, int k) const { return static_cast<const DERIVED_T *>(this)->dist_elem_period(n1,n2,k); }
  inline const Zone_Cl_VDF& la_zcl() const { return static_cast<const DERIVED_T *>(this)->get_la_zcl(); }

  inline double conv_quick_sharp_plus_(const double psc,const double vit_0, const double vit_1, const double vit_0_0, const double dx, const double dm, const double dxam) const
  { return static_cast<const DERIVED_T *>(this)->conv_quick_sharp_plus(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam); }

  inline double conv_quick_sharp_moins_(const double psc,const double vit_0,const double vit_1, const double vit_1_1,const double dx, const double dm,const double dxam) const
  { return static_cast<const DERIVED_T *>(this)->conv_quick_sharp_moins(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam); }

  inline double conv_centre_(const double psc,const double vit_0_0, const double vit_0, const double vit_1, const double vit1_1,double g1, double g2, double g3,double g4) const
  { return static_cast<const DERIVED_T *>(this)->conv_centre(psc,vit_0_0,vit_0,vit_1,vit1_1,g1,g2,g3,g4); }

  inline void calcul_g_(const double dxam, const double dx, const double dxav, double& g1, double& g2, double& g3, double& g4) const
  { static_cast<const DERIVED_T *>(this)->calcul_g(dxam,dx,dxav,g1,g2,g3,g4); }
};

#include <Eval_Conv_VDF_Face.tpp> // templates specializations ici ;)

#endif /* Eval_Conv_VDF_Face_included */
