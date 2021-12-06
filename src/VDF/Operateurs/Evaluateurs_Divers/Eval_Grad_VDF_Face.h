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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Grad_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Grad_VDF_Face_included
#define Eval_Grad_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <CL_Types_Aretes_enum.h>
#include <Eval_VDF_Face.h>
#include <Eval_Grad_VDF.h>
#include <type_traits>

// .DESCRIPTION class Eval_Grad_VDF_Face
// Evaluateur VDF pour le gradient
// .SECTION voir aussi Eval_Grad_VDF
class Eval_Grad_VDF_Face : public Eval_Grad_VDF, public Eval_VDF_Face
{
public:
  static constexpr bool CALC_FA7_SORTIE_LIB = true, CALC_ARR_FL = false, CALC_ARR_BORD = false, CALC_ARR_INT = false, CALC_ARR_MIXTE = false,
                        CALC_ARR_PERIO = false, CALC_ARR_PAR_FL = false, CALC_ARR_PAR = false, CALC_ARR_SYMM_PAR = false, CALC_ARR_SYMM_FL = false;

  /* ****************************************************************************** *
   * YES, we do magic ! Its all about : Substition Failure Is Not An Error (SFINAE) *
   * ****************************************************************************** */

  /* ******************* *
   * CAS SCALAIRE : FLUX *
   * ******************* */

  // TODO : FIXME : a faire void un jour !!!!!!
  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, double>::type
  flux_fa7(const DoubleTab&, int face, const Neumann_sortie_libre& la_cl, int num1) const { return la_cl.flux_impose(face-num1)*surface(face); }

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, double>::type
  flux_fa7(const DoubleTab& inco, int elem, int fac1, int) const { return inco(elem)*surface(fac1); }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, double>::type
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, double>::type
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, double>::type
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, double>::type
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>::type
  flux_arete(const DoubleTab&, int, int, int, int) const { return 0; }

  // void
  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, double&, double&) const  { /* do nothing */ }

  /* ********************* *
   * CAS SCALAIRE : COEFFS *
   * ********************* */

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>::type
  coeffs_fa7(int, const Neumann_sortie_libre&, double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, void>::type
  coeffs_fa7(int, int, int, double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, void>::type
  coeffs_arete(int, int, int, int, double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, void>::type
  coeffs_arete(int, int, int, int, double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  coeffs_arete(int, int, int, int, double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }

  /* ********************* *
   * CAS SCALAIRE : SECMEM *
   * ********************* */

  // TODO : FIXME : a faire void un jour !!!!!!
  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, double>::type
  secmem_fa7(int, const Neumann_sortie_libre&, int ) const { return 0; }

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, double>::type
  secmem_fa7( int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, double>::type
  secmem_arete(int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, double>::type
  secmem_arete(int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, double>::type
  secmem_arete(int, int, int, int ) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, double>::type
  secmem_arete(int, int, int, int) const { return 0; }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, double>::type
  secmem_arete(int, int, int, int ) const { return 0; }

  // void
  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  secmem_arete(int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  secmem_arete(int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  secmem_arete(int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  secmem_arete(int, int, int, int, double&, double&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  secmem_arete(int, int, int, int, double&, double&) const { /* do nothing */ }

  /* ******************** *
   * CAS VECTORIEL : FLUX *
   * ******************** */

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>::type
  flux_fa7(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& ) const;

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, void>::type
  flux_fa7(const DoubleTab&, int, int, int, DoubleVect& ) const;

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& ,DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect& ,DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  flux_arete(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  /* ********************** *
   * CAS VECTORIEL : COEFFS *
   * ********************** */

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>::type
  coeffs_fa7(int , const Neumann_sortie_libre&, DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, void>::type
  coeffs_fa7(int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  coeffs_arete(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  /* ********************** *
   * CAS VECTORIEL : SECMEM *
   * ********************** */

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>::type
  secmem_fa7(int , const Neumann_sortie_libre&, int, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Fa7 Fa7_Type>
  inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, void>::type
  secmem_fa7(int, int, int, DoubleVect& flux) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::INTERNE, void>::type
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::MIXTE, void>::type
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI, void>::type
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE, void>::type
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_PAROI, void>::type
  secmem_arete(int, int, int, int, DoubleVect& ) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::FLUIDE, void>::type
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>::type
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PERIODICITE, void>::type
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>::type
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  template<Type_Flux_Arete Arete_Type>
  inline typename std::enable_if< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>::type
  secmem_arete(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
};

template<Type_Flux_Fa7 Fa7_Type>
inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>::type
Eval_Grad_VDF_Face::flux_fa7(const DoubleTab& , int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

template<Type_Flux_Fa7 Fa7_Type>
inline typename std::enable_if< Fa7_Type == Type_Flux_Fa7::ELEM, void>::type
Eval_Grad_VDF_Face::flux_fa7(const DoubleTab& inco, int elem, int fac1, int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = inco(elem,k)*surface(fac1);
}

#endif /* Eval_Grad_VDF_Face_included */
