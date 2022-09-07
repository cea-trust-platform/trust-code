/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Saturation_base_included
#define Saturation_base_included

#include <Interface_base.h>

class Saturation_base : public Interface_base
{
  Declare_base(Saturation_base);
public:
  void set_param(Param& param) override;
  inline double get_Pref() const { return P_ref_; }

  void Tsat(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void dP_Tsat(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void Psat(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const;
  void dT_Psat(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const;
  void Lvap(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void dP_Lvap(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void Hls(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void dP_Hls(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void Hvs(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;
  void dP_Hvs(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;

  // Methods that can be called if point-to-point calculation is required
  double Tsat(const double P) const { return double_to_span<&Saturation_base::Tsat_>(P); }
  double dP_Tsat(const double P) const { return double_to_span<&Saturation_base::dP_Tsat_>(P); }
  double Psat(const double T) const { return double_to_span<&Saturation_base::Psat_>(T); }
  double dT_Psat(const double T) const { return double_to_span<&Saturation_base::dT_Psat_>(T); }
  double Lvap(const double P) const { return double_to_span<&Saturation_base::Lvap_>(P); }
  double dP_Lvap(const double P) const { return double_to_span<&Saturation_base::dP_Lvap_>(P); }
  double Hls(const double P) const { return double_to_span<&Saturation_base::Hls_>(P); }
  double dP_Hls(const double P) const { return double_to_span<&Saturation_base::dP_Hls_>(P); }
  double Hvs(const double P) const { return double_to_span<&Saturation_base::Hvs_>(P); }
  double dP_Hvs(const double P) const { return double_to_span<&Saturation_base::dP_Hvs_>(P); }

protected:
  double P_ref_ = -1, T_ref_ = -1;

private:
  typedef void(Saturation_base::*function_span_generic)(const SpanD , SpanD , int , int ) const;

  template <function_span_generic FUNC>
  void double_to_span(const double T_ou_P, SpanD res, int ncomp, int ind) const
  {
    ArrayD Pp = {T_ou_P}, res_ = {0.};
    (this->*FUNC)(SpanD(Pp), SpanD(res_),1,0); // fill res_
    for (int i =0; i < (int)res.size()/ncomp; i++) res[i * ncomp + ind] = res_[0]; // fill res
  }

  template <function_span_generic FUNC>
  double double_to_span(const double T_ou_P) const
  {
    ArrayD Pp = {T_ou_P}, res_ = {0.};
    (this->*FUNC)(SpanD(Pp), SpanD(res_),1,0); // fill res_
    return res_[0];
  }

  virtual void Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void dP_Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void dT_Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void dP_Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void dP_Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
  virtual void dP_Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;

  void _Tsat_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::Tsat_>(P,res,ncomp,ind); }
  void _dP_Tsat_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::dP_Tsat_>(P,res,ncomp,ind); }
  void _Psat_(const double T, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::Psat_>(T,res,ncomp,ind); }
  void _dT_Psat_(const double T, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::dT_Psat_>(T,res,ncomp,ind); }
  void _Lvap_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::Lvap_>(P,res,ncomp,ind); }
  void _dP_Lvap_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::dP_Lvap_>(P,res,ncomp,ind); }
  void _Hls_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::Hls_>(P,res,ncomp,ind); }
  void _dP_Hls_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::dP_Hls_>(P,res,ncomp,ind); }
  void _Hvs_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::Hvs_>(P,res,ncomp,ind); }
  void _dP_Hvs_(const double P, SpanD res, int ncomp = 1, int ind = 0) const { double_to_span<&Saturation_base::dP_Hvs_>(P,res,ncomp,ind); }
};

#endif /* Saturation_base_included */
