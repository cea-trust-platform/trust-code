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

#ifndef Source_Flux_interfacial_base_included
#define Source_Flux_interfacial_base_included

#include <Sources_Multiphase_base.h>
#include <TRUST_Ref.h>

class Correlation;

/*! @brief Classe Source_Flux_interfacial_base
 *
 *    Cette classe implemente dans PolyMAC_P0P1NC un operateur de frottement interfacial
 *
 *     de la forme F_{kl} = - F_{lk} = - C_{kl} (u_k - u_l)
 *     le calcul de C_{kl} est realise par la hierarchie Coefficient_Flux_interfacial_base
 *
 * @sa Source_base
 */
class Source_Flux_interfacial_base : public Sources_Multiphase_base
{
  Declare_base(Source_Flux_interfacial_base);
public :
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  void mettre_a_jour(double temps) override;
  void completer() override;

  /* flux paroi-interface (par maille) et ses derivees */
  /* ces tableaux ne sont appelables que dans le terme associe a l'equation d'energie et sont utilises par les autres */
  /* ils peuvent etre remplis par l'operateur de diffusion (flux en paroi), un module de thermique crayon, etc */
  DoubleTab& qpi() const;    //qpi(e, k, l) : puissance deposee dans l'element e de la phase k vers l'interface (k, l) ; unite : W
  DoubleTab& dT_qpi() const; //dT_qpi(e, k, l, n) : sa derivee en T[n]
  DoubleTab& da_qpi() const; //da_qpi(e, k, l, n) : sa derivee en alpha[n]
  DoubleTab& dp_qpi() const; //da_qpi(e, k, l)    : sa derivee en p

private:
  mutable DoubleTab qpi_, dT_qpi_, da_qpi_, dp_qpi_;
  REF(Correlation) correlation_; //correlation donnant le coeff de flux interfacial
  int is_turb_ = 0;
  double dv_min = -1.;
};

#endif /* Source_Flux_interfacial_base_included */
