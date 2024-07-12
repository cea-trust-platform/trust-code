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

#ifndef EOS_to_TRUST_included
#define EOS_to_TRUST_included

#include <eos++.h>
#include <TPPI.h>

#ifdef HAS_EOS
#pragma GCC diagnostic push
#if __GNUC__ > 5 || __clang_major__ > 10
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <EOS/API/EOS_Std_Error_Handler.hxx>
#include <EOS/API/EOS.hxx>
#pragma GCC diagnostic pop
#endif

namespace NEPTUNE { class EOS; }

/*! @brief classe EOS_to_TRUST
 *
 *  Interface commune pour TRUST et ses baltiks qui permet appeler les methodes de la lib EOS
 *  Methods disponibles en temperature et enthalpie, egalement pour la saturation
 */
class EOS_to_TRUST : public TPPI
{
public :
  ~EOS_to_TRUST() override { /* delete fluide; */ }

  void desactivate_handler(bool op = true) override;

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

  MRange all_unknowns_range() override final;

protected :

#ifdef HAS_EOS
  NEPTUNE::EOS_Std_Error_Handler handler ;
  NEPTUNE::EOS *fluide = nullptr;
#endif

  int compute_eos_field(const SpanD P, SpanD res, const char *const pt, const char *const pn, bool is_T = false) const;
  int compute_eos_field(const SpanD P, const SpanD T, SpanD res, const char *const pt, const char *const pn) const;
  int compute_eos_field_h(const SpanD P, const SpanD H, SpanD res, const char *const pt, const char *const pn) const;

private:

#ifdef HAS_EOS
  double tmin_ = -123., tmax_ = -123., pmin_ = -123., pmax_ = -123.;
  double hmin_ = -123., hmax_ = -123., rhomin_ = -123., rhomax_ = -123.;
#endif
};

#endif /* EOS_to_TRUST_included */
