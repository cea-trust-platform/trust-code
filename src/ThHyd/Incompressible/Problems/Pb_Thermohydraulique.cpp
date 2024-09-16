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

#include <Pb_Thermohydraulique.h>
#include <Fluide_Ostwald.h>
#include <Verif_Cl.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Pb_Thermohydraulique, "Pb_Thermohydraulique", Pb_Hydraulique);
// XD pb_thermohydraulique Pb_base pb_thermohydraulique -1 Resolution of thermohydraulic problem.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 1 The fluid medium associated with the problem (only one possibility).
// XD   attr fluide_ostwald fluide_ostwald fluide_ostwald 1 The fluid medium associated with the problem (only one possibility).
// XD   attr fluide_sodium_liquide fluide_sodium_liquide fluide_sodium_liquide 1 The fluid medium associated with the problem (only one possibility).
// XD   attr fluide_sodium_gaz fluide_sodium_gaz fluide_sodium_gaz 1 The fluid medium associated with the problem (only one possibility).
// XD   attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD   attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equation (temperature diffusion convection).

Sortie& Pb_Thermohydraulique::printOn(Sortie& os) const { return Pb_Hydraulique::printOn(os); }
Entree& Pb_Thermohydraulique::readOn(Entree& is) { return Pb_Hydraulique::readOn(is); }

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Thermohydraulique::equation(int i) const
{
  if (i == 0) return eq_hydraulique;
  else if (i == 1) return eq_thermique;
  else if (i < 2 + eq_opt_.size() && i > 1) return eq_opt_[i - 2].valeur();
  else
    {
      Cerr << "Pb_Thermohydraulique::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_hydraulique;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Thermohydraulique::equation(int i)
{
  if (i == 0) return eq_hydraulique;
  else if (i == 1) return eq_thermique;
  else if (i < 2 + eq_opt_.size() && i > 1) return eq_opt_[i - 2].valeur();
  else
    {
      Cerr << "Pb_Thermohydraulique::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_hydraulique;
}

/*! @brief Associe le milieu au probleme Le milieu doit etre de type fluide incompressible
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Thermohydraulique::associer_milieu_base(const Milieu_base& mil)
{
  Pb_Hydraulique::associer_milieu_base(mil);
  if (sub_type(Fluide_base,mil) && ref_cast(Fluide_base, mil).is_incompressible())
    eq_thermique.associer_milieu_base(mil);
  else if (sub_type(Fluide_Ostwald,mil))
    eq_thermique.associer_milieu_base(mil);
}

/*! @brief Teste la compatibilite des equations de la thermique et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appel la fonction de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis_base&,const Domaine_Cl_dis_base&)
 *
 * @return (int) code de retour propage
 */
int Pb_Thermohydraulique::verifier()
{
  const Domaine_Cl_dis_base& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis_base& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  return tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
}



