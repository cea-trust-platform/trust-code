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

#ifndef CoolProp_to_TRUST_generique_included
#define CoolProp_to_TRUST_generique_included

#include <CoolProp_to_TRUST.h>

/*! @brief classe CoolProp_to_TRUST_generique
 *
 *  Interface commune pour TRUST et ses baltiks qui permet appeler les methodes de la lib CoolProp
 *  Methods disponibles pour les fluids en temperature et enthalpie
 *
 *  @sa CoolProp_to_TRUST CoolProp_Supported_Models_Fluids
 */
class CoolProp_to_TRUST_generique: public CoolProp_to_TRUST
{
public:
  void set_fluide_generique(const char *const model_name, const char *const fluid_name) override;

  // methodes particulieres par application pour gagner en performance : utilisees dans Pb_Multiphase (pour le moment !)
  int tppi_get_CPMLB_pb_multiphase_pT(const MSpanD, MLoiSpanD, int ncomp = 1, int id = 0) const override;
  int tppi_get_all_pb_multiphase_pT(const MSpanD, MLoiSpanD, MLoiSpanD, int ncomp = 1, int id = 0) const override;

  // methode particuliere par application pour gagner en performance : utilisee dans F5 (pour le moment !)
  int tppi_get_all_prop_loi_F5(const MSpanD, MLoiSpanD_h, int ncomp = 1, int id = 0, bool is_liq = true) const override;

private:
  int tppi_get_single_property_T_(Loi_en_T, const SpanD, const SpanD, SpanD, int, int) const override;
  int tppi_get_single_property_T__(Loi_en_T, const SpanD, const SpanD, SpanD) const;
  int tppi_get_all_properties_T__(const MSpanD input, MLoiSpanD prop) const;
  int tppi_get_all_properties_T_IF97__(const MSpanD input, MLoiSpanD prop) const;
  int FD_derivative_pT__(Loi_en_T, const SpanD, const SpanD, SpanD, bool wrt_p = true) const;

  int tppi_get_single_property_h_(Loi_en_h, const SpanD, const SpanD, SpanD, int, int) const override;
  int tppi_get_single_property_h__(Loi_en_h, const SpanD, const SpanD, SpanD) const;
  int tppi_get_all_properties_h__(const MSpanD input, MLoiSpanD_h prop) const;
  int tppi_get_all_properties_h_IF97__(const MSpanD input, MLoiSpanD_h prop) const;
  int FD_derivative_ph__(Loi_en_h, const SpanD, const SpanD, SpanD, bool wrt_p = true) const;
};

#endif /* CoolProp_to_TRUST_generique_included */
