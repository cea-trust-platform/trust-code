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

#include <Pb_Thermohydraulique_Turbulent.h>
#include <Fluide_Incompressible.h>

Implemente_instanciable(Pb_Thermohydraulique_Turbulent, "Pb_Thermohydraulique_Turbulent", Pb_Fluide_base);
// XD pb_thermohydraulique_turbulent Pb_base pb_thermohydraulique_turbulent -1 Resolution of thermohydraulic problem, with turbulence modelling.
// XD attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_turbulent navier_stokes_turbulent navier_stokes_turbulent 0 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent convection_diffusion_temperature_turbulent 0 Energy equation (temperature diffusion convection) as well as the associated turbulence model equations.

Sortie& Pb_Thermohydraulique_Turbulent::printOn(Sortie& os) const { return Pb_Fluide_base::printOn(os); }

Entree& Pb_Thermohydraulique_Turbulent::readOn(Entree& is) { return Pb_Fluide_base::readOn(is); }

/*! @brief Renvoie le nombre d'equation, Renvoie 2 car il y a 2 equations a un probleme de
 *
 *     thermo-hydraulique turbulent:
 *      - l'equation de Navier Stokes
 *      - l'equation de la thermique de type Convection_Diffusion_Temperature_Turbulent
 *
 * @return (int) le nombre d'equation
 */
int Pb_Thermohydraulique_Turbulent::nombre_d_equations() const
{
  return 2;
}

const Equation_base& Pb_Thermohydraulique_Turbulent::equation(int i) const
{
  if (!(i == 0 || i == 1))
    {
      Cerr << "\nError in Pb_Thermohydraulique_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;

}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_Turbulent si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature_Turbulent si i=1
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Thermohydraulique_Turbulent::equation(int i)
{
  if (!(i == 0 || i == 1))
    {
      Cerr << "\nError in Pb_Thermohydraulique_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;
}

/*! @brief Associe le milieu au probleme Le milieu doit etre de type fluide incompressible
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Thermohydraulique_Turbulent::associer_milieu_base(const Milieu_base& mil)
{
  if sub_type(Fluide_Incompressible,mil)
    {
      const Fluide_base& mi = ref_cast(Fluide_base,mil);
      eq_hydraulique.associer_milieu_base(mi);
      eq_thermique.associer_milieu_base(mi);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a " << finl;
      Cerr << "un probleme de type Pb_Thermohydraulique_Turbulent " << finl;
      exit();
    }
}
