/****************************************************************************
* Copyright (c) 2017, CEA
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

#include <Pb_Thermohydraulique_Turbulent_QC.h>
#include <Les_mod_turb.h>
#include <Verif_Cl.h>
#include <Verif_Cl_Turb.h>

Implemente_instanciable(Pb_Thermohydraulique_Turbulent_QC,"Pb_Thermohydraulique_Turbulent_QC",Pb_QC_base);

Sortie& Pb_Thermohydraulique_Turbulent_QC::printOn(Sortie& os) const { return Probleme_base::printOn(os); }

Entree& Pb_Thermohydraulique_Turbulent_QC::readOn(Entree& is) { return Probleme_base::readOn(is); }

/*! @brief Renvoie 2 car il y a 2 equations : Navier_Stokes_Turbulent_QC et Convection_Diffusion_Chaleur_Turbulent_QC
 *
 */
int Pb_Thermohydraulique_Turbulent_QC::nombre_d_equations() const { return 2; }

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_Turbulent_QC si i=0 Renvoie l'equation de chaleur de type Convection_Diffusion_Chaleur_Turbulent_QC si i=1
 *
 */
const Equation_base& Pb_Thermohydraulique_Turbulent_QC::equation(int i) const
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

Equation_base& Pb_Thermohydraulique_Turbulent_QC::equation(int i)
{
  return equation_impl(i,eq_hydraulique,eq_thermique);
}

/*! @brief Teste la compatibilite des equations de la fraction massique et de l'hydraulique.
 *
 * Teste la compatibilite des modeles de turbulence
 *
 */
int Pb_Thermohydraulique_Turbulent_QC::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_thermique.domaine_Cl_dis();

  // Verification de la compatibilite des conditions aux limites:
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  if ( sub_type(Mod_turb_hyd_RANS, eq_hydraulique.get_modele(TURBULENCE).valeur() ) )
    {
      const Mod_turb_hyd_RANS& le_mod_RANS = ref_cast(Mod_turb_hyd_RANS, eq_hydraulique.get_modele(TURBULENCE).valeur());
      const Transport_K_Eps_base& eqn = ref_cast(Transport_K_Eps_base, le_mod_RANS.eqn_transp_K_Eps());
      const Domaine_Cl_dis& domaine_Cl_turb = eqn.domaine_Cl_dis();
      tester_compatibilite_hydr_turb(domaine_Cl_hydr, domaine_Cl_turb);
    }

  // Verification de la compatibilite des modeles de turbulence:
  const Mod_turb_hyd& le_mod_turb_hyd = eq_hydraulique.modele_turbulence();
  const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eq_thermique.get_modele(TURBULENCE).valeur());

  if  (sub_type(Modele_turbulence_hyd_K_Eps,le_mod_turb_hyd.valeur()))
    {
      if (!sub_type(Modele_turbulence_scal_Prandtl,le_mod_turb_th))
        {
          Cerr << "Les modeles de turbulence ne sont pas de la meme famille" << finl;
          Cerr << "pour l'hydraulique et la thermique" << finl;
          exit();
        }
    }
  return 1;
}

int Pb_Thermohydraulique_Turbulent_QC::expression_predefini(const Motcle& motlu, Nom& expression)
{
  if (motlu=="VISCOSITE_TURBULENTE")
    {
      expression  = "predefini { pb_champ ";
      expression += le_nom();
      expression += " viscosite_turbulente } ";
      return 1;
    }
  else
    return Pb_QC_base::expression_predefini(motlu,expression);
}
