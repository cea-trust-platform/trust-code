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
// File:        Navier_Stokes_Fluide_Dilatable_Proto.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Navier_Stokes_Fluide_Dilatable_Proto_included
#define Navier_Stokes_Fluide_Dilatable_Proto_included

#include <Ref_IntVect.h>
#include <Champ_Inc.h>
#include <TRUSTTab.h>

class Fluide_Dilatable_base;
class Navier_Stokes_std;
class Matrice_Morse;
class Sortie;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Navier_Stokes_Fluide_Dilatable_Proto
//
//////////////////////////////////////

class Navier_Stokes_Fluide_Dilatable_Proto
{
public :
  Navier_Stokes_Fluide_Dilatable_Proto();
  int impr_impl(const Navier_Stokes_std& eqn,Sortie& os) const;
  void assembler_impl( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem);
  void assembler_avec_inertie_impl(const Navier_Stokes_std& eqn,Matrice_Morse& mat_morse,const DoubleTab& present,DoubleTab& secmem);
  DoubleTab& derivee_en_temps_inco_impl(Navier_Stokes_std&,DoubleTab& res);
  DoubleTab& rho_vitesse_impl(const DoubleTab& tab_rho,const DoubleTab& vit,DoubleTab& rhovitesse) const;

protected:
  Champ_Inc rho_la_vitesse_;
  IntVect orientation_VDF_;
  DoubleTab tab_W; // RHS of div(rho.U)

private:
  mutable double cumulative_;

  // private methods called from derivee_en_temps_inco_impl
  void prepare_and_solve_u_star(Navier_Stokes_std& eqn,const Fluide_Dilatable_base& fluide_dil,DoubleTab& rhoU,DoubleTab& vpoint);
  void update_vpoint_on_boundaries(const Navier_Stokes_std& eqn,const Fluide_Dilatable_base& fluide_dil,DoubleTab& vpoint);
  void solve_pressure_increment(Navier_Stokes_std& eqn,const Fluide_Dilatable_base& fluide_dil,DoubleTab& rhoU,
                                DoubleTab& secmem,DoubleTab& inc_pre,DoubleTab& vpoint );
  void correct_and_compute_u_np1(Navier_Stokes_std& eqn,const Fluide_Dilatable_base& fluide_dil,DoubleTab& rhoU,
                                 DoubleTab& Mmoins1grad,DoubleTab& inc_pre,DoubleTab& gradP,DoubleTab& vpoint);
};

#endif /* Navier_Stokes_Fluide_Dilatable_Proto_included */
