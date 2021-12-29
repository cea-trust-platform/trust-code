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
// File:        Eval_Diff_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Diff_Dift
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Face_included
#define Eval_Diff_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <Evaluateur_VDF.h>
#include <Eval_VDF_Face.h>

// .DESCRIPTION class Eval_Diff_VDF_Face
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face
// .SECTION voir aussi Eval_VDF_Face, Evaluateur_VDF
template <typename DERIVED_T>
class Eval_Diff_VDF_Face : public Eval_VDF_Face, public Evaluateur_VDF
{
public:
  static constexpr bool IS_VAR = false, IS_TURB = false;
  static constexpr bool CALC_FA7_SORTIE_LIB = DERIVED_T::IS_TURB ? true : false, CALC_ARR_PAR_FL = DERIVED_T::IS_TURB ? false : true;

  /* ****************************************************************************** *
   * YES, we do magic ! Its all about : Substition Failure Is Not An Error (SFINAE) *
   * ****************************************************************************** */

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  flux_fa7(const DoubleTab&, int , const Neumann_sortie_libre&, int, ArrOfDouble& ) const { /* Do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  flux_fa7(const DoubleTab&, int, int, int, ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble& ) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble& ) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble& ) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<(Arete_Type == Type_Flux_Arete::SYMETRIE), void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type> inline
  enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE  || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble&, ArrOfDouble&) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble&, ArrOfDouble&) const ;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
  flux_arete(const DoubleTab&, int, int, int, int, ArrOfDouble&, ArrOfDouble&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme. Ask the TRUST support to code like Eval_Amont_VDF_Face !" << finl;
    Process::exit();
  }

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  coeffs_fa7(int , const Neumann_sortie_libre&, ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  coeffs_fa7(int, int, int, ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< (Arete_Type == Type_Flux_Arete::SYMETRIE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  coeffs_arete(int, int, int, int, ArrOfDouble& , ArrOfDouble& , ArrOfDouble& ) const { /* Do nothing */ }

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
  secmem_fa7(int , const Neumann_sortie_libre&, int, ArrOfDouble& ) const { /* Do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
  secmem_fa7(int, int, int, ArrOfDouble& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI || Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>
  secmem_arete(int, int, int, int , ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline
  enable_if_t<(Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::SYMETRIE), void>
  secmem_arete(int, int, int, int , ArrOfDouble& ) const { /* Do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
  secmem_arete(int, int, int, int, ArrOfDouble&, ArrOfDouble& ) const;

  template<Type_Flux_Arete Arete_Type> inline enable_if_t<(Arete_Type == Type_Flux_Arete::PERIODICITE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE), void>
  secmem_arete(int, int, int, int, ArrOfDouble&, ArrOfDouble& ) const { /* Do nothing */ }

private:
  inline double surface_(int i,int j) const { return (DERIVED_T::IS_VAR || DERIVED_T::IS_TURB) ? 0.5*(surface(i)+surface(j)) : 0.5*(surface(i)*porosite(i)+surface(j)*porosite(j)); }
  inline double porosity_(int i,int j) const { return (DERIVED_T::IS_VAR || DERIVED_T::IS_TURB) ? 0.5*(porosite(i)+porosite(j)) : 1.0; }

  // CRTP pattern to static_cast the appropriate class and get the implementation : This is magic !
  inline double nu_mean_2pts(int i=0, int j=0, int compo=0) const { return static_cast<const DERIVED_T *>(this)->nu_1_impl_face(i, j, compo); }
  inline double nu_mean_4pts(int i, int j, int k, int l, int compo=0) const { return static_cast<const DERIVED_T *>(this)->nu_2_impl_face(i, j, k, l,compo); }
  inline double nu_lam(int i, int j=0) const { return static_cast<const DERIVED_T *>(this)->nu_2_impl(i,j); } // Attention nu_2_impl and not nu_1_impl for Dift ...
  inline double nu_lam_mean_4pts(int i, int j, int k, int l, int compo=0) const { return static_cast<const DERIVED_T *>(this)->nu_lam_impl_face(i,j,k,l,compo); }
  inline double nu_lam_mean_2pts(int i, int j, int compo=0) const { return static_cast<const DERIVED_T *>(this)->nu_lam_impl_face2(i,j,compo); }
  inline double nu_turb(int i, int compo=0) const { return static_cast<const DERIVED_T *>(this)->nu_t_impl(i,compo); }
  inline double tau_tan(int i, int j) const { return static_cast<const DERIVED_T *>(this)->tau_tan_impl(i,j); }
  inline bool uses_wall_law() const { return static_cast<const DERIVED_T *>(this)->uses_wall(); }
  inline bool uses_mod_turb() const { return static_cast<const DERIVED_T *>(this)->uses_mod(); }
  inline const DoubleTab& k_elem() const { return static_cast<const DERIVED_T *>(this)->get_k_elem(); } // pour F5 seulement ...
};

#include <Eval_Diff_VDF_Face.tpp> // templates specializations ici ;)

#endif /* Eval_Diff_VDF_Face_included */
