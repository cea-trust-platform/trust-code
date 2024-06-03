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

#include <Terme_Puissance_Thermique.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Discretisation_base.h>
#include <Champ_val_tot_sur_vol_base.h>

/*! @brief Lit le terme de puissance thermique a partir d'un flot d'entree et d une equation pour eventuellement
 *
 *     acceder a la discretisation afin de typer la_puissance.
 *     Lit uniquement un champ donne representant la_puissance.
 *
 * @param (Entree& is) un flot d'entree
 */
void Terme_Puissance_Thermique::lire_donnees(Entree& is,const Equation_base& eqn)
{
  Cerr << "Lecture du Champ Puissance" << finl;

  Motcle type;
  is >> type;
  if (type=="Valeur_totale_sur_volume")
    {
      if (eqn.discretisation().is_vdf())
        type += "_VDF";
      else  if (eqn.discretisation().is_vef())
        type += "_VEF";
    }
  la_puissance_lu.typer(type);
  Champ_Don_base& ch_puissance_lu = ref_cast(Champ_Don_base,la_puissance_lu.valeur());
  is >> ch_puissance_lu;
  const int nb_comp = ch_puissance_lu.nb_comp();

  eqn.probleme().discretisation().discretiser_champ(eqn.discretisation().is_polymac_family() ? "temperature" : "champ_elem", eqn.domaine_dis(), "pp", "1",nb_comp,0., la_puissance);
  la_puissance_lu->fixer_nb_comp(nb_comp);
  if (ch_puissance_lu.le_nom()=="anonyme") ch_puissance_lu.nommer("Puissance_volumique");

  for (int n = 0; n < nb_comp; n++) la_puissance_lu->fixer_nom_compo(n, ch_puissance_lu.le_nom() + (nb_comp > 1 ? Nom(n) :""));
  for (int n = 0; n < nb_comp; n++) la_puissance->fixer_nom_compo(n, ch_puissance_lu.le_nom() + (nb_comp > 1 ? Nom(n) :""));
  // PL: Il faut faire nommer_completer_champ_physique les 2 champs (plantage sinon pour une puissance de type Champ_fonc_tabule)
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(),ch_puissance_lu.le_nom(),"W/m3",la_puissance_lu,eqn.probleme());
  eqn.discretisation().nommer_completer_champ_physique(eqn.domaine_dis(),ch_puissance_lu.le_nom(),"W/m3",la_puissance,eqn.probleme());
  la_puissance.valeur().valeurs() = 0;
  la_puissance.valeur().affecter(ch_puissance_lu);
}

void Terme_Puissance_Thermique::initialiser_champ_puissance(const Equation_base& eqn)
{
  if (sub_type(Champ_val_tot_sur_vol_base,la_puissance_lu.valeur()))
    {
      const Domaine_dis_base& zdis = eqn.domaine_dis().valeur();
      const Domaine_Cl_dis_base& zcldis = eqn.domaine_Cl_dis().valeur();
      Champ_val_tot_sur_vol_base& champ_puis = ref_cast(Champ_val_tot_sur_vol_base,la_puissance_lu.valeur());
      champ_puis.evaluer(zdis,zcldis);
    }
}

void Terme_Puissance_Thermique::mettre_a_jour(double temps)
{
  la_puissance_lu.mettre_a_jour(temps);
  la_puissance.valeur().affecter(la_puissance_lu.valeur());
}

void Terme_Puissance_Thermique::modify_name_file(Nom& fichier) const
{
  if (la_puissance.le_nom()!="Puissance_volumique")
    {
      fichier+="_";
      fichier+=la_puissance.le_nom();
    }
}
