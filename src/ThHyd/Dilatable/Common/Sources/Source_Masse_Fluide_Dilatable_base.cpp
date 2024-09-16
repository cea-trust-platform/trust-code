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

#include <Source_Masse_Fluide_Dilatable_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_base(Source_Masse_Fluide_Dilatable_base, "Source_Masse_Fluide_Dilatable_base", Objet_U);

// XD mass_source objet_u mass_source -1 Mass source used in a dilatable simulation to add/reduce a mass at the boundary (volumetric source in the first cell of a given boundary).

Sortie& Source_Masse_Fluide_Dilatable_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Source_Masse_Fluide_Dilatable_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("bord", &nom_bord_, Param::REQUIRED); // XD_ADD_P chaine Name of the boundary where the source term is applied
  param.ajouter("surfacic_flux", &ch_front_source_, Param::REQUIRED); // XD_ADD_P front_field_base The boundary field that the user likes to apply: for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_t
  param.lire_avec_accolades_depuis(is);

  if (!sub_type(Champ_front_base, ch_front_source_.valeur()))
    {
      Cerr << "Source_Masse_Fluide_Dilatable_base::readOn() - param 'surfacic_flux' must be a Champ_front!!" << finl;
      Process::exit();
    }
  return is;
}

void Source_Masse_Fluide_Dilatable_base::associer_domaine_cl(const Domaine_Cl_dis_base& z)
{
  domaine_cl_dis_ = z;
}

void Source_Masse_Fluide_Dilatable_base::completer()
{
  Cerr << "Source_Masse_Fluide_Dilatable_base::completer() ..." << finl;
  // Initialize properly the champ_front - not done automatically because we are not in the scope of
  // boundary conditions. This will:
  //   - call associer_fr_dis_base()
  //   - fix the number of temporal values for champ_front.
  //   - call initialiser()
  // This is all inspired by what happens in Conds_lim::completer() and Conds_lim::initialiser() ...
  bool ok = false;
  for (int n_bord = 0; n_bord < domaine_cl_dis_->nb_cond_lim(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_cl_dis_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      if (le_bord.le_nom() == nom_bord_)
        {
          ch_front_source_->associer_fr_dis_base(la_cl->frontiere_dis());
          ok = true;
          break;
        }
    }

  if (!ok)
    {
      Cerr << "Source_Masse_Fluide_Dilatable_base::associer_domaine(): Internal error - should not happen !!" << finl;
      Process::exit();
    }

  // Fix the number of temporal values for champ_front.
  int nb_cases = domaine_cl_dis_->equation().schema_temps().nb_valeurs_temporelles();
  ch_front_source_->fixer_nb_valeurs_temporelles(nb_cases);

  // Initialisation of the champ_front:
  double tps = domaine_cl_dis_->equation().schema_temps().temps_courant();
  ch_front_source_->initialiser(tps, domaine_cl_dis_->inconnue());

  ncomp_ = ch_front_source_->valeurs().line_size();
}

void Source_Masse_Fluide_Dilatable_base::mettre_a_jour(double temps)
{
  //ch_front_source_->avancer(temps);
}

void Source_Masse_Fluide_Dilatable_base::changer_temps_futur(double temps, int i)
{
  ch_front_source_->changer_temps_futur(temps, i);
}

void Source_Masse_Fluide_Dilatable_base::set_temps_defaut(double temps)
{
  ch_front_source_->set_temps_defaut(temps);
}
