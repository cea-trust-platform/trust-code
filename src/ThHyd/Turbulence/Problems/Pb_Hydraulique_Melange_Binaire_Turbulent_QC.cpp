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

#include <Pb_Hydraulique_Melange_Binaire_Turbulent_QC.h>

Implemente_instanciable(Pb_Hydraulique_Melange_Binaire_Turbulent_QC, "Pb_Hydraulique_Melange_Binaire_Turbulent_QC", Pb_QC_base);
// XD pb_hydraulique_melange_binaire_turbulent_qc Pb_base pb_hydraulique_melange_binaire_turbulent_qc -1 Resolution of a turbulent binary mixture problem for a quasi-compressible fluid with an iso-thermal condition.
// XD attr fluide_quasi_compressible fluide_quasi_compressible fluide_quasi_compressible 0 The fluid medium associated with the problem.
// XD attr navier_stokes_turbulent_qc navier_stokes_turbulent_qc navier_stokes_turbulent_qc 0 Navier-Stokes equation for a quasi-compressible fluid as well as the associated turbulence model equations.
// XD attr Convection_Diffusion_Espece_Binaire_Turbulent_QC Convection_Diffusion_Espece_Binaire_Turbulent_QC Convection_Diffusion_Espece_Binaire_Turbulent_QC 0 Species conservation equation for a quasi-compressible fluid as well as the associated turbulence model equations.

Sortie& Pb_Hydraulique_Melange_Binaire_Turbulent_QC::printOn(Sortie& os) const { return Probleme_base::printOn(os); }

Entree& Pb_Hydraulique_Melange_Binaire_Turbulent_QC::readOn(Entree& is) { return Probleme_base::readOn(is); }

/*! @brief Renvoie 2 car il y a 2 equations : Navier_Stokes_Turbulent_QC et Convection_Diffusion_Espece_Binaire_Turbulent_QC
 *
 */
int Pb_Hydraulique_Melange_Binaire_Turbulent_QC::nombre_d_equations() const
{
  return 2;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_Turbulent_QC si i=0 Renvoie l'equation de conv/diff fraction massique de type Convection_Diffusion_Espece_Binaire_Turbulent_QC si i=1
 *
 */
const Equation_base& Pb_Hydraulique_Melange_Binaire_Turbulent_QC::equation(int i) const
{
  return equation_impl(i, eq_hydraulique, eq_frac_mass);
}

Equation_base& Pb_Hydraulique_Melange_Binaire_Turbulent_QC::equation(int i)
{
  return equation_impl(i, eq_hydraulique, eq_frac_mass);
}

int Pb_Hydraulique_Melange_Binaire_Turbulent_QC::expression_predefini(const Motcle& motlu, Nom& expression)
{
  if (motlu == "VISCOSITE_TURBULENTE")
    {
      expression = "predefini { pb_champ ";
      expression += le_nom();
      expression += " viscosite_turbulente } ";
      return 1;
    }
  else
    return Pb_QC_base::expression_predefini(motlu, expression);
}
