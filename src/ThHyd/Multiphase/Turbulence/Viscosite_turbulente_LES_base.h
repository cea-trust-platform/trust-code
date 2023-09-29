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

#ifndef Viscosite_turbulente_LES_base_included
#define Viscosite_turbulente_LES_base_included

#include <Viscosite_turbulente_base.h>

class Viscosite_turbulente_LES_base: public Viscosite_turbulente_base
{
  Declare_base(Viscosite_turbulente_LES_base);
public:
  void reynolds_stress(DoubleTab& R_ij) const override
  {
    Process::exit("Viscosite_turbulente_LES_base::reynolds_stress(...) should not be called !");
  }

  void k_over_eps(DoubleTab& k_sur_eps) const override
  {
    Process::exit("Viscosite_turbulente_LES_base::k_over_eps(...) should not be called !");
  }

  void eps(DoubleTab& eps) const override
  {
    Process::exit("Viscosite_turbulente_LES_base::eps(...) should not be called !");
  }

protected:
  double mod_const_ = -123.;
  DoubleVect l_;

private:
  void calculer_longueurs_caracteristiques();
};

#endif /* Viscosite_turbulente_LES_base_included */
