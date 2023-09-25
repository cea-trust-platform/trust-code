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

#ifndef Interface_base_included
#define Interface_base_included

#include <TRUST_Ref.h>
#include <Champ_Don.h>
#include <span.hpp>
#include <Param.h>

class Probleme_base;

using VectorD = std::vector<double>;
using ArrayD = std::array<double,1>;
using SpanD = tcb::span<double>;

class Interface_base : public Objet_U
{
  Declare_base(Interface_base);
public:
  virtual void set_param(Param& param);
  virtual void mettre_a_jour(double , int , int );
  void assoscier_pb(const Probleme_base& pb) { pb_ = pb; }

  Champ_Don& get_sigma_champ() { return ch_sigma_; }
  const Champ_Don& get_sigma_champ() const { return ch_sigma_; }

  DoubleTab& get_sigma_tab() { return ch_sigma_->valeurs(); }
  const DoubleTab& get_sigma_tab() const { return ch_sigma_->valeurs(); }

  double sigma(const double T, const double P) const; // can be called if point-to-point calculation is required
  void sigma(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const;

protected:
  REF(Probleme_base) pb_;
  Champ_Don ch_sigma_;
  double sigma__ = -1.;

  virtual void sigma_(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int ind = 0) const = 0;
};

#endif /* Interface_base_included */
