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

#include <Convection_Diffusion_Chaleur_Turbulent_QC.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Param.h>

Implemente_instanciable(Convection_Diffusion_Chaleur_Turbulent_QC, "Convection_Diffusion_Chaleur_Turbulent_QC", Convection_Diffusion_Chaleur_QC);
// XD convection_diffusion_chaleur_turbulent_qc convection_diffusion_chaleur_QC convection_diffusion_chaleur_turbulent_qc -1 Temperature equation for a quasi-compressible fluid as well as the associated turbulence model equations.
// XD attr modele_turbulence modele_turbulence_scal_base modele_turbulence 1 Turbulence model for the temperature (energy) conservation equation.

Sortie& Convection_Diffusion_Chaleur_Turbulent_QC::printOn(Sortie& is) const
{
  return Equation_base::printOn(is);
}

Entree& Convection_Diffusion_Chaleur_Turbulent_QC::readOn(Entree& is)
{
  return Convection_Diffusion_Chaleur_QC::readOn(is);
}

void Convection_Diffusion_Chaleur_Turbulent_QC::set_param(Param& param)
{
  Convection_Diffusion_Chaleur_QC::set_param(param);
  param.ajouter_non_std("modele_turbulence", (this), Param::REQUIRED);
}

int Convection_Diffusion_Chaleur_Turbulent_QC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      lire_op_diff_turbulent(is, *this, terme_diffusif);
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else if (mot == "modele_turbulence")
    {
      lire_modele(is, *this);
      RefObjU le_modele;
      le_modele = le_modele_turbulence.valeur();
      liste_modeles_.add_if_not(le_modele);
      return 1;
    }
  else
    return Convection_Diffusion_Chaleur_QC::lire_motcle_non_standard(mot, is);
}

/*! @brief Sauvegarde sur un flot de sortie, double appel a: Convection_Diffusion_Temperature::sauvegarder(Sortie& );
 *
 *       Convection_Diffusion_Turbulent::sauvegarder(Sortie& );
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Convection_Diffusion_Chaleur_Turbulent_QC::sauvegarder(Sortie& os) const
{
  int bytes = 0;
  bytes += Convection_Diffusion_Chaleur_QC::sauvegarder(os);
  bytes += Convection_Diffusion_Turbulent::sauvegarder(os);
  return bytes;
}

/*! @brief Reprise a partir d'un flot d'entree, double appel a: Convection_Diffusion_Temperature::reprendre(Entree& );
 *
 *       Convection_Diffusion_Turbulent::reprendre(Entree&);
 *
 * @param (Entree& is) un flot d'entree
 * @return (int) renvoie toujours 1
 */
int Convection_Diffusion_Chaleur_Turbulent_QC::reprendre(Entree& is)
{
  Convection_Diffusion_Chaleur_QC::reprendre(is);
  Convection_Diffusion_Turbulent::reprendre(is);
  return 1;
}

/*! @brief Double appel a: Convection_Diffusion_Turbulent::completer()
 *
 *      Convection_Diffusion_Temperature::completer()
 *
 */
void Convection_Diffusion_Chaleur_Turbulent_QC::completer()
{
  Convection_Diffusion_Turbulent::completer();
  Convection_Diffusion_Chaleur_QC::completer();
}

/*! @brief Mise a jour en temps de l'equation, double appel a: Convection_Diffusion_Temperature::mettre_a_jour(double );
 *
 *       Convection_Diffusion_Turbulent::mettre_a_jour(double );
 *
 * @param (double temps) le temps de mise a jour
 */
void Convection_Diffusion_Chaleur_Turbulent_QC::mettre_a_jour(double temps)
{
  Convection_Diffusion_Chaleur_QC::mettre_a_jour(temps);
}
void Convection_Diffusion_Chaleur_Turbulent_QC::mettre_a_jour_modele(double temps)
{
  Convection_Diffusion_Turbulent::mettre_a_jour(temps);

}

void Convection_Diffusion_Chaleur_Turbulent_QC::creer_champ(const Motcle& motlu)
{
  Convection_Diffusion_Chaleur_QC::creer_champ(motlu);

  if (le_modele_turbulence.non_nul())
    le_modele_turbulence->creer_champ(motlu);
}

bool Convection_Diffusion_Chaleur_Turbulent_QC::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (Convection_Diffusion_Chaleur_QC::has_champ(nom))
    return Convection_Diffusion_Chaleur_QC::has_champ(nom, ref_champ);

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom))
      return le_modele_turbulence->has_champ(nom, ref_champ);

  return false; /* rien trouve */
}

bool Convection_Diffusion_Chaleur_Turbulent_QC::has_champ(const Motcle& nom) const
{
  if (Convection_Diffusion_Chaleur_QC::has_champ(nom))
    return true;

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom))
      return true;

  return false; /* rien trouve */
}

const Champ_base& Convection_Diffusion_Chaleur_Turbulent_QC::get_champ(const Motcle& nom) const
{
  if (Convection_Diffusion_Chaleur_QC::has_champ(nom))
    return Convection_Diffusion_Chaleur_QC::get_champ(nom);

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom))
      return le_modele_turbulence->get_champ(nom);

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}

void Convection_Diffusion_Chaleur_Turbulent_QC::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Convection_Diffusion_Chaleur_QC::get_noms_champs_postraitables(nom, opt);

  if (le_modele_turbulence.non_nul())
    le_modele_turbulence->get_noms_champs_postraitables(nom, opt);
}

/*! @brief Double appel a: Convection_Diffusion_Turbulent::preparer_calcul()
 *
 *       Convection_Diffusion_Temperature::preparer_calcul()
 *
 * @return (int) renvoie toujours 1
 */
int Convection_Diffusion_Chaleur_Turbulent_QC::preparer_calcul()
{
  Convection_Diffusion_Chaleur_QC::preparer_calcul();
  Convection_Diffusion_Turbulent::preparer_calcul();

  return 1;
}
void Convection_Diffusion_Chaleur_Turbulent_QC::imprimer(Sortie& os) const
{
  Convection_Diffusion_Chaleur_QC::imprimer(os);
  le_modele_turbulence->imprimer(os);
}

bool Convection_Diffusion_Chaleur_Turbulent_QC::initTimeStep(double dt)
{
  bool ok = Convection_Diffusion_Turbulent::initTimeStep(dt);
  ok = ok && Convection_Diffusion_Chaleur_QC::initTimeStep(dt);
  return ok;
}

const RefObjU& Convection_Diffusion_Chaleur_Turbulent_QC::get_modele(Type_modele type) const
{
  for (const auto &itr : liste_modeles_)
    {
      const RefObjU& mod = itr;
      if (mod.non_nul())
        if ((sub_type(Modele_turbulence_scal_base, mod.valeur())) && (type == TURBULENCE))
          return mod;
    }
  return Equation_base::get_modele(type);
}
