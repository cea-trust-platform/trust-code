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

#include <Modele_turbulence_scal_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Turbulence_paroi_scal_base.h>
#include <Convection_Diffusion_std.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Domaine_VF.h>
#include <Champ_Don.h>
#include <SFichier.h>

Implemente_base_sans_constructeur(Turbulence_paroi_scal_base, "Turbulence_paroi_scal_base", Objet_U);

Sortie& Turbulence_paroi_scal_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Turbulence_paroi_scal_base::readOn(Entree& is)
{
  return is;
}

void Turbulence_paroi_scal_base::creer_champ(const Motcle& motlu)
{
}

/*! @brief Lit les caracteristques de la loi de parois a partir d'un flot d'entree.
 *
 *     Format: type_de_loi_de_paroi
 *     Les valeurs possibles du type de loi de paroi sont:
 *       - "loi_standard_hydr"
 *       - "negligeable"
 *       - "loi_VanDriest"
 *       - "loi_standard_hydr_scalaire"
 *
 */
void Turbulence_paroi_scal_base::typer_lire_turbulence_paroi_scal(OWN_PTR(Turbulence_paroi_scal_base)& turb_par, const Modele_turbulence_scal_base& mod_turb_scal, Entree& s)
{
  Cerr << "Lecture du type de loi de parois " << finl;
  Motcle typ;
  s >> typ;

  const Equation_base& eqn = mod_turb_scal.equation().probleme().equation(0); // equation hydraulique
  const RefObjU& modele_turbulence = eqn.get_modele(TURBULENCE);
  const Modele_turbulence_hyd_base& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();

  if (typ != "negligeable_scalaire")
    if ((loi->que_suis_je() == "negligeable_VDF") || (loi->que_suis_je() == "negligeable_VEF"))
      {
        Cerr << "La loi de paroi de type " << typ << " choisie pour le scalaire n'est pas compatible avec" << finl;
        Cerr << "la loi de type " << loi->que_suis_je() << " choisie pour l'hydraulique" << finl;
        Cerr << "Utiliser le type 'negligeable_scalaire' pour le scalaire ou utiliser une loi de paroi" << finl;
        Cerr << "non negligeable pour l hydraulique" << finl;
        exit();
      }
  typ += "_";

  Nom discr = eqn.discretisation().que_suis_je();

  //  les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
  if (discr == "VEFPreP1B")
    discr = "VEF";
  typ += discr;

  if (typ == "loi_analytique_scalaire_VDF")
    {
      Cerr << "La loi de paroi scalaire de type loi_analytique_scalaire" << finl;
      Cerr << "n est utilisable qu avec une discretisation de type VEF" << finl;
      exit();
    }

  Cerr << "et typage : " << typ << finl;
  turb_par.typer(typ);
  turb_par->associer_modele(mod_turb_scal);
  turb_par->associer(eqn.domaine_dis(), eqn.domaine_Cl_dis());
}

const Champ_base& Turbulence_paroi_scal_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}

void Turbulence_paroi_scal_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  if (opt == DESCRIPTION)
    Cerr << "Turbulence_paroi_scal_base : " << champs_compris_.liste_noms_compris() << finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

}
/*! @brief Ouverture/creation d'un fichier d'impression de Face, d_eq, Nu local, h
 *
 */
void Turbulence_paroi_scal_base::ouvrir_fichier_partage(EcrFicPartage& Nusselt, const Nom& extension) const
{
  const Probleme_base& pb = mon_modele_turb_scal->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();

  Nom fichier = Objet_U::nom_du_cas() + "_" + pb.le_nom() + "_" + extension + ".face";

  // On cree le fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr_ == 0 && !pb.reprise_effectuee())
    Nusselt.ouvrir(fichier);
  else // Sinon on l'ouvre
    Nusselt.ouvrir(fichier, ios::app);

  if (je_suis_maitre())
    {
      EcrFicPartage& fic = Nusselt;
      fic << "Temps : " << sch.temps_courant();
    }
  nb_impr_++;
}

/*! @brief Give a boolean indicating if we need to use equivant distance by default we consider that we use the equivalent distance
 *
 * @return (boolean)
 */
bool Turbulence_paroi_scal_base::use_equivalent_distance() const
{
  return true;
}
