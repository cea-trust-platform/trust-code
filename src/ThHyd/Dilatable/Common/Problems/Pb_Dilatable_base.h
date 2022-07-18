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

#ifndef Pb_Dilatable_base_included
#define Pb_Dilatable_base_included

#include <Ref_Fluide_Dilatable_base.h>
#include <Pb_Fluide_base.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Pb_Dilatable_base
//    Cette classe est censee factoriser ce qui est commun a l'ensemble
//    des problemes dilatables.
//
// .SECTION voir aussi Pb_Fluide_base
//
//////////////////////////////////////////////////////////////////////////////

class Pb_Dilatable_base : public Pb_Fluide_base
{
  Declare_base(Pb_Dilatable_base);

public:
  bool initTimeStep(double dt) override;
  void preparer_calcul() override;
  void mettre_a_jour(double temps) override; // Ne met a jour que les postraitements
  void associer_milieu_base(const Milieu_base& ) override;
  void associer_sch_tps_base(const Schema_Temps_base&) override;
  virtual void update_pressure_fields(double );
  bool iterateTimeStep(bool& converged) override;
  virtual void solve_pressure_thermo() = 0;

protected :
  REF(Fluide_Dilatable_base) le_fluide_;
};

#endif /* Pb_Dilatable_base_included */
