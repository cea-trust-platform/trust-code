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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_front_debit.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_debit.h>
#include <Champ_Don.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Zone_VF.h>
#include <TRUSTTrav.h>
Implemente_instanciable(Champ_front_debit,"Champ_front_debit",Champ_front_xyz_debit);
// XD champ_front_debit front_field_base champ_front_debit 0 This field is used to define a flow rate field instead of a velocity field for a Dirichlet boundary condition on Navier-Stokes equations.
// XD attr ch front_field_base ch 0 uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_txyz that depends only on time.
Sortie& Champ_front_debit::printOn(Sortie& os) const
{
  return Champ_front_xyz_debit::printOn(os);
}

Entree& Champ_front_debit::readOn(Entree& is)
{
  flow_rate_alone_=1;
  is >> flow_rate_;
  fixer_nb_comp(dimension * flow_rate_.nb_comp());
  return is;
}

// Description:
// Mise a jour du temps
int Champ_front_debit::initialiser(double tps, const Champ_Inc_base& inco)
{
//  Cerr << "Champ_front_debit::initialiser" << finl;
  Champ_front_xyz_debit::initialiser(tps, inco);
  return 1;
}

