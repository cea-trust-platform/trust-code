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

#ifndef Pb_Thermohydraulique_Turbulent_included
#define Pb_Thermohydraulique_Turbulent_included

#include <Convection_Diffusion_Temperature_Turbulent.h>
#include <Navier_Stokes_Turbulent.h>
#include <Pb_Fluide_base.h>

class Champ_Fonc;

/*! @brief classe Pb_Thermohydraulique_Turbulent Cette classe represente un probleme de thermohydraulique
 *
 *     avec modelisation de la turbulence:
 *      - Equations de Navier_Stokes en regime turbulent
 *        pour un fluide incompressible
 *      - Equation d'energie en regime turbulent
 *     L'hydraulique et la thermique sont couplees par des forces de volume
 *     (approximation de Boussinesq)
 *
 * @sa Pb_Fluide_base Pb_Thermohydraulique Terme_Boussinesq_base
 */
class Pb_Thermohydraulique_Turbulent: public Pb_Fluide_base
{
  Declare_instanciable(Pb_Thermohydraulique_Turbulent);
public:
  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override;
  Equation_base& equation(int) override;
  inline const Champ_Fonc& viscosite_turbulente() const { return eq_hydraulique.viscosite_turbulente(); }
  void associer_milieu_base(const Milieu_base&) override;
  int verifier() override;

protected:
  Navier_Stokes_Turbulent eq_hydraulique;
  Convection_Diffusion_Temperature_Turbulent eq_thermique;
};

#endif /* Pb_Thermohydraulique_Turbulent_included */
