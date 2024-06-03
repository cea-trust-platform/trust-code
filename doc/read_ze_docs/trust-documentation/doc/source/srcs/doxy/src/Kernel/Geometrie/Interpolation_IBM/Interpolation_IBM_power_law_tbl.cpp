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

#include <Interpolation_IBM_power_law_tbl.h>

Implemente_instanciable( Interpolation_IBM_power_law_tbl, "Interpolation_IBM_power_law_tbl|IBM_power_law_tbl", Interpolation_IBM_elem_fluid ) ;
// XD interpolation_ibm_power_law_tbl interpolation_ibm_elem_fluid ibm_power_law_tbl 1 Immersed Boundary Method (IBM): power law interpolation.

Sortie& Interpolation_IBM_power_law_tbl::printOn( Sortie& os ) const
{
  Interpolation_IBM_elem_fluid::printOn( os );
  return os;
}

Entree& Interpolation_IBM_power_law_tbl::readOn( Entree& is )
{
  Param param(que_suis_je());
  set_param(param);
  // Interpolation_IBM_elem_fluid::readOn( is );
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interpolation_IBM_power_law_tbl::set_param(Param& param)
{
  Interpolation_IBM_elem_fluid::set_param( param );
  param.ajouter("formulation_linear_pwl",&formulation_linear_pwl_,Param::OPTIONAL);  // XD_ADD_P entier Choix formulation lineaire ou non
}
