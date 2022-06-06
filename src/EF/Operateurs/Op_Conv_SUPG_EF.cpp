/****************************************************************************
* Copyright (c) 2017, CEA
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

#include <Op_Conv_SUPG_EF.h>
#include <Param.h>

Implemente_instanciable( Op_Conv_SUPG_EF, "Op_Conv_SUPG_EF", Op_Conv_BTD_EF ) ;

Sortie& Op_Conv_SUPG_EF::printOn( Sortie& os ) const
{
  Op_Conv_BTD_EF::printOn( os );
  return os;
}

Entree& Op_Conv_SUPG_EF::readOn( Entree& is )
{
  btd_=3; // utilise dans calculer_pas_de_temps
  Param param(que_suis_je()); // XD convection_supg convection_deriv supg 1 Only for EF discretization.
  param.ajouter("facteur",&ksupg_,Param::REQUIRED); // XD_ADD_P double not_set

  param.lire_avec_accolades(is);
  return is;
}

double Op_Conv_SUPG_EF::coefficient_btd() const
{
  //double dt=equation().schema_temps().pas_de_temps();
  //double f=btd*dt/2;
  // f *  (2.0/DT) * (min_dx_/max_ue_);
  double f=ksupg_*(min_dx_/max_ue_);

  return f;
}
