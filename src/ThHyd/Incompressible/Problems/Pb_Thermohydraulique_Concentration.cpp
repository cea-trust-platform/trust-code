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

#include <Pb_Thermohydraulique_Concentration.h>
#include <Fluide_Incompressible.h>
#include <Constituant.h>
#include <Verif_Cl.h>

Implemente_instanciable(Pb_Thermohydraulique_Concentration, "Pb_Thermohydraulique_Concentration", Pb_Thermohydraulique);
// XD pb_thermohydraulique_concentration Pb_base pb_thermohydraulique_concentration -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD   attr constituant constituant constituant 1 Constituents.
// XD   attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD   attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport equations (concentration diffusion convection).
// XD   attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equation (temperature diffusion convection).

Sortie& Pb_Thermohydraulique_Concentration::printOn(Sortie& os) const { return Pb_Thermohydraulique::printOn(os); }
Entree& Pb_Thermohydraulique_Concentration::readOn(Entree& is) { return Pb_Thermohydraulique::readOn(is); }

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *     Renvoie l'equation de la thermique de type
 *     Convection_Diffusion_Concentration si i=2
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Thermohydraulique_Concentration::equation(int i) const
{
  if (i == 2) return eq_concentration;
  return Pb_Thermohydraulique::equation(i);
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *     Renvoie l'equation de la thermique de type
 *     Convection_Diffusion_Concentration si i=2
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Thermohydraulique_Concentration::equation(int i)
{
  if (i == 2) return eq_concentration;
  return Pb_Thermohydraulique::equation(i);
}

/*! @brief Associe un milieu au probleme, Si le milieu est de type
 *
 *       - Fluide_Incompressible, il sera associe a l'equation de l'hydraulique
 *         et a l'equation d'energie.
 *       - Constituant, il sera associe a l'equation de convection-diffusion
 *     Un autre type de milieu provoque une erreur
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Thermohydraulique_Concentration::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Constituant, mil))
    eq_concentration.associer_milieu_base(mil);
  else
    Pb_Thermohydraulique::associer_milieu_base(mil);
}

/*! @brief Teste la compatibilite des equations de convection-diffusion et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appels aux fonctions de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis_base&,const Domaine_Cl_dis_base&)
 *       tester_compatibilite_hydr_concentration(const Domaine_Cl_dis_base&,const Domaine_Cl_dis_base&)
 *
 * @return (int) code de retour propage
 */
int Pb_Thermohydraulique_Concentration::verifier()
{
  Pb_Thermohydraulique::verifier();
  const Domaine_Cl_dis_base& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis_base& domaine_Cl_co = eq_concentration.domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
}
