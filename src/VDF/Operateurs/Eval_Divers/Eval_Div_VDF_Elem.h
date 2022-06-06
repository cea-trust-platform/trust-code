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

#ifndef Eval_Div_VDF_Elem_included
#define Eval_Div_VDF_Elem_included

#include <CL_Types_include.h>
#include <Eval_VDF_Elem.h>
#include <Eval_Div_VDF.h>

// .DESCRIPTION class Eval_Div_VDF_Elem
// Evaluateur VDF pour la divergence
class Eval_Div_VDF_Elem : public Eval_Div_VDF, public Eval_VDF_Elem
{
public:
  // Overload Eval_VDF_Elem
  static constexpr bool CALC_FLUX_FACES_PAR = false, CALC_FLUX_FACES_PAR_FIXE = false, CALC_FLUX_FACES_SORTIE_LIB = true;

  /* ************************************** *
   * *********  POUR L'EXPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double> // Generic return
  inline void flux_face(const DoubleTab& inco, const int face, const BC&, const int , Type_Double& flux) const
  { for (int k=0; k<flux.size_array(); k++) flux[k] = inco(face,k)*surface(face)*porosite(face); }

  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int , const Symetrie&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int , const Dirichlet_paroi_fixe&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void flux_face(const DoubleTab&, const int , const Dirichlet_paroi_defilante&, const int, Type_Double& ) const { /* Do nothing */ }

  template <typename Type_Double>
  inline void flux_face(const DoubleTab& inco, const int boundary_index, const int face, const int local_face, const Echange_externe_impose&, const int, Type_Double& flux) const
  { for (int k=0; k<flux.size_array(); k++) flux[k] = inco(face,k)*surface(face)*porosite(face); }

  template <typename Type_Double>
  inline void flux_faces_interne(const DoubleTab& inco, const int face, Type_Double& flux) const
  { for (int k=0; k<flux.size_array(); k++) flux[k] = inco(face,k)*surface(face)*porosite(face); }

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double>
  inline void coeffs_face(const int, const int, const BC&, Type_Double& , Type_Double&  ) const { /* Do nothing */ }

  template <typename Type_Double> inline void coeffs_face(const int, const int, const int, const int, const Echange_externe_impose&, Type_Double& , Type_Double&  ) const { /* Do nothing */ }
  template <typename Type_Double> inline void coeffs_faces_interne(const int, Type_Double& , Type_Double&  ) const { /* Do nothing */ }

  /* ************************************** *
   * *********  POUR L'IMPLICITE ********** *
   * ************************************** */

  template <typename BC, typename Type_Double>
  inline void secmem_face(const int, const BC&, const int, Type_Double& ) const { /* Do nothing */ }

  template <typename Type_Double> inline void secmem_face(const int, const int, const int, const Echange_externe_impose&, const int, Type_Double& ) const { /* Do nothing */ }
  template <typename Type_Double> inline void secmem_faces_interne(const int, Type_Double& ) const { /* Do nothing */ }
};

#endif /* Eval_Div_VDF_Elem_included */
