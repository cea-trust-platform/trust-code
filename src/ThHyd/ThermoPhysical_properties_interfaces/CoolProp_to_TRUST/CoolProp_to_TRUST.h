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

#ifndef CoolProp_to_TRUST_included
#define CoolProp_to_TRUST_included

#include <coolprop++.h>
#include <TPPI.h>

#ifdef HAS_COOLPROP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wshadow"
#include <AbstractState.h>
#include <CoolPropTools.h>
#include <CoolPropLib.h>
#include <CoolProp.h>
#pragma GCC diagnostic pop
#endif

namespace CoolProp { class AbstractState; }

class CoolProp_to_TRUST : public TPPI
{
public :
  ~CoolProp_to_TRUST() override;
  void desactivate_handler(bool op = true) override { /* Do nothing */ }
  void verify_model_fluid(const Motcle& model_name, const Motcle& fluid_name) override;

  int get_model_index(const Motcle& model_name) override;
  int get_fluid_index(const Motcle& model_name, const Motcle& fluid_name) override;
  const char* get_tppi_model_name(const int ind) override;
  const char* get_tppi_fluid_name(const Motcle& model_name, const int ind) override;

  double tppi_get_p_min() const override final;
  double tppi_get_p_max() const override final;

  double tppi_get_h_min() const override final;
  double tppi_get_h_max() const override final;

  double tppi_get_T_min() const override final;
  double tppi_get_T_max() const override final;

  double tppi_get_rho_min() const override final;
  double tppi_get_rho_max() const override final;

  double tppi_get_p_crit() const override final;
  double tppi_get_h_crit() const override final;
  double tppi_get_T_crit() const override final;
  double tppi_get_rho_crit() const override final;

protected:

#ifdef HAS_COOLPROP
  CoolProp::AbstractState * fluide = nullptr;
#endif

  CoolProp_Supported_Models_Fluids supp_;

};

#endif /* CoolProp_to_TRUST_included */
