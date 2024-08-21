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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Robin_VEF.cpp
// Directory : $TRUST_ROOT/src/Kernel/Cond_Lim
//
/////////////////////////////////////////////////////////////////////////////


#include <Robin_VEF.h>
#include <Param.h>

Implemente_instanciable( Robin_VEF, "Robin_VEF", Cond_lim_base ) ;

Sortie& Robin_VEF::printOn( Sortie& os ) const
{
  Cond_lim_base::printOn( os );
  return os;
}

Entree& Robin_VEF::readOn( Entree& is )
{
  Param param(que_suis_je());
  // reading Robin border conditions parameters

  param.ajouter("alpha", 		&alpha_robin_cl_, 		Param::REQUIRED);
  param.ajouter("beta" , 		&beta_robin_cl_ , 		Param::REQUIRED);
  param.ajouter("champ_front_normal_et_tangentiel_robin",&le_champ_front,Param::REQUIRED); // Champ_front_tangentiel_robin::readOn(is);
  param.lire_avec_accolades_depuis(is);


  Cerr << "Reading Robin boundary condition, the value of the normal Robin coefficient is  "
       << alpha_robin_cl_ << " and the tangential Robin coefficient is  " << beta_robin_cl_ << finl;

  if (alpha_robin_cl_==0 || beta_robin_cl_==0)
    {
      Cerr << "Error of Robin boundary conditions : Alpha and Beta parameters should not be zero  " << finl;
      exit() ;
    }


  return is;
}

int Robin_VEF::compatible_avec_eqn( const Equation_base& eqn) const
{
  // TOD : if we need to do some verification ?
  return 1;
}
