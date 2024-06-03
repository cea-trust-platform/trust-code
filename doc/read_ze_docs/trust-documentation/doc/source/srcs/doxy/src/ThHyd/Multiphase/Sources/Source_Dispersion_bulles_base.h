/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Source_Dispersion_bulles_base_included
#define Source_Dispersion_bulles_base_included

#include <Sources_Multiphase_base.h>
#include <Correlation.h>
#include <TRUST_Ref.h>

/*! @brief Classe Source_Dispersion_bulles_base
 *
 *  Cette classe implemente un operateur de dispersion turbulente
 *
 *       F_{kl} = - F_{lk} = - C_{kl} grad(alpha{k}) + C_{lk} grad(alpha{l}) ou la phase
 *       l est la phase liquide porteuse et k != 0 une phase quelconque
 *     le calcul de C_{n_l, k} est realise par la hierarchie Dispersion_turbulente_base
 *
 * @sa Source_base
 */
class Source_Dispersion_bulles_base: public Sources_Multiphase_base
{
  Declare_base(Source_Dispersion_bulles_base);
public :
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override = 0;
  const Correlation& correlation() const {return correlation_;};

protected:
  REF(Correlation) correlation_; //correlation donnant le coeff de dispersion turbulente
  int is_turb = 0;
  double beta_ = 1.; // To adjust the force in .data

  virtual void dimensionner_blocs_aux(IntTrav&) const = 0;
};

#endif /* Source_Dispersion_bulles_base_included */
