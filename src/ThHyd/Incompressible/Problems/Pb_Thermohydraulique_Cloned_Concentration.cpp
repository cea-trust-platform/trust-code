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

#include <Pb_Thermohydraulique_Cloned_Concentration.h>
#include <Fluide_Incompressible.h>
#include <Op_Diff_negligeable.h>
#include <Champ_Uniforme.h>
#include <Operateur_Diff.h>
#include <Constituant.h>
#include <Verif_Cl.h>
#include <EChaine.h>
#include <sstream>
#include <iomanip>

Implemente_instanciable(Pb_Thermohydraulique_Cloned_Concentration, "Pb_Thermohydraulique_Cloned_Concentration", Pb_Thermohydraulique);
// XD Pb_Thermohydraulique_Cloned_Concentration Pb_base Pb_Thermohydraulique_Cloned_Concentration -1 Resolution of Navier-Stokes/energy/multiple constituent transport equations.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD   attr constituant constituant constituant 1 Constituents.
// XD   attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD   attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport equations (concentration diffusion convection).
// XD   attr convection_diffusion_temperature convection_diffusion_temperature convection_diffusion_temperature 1 Energy equation (temperature diffusion convection).

Sortie& Pb_Thermohydraulique_Cloned_Concentration::printOn(Sortie& os) const { return Pb_Thermohydraulique::printOn(os); }
Entree& Pb_Thermohydraulique_Cloned_Concentration::readOn(Entree& is)
{
  /* Step 1 : Add first equation to list ! */
  Cerr << "Adding the first Convection_Diffusion_Concentration to the list ... " << finl;
  list_eq_concentration_.add(Convection_Diffusion_Concentration());
  Convection_Diffusion_Concentration& eqn = list_eq_concentration_.dernier();
  eqn.associer_pb_base(*this);
  eqn.associer_sch_tps_base(le_schema_en_temps_);
  eqn.associer_domaine_dis(domaine_dis());

  /* Step 2 : Go to Probleme_base::readOn !! */
  return Pb_Thermohydraulique::readOn(is);
}

void Pb_Thermohydraulique_Cloned_Concentration::typer_lire_milieu(Entree& is)
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
  const DoubleTab& vals = les_consts.diffusivite_constituant()->valeurs();
  nb_consts_ = les_consts.nb_constituants();
  mil_constituants_.resize(nb_consts_);

  if (!sub_type(Champ_Uniforme, les_consts.diffusivite_constituant().valeur()))
    {
      Cerr << "Error in Pb_Thermohydraulique_Cloned_Concentration::typer_lire_milieu. You can not use a diffusion coefficient of type " << les_consts.diffusivite_constituant()->que_suis_je() << " !!!" << finl;
      Cerr << "We only accept uniform fields for the moment ... Fix your data set !!!" << finl;
      Process::exit();
    }

  for (int i = 0; i < nb_consts_; i++)
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

  const Nom nom_const = nb_consts_ > 1 ? "coefficient_diffusion0" : "coefficient_diffusion";
  ref_cast(Constituant, list_eq_concentration_.dernier().milieu()).discretiser_multi_concentration(nom_const, (*this), la_discretisation_.valeur()); // Conc
}

Entree& Pb_Thermohydraulique_Cloned_Concentration::lire_equations(Entree& is, Motcle& dernier_mot)
{
  rename_equation_unknown(0);
  Pb_Thermohydraulique::lire_equations(is, dernier_mot);
  clone_equations();
  return is;
}

void Pb_Thermohydraulique_Cloned_Concentration::rename_equation_unknown(const int i)
{
  if (nb_consts_ > 1)
    {
      Convection_Diffusion_Concentration& eqn = list_eq_concentration_.dernier();
      Nom nom_inco = "concentration", nom_eq = "Convection_Diffusion_Concentration";
      nom_inco += Nom(i), nom_eq += Nom(i);
      Cerr << "The unknown name of the Convection_Diffusion_Concentration equation " << i << " is modified => " << nom_inco << finl;
      eqn.inconnue()->nommer(nom_inco);
      Cerr << "The Convection_Diffusion_Concentration equation name, of number " << i << ", is modified => " << nom_eq << finl;
      eqn.nommer(nom_eq);
    }
}

void Pb_Thermohydraulique_Cloned_Concentration::clone_equations()
{
  if (nb_consts_ > 1)
    {
      Cerr << "Cloning " << nb_consts_ << " concentration equations in progress ..." << finl;
      for (int i = 1; i < nb_consts_; i++)
        {
          Cerr << "Adding another Convection_Diffusion_Concentration to the list ... " << finl;
          list_eq_concentration_.add(list_eq_concentration_.front());
          Convection_Diffusion_Concentration& eqn = list_eq_concentration_.dernier();
          eqn.associer_pb_base(*this);
          eqn.associer_sch_tps_base(le_schema_en_temps_);
          eqn.associer_domaine_dis(domaine_dis());

          associer_milieu_base(mil_constituants_[i].valeur()); // 1er eq concentration pour le moment
          eqn.associer_milieu_equation();
          const Nom nom_const = Nom("coefficient_diffusion") + Nom(i);
          ref_cast(Constituant, eqn.milieu()).discretiser_multi_concentration(nom_const, (*this), la_discretisation_.valeur()); // Conc

          rename_equation_unknown(i); // XXX

          eqn.get_champ_compris().clear_champs_compris();
          eqn.get_champ_compris().ajoute_champ(eqn.inconnue());

          // XXX : Elie Saikali : that is REQUIRED !!!! Otherwise we use the operators of the original equation !!!
          for (int op = 0; op < eqn.nombre_d_operateurs(); op++)
            {
              eqn.operateur(op).associer_eqn(eqn);
              eqn.operateur(op).l_op_base().associer_eqn(eqn);
            }

          Operateur_Diff& op_diff = dynamic_cast<Operateur_Diff&>(eqn.operateur(0));
          op_diff.associer_diffusivite(eqn.diffusivite_pour_transport());
          op_diff->associer_diffusivite(eqn.diffusivite_pour_transport());
          op_diff.associer_diffusivite_pour_pas_de_temps(eqn.diffusivite_pour_pas_de_temps());
          op_diff->associer_diffusivite_pour_pas_de_temps(eqn.diffusivite_pour_pas_de_temps());

          Nom nom_fich = "Diffusion_";
          nom_fich += eqn.inconnue().le_nom();
          op_diff.set_fichier(nom_fich);
          op_diff.set_description((Nom) "Diffusion mass transfer rate=Integral(alpha*grad(C)*ndS) [m" + (Nom) (dimension + bidim_axi) + ".Mol.s-1]");

          Operateur_Conv& op_conv = dynamic_cast<Operateur_Conv&>(eqn.operateur(1));
          const Champ_base& ch_vitesse_transportante = eqn.vitesse_pour_transport();
          eqn.associer_vitesse(ch_vitesse_transportante);
          op_conv->associer_vitesse(ch_vitesse_transportante);

          // Pareil ici !!!
          eqn.solv_masse().associer_eqn(eqn);
          eqn.solv_masse()->associer_eqn(eqn);
        }
    }
}

const Equation_base& Pb_Thermohydraulique_Cloned_Concentration::equation(int i) const
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Thermohydraulique_Cloned_Concentration::equation => wrong equation number !");

  if (i < 2)
    return Pb_Thermohydraulique::equation(i);
  else
    return list_eq_concentration_(i - 2);
}

Equation_base& Pb_Thermohydraulique_Cloned_Concentration::equation(int i)
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Thermohydraulique_Cloned_Concentration::equation => wrong equation number !");

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
void Pb_Thermohydraulique_Cloned_Concentration::associer_milieu_base(const Milieu_base& mil)
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
int Pb_Thermohydraulique_Cloned_Concentration::verifier()
{
  Pb_Thermohydraulique::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = list_eq_concentration_.front().domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
}
