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
#include <EcritureLectureSpecial.h>
#include <Convection_Diffusion_std.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Param.h>

Implemente_base(Modele_turbulence_scal_base, "Modele_turbulence_scal_base", Objet_U);
// XD turbulence_paroi_scalaire_base objet_u turbulence_paroi_scalaire_base -1 Basic class for wall laws for energy equation.

// XD modele_turbulence_scal_base objet_u modele_turbulence_scal_base -1 Basic class for turbulence model for energy equation.

Sortie& Modele_turbulence_scal_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_scal_base::readOn(Entree& is)
{
  Cerr << "Reading of data for a " << que_suis_je() << " scalar turbulence model" << finl;
  Param param(que_suis_je());
  Motcle mot = "turbulence_paroi";
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  const Probleme_base& le_pb = mon_equation_->probleme();
  // lp loi de paroi du modele de turbulence de l'hydraulique
  const RefObjU& modele_turbulence = le_pb.equation(0).get_modele(TURBULENCE);
  const Modele_turbulence_hyd_base& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
  if (loipar_.est_nul() && mod_turb_hydr.has_loi_paroi_hyd())
    {
      Cerr << finl;
      Cerr << "Mot cle " << mot << "non trouve !" << finl;
      Cerr << "On doit lire une loi de paroi dans le modele de turbulence pour un scalaire." << finl;
      Cerr << finl;
      exit();
    }
  return is;
}

void Modele_turbulence_scal_base::set_param(Param& param)
{
  param.ajouter("dt_impr_nusselt", &dt_impr_nusselt_); // XD_ADD_P floattant Keyword to print local values of Nusselt number and temperature near a wall during a turbulent calculation. The values will be printed in the _Nusselt.face file each dt_impr_nusselt time period. The local Nusselt expression is as follows : Nu = ((lambda+lambda_t)/lambda)*d_wall/d_eq where d_wall is the distance from the first mesh to the wall and d_eq is given by the wall law. This option also gives the value of d_eq and h = (lambda+lambda_t)/d_eq and the fluid temperature of the first mesh near the wall. NL2 For the Neumann boundary conditions (flux_impose), the <<equivalent>> wall temperature given by the wall law is also printed (Tparoi equiv.) preceded for VEF calculation by the edge temperature <<T face de bord>>.
  param.ajouter_non_std("dt_impr_nusselt_mean_only", (this)); // XD attr dt_impr_nusselt_mean_only dt_impr_nusselt_mean_only dt_impr_nusselt_mean_only 1 This keyword is used to print the mean values of Nusselt ( obtained with the wall laws) on each boundary, into a file named datafile_ProblemName_nusselt_mean_only.out. periode refers to the printing period, this value is expressed in seconds. If you don\'t use the optional keyword boundaries, all the boundaries will be considered. If you use it, you must specify nb_boundaries which is the number of boundaries on which you want to calculate the mean values, then you have to specify their names.
  param.ajouter_non_std("turbulence_paroi", this); // XD_ADD_P turbulence_paroi_scalaire_base Keyword to set the wall law.
}
int Modele_turbulence_scal_base::lire_motcle_non_standard(const Motcle& motlu, Entree& is)
{
  Motcle mot = "turbulence_paroi";
  if (motlu == mot)
    {
      Turbulence_paroi_scal_base::typer_lire_turbulence_paroi_scal(loipar_, *this, is);

      is >> loipar_.valeur();
      return 1;
    }
  else if (!loipar_->que_suis_je().contient("negligeable")) // ToDo factorize with Modele_turbulence_hyd_base::lire_motcle_non_standard
    {
      if (mot == "dt_impr_nusselt_mean_only")
        {
          // XD dt_impr_nusselt_mean_only objet_lecture nul 1 not_set
          // XD attr dt_impr floattant dt_impr 0 not_set
          // XD attr boundaries listchaine boundaries 1 not_set
          Nom accolade_ouverte = "{";
          Nom accolade_fermee = "}";
          nom_fichier_ = Objet_U::nom_du_cas() + "_" + equation().probleme().le_nom() + "_nusselt_mean_only";
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
              Cerr << "A specification of kind : dt_impr_nusselt_mean_only { dt_impr periode [boundaries nb_boundaries boundary_name1 boundary_name2 ... ] } was expected." << finl;
              exit();
            }
          is >> motlu;
          if (motlu != "dt_impr")
            {
              Cerr << "We expected dt_impr..." << finl;
              exit();
            }
          is >> dt_impr_nusselt_mean_only_;

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
                              Cerr << "Problem in the dt_impr_nusselt_mean_only instruction:" << finl;
                              Cerr << "The boundary named '" << boundaries_list_[i] << "' is not a boundary of the domain " << dom.le_nom() << "." << finl;
                              exit();
                            }
                        }
                    }
                  // lecture accolade fermee
                  is >> motlu;
                  if (motlu != accolade_fermee)
                    {
                      Cerr << "Problem in the dt_impr_nusselt_mean_only instruction:" << finl;
                      Cerr << "TRUST wants to read a '" << accolade_fermee << "' but find '" << motlu << "'!!" << finl;
                      exit();
                    }
                }
              else
                {
                  Cerr << motlu << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard" << finl;
                  Cerr << "A specification of kind : dt_impr_nusselt_mean_only { dt_impr periode [boundaries nb_boundaries boundary_name1 boundary_name2 ... ] } was expected." << finl;
                  exit();
                }
            }
        } // fin dt_impr_nusselt_mean_only
      else
        {
          Cerr << "Please remove dt_impr_nusselt option if the wall law is of Negligeable type." << finl;
          exit();
        }
    } // fin loi paroi negligeable
  else
    {
      Cerr << mot << "n'est pas un mot compris par " << que_suis_je() << "dans lire_motcle_non_standard" << finl;
      exit();
    }
  return -1;

}

/*! @brief Associe l'equation passe en parametre au modele de turbulence.
 *
 * L'equation est caste en Convection_Diffusion_std.
 *
 * @param (Equation_base& eqn) l'equation a laquelle l'objet s'associe
 */
void Modele_turbulence_scal_base::associer_eqn(const Equation_base& eqn)
{
  mon_equation_ = ref_cast(Convection_Diffusion_std, eqn);
}

/*! @brief NE FAIT RIEN
 *
 * @param (Domaine_dis_base&) un domaine discretise
 * @param (Domaine_Cl_dis_base&) un domaine de conditions aux limites discretisees
 */
void Modele_turbulence_scal_base::associer(const Domaine_dis_base&, const Domaine_Cl_dis_base&)
{
  ;
}

/*! @brief Discretise le modele de turbulence.
 *
 */
void Modele_turbulence_scal_base::discretiser()
{
  Cerr << "Turbulence scalar model discretization" << finl;
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  const Discretisation_base& dis = mon_equation_->discretisation();
  const Domaine_dis_base& z = mon_equation_->domaine_dis();
  dis.discretiser_champ("champ_elem", z, "diffusivite_turbulente", "m2/s", 1, sch.temps_courant(), diffusivite_turbulente_);
  dis.discretiser_champ("champ_elem", z, "conductivite_turbulente", "W/m/K", 1, sch.temps_courant(), conductivite_turbulente_);
  champs_compris_.ajoute_champ(diffusivite_turbulente_);
}

/*! @brief Complete le modele de turbulence: met a jour des references de l'objet.
 *
 */
void Modele_turbulence_scal_base::completer()
{
}

/*! @brief A surcharger dans les classes derivees qui possedent une ou des equations Appeler preparer_pas_de_temps sur ces equations.
 *
 */
bool Modele_turbulence_scal_base::initTimeStep(double dt)
{
  return true;
}

/*! @brief NE FAIT RIEN
 *
 * @return (int) renvoie toujours 1
 */
int Modele_turbulence_scal_base::preparer_calcul()
{
  if (loipar_.non_nul())
    loipar_->init_lois_paroi();
  mettre_a_jour(0.);
  return 1;
}

/*! @brief NE FAIT RIEN
 *
 * @param (Sortie&) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Modele_turbulence_scal_base::sauvegarder(Sortie& os) const
{
  if (loipar_.non_nul())
    return loipar_->sauvegarder(os);
  else
    return 0;
}

/*! @brief Effectue l'ecriture d'une identite si cela est necessaire.
 *
 */
void Modele_turbulence_scal_base::a_faire(Sortie& os) const
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

/*! @brief NE FAIT RIEN
 *
 * @param (Entree&) un flot d'entree
 * @return (int) renvoie toujours 1
 */
int Modele_turbulence_scal_base::reprendre(Entree& is)
{
  // Pour l'instant on ne lit plus dans le .sauv
  /*Nom typ ;
   is>>typ;
   if (!loipar.non_nul())
   loipar.typer(typ);        */

  if (loipar_.non_nul())
    loipar_->reprendre(is);
  return 1;
}

void Modele_turbulence_scal_base::creer_champ(const Motcle& motlu)
{
  if (loipar_.non_nul())
    loipar_->creer_champ(motlu);
}

bool Modele_turbulence_scal_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (champs_compris_.has_champ(nom, ref_champ))
    return true;

  if (loipar_.non_nul())
    if (loipar_->has_champ(nom, ref_champ))
      return true;

  return false; /* rien trouve */
}

bool Modele_turbulence_scal_base::has_champ(const Motcle& nom) const
{
  if (champs_compris_.has_champ(nom))
    return true;

  if (loipar_.non_nul())
    if (loipar_->has_champ(nom))
      return true;

  return false; /* rien trouve */
}

const Champ_base& Modele_turbulence_scal_base::get_champ(const Motcle& nom) const
{
  OBS_PTR(Champ_base) ref_champ;

  if (champs_compris_.has_champ(nom, ref_champ))
    return ref_champ;

  if (loipar_.non_nul())
    if (loipar_->has_champ(nom, ref_champ))
      return ref_champ;

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}

void Modele_turbulence_scal_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  if (opt == DESCRIPTION)
    Cerr << que_suis_je() << " : " << champs_compris_.liste_noms_compris() << finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

  if (loipar_.non_nul())
    loipar_->get_noms_champs_postraitables(nom, opt);
}

/*! @brief Indique s'il faut imprimer ou non le Nusselt local
 *
 * @param (double temps_courant, double dt) un flot d'entree
 * @return (int) renvoie 1 si on imprime, 0 sinon
 */
int Modele_turbulence_scal_base::limpr_nusselt(double temps_courant, double temps_prec, double dt, double dt_nusselt) const
{
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  if (sch.nb_pas_dt() == 0)
    return 0;
  if (dt_nusselt <= dt
      || ((sch.temps_cpu_max_atteint() || (!get_disable_stop() && sch.stop_lu()) || sch.temps_final_atteint() || sch.nb_pas_dt_max_atteint() || sch.nb_pas_dt() == 1 || sch.stationnaire_atteint())
          && dt_nusselt != DMAXFLOAT))
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j, epsilon = 1.e-8;
      modf(temps_courant / dt_nusselt + epsilon, &i);
      modf(temps_prec / dt_nusselt + epsilon, &j);
      return (i > j);
    }
}

/*! @brief Effectue l'impression si cela est necessaire.
 *
 * @return renvoie toujours 1
 */
void Modele_turbulence_scal_base::imprimer(Sortie& os) const
{
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  double temps_courant = sch.temps_courant();
  double dt = sch.pas_de_temps();
  if (limpr_nusselt(temps_courant, sch.temps_precedent(), dt, dt_impr_nusselt_))
    loipar_->imprimer_nusselt(os);
  if (limpr_nusselt(temps_courant, sch.temps_precedent(), dt, dt_impr_nusselt_mean_only_))
    loipar_->imprimer_nusselt_mean_only(os, boundaries_, boundaries_list_, nom_fichier_);
}
