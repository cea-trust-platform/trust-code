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

#include <Pb_Thermohydraulique_Concentration_Turbulent.h>
#include <Fluide_Incompressible.h>
#include <Constituant.h>
#include <Verif_Cl.h>
#include <Verif_Cl_Turb.h>
#include <Les_mod_turb.h>

Implemente_instanciable(Pb_Thermohydraulique_Concentration_Turbulent,"Pb_Thermohydraulique_Concentration_Turbulent",Pb_Fluide_base);


/*! @brief Simple appel a: Pb_Fluide_base::printOn(Sortie&) Ecrit le probleme sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Pb_Thermohydraulique_Concentration_Turbulent::printOn(Sortie& os) const
{
  return Pb_Fluide_base::printOn(os);
}


/*! @brief Simple appel a: Pb_Fluide_base::readOn(Entree&) Lit le probleme a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Pb_Thermohydraulique_Concentration_Turbulent::readOn(Entree& is)
{
  return Pb_Fluide_base::readOn(is);
}

/*! @brief Renvoie le nombre d'equation, Renvoie 3 car il y a 2 equations a un probleme de
 *
 *     thermo-hydraulique avec concentration:
 *         - l'equation de Navier Stokes turbulent
 *         - equation d'energie en regime turbulent
 *         - une equation de convection-diffusion turbulente
 *
 * @return (int) le nombre d'equations
 */
int Pb_Thermohydraulique_Concentration_Turbulent::nombre_d_equations() const
{
  return 3;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_Turbulent si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature_Turbulent si i=1
 *     Renvoie l'equation de la thermique de type
 *     Convection_Diffusion_Concentration_Turbulent si i=2
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Thermohydraulique_Concentration_Turbulent::equation(int i) const
{
  if ( !( i==0 || i==1 || i==2 ) )
    {
      Cerr << "\nError in Pb_Thermohydraulique_Concentration_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else if (i == 1)
    return eq_thermique;
  else
    return eq_concentration;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_Turbulent si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature_Turbulent si i=1
 *     Renvoie l'equation de la thermique de type
 *     Convection_Diffusion_Concentration_Turbulent si i=2
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Thermohydraulique_Concentration_Turbulent::equation(int i)
{
  if ( !( i==0 || i==1 || i==2 ) )
    {
      Cerr << "\nError in Pb_Thermohydraulique_Concentration_Turbulent::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else if (i == 1)
    return eq_thermique;
  else
    return eq_concentration;
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
void Pb_Thermohydraulique_Concentration_Turbulent::
associer_milieu_base(const Milieu_base& mil)
{
  if ( sub_type(Fluide_Incompressible,mil) )
    {
      eq_hydraulique.associer_milieu_base(mil);
      eq_thermique.associer_milieu_base(mil);
    }
  else if ( sub_type(Constituant,mil) )
    eq_concentration.associer_milieu_base(mil);
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a " << finl;
      Cerr << "un probleme de type Pb_Thermohydraulique_Concentration_Turbulent " << finl;
      exit();
    }
}


/*! @brief Teste la compatibilite des equations de convection-diffusion et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation ainsi que sur
 *     les modeles de turbulence des equations qui doivent etre
 *     de la meme famille.
 *     Appels aux fonctions de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *       tester_compatibilite_hydr_concentration(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *
 * @return (int) renvoie toujours 1
 * @throws les modeles de turbulence ne sont pas de la meme
 * famille pour l'hydraulique et la thermique
 * @throws les modeles de turbulence ne sont pas de la meme
 * famille pour l'hydraulique et l'equation de concentration
 */
int Pb_Thermohydraulique_Concentration_Turbulent::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = eq_concentration.domaine_Cl_dis();

  // Verification de la compatibilite des conditions aux limites
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  tester_compatibilite_hydr_concentration(domaine_Cl_hydr,domaine_Cl_co);
  if ( sub_type(Mod_turb_hyd_RANS, eq_hydraulique.get_modele(TURBULENCE).valeur() ))
    {
      const Mod_turb_hyd_RANS& le_mod_RANS = ref_cast(Mod_turb_hyd_RANS, eq_hydraulique.get_modele(TURBULENCE).valeur());
      const Transport_K_Eps_base& eqn = ref_cast(Transport_K_Eps_base, le_mod_RANS.eqn_transp_K_Eps());
      const Domaine_Cl_dis& domaine_Cl_turb = eqn.domaine_Cl_dis();
      tester_compatibilite_hydr_turb(domaine_Cl_hydr, domaine_Cl_turb);
    }

  // Verification de la compatibilite des modeles de turbulence
  const Mod_turb_hyd& le_mod_turb_hyd = eq_hydraulique.modele_turbulence();
  const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eq_thermique.get_modele(TURBULENCE).valeur());
  const Modele_turbulence_scal_base& le_mod_turb_co = ref_cast(Modele_turbulence_scal_base,eq_concentration.get_modele(TURBULENCE).valeur());
  if ((sub_type(Mod_turb_hyd_ss_maille,le_mod_turb_hyd.valeur()))  || (sub_type(Modele_turbulence_hyd_K_Eps,le_mod_turb_hyd.valeur())))
    {
      if (!sub_type(Modele_turbulence_scal_Prandtl,le_mod_turb_th))
        {
          Cerr << "Les modeles de turbulence ne sont pas de la meme famille" << finl;
          Cerr << "pour l'hydraulique et la thermique" << finl;
          exit();
        }
      if (!sub_type(Modele_turbulence_scal_Schmidt,le_mod_turb_co))
        {
          Cerr << "Les modeles de turbulence ne sont pas de la meme famille" << finl;
          Cerr << "pour l'hydraulique et l'equation de concentration" << finl;
          if (sub_type(Modele_turbulence_scal_Prandtl,le_mod_turb_co))
            {
              Cerr << "Pour le modele de turbulence de l'equation de concentration, la syntaxe a changee:" << finl;
              Cerr << "Utiliser le mot cle Schmidt au lieu du mot cle Prandtl." << finl;
            }
          exit();
        }

    }
  return 1;
}
