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

#include <Pb_Multiphase_Enthalpie.h>

Implemente_instanciable(Pb_Multiphase_Enthalpie, "Pb_Multiphase_Enthalpie|Pb_Multiphase_h", Pb_Multiphase);

Sortie& Pb_Multiphase_Enthalpie::printOn(Sortie& os) const { return Pb_Multiphase::printOn(os); }

Entree& Pb_Multiphase_Enthalpie::readOn(Entree& is) { return Pb_Multiphase::readOn(is); }

void Pb_Multiphase_Enthalpie::associer_milieu_base(const Milieu_base& mil)
{
  equation_qdm().associer_milieu_base(mil);
  eq_energie_enthalpie_.associer_milieu_base(mil);
  equation_masse().associer_milieu_base(mil);
}

int Pb_Multiphase_Enthalpie::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = equation_qdm().domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_energie_enthalpie_.domaine_Cl_dis();
  return tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
}
