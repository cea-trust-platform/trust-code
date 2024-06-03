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

#include <Verif_Cl.h>
#include <Domaine_Cl_dis.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Entree_fluide_concentration_imposee.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Entree_fluide_T_h_imposee.h>
#include <Neumann_paroi.h>
#include <Neumann_paroi_adiabatique.h>
#include <Neumann_paroi_flux_nul.h>
#include <Symetrie.h>
#include <Echange_global_impose.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <Scalaire_impose_paroi.h>
#include <Motcle.h>
#include <Frontiere_dis_base.h>

/*! @brief Teste la compatibilite des conditions aux limites thermiques et hydrauliques.
 *
 *     La liste des compatibilites est la suivante:
 *     -----------------------------------------------------------------------
 *     Hydraulique                      |       Thermique
 *     -----------------------------------------------------------------------
 *     Entree_fluide_vitesse_imposee ===> Entree_fluide_temperature_imposee
 *     Entree_fluide_vitesse_imposee_libre => Neumann_sortie_libre
 *     -----------------------------------------------------------------------
 *     Dirichlet_paroi_fixe |
 *     Dirichlet_paroi_defilante =======> Neumann_paroi_adiabatique
 *     =================================> Neumann_paroi
 *     =================================> Echange_global_impose
 *     =================================> Echange_externe_impose
 *     =================================> Scalaire_impose_paroi
 *     -----------------------------------------------------------------------
 *     Neumann_sortie_libre ============> Entree_fluide_temperature_imposee
 *     =================================> Neumann_sortie_libre
 *     -----------------------------------------------------------------------
 *     Symetrie ========================> Neumann_paroi_adiabatique
 *     =================================> Neumann_paroi
 *     =================================> Echange_global_impose
 *     =================================> Echange_externe_impose
 *     =================================> Scalaire_impose_paroi
 *     =================================> Symetrie
 *     -----------------------------------------------------------------------
 *     Periodique ======================> Periodique
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param[in] (Domaine_Cl_dis& domaine_Cl_th)
 * @return (int) renvoie toujours 1
 * @throws nombres de conditions aux limites differents
 * @throws conditions aux limites hydraulique et thermique incompatibles
 */
int tester_compatibilite_hydr_thermique(const Domaine_Cl_dis& domaine_Cl_hydr, const Domaine_Cl_dis& domaine_Cl_th)
{

  int nb_Cl = domaine_Cl_hydr.nb_cond_lim();

  if (domaine_Cl_th.nb_cond_lim() != nb_Cl)
    {
      Cerr << "The two objects of Domaine_Cl_dis type don't have" << finl;
      Cerr << "the same number of boundary conditions." << finl;
      Process::exit();
    }

  for (int num_Cl = 0; num_Cl < nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_hydr = domaine_Cl_hydr.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_th = domaine_Cl_th.les_conditions_limites(num_Cl);
      if (sub_type(Entree_fluide_vitesse_imposee_libre, la_cl_hydr.valeur()))
        {
          if ((sub_type(Entree_fluide_temperature_imposee, la_cl_th.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_th.valeur())))
            { /* Do nothing */ }
          else if ((sub_type(Entree_fluide_T_h_imposee, la_cl_th.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_th.valeur())))
            { /* Do nothing */ }
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
      else if (sub_type(Entree_fluide_vitesse_imposee, la_cl_hydr.valeur()))
        {
          if (sub_type(Entree_fluide_temperature_imposee, la_cl_th.valeur()))
            { /* Do nothing */ }
          else if (sub_type(Entree_fluide_T_h_imposee, la_cl_th.valeur()))
            { /* Do nothing */ }
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
      else if ((sub_type(Dirichlet_paroi_fixe, la_cl_hydr.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl_hydr.valeur())))
        {
          if ((sub_type(Neumann_paroi_adiabatique, la_cl_th.valeur())) || (sub_type(Neumann_paroi, la_cl_th.valeur())) || (sub_type(Echange_global_impose, la_cl_th.valeur()))
              || (sub_type(Echange_externe_impose, la_cl_th.valeur())) || (sub_type(Scalaire_impose_paroi, la_cl_th.valeur())))
            { /* Do nothing */ }
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
      else if (sub_type(Neumann_sortie_libre, la_cl_hydr.valeur()))
        {
          if ((sub_type(Entree_fluide_temperature_imposee, la_cl_th.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_th.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
      else if (sub_type(Symetrie, la_cl_hydr.valeur()))
        {
          if ((sub_type(Neumann_paroi_adiabatique, la_cl_th.valeur())) || (sub_type(Neumann_paroi, la_cl_th.valeur())) || (sub_type(Echange_global_impose, la_cl_th.valeur()))
              || (sub_type(Echange_externe_impose, la_cl_th.valeur())) || (sub_type(Scalaire_impose_paroi, la_cl_th.valeur())) || (sub_type(Symetrie, la_cl_th.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
      else if (sub_type(Periodique, la_cl_hydr.valeur()))
        {
          if (sub_type(Periodique, la_cl_th.valeur()))
            { /* Do nothing */}
          else
            {
              message_erreur_therm(la_cl_hydr, la_cl_th, num_Cl);
            }
        }
    }
  return 1;
}

/*! @brief Affiche un message d'erreur pour la fonction precedente
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param (Domaine_Cl_dis& domaine_Cl_th)
 * @param (int num_Cl) numero de la CL
 * @return (int) renvoie toujours 1
 */
int message_erreur_therm(const Cond_lim& la_cl_hydr, const Cond_lim& la_cl_th, int& num_Cl)
{
  Cerr << "The hydraulic and thermal boundary conditions are not consitent on border:" << finl;
  Cerr << "Boundary conditions number " << num_Cl << " \"" << la_cl_th.frontiere_dis().le_nom() << "\" have been assigned to : " << finl;
  Cerr << la_cl_hydr.valeur().que_suis_je() << " and " << la_cl_th.valeur().que_suis_je() << " !! " << finl;
  Process::exit();
  return 1;
}

/*! @brief Teste la compatibilite des conditions aux limites concentration et hydrauliques.
 *
 *     La liste des compatibilites est la suivante:
 *     -----------------------------------------------------------------------
 *     Hydraulique                      |       Concentration
 *     -----------------------------------------------------------------------
 *     Entree_fluide_vitesse_imposee ===> Entree_fluide_concentration_imposee
 *     =================================> Neumann_sortie_libre
 *     -----------------------------------------------------------------------
 *     Dirichlet_paroi_fixe |
 *     Dirichlet_paroi_defilante =======> Neumann_paroi_flux_nul
 *     -----------------------------------------------------------------------
 *     Neumann_sortie_libre ============> Neumann_sortie_libre
 *     =================================> Entree_fluide_concentration_imposee
 *     -----------------------------------------------------------------------
 *     Symetrie ========================> Symetrie
 *     =================================> Neumann_paroi_flux_nul
 *     -----------------------------------------------------------------------
 *     Periodique ======================> Periodique
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param (Domaine_Cl_dis& domaine_Cl_co)
 * @return (int) renvoie toujours 1
 * @throws nombres de conditions aux limites differents
 * @throws conditions aux limite hydraulique et concentration incompatible
 */
int tester_compatibilite_hydr_concentration(const Domaine_Cl_dis& domaine_Cl_hydr, const Domaine_Cl_dis& domaine_Cl_co)
{

  int nb_Cl = domaine_Cl_hydr.nb_cond_lim();

  if (domaine_Cl_co.nb_cond_lim() != nb_Cl)
    {
      Cerr << "The two objects of Domaine_Cl_dis type don't have" << finl;
      Cerr << "the same number of boundary conditions." << finl;
      Process::exit();
    }

  for (int num_Cl = 0; num_Cl < nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_hydr = domaine_Cl_hydr.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_co = domaine_Cl_co.les_conditions_limites(num_Cl);

      if (sub_type(Entree_fluide_vitesse_imposee, la_cl_hydr.valeur()))
        {
          if ((sub_type(Entree_fluide_concentration_imposee, la_cl_co.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_co.valeur())))
            { /* Do nothing */ }
          else
            {
              message_erreur_conc(la_cl_hydr, la_cl_co, num_Cl);
            }
        }
      else if ((sub_type(Dirichlet_paroi_fixe, la_cl_hydr.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl_hydr.valeur())))
        {
          if ((!sub_type(Neumann_paroi_flux_nul, la_cl_co.valeur())) && (!sub_type(Neumann_paroi, la_cl_co.valeur())) && (!sub_type(Scalaire_impose_paroi, la_cl_co.valeur())))
            {
              message_erreur_conc(la_cl_hydr, la_cl_co, num_Cl);
            }
        }
      else if (sub_type(Neumann_sortie_libre, la_cl_hydr.valeur()))
        {
          if ((sub_type(Neumann_sortie_libre, la_cl_co.valeur())) || (sub_type(Entree_fluide_concentration_imposee, la_cl_co.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_conc(la_cl_hydr, la_cl_co, num_Cl);
            }
        }
      else if (sub_type(Symetrie, la_cl_hydr.valeur()))
        {
          if ((sub_type(Symetrie, la_cl_co.valeur())) || (sub_type(Neumann_paroi_flux_nul, la_cl_co.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_conc(la_cl_hydr, la_cl_co, num_Cl);
            }
        }
      else if (sub_type(Periodique, la_cl_hydr.valeur()))
        {
          if (sub_type(Periodique, la_cl_co.valeur()))
            { /* Do nothing */}
          else
            {
              message_erreur_conc(la_cl_hydr, la_cl_co, num_Cl);
            }
        }
    }

  return 1;
}

/*! @brief Affiche un message d'erreur pour la fonction precedente
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param (Domaine_Cl_dis& domaine_Cl_co)
 * @param (int num_Cl) numero de la CL
 * @return (int) renvoie toujours 1
 */
int message_erreur_conc(const Cond_lim& la_cl_hydr, const Cond_lim& la_cl_co, int& num_Cl)
{
  Cerr << "The hydraulic and concentration boundary conditions are not consitent on border:" << finl;
  Cerr << "Boundary conditions number " << num_Cl << " \"" << la_cl_co.frontiere_dis().le_nom() << "\" have been assigned to : " << finl;
  Cerr << la_cl_hydr.valeur().que_suis_je() << " and " << la_cl_co.valeur().que_suis_je() << " !! " << finl;
  Process::exit();
  return 1;
}

/*! @brief Teste la compatibilite des conditions aux limites fraction massqiues et hydrauliques.
 *
 *     La liste des compatibilites est la suivante:
 *     -----------------------------------------------------------------------
 *     Hydraulique                      |       Fraction massique
 *     -----------------------------------------------------------------------
 *     Entree_fluide_vitesse_imposee ===> Entree_fluide_fraction_massique_imposee
 *     Entree_fluide_vitesse_imposee_libre => Neumann_sortie_libre
 *     Entree_fluide_vitesse_imposee ===> Echange_externe_impose
 *     Entree_fluide_vitesse_imposee ===> Neumann_paroi
 *     -----------------------------------------------------------------------
 *     Dirichlet_paroi_fixe |
 *     Dirichlet_paroi_defilante =======> Neumann_paroi_flux_nul
 *     =================================> Echange_externe_impose
 *     -----------------------------------------------------------------------
 *     Neumann_sortie_libre ============> Entree_fluide_fraction_massique_imposee
 *     =================================> Neumann_sortie_libre
 *     -----------------------------------------------------------------------
 *     Symetrie ========================> Symetrie
 *     =================================> Neumann_paroi_flux_nul
 *     =================================> Neumann_paroi
 *     -----------------------------------------------------------------------
 *     Periodique ======================> Periodique
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param[in] (Domaine_Cl_dis& domaine_Cl_fm)
 * @return (int) renvoie toujours 1
 * @throws nombres de conditions aux limites differents
 * @throws conditions aux limites hydraulique et fraction massiques incompatibles
 */
int tester_compatibilite_hydr_fraction_massique(const Domaine_Cl_dis& domaine_Cl_hydr, const Domaine_Cl_dis& domaine_Cl_fm)
{

  int nb_Cl = domaine_Cl_hydr.nb_cond_lim();

  if (domaine_Cl_fm.nb_cond_lim() != nb_Cl)
    {
      Cerr << "The two objects of Domaine_Cl_dis type don't have" << finl;
      Cerr << "the same number of boundary conditions." << finl;
      Process::exit();
    }

  for (int num_Cl = 0; num_Cl < nb_Cl; num_Cl++)
    {
      const Cond_lim& la_cl_hydr = domaine_Cl_hydr.les_conditions_limites(num_Cl);
      const Cond_lim& la_cl_fm = domaine_Cl_fm.les_conditions_limites(num_Cl);
      if (sub_type(Entree_fluide_vitesse_imposee_libre, la_cl_hydr.valeur()))
        {
          if ((sub_type(Entree_fluide_fraction_massique_imposee, la_cl_fm.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_fm.valeur())) || (sub_type(Echange_externe_impose, la_cl_fm.valeur()))
              || (sub_type(Neumann_paroi, la_cl_fm.valeur())))
            { /* Do nothing */ }
          else
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }
        }
      else if (sub_type(Entree_fluide_vitesse_imposee, la_cl_hydr.valeur()))
        {
          if (sub_type(Entree_fluide_fraction_massique_imposee, la_cl_fm.valeur()))
            { /* Do nothing */ }
          else
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }
        }
      else if ((sub_type(Dirichlet_paroi_fixe, la_cl_hydr.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl_hydr.valeur())))
        {
          if ((!sub_type(Neumann_paroi_flux_nul, la_cl_fm.valeur())) && (!sub_type(Neumann_paroi, la_cl_fm.valeur())) && (!sub_type(Scalaire_impose_paroi, la_cl_fm.valeur()))
              && (!sub_type(Echange_externe_impose, la_cl_fm.valeur())))
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }

        }
      else if (sub_type(Neumann_sortie_libre, la_cl_hydr.valeur()))
        {
          if ((sub_type(Entree_fluide_fraction_massique_imposee, la_cl_fm.valeur())) || (sub_type(Neumann_sortie_libre, la_cl_fm.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }
        }
      else if (sub_type(Symetrie, la_cl_hydr.valeur()))
        {
          if ((sub_type(Symetrie, la_cl_fm.valeur())) || (sub_type(Neumann_paroi_flux_nul, la_cl_fm.valeur())) || (sub_type(Neumann_paroi, la_cl_fm.valeur())))
            { /* Do nothing */}
          else
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }
        }
      else if (sub_type(Periodique, la_cl_hydr.valeur()))
        {
          if (sub_type(Periodique, la_cl_fm.valeur()))
            { /* Do nothing */}
          else
            {
              message_erreur_fraction_massique(la_cl_hydr, la_cl_fm, num_Cl);
            }
        }
    }
  return 1;
}

/*! @brief Affiche un message d'erreur pour la fonction precedente
 *
 * @param (Domaine_Cl_dis& domaine_Cl_hydr)
 * @param (Domaine_Cl_dis& domaine_Cl_frac_mass)
 * @param (int num_Cl) numero de la CL
 * @return (int) renvoie toujours 1
 */
int message_erreur_fraction_massique(const Cond_lim& la_cl_hydr, const Cond_lim& la_cl_frac_mass, int& num_Cl)
{
  Cerr << "The hydraulic and massic fraction boundary conditions are not consitent on border:" << finl;
  Cerr << "Boundary conditions number " << num_Cl << " \"" << la_cl_frac_mass.frontiere_dis().le_nom() << "\" have been assigned to : " << finl;
  Cerr << la_cl_hydr.valeur().que_suis_je() << " and " << la_cl_frac_mass.valeur().que_suis_je() << " !! " << finl;
  Process::exit();
  return 1;
}
