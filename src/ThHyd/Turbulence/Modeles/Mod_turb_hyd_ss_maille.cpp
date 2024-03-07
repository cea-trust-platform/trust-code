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

#include <Mod_turb_hyd_ss_maille.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Param.h>
#include <Discretisation_base.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Debog.h>
#include <stat_counters.h>
#include <Milieu_base.h>

Implemente_base_sans_constructeur(Mod_turb_hyd_ss_maille, "Mod_turb_hyd_ss_maille", Modele_turbulence_hyd_base);

Mod_turb_hyd_ss_maille::Mod_turb_hyd_ss_maille()
{
  methode = "volume"; // Parametre par defaut pour calculer la longueur caracteristique
}

Sortie& Mod_turb_hyd_ss_maille::printOn(Sortie& is) const
{
  return Modele_turbulence_hyd_base::printOn(is);
}

Entree& Mod_turb_hyd_ss_maille::readOn(Entree& is)
{
  return Modele_turbulence_hyd_base::readOn(is);
}

void Mod_turb_hyd_ss_maille::set_param(Param& param)
{
  Modele_turbulence_hyd_base::set_param(param);
  param.ajouter("longueur_maille", &methode);
}

int Mod_turb_hyd_ss_maille::preparer_calcul()
{
  Modele_turbulence_hyd_base::preparer_calcul();
  if (methode != "??")
    Cerr << "The characteristic length of the subgrid scale model is calculated with the method : " << methode << finl;
  calculer_longueurs_caracteristiques();
  mettre_a_jour(0.);
  return 1;
}

/*! @brief Discretise le modele de turbulence.
 *
 * Appelle Modele_turbulence_hyd_base::discretiser()
 *     Initialise les integrales statistiques.
 *
 */
void Mod_turb_hyd_ss_maille::discretiser()
{
  Modele_turbulence_hyd_base::discretiser();
  discretiser_K(mon_equation->schema_temps(), mon_equation->domaine_dis(), energie_cinetique_turb_);
  champs_compris_.ajoute_champ(energie_cinetique_turb_);
}

// E.Saikali
// In the current version of TrioCFD and the FT part in particular, it is possible only to use LES WALE (Smagorinsky,1963,Mon.Weather Rev. 91,99)
// For this reason, associating the wall laws of multi-phase flows is done in this method Mod_turb_hyd_ss_maille::completer()
// We only go inside if the user defines the wall law as loi_standard_hydr, for both discretizations VEF and VDF
// Otherwise, in the case of negligeable, the multiphase wall law is not used
void Mod_turb_hyd_ss_maille::verifie_loi_paroi_diphasique()
{
  const Milieu_base& mil = equation().milieu(); // returns Fluide_Diphasique or Fluide_Incompressible
  const Nom& nom_mil = mil.que_suis_je();
  const Nom& nom_loipar = loipar.valeur().que_suis_je();
  const Nom& nom_eq = equation().que_suis_je();

  if ((nom_loipar == "loi_standard_hydr_VEF" || nom_loipar == "loi_standard_hydr_VDF") && nom_eq == "Navier_Stokes_FT_Disc" && nom_mil == "Fluide_Diphasique")
    {
      const Nom& discr = equation().discretisation().que_suis_je();
      if (discr == "VEF" || discr == "VEFPreP1B")
        {
          loipar.typer("loi_standard_hydr_diphasique_VEF");
          Cerr << "Associating the two-phase hydraulic wall turbulence model : loi_standard_hydr_diphasique_VEF ..." << finl;
        }
      else if (discr == "VDF")
        {
          loipar.typer("loi_standard_hydr_diphasique_VDF");
          Cerr << "Associating the two-phase hydraulic wall turbulence model : loi_standard_hydr_diphasique_VDF ..." << finl;
        }
      else
        {
          Cerr << "Unknown discretization type !!!";
          Process::exit();
        }
      loipar.valeur().associer_modele(*this);
      loipar.valeur().associer(equation().domaine_dis(), equation().domaine_Cl_dis());
    }
}

void Mod_turb_hyd_ss_maille::completer()
{
  Cerr << "Mod_turb_hyd_ss_maille::completer()" << finl;
  verifie_loi_paroi_diphasique();
}

void Mod_turb_hyd_ss_maille::mettre_a_jour(double)
{
  statistiques().begin_count(nut_counter_);
  calculer_viscosite_turbulente();
  calculer_energie_cinetique_turb();
  loipar->calculer_hyd(la_viscosite_turbulente, energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  if (equation().probleme().is_dilatable())
    correction_nut_et_cisaillement_paroi_si_qc(*this);
  energie_cinetique_turb_.valeurs().echange_espace_virtuel();
  la_viscosite_turbulente->valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}

void Mod_turb_hyd_ss_maille::calculer_energie_cinetique_turb()
{
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  double Cq = 0.094;

  // PQ : 10/08/06 : on utilise ici la formule de Schuman : q_sm = (nu_t)^2 / (Cq.l)^2

  const int nb_elem = visco_turb.size();
  if (k.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  for (int elem = 0; elem < nb_elem; elem++)
    k(elem) = visco_turb[elem] * visco_turb[elem] / (Cq * Cq * l_(elem) * l_(elem));

  double temps = mon_equation->inconnue().temps();
  energie_cinetique_turb_.changer_temps(temps);
}

