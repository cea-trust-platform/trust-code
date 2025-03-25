/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Pb_Thermohydraulique_IBM_Turbulent.h>
#include <Fluide_Incompressible.h>
#include <Verif_Cl.h>

Implemente_instanciable(Pb_Thermohydraulique_IBM_Turbulent, "Pb_Thermohydraulique_IBM_Turbulent", Pb_Fluide_base);
// XD Pb_Thermohydraulique_IBM_Turbulent Pb_base Pb_Thermohydraulique_IBM_Turbulent -1 Resolution of thermohydraulic problem, with turbulence modelling.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_ibm_turbulent navier_stokes_ibm_turbulent navier_stokes_ibm_turbulent 0 IBM Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_temperature_ibm_turbulent convection_diffusion_temperature_ibm_turbulent convection_diffusion_temperature_ibm_turbulent 0 Energy equation (temperature diffusion convection) as well as the associated turbulence model equations.

Sortie& Pb_Thermohydraulique_IBM_Turbulent::printOn(Sortie& os) const { return Pb_Fluide_base::printOn(os); }

Entree& Pb_Thermohydraulique_IBM_Turbulent::readOn(Entree& is) { return Pb_Fluide_base::readOn(is); }

int Pb_Thermohydraulique_IBM_Turbulent::nombre_d_equations() const
{
  return 2;
}

const Equation_base& Pb_Thermohydraulique_IBM_Turbulent::equation(int i) const
{
  if (!(i == 0 || i == 1))
    {
      Cerr << "\nError in Pb_Thermohydraulique_IBM_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;

}

Equation_base& Pb_Thermohydraulique_IBM_Turbulent::equation(int i)
{
  if (!(i == 0 || i == 1))
    {
      Cerr << "\nError in Pb_Thermohydraulique_IBM_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;
}

void Pb_Thermohydraulique_IBM_Turbulent::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_base, mil) && ref_cast(Fluide_base, mil).is_incompressible())
    {
      const Fluide_base& mi = ref_cast(Fluide_base,mil);
      eq_hydraulique.associer_milieu_base(mi);
      eq_thermique.associer_milieu_base(mi);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a " << finl;
      Cerr << "un probleme de type Pb_Thermohydraulique_IBM_Turbulent " << finl;
      exit();
    }
}

int Pb_Thermohydraulique_IBM_Turbulent::verifier()
{
  const Domaine_Cl_dis_base& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis_base& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  return 1;
}
