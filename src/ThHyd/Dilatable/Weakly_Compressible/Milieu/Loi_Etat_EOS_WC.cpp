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

#include <EOS_to_TRUST_generique.h>
#include <Loi_Etat_EOS_WC.h>

Implemente_instanciable(Loi_Etat_EOS_WC, "Loi_Etat_EOS_WC", Loi_Etat_TPPI_WC_base);
// XD EOS_WC loi_etat_tppi_base EOS_WC 1 Class for using EOS with WC problem
// XD attr Cp double Cp 0 Specific heat at constant pressure (J/kg/K).
// XD attr fluid chaine fluid 0 Fluid name in the EOS model
// XD attr model chaine model 0 EOS model name

Sortie& Loi_Etat_EOS_WC::printOn(Sortie& os) const { return os << que_suis_je() << finl; }

Entree& Loi_Etat_EOS_WC::readOn(Entree& is)
{
  Cerr << "Lecture de la loi d'etat EOS_WC ... " << finl;
  Loi_Etat_TPPI_WC_base::readOn(is);

  TPPI_ = std::make_shared<EOS_to_TRUST_generique>();
  TPPI_->set_fluide_generique(model_name_, fluid_name_);
  TPPI_->desactivate_handler(false); // throw on error

  return is;
}
