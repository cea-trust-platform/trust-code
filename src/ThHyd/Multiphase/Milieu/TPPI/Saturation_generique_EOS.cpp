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

#include <EOS_to_TRUST_Sat_generique.h>
#include <Saturation_generique_EOS.h>

Implemente_instanciable(Saturation_generique_EOS, "Saturation_generique_EOS", Saturation_generique_TPPI_base);

Sortie& Saturation_generique_EOS::printOn(Sortie& os) const { return os; }

Entree& Saturation_generique_EOS::readOn(Entree& is)
{
  Saturation_generique_TPPI_base::readOn(is);

  TPPI_ = std::make_shared<EOS_to_TRUST_Sat_generique>();

  TPPI_->set_saturation_generique(model_name_, fluid_name_);
  TPPI_->desactivate_handler(false); // throw on error
  TPPI_->set_user_uniform_sigma(user_uniform_sigma_);
  return is;
}

void Saturation_generique_EOS::set_param(Param& param)
{
  Saturation_base::set_param(param); // T_ref_ et P_ref_ ?? sais pas si utile ...
  param.ajouter("model|modele", &model_name_, Param::REQUIRED);
  param.ajouter("fluid|fluide", &fluid_name_, Param::REQUIRED);
  param.ajouter("user_uniform_sigma", &user_uniform_sigma_, Param::OPTIONAL); // optional : because of issues when we call surface tension in TTSE in coolprop ! Try without and if calculation doesn't pass, input sigma
}
