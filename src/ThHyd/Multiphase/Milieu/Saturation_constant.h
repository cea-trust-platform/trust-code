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

#ifndef Saturation_constant_included
#define Saturation_constant_included

#include <Saturation_base.h>
#include <Double.h>

class Saturation_constant : public Saturation_base
{
  Declare_instanciable(Saturation_constant);

private:
  void Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void dP_Tsat_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override;
  void dT_Psat_(const SpanD T, SpanD res, int ncomp = 1, int ind = 0) const override;
  void Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void dP_Lvap_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void dP_Hls_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;
  void dP_Hvs_(const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  void sigma_(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const override;

  double tsat_ = -DMAXFLOAT, psat_ = -DMAXFLOAT, lvap_ = -DMAXFLOAT, hls_  = -DMAXFLOAT, hvs_  = -DMAXFLOAT;
};

#endif /* Saturation_constant_included */
