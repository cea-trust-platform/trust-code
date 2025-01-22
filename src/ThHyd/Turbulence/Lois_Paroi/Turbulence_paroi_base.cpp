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
#include <Turbulence_paroi_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Front_VF.h>
#include <Param.h>
#include <Noms.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>


Implemente_base(Turbulence_paroi_base, "Turbulence_paroi_base", Objet_U);
// XD turbulence_paroi_base objet_u turbulence_paroi_base -1 Basic class for wall laws for Navier-Stokes equations.

// XD negligeable turbulence_paroi_base negligeable 0 Keyword to suppress the calculation of a law of the wall with a turbulence model. The wall stress is directly calculated with the derivative of the velocity, in the direction perpendicular to the wall (tau_tan /rho= nu dU/dy). NL2 Warning: This keyword is not available for k-epsilon models. In that case you must choose a wall law.
// XD negligeable_scalaire turbulence_paroi_scalaire_base negligeable_scalaire 0 Keyword to suppress the calculation of a law of the wall with a turbulence model for thermohydraulic problems. The wall stress is directly calculated with the derivative of the velocity, in the direction perpendicular to the wall.

Sortie& Turbulence_paroi_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Turbulence_paroi_base::readOn(Entree& is)
{
  return is;
}

/*! @brief Lit les caracteristques de la loi de parois a partir d'un flot d'entree.
 *
 *     Format:
 *       type_de_loi_de_paroi
 *     Les valeurs possibles du type de loi de paroi sont:
 *       - "loi_standard_hydr"
 *       - "negligeable"
 *       - "loi_VanDriest"
 *
 */
void Turbulence_paroi_base::typer_lire_turbulence_paroi(OWN_PTR(Turbulence_paroi_base) &loi_par, const Modele_turbulence_hyd_base& mod_turb_hyd, Entree& s)
{
  Cerr << "Lecture du type de loi de parois " << finl;
  Motcle typ;
  s >> typ;

  const Equation_base& eqn = mod_turb_hyd.equation();
  if (typ == "loi_standard_hydr_scalaire" || typ == "loi_paroi_2_couches_scalaire" || typ == "negligeable_scal")
    {
      Cerr << "Le format du jeu de donnees a change:" << finl;
      Cerr << "Chaque modele de turbulence doit avoir sa loi de paroi specifiee." << finl;
      Cerr << "Ainsi par exemple, loi_standard_hydr sera pour le modele de turbulence de l'equation de qdm" << finl;
      Cerr << "et loi_standard_hydr_scalaire pour le modele de turbulence de l'equation d'energie." << typ << finl;
      Process::exit();
    }
  typ += "_";

  Nom discr = eqn.discretisation().que_suis_je();

  //  les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
  if (discr == "VEFPreP1B") discr = "VEF";
  typ += discr;

  Cerr << "et typage : " << typ << finl;
  loi_par.typer(typ);
  loi_par->associer_modele(mod_turb_hyd);
  loi_par->associer(eqn.domaine_dis(), eqn.domaine_Cl_dis());
}

void Turbulence_paroi_base::creer_champ(const Motcle& motlu)
{
  if (motlu == "u_star" && champ_u_star_.est_nul())
    {
      int nb_comp = 1;
      Noms noms(1);
      noms[0] = "u_star";
      Noms unites(1);
      unites[0] = "m2/s2";
      double temps = 0.;
      const Equation_base& equation = mon_modele_turb_hyd->equation();
      const Discretisation_base& discr = equation.probleme().discretisation();
      discr.discretiser_champ("champ_face", equation.domaine_dis(), scalaire, noms, unites, nb_comp, temps, champ_u_star_);
      champs_compris_.ajoute_champ(champ_u_star_);
    }
}

bool Turbulence_paroi_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (nom == champ_u_star_.le_nom())
    {
      ref_champ = Turbulence_paroi_base::get_champ(nom);
      return true;
    }
  else
    return champs_compris_.has_champ(nom, ref_champ);
}

bool Turbulence_paroi_base::has_champ(const Motcle& nom) const
{
  if (nom == champ_u_star_.le_nom())
    return true;
  else
    return champs_compris_.has_champ(nom);
}

const Champ_base& Turbulence_paroi_base::get_champ(const Motcle& nom) const
{
  if (nom == champ_u_star_.le_nom())
    {
      // Initialisation a 0 du champ volumique u_star
      DoubleTab& valeurs = champ_u_star_->valeurs();
      valeurs = 0;
      const Equation_base& my_eqn = mon_modele_turb_hyd->equation();
      if (tab_u_star_.size_array() > 0)
        {
          // Boucle sur les frontieres pour recuperer u_star si tab_u_star dimensionne
          int nb_front = my_eqn.domaine_dis().nb_front_Cl();
          for (int n_bord = 0; n_bord < nb_front; n_bord++)
            {
              const Cond_lim& la_cl = my_eqn.domaine_Cl_dis().les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int num_face = ndeb; num_face < nfin; num_face++)
                valeurs(num_face) = tab_u_star_(num_face);
            }
        }
      valeurs.echange_espace_virtuel();
      // Met a jour le temps du champ:
      champ_u_star_->mettre_a_jour(my_eqn.schema_temps().temps_courant());
      return champs_compris_.get_champ(nom);
    }
  else
    return champs_compris_.get_champ(nom);
}

void Turbulence_paroi_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Noms noms_compris = champs_compris_.liste_noms_compris();
  noms_compris.add("u_star");
  if (opt == DESCRIPTION)
    Cerr << que_suis_je() << " : " << noms_compris << finl;
  else
    nom.add(noms_compris);
}

/*! @brief Ouverture/creation d'un fichier d'impression de Face, uplus_, dplus_, tab_u_star, Cisaillement_paroi_
 *
 */
void Turbulence_paroi_base::ouvrir_fichier_partage(EcrFicPartage& Ustar, const Nom& extension) const
{

  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();

  Nom fichier = Objet_U::nom_du_cas() + "_" + pb.le_nom() + "_" + extension + ".face";

  // On cree le fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr_ == 0 && !pb.reprise_effectuee())
    {
      Ustar.ouvrir(fichier);
    }
  // Sinon on l'ouvre
  else
    {
      Ustar.ouvrir(fichier, ios::app);
    }

  if (je_suis_maitre())
    {
      EcrFicPartage& fic = Ustar;
      fic << "Temps : " << sch.temps_courant();
    }

  nb_impr_++;
}

/*! @brief Ouverture/creation d'un fichier d'impression de moyennes de uplus_, dplus_, tab_u_star
 *
 */
void Turbulence_paroi_base::ouvrir_fichier_partage(EcrFicPartage& fichier, const Nom& nom_fichier, const Nom& extension) const
{
  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
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

/**
 * @brief Writes header line for u* (friction velocity) statistics file
 *
 * @param boundaries_ Flag to control boundary selection (0: all boundaries, 1: specified boundaries)
 * @param boundaries_list List of boundary names to process
 * @param nom_fichier_ Output filename
 * @param domaine_dis Domain discretization
 * @param domaine_cl_dis Boundary conditions discretization
 *
 * @details Creates header line with column names for u* and d+ statistics.
 * Format: "Time Mean(u*) Mean(d+) [boundary1(u*) boundary1(d+) ...]"
 * Writes warning messages if specified boundaries are not of correct wall type
 * (Dirichlet_paroi_fixe or Dirichlet_paroi_defilante).
 */
void Turbulence_paroi_base::imprimer_premiere_ligne_ustar_impl(int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_, const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis) const
{
  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");
  Nom ligne, err;
  err = "";
  ligne = "# Time   \tMean(u*) \tMean(d+)";

  for (int n_bord = 0; n_bord < domaine_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis.les_conditions_limites(n_bord);
      const Nom& nom_bord = la_cl->frontiere_dis().le_nom();
      if (je_suis_maitre() && (boundaries_list.contient(nom_bord) || boundaries_list.size() == 0))
        {
          if ( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
            {
              ligne += " \t";
              ligne += nom_bord;
              ligne += "(u*)";
              ligne += " \t";
              ligne += nom_bord;
              ligne += "(d+)";
            }
          else if (boundaries_list.size() > 0)
            {
              err += "The boundary named '";
              err += nom_bord;
              err += "' is not of type Dirichlet_paroi_fixe or Dirichlet_paroi_defilante.\n";
              err += "So TRUST will not write his u_star and d_plus means.\n\n";
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
 * @brief Prints mean friction velocity (u*) statistics to a file for specified boundaries
 *
 * @param os Output stream for messages
 * @param boundaries_ Flag to control boundary selection (0: all boundaries, 1: specified boundaries)
 * @param boundaries_list List of boundary names to process when boundaries_=1
 * @param nom_fichier_ Output filename
 * @param domaine_dis Domain discretization
 * @param domaine_cl_dis Boundary conditions discretization
 *
 * @details Calculates and writes average u* (friction velocity) and d+ values for wall boundaries.
 * Results are written for all boundaries combined and then for each boundary separately.
 * Only processes fixed walls (Dirichlet_paroi_fixe) and moving walls (Dirichlet_paroi_defilante).
 * Output format: time mean_u* mean_d+ [boundary1_u* boundary1_d+ boundary2_u* boundary2_d+ ...]
 */
void Turbulence_paroi_base::imprimer_ustar_mean_only_impl(Sortie& os, int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_, const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_cl_dis) const
{
  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();
  int ndeb, nfin, size0, num_bord;
  num_bord = 0;

  if (boundaries_list.size() != 0)
    {
      size0 = boundaries_list.size();
    }
  else
    {
      size0 = domaine_dis.nb_front_Cl();
    }
  DoubleTrav moy_bords(size0 + 1, 3);
  moy_bords = 0.;

  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");

  for (int n_bord = 0; n_bord < domaine_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis.les_conditions_limites(n_bord);
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
                  moy_bords(0, 0) += tab_u_star(num_face);
                  moy_bords(0, 1) += 1;
                  moy_bords(0, 2) += tab_d_plus(num_face);
                  moy_bords(num_bord + 1, 0) += tab_u_star(num_face);
                  moy_bords(num_bord + 1, 1) += 1;
                  moy_bords(num_bord + 1, 2) += tab_d_plus(num_face);
                }
              num_bord += 1;
            }
        }
    }
  mp_sum_for_each_item(moy_bords);

// affichages des lignes dans le fichier
  if (je_suis_maitre() && moy_bords(0, 1) != 0)
    {
      fichier << sch.temps_courant() << " \t" << moy_bords(0, 0) / moy_bords(0, 1) << " \t" << moy_bords(0, 2) / moy_bords(0, 1);
    }

  num_bord = 0;
  for (int n_bord = 0; n_bord < domaine_dis.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis.les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          if (boundaries_ == 0 || (boundaries_ == 1 && boundaries_list.contient(le_bord.le_nom())))
            {
              if (je_suis_maitre())
                {
                  fichier << " \t" << moy_bords(num_bord + 1, 0) / moy_bords(num_bord + 1, 1) << " \t" << moy_bords(num_bord + 1, 2) / moy_bords(num_bord + 1, 1);
                }
              num_bord += 1;
            }
        }
    }

  if (je_suis_maitre())
    fichier << finl;
  fichier.syncfile();
}

