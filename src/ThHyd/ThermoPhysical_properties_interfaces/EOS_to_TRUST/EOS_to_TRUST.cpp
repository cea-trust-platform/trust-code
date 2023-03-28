/****************************************************************************
 * Copyright (c) 2021, CEA
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

#include <EOS_to_TRUST.h>
#include <algorithm>
#include <Process.h>

using namespace NEPTUNE;

int EOS_to_TRUST::get_model_index(const Motcle& model_name)
{
  int ind_model = -1;
  for (int i = 0; i < (int) supp_.AVAIL_MODELS.size(); i++)
    if (supp_.AVAIL_MODELS[i] == model_name) ind_model = i;

  return ind_model;
}

int EOS_to_TRUST::get_fluid_index(const Motcle& model_name, const Motcle& fluid_name)
{
  int ind_fluid = -1;
  if (model_name == "CATHARE2")
    {
      for (int i = 0; i < (int) supp_.AVAIL_FLUIDS_C2.size(); i++)
        if (supp_.AVAIL_FLUIDS_C2[i] == fluid_name) ind_fluid = i;
    }
  else if (model_name == "FLICA4")
    {
      for (int i = 0; i < (int) supp_.AVAIL_FLUIDS_F4.size(); i++)
        if (supp_.AVAIL_FLUIDS_F4[i] == fluid_name) ind_fluid = i;
    }
  else if (model_name == "REFPROP10")
    {
      for (int i = 0; i < (int) supp_.AVAIL_FLUIDS_REFP10.size(); i++)
        if (supp_.AVAIL_FLUIDS_REFP10[i] == fluid_name) ind_fluid = i;
    }
  return ind_fluid;
}

const char* EOS_to_TRUST::get_eos_model_name(const int ind)
{
  return supp_.EOS_MODELS[ind];
}

const char* EOS_to_TRUST::get_eos_fluid_name(const Motcle& model_name, const int ind)
{
  if (model_name == "CATHARE2") return supp_.EOS_FLUIDS_C2[ind];
  else if (model_name == "FLICA4") return supp_.EOS_FLUIDS_F4[ind];
  else if (model_name == "REFPROP10") return supp_.EOS_FLUIDS_REFP10[ind];
  else return "NOT_A_FLUID";
}

void EOS_to_TRUST::verify_model_fluid(const Motcle& model_name, const Motcle& fluid_name)
{
  if (!(std::find(supp_.AVAIL_MODELS.begin(), supp_.AVAIL_MODELS.end(), model_name) != supp_.AVAIL_MODELS.end()))
    {
      Cerr << "You define the : < " << model_name << " model which is not yet tested !" << finl;
      Cerr << "Please use one of the available models or contact the TRUST team." << finl;
      Cerr << finl;
      Cerr << "Available Models : " << finl;
      for (const auto &itr : supp_.AVAIL_MODELS) Cerr << itr << finl;

      Process::exit();
    }

  if (model_name == "CATHARE2")
    {
      if (!(std::find(supp_.AVAIL_FLUIDS_C2.begin(), supp_.AVAIL_FLUIDS_C2.end(), fluid_name) != supp_.AVAIL_FLUIDS_C2.end()))
        {
          Cerr << "You define the : < " << fluid_name << " fluid which is not available !" << finl;
          Cerr << "Please use one of the available fluids or contact the TRUST team." << finl;
          Cerr << finl;
          Cerr << "Available Fluids : " << finl;
          for (const auto &itr : supp_.AVAIL_FLUIDS_C2) Cerr << itr << finl;

          Process::exit();
        }
    }
  else if (model_name == "FLICA4")
    {
      if (!(std::find(supp_.AVAIL_FLUIDS_F4.begin(), supp_.AVAIL_FLUIDS_F4.end(), fluid_name) != supp_.AVAIL_FLUIDS_F4.end()))
        {
          Cerr << "You define the : < " << fluid_name << " fluid which is not available !" << finl;
          Cerr << "Please use one of the available fluids or contact the TRUST team." << finl;
          Cerr << finl;
          Cerr << "Available Fluids : " << finl;
          for (const auto &itr : supp_.AVAIL_FLUIDS_F4) Cerr << itr << finl;

          Process::exit();
        }
    }
  else if (model_name == "REFPROP10")
    {
      if (!(std::find(supp_.AVAIL_FLUIDS_REFP10.begin(), supp_.AVAIL_FLUIDS_REFP10.end(), fluid_name) != supp_.AVAIL_FLUIDS_REFP10.end()))
        {
          Cerr << "You define the : < " << fluid_name << " fluid which is not available !" << finl;
          Cerr << "Please use one of the available fluids or contact the TRUST team." << finl;
          Cerr << finl;
          Cerr << "Available Fluids : " << finl;
          for (const auto &itr : supp_.AVAIL_FLUIDS_REFP10) Cerr << itr << finl;

          Process::exit();
        }
    }
  else
    Process::exit("What happened ?? Seems the method EOS_to_TRUST::verify_model_fluid is not up to date ! call the 911 !! ");
}

void EOS_to_TRUST::desactivate_handler(bool op)
{
#ifdef HAS_EOS
  if (op)
    {
      handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature); // EOS should never exit()
      handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature); // EOS should never throw() an exception
      handler.set_dump_on_error(EOS_Std_Error_Handler::disable_feature);
      // handler.set_dump_on_error(ok);
      // handler.set_dump_stream(cerr);
    }
  else
    {
      handler.set_exit_on_error(ok);
      handler.set_throw_on_error(ok);
      handler.set_dump_on_error(ok);
    }
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST::compute_eos_field(const SpanD P_ou_T, SpanD res,const char *const property_title, const char *const property_name, bool is_T) const
{
#ifdef HAS_EOS
  const char *const pp_ = is_T ? "Temperature" : "Pressure";
  const char *const tt_ = is_T ? "T" : "P";
  EOS_Field P_fld(pp_, tt_,  (int)P_ou_T.size(), (double*)P_ou_T.begin());
  EOS_Field z_fld(property_title,property_name, (int)res.size(), (double*)res.begin());
  ArrOfInt tmp((int)P_ou_T.size());
  EOS_Error_Field ferr(tmp);
  EOS_Error cr = fluide->compute(P_fld, z_fld, ferr);
  return (int)cr;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST::compute_eos_field(const SpanD P, const SpanD T, SpanD res,const char *const property_title, const char *const property_name) const
{
#ifdef HAS_EOS
  EOS_Field T_fld("Temperature", "T", (int)T.size(),(double*)T.begin()), P_fld("Pressure", "P", (int)P.size(), (double*)P.begin());
  EOS_Field z_fld(property_title,property_name, (int)res.size(), (double*)res.begin());
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Error cr = fluide->compute(P_fld, T_fld, z_fld, ferr);
  return (int)cr;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

int EOS_to_TRUST::compute_eos_field_h(const SpanD P, const SpanD H, SpanD res,const char *const property_title, const char *const property_name) const
{
#ifdef HAS_EOS
  EOS_Field H_fld("Enthalpy", "h", (int)H.size(),(double*)H.begin()), P_fld("Pressure", "P", (int)P.size(), (double*)P.begin());
  EOS_Field z_fld(property_title,property_name, (int)res.size(), (double*)res.begin());
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  EOS_Error cr = fluide->compute(P_fld, H_fld, z_fld, ferr);
  return (int)cr;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

MRange EOS_to_TRUST::all_unknowns_range()
{
#ifdef HAS_EOS
  if (tmax_ < -10.)
    {
      tmin_ = tppi_get_T_min();
      tmax_ = tppi_get_T_max();
      pmin_ = tppi_get_p_min();
      pmax_ = tppi_get_p_max();
      hmin_ = tppi_get_h_min();
      hmax_ = tppi_get_h_max();
      rhomin_ = tppi_get_rho_min();
      rhomax_ = tppi_get_rho_max();
    }

  return { { "temperature", { tmin_, tmax_}}, { "pression", { pmin_, pmax_}},
    { "enthalpie", { hmin_, hmax_}}, { "rho", { rhomin_, rhomax_}}
  };
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_p_min() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_p_min(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_p_max() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_p_max(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_h_min() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_h_min(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_h_max() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_h_max(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_T_min() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_T_min(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_T_max() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_T_max(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_rho_min() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_rho_min(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_rho_max() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_rho_max(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_p_crit() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_p_crit(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_h_crit() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_h_crit(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_T_crit() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_T_crit(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

double EOS_to_TRUST::tppi_get_rho_crit() const
{
#ifdef HAS_EOS
  double tmp_;
  fluide->get_rho_crit(tmp_);
  return tmp_;
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}
