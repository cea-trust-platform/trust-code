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

#include <Fluide_generique_EOS.h>

Implemente_instanciable(Fluide_generique_EOS, "Fluide_generique_EOS", Fluide_reel_base);

Sortie& Fluide_generique_EOS::printOn(Sortie& os) const { return os; }

Entree& Fluide_generique_EOS::readOn(Entree& is)
{
  Fluide_reel_base::readOn(is);
  EOStT.verify_model_fluid(model_name_, fluid_name_);

  const int ind_model = EOStT.get_model_index(model_name_);
  const int ind_fluid = EOStT.get_fluid_index(model_name_, fluid_name_);

  assert(ind_model > -1 && ind_fluid > -1);

  // Lets start playing :-)
  const char *const model = EOStT.get_eos_model_name(ind_model);
  const char *const fld = EOStT.get_eos_fluid_name(model_name_, ind_fluid);

  EOStT.set_EOS_generique(model, fld);
  EOStT.desactivate_handler(false); // throw on error

  if (model_name_ == "CATHARE2")
    {
      tmin_ = EOStT.tppi_get_T_min();
      tmax_ = EOStT.tppi_get_T_max();
      pmin_ = EOStT.tppi_get_p_min();
      pmax_ = EOStT.tppi_get_p_max();
    }

  return is;
}

void Fluide_generique_EOS::set_param(Param& param)
{
  Fluide_reel_base::set_param(param); // T_ref_ et P_ref_ ?? sais pas si utile ...
  param.ajouter("model", &model_name_, Param::REQUIRED);
  param.ajouter("fluid", &fluid_name_, Param::REQUIRED);
}
