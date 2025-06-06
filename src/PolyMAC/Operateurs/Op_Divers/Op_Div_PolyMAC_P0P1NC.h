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

#ifndef Op_Div_PolyMAC_P0P1NC_included
#define Op_Div_PolyMAC_P0P1NC_included

#include <Op_Div_PolyMAC.h>

/*! @brief class Op_Div_PolyMAC_P0P1NC
 *
 *   Cette classe represente l'operateur de divergence La discretisation est PolyMAC_P0P1NC
 *   On calcule la divergence d'un champ_P1NC (la vitesse)
 *
 * @sa Op_Div_PolyMAC
 *
 */
class Op_Div_PolyMAC_P0P1NC: public Op_Div_PolyMAC
{
  Declare_instanciable(Op_Div_PolyMAC_P0P1NC);
public:
  /* interface ajouter_blocs */
  int has_interface_blocs() const override { return 1; }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = { }) const override;
  void ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = { }) const override;
};

#endif /* Op_Div_PolyMAC_P0P1NC_included */
