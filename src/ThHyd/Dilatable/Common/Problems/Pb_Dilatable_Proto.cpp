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

#include <Pb_Dilatable_Proto.h>
#include <Verif_Cl.h>

const Equation_base& Pb_Dilatable_Proto::equation_impl(int i, const Equation_base& eq1, const Equation_base& eq2) const
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "Error in Pb_Dilatable_Proto::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  return (i == 0) ? eq1 : eq2;
}

Equation_base& Pb_Dilatable_Proto::equation_impl(int i, Equation_base& eq1, Equation_base& eq2)
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "Error in Pb_Dilatable_Proto::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  return (i == 0) ? eq1 : eq2;
}

int Pb_Dilatable_Proto::verifier_impl(const Equation_base& eq1, const Equation_base& eq2, const bool is_thermal)
{
  const Zone_Cl_dis& zone_Cl_1 = eq1.zone_Cl_dis(), zone_Cl_2 = eq2.zone_Cl_dis();
  return is_thermal == true ? tester_compatibilite_hydr_thermique(zone_Cl_1,zone_Cl_2) :
         tester_compatibilite_hydr_fraction_massique(zone_Cl_1, zone_Cl_2);
}
