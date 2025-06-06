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

#include <Champ_Fonc_Tabule_P0_EF.h>
#include <Domaine_VF.h>
#include <Table.h>

Implemente_instanciable(Champ_Fonc_Tabule_P0_EF, "Champ_Fonc_Tabule_P0_EF", Champ_Fonc_P0_EF);

Sortie& Champ_Fonc_Tabule_P0_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Tabule_P0_EF::readOn(Entree& s) { return s; }

void Champ_Fonc_Tabule_P0_EF::associer_param(const VECT(OBS_PTR(Champ_base)) &les_champs, const Table& une_table)
{
  les_ch_param = les_champs;
  la_table = une_table;
}

void Champ_Fonc_Tabule_P0_EF::mettre_a_jour(double t)
{
  DoubleTab& mes_valeurs = valeurs();
  int nb_elem = le_dom_VF->nb_elem(), nb_elem_tot = le_dom_VF->nb_elem_tot(), nb_param = les_ch_param.size();
  DoubleTabs val_params_aux_elems;
  for (int i = 0; i < nb_param; i++)
    {
      assert(les_ch_param[i]->valeurs().dimension(1) == 1 || les_ch_param[i]->valeurs().dimension(1) == mes_valeurs.dimension(1));
      DoubleTab vp(nb_elem_tot, les_ch_param[i]->valeurs().dimension(1));
      val_params_aux_elems.add(vp);
    }
  const DoubleTab& centres_de_gravites = le_dom_VF->xp();
  IntVect les_polys(nb_elem_tot);
  for (int elem = 0; elem < nb_elem_tot; elem++)
    les_polys(elem) = elem;

  // Estimate the field parameter on cells:
  for (int i = 0; i < nb_param; i++)
    les_ch_param[i]->valeur_aux_elems(centres_de_gravites, les_polys, val_params_aux_elems[i]);
  // Compute the field according to the parameter field
  if (la_table->isfonction() != 2)
    {
      const int nbcomp = mes_valeurs.dimension(1);
      std::vector<double> vals;
      vals.reserve(nb_param); // Pre-allocate space once
      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
        for (int ncomp = 0; ncomp < nbcomp; ncomp++)
          {
            vals.clear();
            for (int n = 0; n < nb_param; n++)
              vals.push_back(val_params_aux_elems[n](num_elem, les_ch_param[n]->valeurs().dimension(1) == 1 ? 0 : ncomp));
            mes_valeurs(num_elem, ncomp) = la_table->val(vals, ncomp);
          }
    }
  else
    {
      la_table->valeurs(val_params_aux_elems[0], centres_de_gravites, t, mes_valeurs);
    }

  Champ_Fonc_base::mettre_a_jour(t);
}

int Champ_Fonc_Tabule_P0_EF::initialiser(const double)
{
  Champ_Fonc_Tabule_P0_EF::mettre_a_jour(temps());
  return 1;
}
