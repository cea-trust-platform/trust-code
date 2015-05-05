/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_decalee_Robin.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_decalee_Robin.h>
#include <Equation_base.h>
#include <Navier_Stokes_Turbulent.h>
#include <Convection_Diffusion_Temperature_Turbulent.h>
#include <Param.h>


Implemente_instanciable(Paroi_decalee_Robin,"Paroi_decalee_Robin",Symetrie);


// printOn
Sortie& Paroi_decalee_Robin::printOn(Sortie& s) const
{
  return s << que_suis_je();
}


// readOn
Entree& Paroi_decalee_Robin::readOn(Entree& s)
{
  le_champ_front.typer("Champ_front_uniforme");
  le_champ_front->fixer_nb_comp(0);

  Param param(que_suis_je());
  param.ajouter("delta",&delta,Param::REQUIRED);
  param.lire_avec_accolades_depuis(s);
  return s;
}


// Cette condition aux limites ne supporte que les equations Navier_Stokes_turbulent et Convection_Diffusion_Temperature_Turbulent
int Paroi_decalee_Robin::compatible_avec_eqn(const Equation_base& eqn) const
{
  if (sub_type(Navier_Stokes_Turbulent,eqn) || sub_type(Convection_Diffusion_Temperature_Turbulent,eqn))
    {
      return 1;
    }
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}
