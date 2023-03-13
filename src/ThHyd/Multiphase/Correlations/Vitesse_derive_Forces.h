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

#ifndef Vitesse_derive_Forces_included
#define Vitesse_derive_Forces_included

#include <Vitesse_derive_base.h>

/*! @brief classe Vitesse_derive_Forces
 *
 *  vitesse de derive entre une phase gaz et une phase liquide
 *      -> correlation de Ishii de la forme : v_g = C0 * (alpha_g * v_g + alpha_l * v_l) + v_g0
 *
 */

class Vitesse_derive_Forces : public Vitesse_derive_base
{
  Declare_instanciable(Vitesse_derive_Forces);

public:
  bool needs_grad_alpha() const override {return needs_grad_alpha_;};
  bool needs_vort() const override {return needs_vort_;};
  void completer() override;

protected:
  void evaluate_C0_vg0(const input_t& input) const override;

  // correlation parameters (circular channel)
  bool needs_grad_alpha_ = 0;
  bool needs_vort_ = 0;
  double alpha_lim_=1.e-5;
};

#endif /* Vitesse_derive_Forces_included */
