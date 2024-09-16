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

#ifndef Op_Diff_PolyVEF_P0P1_Elem_included
#define Op_Diff_PolyVEF_P0P1_Elem_included

#include <Op_Diff_PolyMAC_P0P1NC_Elem.h>
#include <Matrice_Morse.h>

/*! @brief : class Op_Diff_PolyVEF_P0P1_Elem
 *
 *  <Description of class Op_Diff_PolyVEF_P0P1_Elem>
 *
 *
 *
 */

class Op_Diff_PolyVEF_P0P1_Elem : public Op_Diff_PolyMAC_P0P1NC_Elem
{

  Declare_instanciable_sans_constructeur( Op_Diff_PolyVEF_P0P1_Elem ) ;

public :
  Op_Diff_PolyVEF_P0P1_Elem();
  void completer() override;
  // virtual void calculer_flux_bord(const DoubleTab& inco) const { abort(); };

  /* interface {dimensionner,ajouter}_blocs */
  double calculer_dt_stab() const override;
  void dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;

  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const override { };

  void set_multiscalar(int multi) override { multiscalar_ = multi; }
};

#endif /* Op_Diff_PolyVEF_P0P1_Elem_included */
