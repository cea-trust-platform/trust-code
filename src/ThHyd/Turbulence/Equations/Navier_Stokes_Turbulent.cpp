/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Modele_turbulence_hyd_base.h>
#include <Navier_Stokes_Turbulent.h>
#include <Op_Diff_Turbulent_base.h>
#include <Fluide_Incompressible.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <TRUST_2_PDI.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Param.h>

Implemente_instanciable(Navier_Stokes_Turbulent, "Navier_Stokes_Turbulent", Navier_Stokes_std);
// XD navier_stokes_turbulent navier_stokes_standard navier_stokes_turbulent -1 Navier-Stokes equations as well as the associated turbulence model equations.
// XD attr modele_turbulence modele_turbulence_hyd_deriv modele_turbulence 1 Turbulence model for Navier-Stokes equations.

Sortie& Navier_Stokes_Turbulent::printOn(Sortie& is) const { return Equation_base::printOn(is); }

Entree& Navier_Stokes_Turbulent::readOn(Entree& is) { return Navier_Stokes_std::readOn(is); }

void Navier_Stokes_Turbulent::set_param(Param& param)
{
  Navier_Stokes_std::set_param(param);
  param.ajouter_non_std("modele_turbulence", (this), Param::REQUIRED);
}

int Navier_Stokes_Turbulent::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : ";
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      lire_op_diff_turbulent(is);
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else if (mot == "modele_turbulence")
    return typer_lire_mod_turb_hyd(is);
  else
    return Navier_Stokes_std::lire_motcle_non_standard(mot, is);
}

int Navier_Stokes_Turbulent::typer_lire_mod_turb_hyd(Entree& s)
{
  Cerr << "Reading and typing of the turbulence model : ";

  Motcle typ;
  s >> typ;
  Motcle nom1("Modele_turbulence_hyd_");
  nom1 += typ;
  Nom discr = discretisation().que_suis_je();

  if (typ.debute_par("SOUS_MAILLE") || discr == "VDF_Hyper" || typ.debute_par("LONGUEUR_MELANGE") || (typ == "K_Epsilon_V2"))
    {
      if (dimension == 2 && discr != "VDF_Hyper")
        {
          Cerr << "Vous traitez un cas turbulent en dimension 2 avec un modele sous maille" << finl;
          Cerr << "Attention a l'interpretation des resultats !!" << finl;
        }

      nom1 += "_";
      // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
      if (discr == "VEFPreP1B") discr = "VEF";
      nom1 += discr;
    }
  if (nom1 == "MODELE_TURBULENCE_HYD_SOUS_MAILLE_LM_VEF")
    {
      Cerr << "Le mot cle Sous_maille_LM s'appelle desormais Longueur_Melange pour etre coherent en VDF et VEF." << finl;
      Cerr << "Changer votre jeu de donnees." << finl;
      Process::exit();
    }

  Cerr << nom1 << finl;

  le_modele_turbulence.typer(nom1);
  le_modele_turbulence->associer_eqn(*this);
  le_modele_turbulence->associer(domaine_dis(), domaine_Cl_dis());
  s >> le_modele_turbulence.valeur(); // on lit !

  le_modele_turbulence->discretiser();
  RefObjU le_modele;
  le_modele = le_modele_turbulence.valeur();
  liste_modeles_.add_if_not(le_modele);

  return 1;
}

const Champ_Don_base& Navier_Stokes_Turbulent::diffusivite_pour_transport() const
{
  return fluide().viscosite_cinematique();
}

const Champ_base& Navier_Stokes_Turbulent::diffusivite_pour_pas_de_temps() const
{
  return fluide().viscosite_cinematique();
}

// Lecture et typage de l'operateur diffusion turbulente.
// Attention : il faut avoir fait "terme_diffusif.associer_diffusivite" avant d'enter ici.
Entree& Navier_Stokes_Turbulent::lire_op_diff_turbulent(Entree& is)
{
  Motcle accouverte = "{", accfermee = "}";
  Nom type = "Op_Dift_";

  Nom discr = discretisation().que_suis_je();
  // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
  if (discr == "VEFPreP1B") discr = "VEF";

  type += discr;

  Nom nb_inc;
  if (terme_diffusif.diffusivite().nb_comp() == 1)
    nb_inc = "_";
  else
    nb_inc = "_Multi_inco_";
  type += nb_inc;

  Nom type_inco = inconnue().que_suis_je();
  if (type_inco == "Champ_Q1_EF") type += "Q1";
  else type += (type_inco.suffix("Champ_"));

  if (axi) type += "_Axi";

  Motcle motbidon;
  is >> motbidon;
  if (motbidon != accouverte)
    {
      Cerr << "A { was expected while reading the turbulent diffusive term" << finl;
      Process::exit();
    }
  is >> motbidon;
  if (motbidon == "negligeable")
    {
      type = "Op_Dift_negligeable";
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is >> motbidon;
      //on lit la fin de diffusion { }
      if (motbidon != accfermee)
        Cerr << " On ne peut plus entrer d option apres negligeable " << finl;
    }
  else if (motbidon == "tenseur_Reynolds_externe")
    {
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      Cerr << terme_diffusif->que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is >> motbidon;
      //on lit la fin de diffusion { }
      if (motbidon != accfermee)
        Cerr << " On ne peut plus entrer d option apres tenseur_Reynolds_externe " << finl;
    }
  else if (motbidon == "standard")
    {
      type += "_standard";
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is >> terme_diffusif.valeur();
    }
  else if (motbidon == accfermee)
    {
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      Cerr << terme_diffusif->que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
    }
  else if (motbidon == "stab")
    {
      type += "_stab";
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is >> terme_diffusif.valeur();
    }
  else if (motbidon == "option")
    {
      type += "option";
      if (discr == "EF")
        type = "Op_Dift_EF_Q1_option";
      terme_diffusif.typer(type);
      Cerr << terme_diffusif->que_suis_je() << finl;
      terme_diffusif.l_op_base().associer_eqn(*this);
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is >> terme_diffusif.valeur();
      is >> motbidon;
      //on lit la fin de diffusion { }
      if (motbidon != accfermee)
        Cerr << " On ne peut plus entrer d option apres option " << finl;
    }
  else
    {
      type += motbidon;
      is >> motbidon;
      if (motbidon != accfermee)
        Cerr << " No option are now readable for the turbulent diffusive term" << finl;

      if (discr == "VEF")
        type += "_P1NC";
      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(*this);
      Cerr << terme_diffusif->que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
    }
  return is;
}

/*! @brief Prepare le calcul.
 *
 * Simple appe a Modele_turbulence_hyd_base->preparer_caclul() sur
 *     le membre reprresentant la turbulence.
 *
 * @return (int) renvoie toujours 1
 */
int Navier_Stokes_Turbulent::preparer_calcul()
{

  Turbulence_paroi_base& loipar = le_modele_turbulence->loi_paroi();
  if (le_modele_turbulence->has_loi_paroi_hyd())
    loipar.init_lois_paroi();

  Navier_Stokes_std::preparer_calcul();
  le_modele_turbulence->preparer_calcul();
  return 1;
}

bool Navier_Stokes_Turbulent::initTimeStep(double dt)
{
  bool ok = Navier_Stokes_std::initTimeStep(dt);
  ok = ok && le_modele_turbulence->initTimeStep(dt);
  return ok;
}

/*! @brief for PDI IO: retrieve name, type and dimensions of the fields to save/restore
 *
 */
std::vector<YAML_data> Navier_Stokes_Turbulent::data_a_sauvegarder() const
{
  std::vector<YAML_data> data = Navier_Stokes_std::data_a_sauvegarder();
  std::vector<YAML_data> mod_turb = le_modele_turbulence->data_a_sauvegarder();
  data.insert(data.end(), mod_turb.begin(), mod_turb.end());
  return data;
}

/*! @brief Sauvegarde l'equation (et son modele de turbulence) sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Navier_Stokes_Turbulent::sauvegarder(Sortie& os) const
{
  int bytes = 0;
  bytes += Navier_Stokes_std::sauvegarder(os);
  assert(bytes % 4 == 0);
  bytes += le_modele_turbulence->sauvegarder(os);
  assert(bytes % 4 == 0);
  return bytes;
}

/*! @brief Reprise de l'equation et de son modele de turbulence a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (int) renvoie toujours 1
 * @throws fin de fichier rencontre pendant la reprise
 */
int Navier_Stokes_Turbulent::reprendre(Entree& is)
{
  Navier_Stokes_std::reprendre(is);
  double temps = schema_temps().temps_courant();
  Nom ident_modele(le_modele_turbulence->que_suis_je());
  ident_modele += probleme().domaine().le_nom();
  ident_modele += Nom(temps, probleme().reprise_format_temps());

  if(!TRUST_2_PDI::is_PDI_restart())
    avancer_fichier(is, ident_modele);
  le_modele_turbulence->reprendre(is);

  return 1;
}

/*! @brief Appels successifs a: Navier_Stokes_std::completer()
 *
 *       Mod_Turb_Hyd::completer() [sur le membre concerne]
 *
 */
void Navier_Stokes_Turbulent::completer()
{
  Navier_Stokes_std::completer();
  le_modele_turbulence->completer();
  le_modele_turbulence->loi_paroi().completer();
}

/*! @brief Effecttue une mise a jour en temps de l'equation.
 *
 * @param (double temps) le temps de mise a jour
 */
void Navier_Stokes_Turbulent::mettre_a_jour(double temps)
{
  Navier_Stokes_std::mettre_a_jour(temps);
  le_modele_turbulence->mettre_a_jour(temps);
}

bool Navier_Stokes_Turbulent::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (Navier_Stokes_std::has_champ(nom, ref_champ))
    return true;

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom, ref_champ))
      return true;

  return false; /* rien trouve */
}

bool Navier_Stokes_Turbulent::has_champ(const Motcle& nom) const
{
  if (Navier_Stokes_std::has_champ(nom))
    return true;

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom))
      return true;

  return false; /* rien trouve */
}

const Champ_base& Navier_Stokes_Turbulent::get_champ(const Motcle& nom) const
{
  OBS_PTR(Champ_base) ref_champ;

  if (Navier_Stokes_std::has_champ(nom, ref_champ))
    return ref_champ;

  if (le_modele_turbulence.non_nul())
    if (le_modele_turbulence->has_champ(nom, ref_champ))
      return ref_champ;

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}

void Navier_Stokes_Turbulent::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Navier_Stokes_std::get_noms_champs_postraitables(nom, opt);
  if (le_modele_turbulence.non_nul())
    le_modele_turbulence->get_noms_champs_postraitables(nom, opt);
}

void Navier_Stokes_Turbulent::imprimer(Sortie& os) const
{
  Navier_Stokes_std::imprimer(os);
  le_modele_turbulence->imprimer(os);
}

const RefObjU& Navier_Stokes_Turbulent::get_modele(Type_modele type) const
{
  for (const auto &itr : liste_modeles_)
    {
      const RefObjU& mod = itr;
      if (mod.non_nul())
        if ((sub_type(Modele_turbulence_hyd_base, mod.valeur())) && (type == TURBULENCE))
          return mod;
    }
  return Equation_base::get_modele(type);
}
