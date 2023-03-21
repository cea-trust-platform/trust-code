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

#include <Saturation_generique_EOS.h>

Implemente_instanciable( Saturation_generique_EOS, "Saturation_generique_EOS", Saturation_base);

Sortie& Saturation_generique_EOS::printOn( Sortie& os ) const { return os; }

Entree& Saturation_generique_EOS::readOn( Entree& is )
{
  Param param(que_suis_je());
  param.ajouter("model",&model_name_,Param::REQUIRED);
  param.ajouter("fluid",&fluid_name_,Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  EOStT.verify_model_fluid(model_name_,fluid_name_);

  int ind_model = -1, ind_fluid = -1;

  for (int i = 0; i < (int) EOStT.supp.AVAIL_MODELS.size(); i++)
    if (EOStT.supp.AVAIL_MODELS[i] == model_name_) ind_model = i;

  for (int i = 0; i < (int) EOStT.supp.AVAIL_FLUIDS.size(); i++)
    if (EOStT.supp.AVAIL_FLUIDS[i] == fluid_name_) ind_fluid = i;

  assert (ind_model > -1 && ind_model < (int)EOStT.supp.AVAIL_MODELS.size());
  assert (ind_fluid > -1 && ind_fluid < (int)EOStT.supp.AVAIL_FLUIDS.size());

  // Lets start playing :-)
  const char *const model = EOStT.supp.EOS_MODELS[ind_model];
  const char *const fld = EOStT.supp.EOS_FLUIDS[ind_fluid];
  EOStT.set_EOS_Sat_generique(model,fld);
  EOStT.desactivate_handler(false); // throw on error

  return is;
}
