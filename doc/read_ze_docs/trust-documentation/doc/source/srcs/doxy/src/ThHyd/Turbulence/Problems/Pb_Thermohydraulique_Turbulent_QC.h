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

#ifndef Pb_Thermohydraulique_Turbulent_QC_included
#define Pb_Thermohydraulique_Turbulent_QC_included

#include <Convection_Diffusion_Chaleur_Turbulent_QC.h>
#include <Navier_Stokes_Turbulent_QC.h>
#include <Pb_Dilatable_Proto.h>
#include <Pb_QC_base.h>

/*! @brief classe Pb_Thermohydraulique_Turbulent Cette classe represente un probleme de thermohydraulique en fluide quasi compressible
 *
 *     avec modelisation de la turbulence:
 *      - Equations de Navier_Stokes en regime turbulent
 *      - Equation d'energie en regime turbulent, sous forme generique (equation de la chaleur)
 *
 * @sa Probleme_base Pb_Thermohydraulique_QC Fluide_Quasi_Compressible
 */
class Pb_Thermohydraulique_Turbulent_QC : public Pb_QC_base, public Pb_Dilatable_Proto
{
  Declare_instanciable(Pb_Thermohydraulique_Turbulent_QC);

public:
  int verifier() override;
  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override;
  Equation_base& equation(int) override;
  int expression_predefini(const Motcle& motlu, Nom& expression) override;
  inline const Champ_Fonc& viscosite_turbulente() const { return eq_hydraulique.viscosite_turbulente(); }

protected:
  Navier_Stokes_Turbulent_QC eq_hydraulique;
  Convection_Diffusion_Chaleur_Turbulent_QC eq_thermique;
};

#endif /* Pb_Thermohydraulique_Turbulent_QC_included */
