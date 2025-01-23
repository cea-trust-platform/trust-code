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
#include <SFichier.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>

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
  const Turbulence_paroi_base& loi = mod_turb_hydr.loi_paroi();

  if (typ != "negligeable_scalaire")
    if ((loi.que_suis_je() == "negligeable_VDF") || (loi.que_suis_je() == "negligeable_VEF"))
      {
        Cerr << "La loi de paroi de type " << typ << " choisie pour le scalaire n'est pas compatible avec" << finl;
        Cerr << "la loi de type " << loi.que_suis_je() << " choisie pour l'hydraulique" << finl;
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

bool Turbulence_paroi_scal_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  return champs_compris_.has_champ(nom, ref_champ);
}
bool Turbulence_paroi_scal_base::has_champ(const Motcle& nom) const
{
  return champs_compris_.has_champ(nom);
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

/*! @brief Ouverture/creation d'un fichier d'impression de moyennes
 *
 */
void Turbulence_paroi_scal_base::ouvrir_fichier_partage(EcrFicPartage& fichier, const Nom& nom_fichier, const Nom& extension) const
{
  const Probleme_base& pb = mon_modele_turb_scal->equation().probleme();
  Nom nom_fic = nom_fichier + "." + extension;

  // On cree le fichier nom_fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr0_ == 0 && !pb.reprise_effectuee())
    {
      fichier.ouvrir(nom_fic);
    }
  // Sinon on l'ouvre
  else
    {
      fichier.ouvrir(nom_fic, ios::app);
    }
  nb_impr0_++;
}

/*! @brief Give a boolean indicating if we need to use equivant distance by default we consider that we use the equivalent distance
 *
 * @return (boolean)
 */
bool Turbulence_paroi_scal_base::use_equivalent_distance() const
{
  return true;
}

/**
 * @brief Writes header line for Nusselt number and heat transfer statistics file
 *
 * @param boundaries_ Flag to control boundary selection (0: all boundaries, 1: specified boundaries)
 * @param boundaries_list List of boundary names to process
 * @param nom_fichier_ Output filename
 *
 * @details Creates header line with column names for thermal statistics including:
 * - Characteristic distance (m)
 * - Local Nusselt number
 * - Convective heat transfer coefficient (W/m²/K)
 * - Near-wall fluid temperature (K)
 * - Boundary face temperature (K)
 * - Equivalent wall temperature (K)
 *
 * Format: "Time Mean(field1) Mean(field2) ... [boundary1(field1) boundary1(field2) ...]"
 * Writes warning messages if specified boundaries are not valid wall types.
 */
void Turbulence_paroi_scal_base::imprimer_premiere_ligne_nusselt(int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");
  Nom ligne, err;
  err = "";
  Noms fields(nb_fields_);
  fields[0] = "dist. carac. (m)";
  fields[1] = "Nusselt (local)";
  fields[2] = "h (Conv. W/m2/K)";
  fields[3] = "Tf cote paroi (K)";
  fields[4] = "T face de bord (K)";
  fields[5] = "Tparoi equiv.(K)";
  ligne = "# Time  ";
  for (int i=0; i<nb_fields_; i++)
    {
      ligne += " \tMean(";
      ligne += fields[i];
      ligne += ")";
    }
  for (int n_bord = 0; n_bord < le_dom_dis_->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_dis_->les_conditions_limites(n_bord);
      const Nom& nom_bord = la_cl->frontiere_dis().le_nom();
      if (je_suis_maitre() && (boundaries_list.contient(nom_bord) || boundaries_list.size() == 0))
        {
          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
            {
              for (int i=0; i<nb_fields_; i++)
                {
                  ligne += " \t";
                  ligne += nom_bord;
                  ligne += "(";
                  ligne += fields[i];
                  ligne += ")";
                }
            }
          else if (boundaries_list.size() > 0)
            {
              err += "The boundary named '";
              err += nom_bord;
              err += "' is not of type Dirichlet_paroi_fixe or Dirichlet_paroi_defilante.\n";
              err += "So TRUST will not write Nusselt mean values.\n\n";
            }
        }
    }
  if (je_suis_maitre())
    {
      fichier << err;
      fichier << ligne;
      fichier << finl;
    }
  fichier.syncfile();
}

/**
 * @brief Prints mean Nusselt number and heat transfer statistics to a file for specified boundaries
 *
 * @param os Output stream for messages
 * @param boundaries_ Flag to control boundary selection (0: all boundaries, 1: specified boundaries)
 * @param boundaries_list List of boundary names to process when boundaries_=1
 * @param nom_fichier_ Output filename
 *
 * @details Calculates and writes average values for multiple thermal parameters:
 * - Characteristic distance
 * - Nusselt number
 * - Heat transfer coefficient
 * - Near-wall temperatures
 * - Wall temperatures
 *
 * Results are written for all boundaries combined and then for each boundary separately.
 * Only processes fixed walls (Dirichlet_paroi_fixe) and moving walls (Dirichlet_paroi_defilante).
 * Output format: time [mean_values] [boundary1_values] [boundary2_values] ...
 */
void Turbulence_paroi_scal_base::imprimer_nusselt_mean_only(Sortie& os, int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  const Probleme_base& pb = mon_modele_turb_scal->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();
  int ndeb, nfin, size0, num_bord;
  num_bord = 0;

  if (boundaries_list.size() != 0)
    {
      size0 = boundaries_list.size();
    }
  else
    {
      size0 = le_dom_dis_->nb_front_Cl();
    }
  DoubleTrav moy_bords(size0 + 1, nb_fields_+1);
  moy_bords = 0.;

  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");

  for (int n_bord = 0; n_bord < le_dom_dis_->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_dis_->les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          if (boundaries_ == 0 || (boundaries_ == 1 && boundaries_list.contient(le_bord.le_nom())))
            {
              for (int num_face = ndeb; num_face < nfin; num_face++)
                {
                  // Calcul des valeurs moyennes par bord (en supposant maillage regulier)
                  for (int i=0; i<nb_fields_; i++)
                    {
                      moy_bords(0, i) += tab_(num_face, i);
                      moy_bords(num_bord + 1, 0) += tab_(num_face, i);
                    }
                  moy_bords(0, nb_fields_) += 1;
                  moy_bords(num_bord + 1, nb_fields_) += 1;
                }
              num_bord += 1;
            }
        }
    }
  mp_sum_for_each_item(moy_bords);

// affichages des lignes dans le fichier
  if (je_suis_maitre() && moy_bords(0, nb_fields_) != 0)
    {
      fichier << sch.temps_courant();
      for (int i=0; i<nb_fields_; i++)
        fichier << " \t" << moy_bords(0, i) / moy_bords(0, nb_fields_);
    }

  num_bord = 0;
  for (int n_bord = 0; n_bord < le_dom_dis_->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_dis_->les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          if (boundaries_ == 0 || (boundaries_ == 1 && boundaries_list.contient(le_bord.le_nom())))
            {
              if (je_suis_maitre())
                {
                  for (int i=0; i<nb_fields_; i++)
                    fichier << " \t" << moy_bords(num_bord + 1, i) / moy_bords(num_bord + 1, nb_fields_);
                }
              num_bord += 1;
            }
        }
    }

  if (je_suis_maitre())
    fichier << finl;
  fichier.syncfile();
}
