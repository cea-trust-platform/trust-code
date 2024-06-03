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

#include <CoolProp_to_TRUST_generique.h>
#include <Fluide_generique_CoolProp.h>

Implemente_instanciable(Fluide_generique_CoolProp, "Fluide_generique_CoolProp", Fluide_generique_TPPI_base);

Sortie& Fluide_generique_CoolProp::printOn(Sortie& os) const { return os; }

Entree& Fluide_generique_CoolProp::readOn(Entree& is)
{
  Fluide_generique_TPPI_base::readOn(is);

  TPPI_ = std::make_shared<CoolProp_to_TRUST_generique>();

  if (model_name_ == "REFPROP") TPPI_->set_path_refprop();
  if (phase_ != "??") TPPI_->verify_phase(phase_);

  TPPI_->set_fluide_generique(model_name_, fluid_name_);
  if (phase_ != "??") TPPI_->set_phase(phase_);
//  CoolProptT.desactivate_handler(false); // throw on error

  tmin_ = TPPI_->tppi_get_T_min();
  tmax_ = TPPI_->tppi_get_T_max();
  pmin_ = TPPI_->tppi_get_p_min();
  pmax_ = TPPI_->tppi_get_p_max();

  return is;
}

void Fluide_generique_CoolProp::set_param(Param& param)
{
  Fluide_reel_base::set_param(param); // T_ref_ et P_ref_ ?? sais pas si utile ...
  param.ajouter("model|modele", &model_name_, Param::REQUIRED);
  param.ajouter("fluid|fluide", &fluid_name_, Param::REQUIRED);
  param.ajouter("phase", &phase_, Param::OPTIONAL); // optional : liquid or vapor. PI : specify the phase it is really useful (better perf for coolprop) !
}
