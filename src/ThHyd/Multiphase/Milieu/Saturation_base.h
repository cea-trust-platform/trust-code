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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Saturation_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Saturation_base_included
#define Saturation_base_included
#include <Param.h>
#include <Interface_base.h>

class Saturation_base : public Interface_base
{
  Declare_base(Saturation_base);
public:

  void set_param(Param& param) override;

  double    Tsat(const double P) const;
  double dP_Tsat(const double P) const;
  double    Psat(const double T) const;
  double dT_Psat(const double T) const;
  double    Lvap(const double P) const;
  double dP_Lvap(const double P) const;
  double     Hls(const double P) const;
  double  dP_Hls(const double P) const;
  double     Hvs(const double P) const;
  double  dP_Hvs(const double P) const;

  virtual double    Tsat_(const double P) const = 0;
  virtual double dP_Tsat_(const double P) const = 0;
  virtual double    Psat_(const double T) const = 0;
  virtual double dT_Psat_(const double T) const = 0;
  virtual double    Lvap_(const double P) const = 0;
  virtual double dP_Lvap_(const double P) const = 0;
  virtual double     Hls_(const double P) const = 0;
  virtual double  dP_Hls_(const double P) const = 0;
  virtual double     Hvs_(const double P) const = 0;
  virtual double  dP_Hvs_(const double P) const = 0;
  double get_Pref() const;

protected:
  double P_ref_ = -1, T_ref_ = -1;
};

#endif
