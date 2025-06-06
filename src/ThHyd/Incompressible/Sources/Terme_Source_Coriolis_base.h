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

#ifndef Terme_Source_Coriolis_base_included
#define Terme_Source_Coriolis_base_included

#include <Source_base.h>
#include <TRUSTVect.h>
#include <TRUST_Ref.h>

class Navier_Stokes_std;
class Param;

class Terme_Source_Coriolis_base: public Source_base
{
  Declare_base(Terme_Source_Coriolis_base);
public:

  inline void associer_eqn(const Navier_Stokes_std& eq_hyd) { eq_hydraulique_ = eq_hyd; }
  inline const DoubleVect& omega() const { return omega_; }
  inline const Navier_Stokes_std& eq_hydraulique() const { return eq_hydraulique_.valeur(); }
  void mettre_a_jour(double temps) override { }

protected :
  DoubleVect omega_;
  OBS_PTR(Navier_Stokes_std) eq_hydraulique_;
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
};

#endif
