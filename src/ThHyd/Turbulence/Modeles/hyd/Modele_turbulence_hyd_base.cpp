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

#include <Modele_turbulence_hyd_base.h>
#include <EcritureLectureSpecial.h>
#include <Operateur_Diff_base.h>
#include <Operateur_Conv_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <communications.h>
#include <LecFicDiffuse.h>
#include <EcrFicPartage.h>
#include <Postraitement.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Operateur.h>
#include <Domaine.h>
#include <Param.h>
#include <Debog.h>

Implemente_base(Modele_turbulence_hyd_base, "Modele_turbulence_hyd_base", Objet_U);
// XD modele_turbulence_hyd_deriv objet_lecture modele_turbulence_hyd_deriv -1 Basic class for turbulence model for Navier-Stokes equations.

Sortie& Modele_turbulence_hyd_base::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

/*! @brief Lit les specifications d'un modele de turbulence a partir d'un flot d'entree.
 *
 *     Format:
 *     Modele_turbulence type_modele
 *     {
 *       [Turbulence_paroi la_loi_de_paroi]
 *       bloc de lecture specifique du type de turbulence
 *     }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws loi de paroi incompatible avec le probleme
 */
Entree& Modele_turbulence_hyd_base::readOn(Entree& is)
{
  Cerr << "Reading of data for a " << que_suis_je() << " hydraulic turbulence model" << finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  // Verifications
  // Cas ou Correction_visco_turb_pour_controle_pas_de_temps_parametre est lu sans Correction_visco_turb_pour_controle_pas_de_temps
  if (dt_diff_sur_dt_conv_ != -1)
    calcul_borne_locale_visco_turb_ = 1;
  // Cas ou Correction_visco_turb_pour_controle_pas_de_temps sans Correction_visco_turb_pour_controle_pas_de_temps_parametre
  else if (calcul_borne_locale_visco_turb_)
    dt_diff_sur_dt_conv_ = 1;
  // Cas ou Correction_visco_turb_pour_controle_pas_de_temps_parametre est incorrectement fixe
  if (dt_diff_sur_dt_conv_ <= 0 && calcul_borne_locale_visco_turb_)
    {
      Cerr << "Parameter value Correction_visco_turb_pour_controle_pas_de_temps_parametre must be strictly positive." << finl;
      exit();
    }
  return is;
}

void Modele_turbulence_hyd_base::set_param(Param& param)
{
  param.ajouter_non_std("turbulence_paroi", (this), Param::REQUIRED); // XD attr turbulence_paroi turbulence_paroi_base turbulence_paroi 1 Keyword to set the wall law.
  param.ajouter_non_std("dt_impr_ustar", (this));                     // XD attr dt_impr_ustar floattant dt_impr_ustar 1 This keyword is used to print the values (U +, d+, u$\star$) obtained with the wall laws into a file named datafile_ProblemName_Ustar.face and periode refers to the printing period, this value is expressed in seconds.
  param.ajouter_non_std("dt_impr_ustar_mean_only", (this));           // XD attr dt_impr_ustar_mean_only dt_impr_ustar_mean_only dt_impr_ustar_mean_only 1 This keyword is used to print the mean values of u* ( obtained with the wall laws) on each boundary, into a file named datafile_ProblemName_Ustar_mean_only.out. periode refers to the printing period, this value is expressed in seconds. If you don\'t use the optional keyword boundaries, all the boundaries will be considered. If you use it, you must specify nb_boundaries which is the number of boundaries on which you want to calculate the mean values of u*, then you have to specify their names.
  param.ajouter("nut_max", &XNUTM_);                                  // XD attr nut_max floattant nut_max 1 Upper limitation of turbulent viscosity (default value 1.e8).
  param.ajouter_flag("Correction_visco_turb_pour_controle_pas_de_temps", &calcul_borne_locale_visco_turb_); // XD attr correction_visco_turb_pour_controle_pas_de_temps rien correction_visco_turb_pour_controle_pas_de_temps 1 Keyword to set a limitation to low time steps due to high values of turbulent viscosity. The limit for turbulent viscosity is calculated so that diffusive time-step is equal or higher than convective time-step. For a stationary flow, the correction for turbulent viscosity should apply only during the first time steps and not when permanent state is reached. To check that, we could post process the corr_visco_turb field which is the correction of turbulent viscosity: it should be 1. on the whole domain.
  param.ajouter("Correction_visco_turb_pour_controle_pas_de_temps_parametre", &dt_diff_sur_dt_conv_);       // XD attr correction_visco_turb_pour_controle_pas_de_temps_parametre floattant correction_visco_turb_pour_controle_pas_de_temps_parametre 1 Keyword to set a limitation to low time steps due to high values of turbulent viscosity. The limit for turbulent viscosity is the ratio between diffusive time-step and convective time-step is higher or equal to the given value [0-1]
  //param.ajouter_condition("not(is_read_dt_impr_ustar_mean_only_and_is_read_dt_impr_ustar)","only one of dt_impr_ustar_mean_only and dt_impr_ustar can be used");
}

int Modele_turbulence_hyd_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  int retval = 1;
  if (mot == "turbulence_paroi")
    {
      loipar_.associer_modele(*this);
      is >> loipar_;
      is >> loipar_.valeur();
    }
  else if (loipar_.valeur().que_suis_je() != "negligeable_VDF" && loipar_.valeur().que_suis_je() != "negligeable_VEF" && !loipar_.valeur().que_suis_je().debute_par("negligeable_PolyMAC_P0P1NC"))
    {
      if (mot == "dt_impr_ustar")
        {
          is >> dt_impr_ustar_;
        }
      else if (mot == "dt_impr_ustar_mean_only")
        {
          // XD dt_impr_ustar_mean_only objet_lecture nul 1 not_set
          // XD attr dt_impr floattant dt_impr 0 not_set
          // XD attr boundaries listchaine boundaries 1 not_set
          Nom accolade_ouverte = "{";
          Nom accolade_fermee = "}";
          nom_fichier_ = Objet_U::nom_du_cas() + "_" + equation().probleme().le_nom() + "_ustar_mean_only";
          Domaine& dom = equation().probleme().domaine();
          LIST(Nom) nlistbord_dom;                      //!< liste stockant tous les noms de frontiere du domaine
          int nbfr = dom.nb_front_Cl();
          for (int b = 0; b < nbfr; b++)
            {
              Frontiere& org = dom.frontiere(b);
              nlistbord_dom.add(org.le_nom());
            }
          is >> motlu;
          if (motlu != accolade_ouverte)
            {
              Cerr << motlu << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard" << finl;
              Cerr << "A specification of kind : dt_impr_ustar_mean_only { dt_impr periode [boundaries nb_boundaries boundary_name1 boundary_name2 ... ] } was expected." << finl;
              exit();
            }
          is >> motlu;
          if (motlu != "dt_impr")
            {
              Cerr << "We expected dt_impr..." << finl;
              exit();
            }
          is >> dt_impr_ustar_mean_only_;

          is >> motlu; // boundaries ou accolade_fermee ou pasbon
          if (motlu != accolade_fermee)
            {
              if (motlu == "boundaries")
                {
                  boundaries_ = 1;
                  int nb_bords = 0;
                  Nom nom_bord_lu;

                  // read boundaries number
                  is >> nb_bords;
                  if (nb_bords != 0)
                    {
                      // read boundaries
                      for (int i = 0; i < nb_bords; i++)
                        {
                          is >> nom_bord_lu;
                          boundaries_list_.add(Nom(nom_bord_lu));
                          //  verif nom bords
                          if (!nlistbord_dom.contient(boundaries_list_[i]))
                            {
                              Cerr << "Problem in the dt_impr_ustar_mean_only instruction:" << finl;
                              Cerr << "The boundary named '" << boundaries_list_[i] << "' is not a boundary of the domain " << dom.le_nom() << "." << finl;
                              exit();
                            }
                        }
                    }
                  // lecture accolade fermee
                  is >> motlu;
                  if (motlu != accolade_fermee)
                    {
                      Cerr << "Problem in the dt_impr_ustar_mean_only instruction:" << finl;
                      Cerr << "TRUST wants to read a '" << accolade_fermee << "' but find '" << motlu << "'!!" << finl;
                      exit();
                    }
                }
              else
                {
                  Cerr << motlu << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard" << finl;
                  Cerr << "A specification of kind : dt_impr_ustar_mean_only { dt_impr periode [boundaries nb_boundaries boundary_name1 boundary_name2 ... ] } was expected." << finl;
                  exit();
                }
            }
        } // fin dt_impr_ustar_mean_only
      else
        {
          Cerr << "Please remove dt_impr_ustar option if the wall law is of Negligeable type." << finl;
          exit();
        }
    } // fin loi paroi negligeable

  else
    retval = -1;

  return retval;
}

/*! @brief Associe l'equation passe en parametre au modele de turbulence.
 *
 * @param (Equation_base& eqn) l'equation a laquelle l'objet s'associe
 */
void Modele_turbulence_hyd_base::associer_eqn(const Equation_base& eqn)
{
  mon_equation_ = eqn;
}

/*! @brief Lit le fichier dom_Wall_length.
 *
 * xyz pour remplir le champs wall_length en vue d'un post-traitement de distance_paroi
 *
 */
void Modele_turbulence_hyd_base::lire_distance_paroi()
{
  // PQ : 25/02/04 recuperation de la distance a la paroi dans Wall_length.xyz
  DoubleTab& wall_length = wall_length_.valeurs();
  wall_length = -1.;

  LecFicDiffuse fic;
  Nom nom_fic = equation().probleme().domaine().le_nom() + "_Wall_length.xyz";
  fic.set_bin(1);
  if (!fic.ouvrir(nom_fic))
    {
      Cerr << " File " << nom_fic << " doesn't exist. To generate it, please, refer to html.doc (Distance_paroi) " << finl;
      exit();
    }

  Noms nom_paroi;

  fic >> nom_paroi;

  EcritureLectureSpecial::lecture_special(wall_length_, fic);

}

/*! @brief Discretise le modele de turbulence.
 *
 */
void Modele_turbulence_hyd_base::discretiser()
{
  Cerr << "Turbulence hydraulic model discretization" << finl;
  discretiser_visc_turb(mon_equation_->schema_temps(), mon_equation_->domaine_dis(), la_viscosite_turbulente_);
  champs_compris_.ajoute_champ(la_viscosite_turbulente_);

  discretiser_corr_visc_turb(mon_equation_->schema_temps(), mon_equation_->domaine_dis(), corr_visco_turb_);
  champs_compris_.ajoute_champ(corr_visco_turb_);
  const Discretisation_base& dis = ref_cast(Discretisation_base, mon_equation_->discretisation());
  dis.discretiser_champ("champ_elem", mon_equation_->domaine_dis().valeur(), "distance_paroi", "m", 1, mon_equation_->schema_temps().temps_courant(), wall_length_);
  champs_compris_.ajoute_champ(wall_length_);
}

void Modele_turbulence_hyd_base::discretiser_visc_turb(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  int is_dilat = equation().probleme().is_dilatable();
  if (!is_dilat)
    Cerr << "Turbulent viscosity field discretization" << finl;
  else
    Cerr << "Turbulent dynamic viscosity field discretization" << finl;
  Nom nom = (is_dilat == 1) ? "viscosite_dynamique_turbulente" : "viscosite_turbulente";
  Nom unite = (is_dilat == 1) ? "kg/(m.s)" : "m2/s";
  const Discretisation_base& dis = mon_equation_->discretisation();
  dis.discretiser_champ("champ_elem", z.valeur(), nom, unite, 1, sch.temps_courant(), ch);
}

void Modele_turbulence_hyd_base::discretiser_corr_visc_turb(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Turbulent viscosity correction field discretization" << finl;
  const Discretisation_base& dis = mon_equation_->discretisation();
  dis.discretiser_champ("champ_elem", z.valeur(), "corr_visco_turb", "adimensionnel", 1, sch.temps_courant(), ch);
}

void Modele_turbulence_hyd_base::discretiser_K(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Kinetic turbulent energy field discretisation" << finl;
  const Discretisation_base& dis = mon_equation_->discretisation();
  dis.discretiser_champ("champ_elem", z.valeur(), "K", "m2/s2", 1, sch.temps_courant(), ch);
}

/*! @brief Prepare le calcul.
 *
 * Initialise la loi de paroi.
 *     Remplit le champ wall_length en cas de post traitemetn de la distance_paroi
 *
 * @return (int) code de retour de Turbulence_paroi::init_lois_paroi()
 */
int Modele_turbulence_hyd_base::preparer_calcul()
{
  int res = 1;
  if (loipar_.non_nul())
    res = loipar_.init_lois_paroi();

  bool contient_distance_paroi = false;

  for (auto &itr : equation().probleme().postraitements())
    if (!contient_distance_paroi)
      if (sub_type(Postraitement, itr.valeur()))
        {
          Postraitement& post = ref_cast(Postraitement, itr.valeur());
          for (int i = 0; i < post.noms_champs_a_post().size(); i++)
            {
              // The new global wall distance computation does not use the old method.
              const Nom& chmp = post.noms_champs_a_post()[i];
              if (!chmp.contient("DISTANCE_PAROI_GLOBALE") && chmp.contient("DISTANCE_PAROI"))
                {
                  lire_distance_paroi();
                  contient_distance_paroi = true;
                  break;
                }
            }
        }
  loipar_.imprimer_premiere_ligne_ustar(boundaries_, boundaries_list_, nom_fichier_);
  return res;
}

bool Modele_turbulence_hyd_base::initTimeStep(double dt)
{
  return true;
}

void Modele_turbulence_hyd_base::creer_champ(const Motcle& motlu)
{
  if (loipar_.non_nul())
    {
      loipar_->creer_champ(motlu);
    }
}

const Champ_base& Modele_turbulence_hyd_base::get_champ(const Motcle& nom) const
{
  try
    {
      return champs_compris_.get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }

  if (loipar_.non_nul())
    {
      try
        {
          return loipar_->get_champ(nom);
        }
      catch (Champs_compris_erreur&)
        {
        }
    }
  throw Champs_compris_erreur();
}
void Modele_turbulence_hyd_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  if (opt == DESCRIPTION)
    Cerr << que_suis_je() << " : " << champs_compris_.liste_noms_compris() << finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

  if (loipar_.non_nul())
    loipar_->get_noms_champs_postraitables(nom, opt);
}

/*! @brief Effectue l'impression si cela est necessaire.
 *
 * @return renvoie toujours 1
 */
void Modele_turbulence_hyd_base::imprimer(Sortie& os) const
{
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  double temps_courant = sch.temps_courant();
  double dt = sch.pas_de_temps();
  if (loipar_.non_nul() && limpr_ustar(temps_courant, sch.temps_precedent(), dt, dt_impr_ustar_))
    loipar_.imprimer_ustar(os);
  if (loipar_.non_nul() && limpr_ustar(temps_courant, sch.temps_precedent(), dt, dt_impr_ustar_mean_only_))
    loipar_.imprimer_ustar_mean_only(os, boundaries_, boundaries_list_, nom_fichier_);
}

int Modele_turbulence_hyd_base::limpr_ustar(double temps_courant, double temps_prec, double dt, double dt_ustar) const
{
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  if (sch.nb_pas_dt() == 0)
    return 0;
  if (dt_ustar <= dt
      || ((sch.temps_cpu_max_atteint() || (!get_disable_stop() && sch.stop_lu()) || sch.temps_final_atteint() || sch.nb_pas_dt_max_atteint() || sch.nb_pas_dt() == 1 || sch.stationnaire_atteint())
          && !est_egal(dt_ustar, 1.e20)))
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j, epsilon = 1.e-8;
      modf(temps_courant / dt_ustar + epsilon, &i);
      modf(temps_prec / dt_ustar + epsilon, &j);
      return (i > j);
    }
}

/*! @brief Sauvegarde le modele de turbulence sur un flot de sortie.
 *
 * Sauvegarde le type de l'objet.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Modele_turbulence_hyd_base::sauvegarder(Sortie& os) const
{
  a_faire(os);
  if (loipar_.non_nul())
    {
      loipar_->sauvegarder(os);
    }
  else  // OC: pour le bas Re, on ecrit negligeable => pas de pb a faire ceci ?
    {
      const Discretisation_base& discr = mon_equation_->discretisation();
      Nom nom_discr = discr.que_suis_je();
      Nom type = "negligeable_";
      type += nom_discr.substr_old(1, 3);
      os << type << finl;
    }
  // Verification que l'appel a bien ete fait a Modele_turbulence_hyd_base::limiter_viscosite_turbulente()
  assert(mp_sum(borne_visco_turb_.size()) != 0);
  return 0;
}
/*! @brief Reprend la loi de paroi
 *
 * @param (Entree&) un flot d'entree
 * @return (int) renvoie toujours 0
 */
int Modele_turbulence_hyd_base::reprendre(Entree& is)
{
  if (loipar_.non_nul())
    loipar_->reprendre(is);
  return 0;
}

/*! @brief Effectue l'ecriture d'une identite si cela est necessaire.
 *
 */
void Modele_turbulence_hyd_base::a_faire(Sortie& os) const
{
  int afaire, special;
  EcritureLectureSpecial::is_ecriture_special(special, afaire);

  if (afaire)
    {
      Nom mon_ident(que_suis_je());
      mon_ident += equation().probleme().domaine().le_nom();
      double temps = equation().inconnue().temps();
      mon_ident += Nom(temps, "%e");
      os << mon_ident << finl;
      os << que_suis_je() << finl;
      os.flush();
    }
}

void Modele_turbulence_hyd_base::limiter_viscosite_turbulente()
{
  // On initial
  int size = viscosite_turbulente().valeurs().size();
  if (borne_visco_turb_.size() == 0)
    borne_visco_turb_ = la_viscosite_turbulente_.valeurs();
  borne_visco_turb_ = XNUTM_;
  if (calcul_borne_locale_visco_turb_ && (equation().schema_temps().nb_pas_dt() != 0 || equation().probleme().reprise_effectuee()))
    {
      // On recalcule les bornes de la viscosite turbulente apres le premier pas de temps
      const Operateur_Diff_base& op_diff = ref_cast(Operateur_Diff_base, equation().operateur(0).l_op_base());
      const Operateur_Conv_base& op_conv = ref_cast(Operateur_Conv_base, equation().operateur(1).l_op_base());
      op_diff.calculer_borne_locale(borne_visco_turb_, op_conv.dt_stab_conv(), dt_diff_sur_dt_conv_);
    }
  // On borne la viscosite turbulente
  int nb_elem = equation().domaine_dis().domaine().nb_elem();
  assert(nb_elem == size);
  int compt = 0;
  Debog::verifier("Modele_turbulence_hyd_base::limiter_viscosite_turbulente la_viscosite_turbulente before", la_viscosite_turbulente_.valeurs());

  CDoubleArrView borne_visco_turb = borne_visco_turb_.view_ro();
  DoubleArrView corr_visco_turb = static_cast<DoubleVect&>(corr_visco_turb_.valeurs()).view_wo();
  DoubleArrView visco_turb = static_cast<DoubleVect&>(la_viscosite_turbulente_.valeurs()).view_rw();
  // Remplissage des tableaux de travail:
  Kokkos::parallel_reduce(start_gpu_timer(__KERNEL_NAME__),
                          Kokkos::RangePolicy<>(0, nb_elem), KOKKOS_LAMBDA(
                            const int elem, int& compt_)
  {
    if (visco_turb(elem) > borne_visco_turb(elem))
      {
        compt_++;
        corr_visco_turb(elem) = borne_visco_turb(elem) / visco_turb(elem);
        visco_turb(elem) = borne_visco_turb(elem);
      }
    else
      corr_visco_turb(elem) = 1.;
  }, Kokkos::Sum<int>(compt));
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
  corr_visco_turb_.changer_temps(mon_equation_->inconnue().temps());
  la_viscosite_turbulente_.valeurs().echange_espace_virtuel();
  Debog::verifier("Modele_turbulence_hyd_base::limiter_viscosite_turbulente la_viscosite_turbulente after", la_viscosite_turbulente_.valeurs());

  // On imprime
  int imprimer_compt = 0;
  if (mon_equation_->schema_temps().temps_impr() <= mon_equation_->schema_temps().pas_de_temps())
    imprimer_compt = 1;
  else
    {
      if (mon_equation_->schema_temps().nb_pas_dt() == 0)
        imprimer_compt = 0;
      else
        {
          // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
          double i, j, epsilon = 1.e-8;
          modf(mon_equation_->schema_temps().temps_courant() / mon_equation_->schema_temps().temps_impr() + epsilon, &i);
          modf((mon_equation_->schema_temps().temps_courant() - mon_equation_->schema_temps().pas_de_temps()) / mon_equation_->schema_temps().temps_impr() + epsilon, &j);
          if (i > j)
            imprimer_compt = 1;
        }
    }
  if (imprimer_compt)
    {
      // PL: optimization to avoid 2 mp_sum instead 1:
      ArrOfInt tmp(2);
      tmp[0] = compt;
      tmp[1] = size;
      mp_sum_for_each_item(tmp);
      /*      compt=mp_sum(compt);
       size=mp_sum(size); */
      double pourcent = 100.0 * (double(tmp[0]) / double(tmp[1]));
      if (je_suis_maitre() && pourcent > 0)
        Cout << "\nTurbulent viscosity has been limited on " << pourcent << " % of cells mesh." << finl;
    }
}
