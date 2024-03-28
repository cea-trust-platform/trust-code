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

#include <Pb_Thermohydraulique_Multi_Concentration.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Constituant.h>
#include <Verif_Cl.h>
#include <EChaine.h>
#include <sstream>
#include <iomanip>

Implemente_instanciable(Pb_Thermohydraulique_Multi_Concentration, "Pb_Thermohydraulique_Multi_Concentration", Pb_Thermohydraulique);
// XD Pb_Thermohydraulique_Multi_Concentration Pb_base Pb_Thermohydraulique_Multi_Concentration -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD   attr constituant constituant constituant 1 Constituents.
// XD   attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD   attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport equations (concentration diffusion convection).
// XD   attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equation (temperature diffusion convection).

Sortie& Pb_Thermohydraulique_Multi_Concentration::printOn(Sortie& os) const { return Pb_Thermohydraulique::printOn(os); }
Entree& Pb_Thermohydraulique_Multi_Concentration::readOn(Entree &is)
{
  /* Step 1 : Add first equation to list ! */
  Cerr << "Adding the first Convection_Diffusion_Concentration to the list ... " << finl;
  list_eq_concentration_.add(Convection_Diffusion_Concentration());
  list_eq_concentration_.dernier().associer_pb_base(*this);
  list_eq_concentration_.dernier().associer_sch_tps_base(le_schema_en_temps_);
  list_eq_concentration_.dernier().associer_domaine_dis(domaine_dis());

  /* Step 2 : Go to Probleme_base::readOn !! */
  return Pb_Thermohydraulique::readOn(is);
}

void Pb_Thermohydraulique_Multi_Concentration::typer_lire_milieu(Entree& is)
{
  le_milieu_.resize(2);
  for (int i = 0; i < 2; i++) is >> le_milieu_[i];
  associer_milieu_base(le_milieu_.front().valeur()); // NS : On l'associe a chaque equations (methode virtuelle pour chaque pb ...)

  /*
   * XXX : Elie Saikali :
   *  - le_milieu_[0] => Fluide incompressible et le_milieu_[1] => constituants (n compos).
   *  - le_milieu_[1] pas associe a l'equation car n compos
   *  - mil_constituants_ contient le milieu associe a chaque equation
   */
  const Constituant& les_consts = ref_cast(Constituant, le_milieu_.back().valeur());
  const DoubleTab &vals = les_consts.diffusivite_constituant()->valeurs();
  const int nb_consts = les_consts.nb_constituants();
  mil_constituants_.resize(nb_consts);

  if (!sub_type(Champ_Uniforme, les_consts.diffusivite_constituant().valeur()))
    {
      Cerr << "Error in Pb_Thermohydraulique_Multi_Concentration::typer_lire_milieu. You can not use a diffusion coefficient of type " << les_consts.diffusivite_constituant()->que_suis_je() << " !!!" << finl;
      Cerr << "We only accept uniform fields for the moment ... Fix your data set !!!" << finl;
      Process::exit();
    }

  for (int i = 0; i < nb_consts; i++)
    {
      std::ostringstream oss;
      oss << std::scientific << std::setprecision(15) << vals(0, i); // Setting precision to 3 decimal places

      Nom str = "Constituant { coefficient_diffusion Champ_Uniforme 1 ";
      str += oss.str().c_str();
      str += " }";

      EChaine const1(str);
      const1 >> mil_constituants_[i];
    }
  associer_milieu_base(mil_constituants_[0].valeur()); // 1er eq concentration pour le moment

  // Milieu(x) lu(s) ... Lets go ! On discretise les equations
  discretiser_equations();

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_milieu_equation();

  equation(0).milieu().discretiser((*this), la_discretisation_.valeur()); // NS
  list_eq_concentration_.dernier().milieu().discretiser((*this), la_discretisation_.valeur()); // Conc
}

Entree& Pb_Thermohydraulique_Multi_Concentration::lire_equations(Entree& is, Motcle& dernier_mot)
{
  Nom un_nom;
  const int nb_eq = nombre_d_equations();
  Cerr << "Reading of the equations" << finl;

  for (int i = 0; i < nb_eq; i++)
    {
      is >> un_nom;
      is >> getset_equation_by_name(un_nom);
    }

  is >> dernier_mot;
  return is;
}

const Equation_base& Pb_Thermohydraulique_Multi_Concentration::equation(int i) const
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Thermohydraulique_Multi_Concentration::equation => wrong equation number !");

  if (i < 2)
    return Pb_Thermohydraulique::equation(i);
  else
    return list_eq_concentration_(i - 2);
}

Equation_base& Pb_Thermohydraulique_Multi_Concentration::equation(int i)
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Thermohydraulique_Multi_Concentration::equation => wrong equation number !");

  if (i < 2)
    return Pb_Thermohydraulique::equation(i);
  else
    return list_eq_concentration_(i - 2);
}

/*! @brief Associe un milieu au probleme, Si le milieu est de type
 *
 *       - Fluide_Incompressible, il sera associe a l'equation de l'hydraulique et a l'equation d'energie.
 *       - Constituant, il sera associe a l'equation de convection-diffusion
 *     Un autre type de milieu provoque une erreur
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Thermohydraulique_Multi_Concentration::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Constituant, mil))
    list_eq_concentration_.dernier().associer_milieu_base(mil);
  else
    Pb_Thermohydraulique::associer_milieu_base(mil);
}

/*! @brief Teste la compatibilite des equations de convection-diffusion et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appels aux fonctions de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *       tester_compatibilite_hydr_concentration(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *
 * @return (int) code de retour propage
 */
int Pb_Thermohydraulique_Multi_Concentration::verifier()
{
  Pb_Thermohydraulique::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = list_eq_concentration_.front().domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
}



