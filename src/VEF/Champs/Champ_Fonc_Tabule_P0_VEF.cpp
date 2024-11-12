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

#include <Champ_Fonc_Tabule_P0_VEF.h>
#include <TRUSTTrav.h>
#include <Domaine_VF.h>
#include <Table.h>
#include <ParserView.h>

Implemente_instanciable(Champ_Fonc_Tabule_P0_VEF, "Champ_Fonc_Tabule_P0_VEF", Champ_Fonc_P0_VEF);

Sortie& Champ_Fonc_Tabule_P0_VEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Fonc_Tabule_P0_VEF::readOn(Entree& s) { return s; }

void Champ_Fonc_Tabule_P0_VEF::associer_param(const VECT(OBS_PTR(Champ_base)) &les_champs, const Table& une_table)
{
  les_ch_param = les_champs;
  la_table = une_table;
}

void Champ_Fonc_Tabule_P0_VEF::mettre_a_jour(double t)
{
  const Domaine_VF& domaine_VF = le_dom_VF.valeur();
  const Table& table = la_table.valeur();
  DoubleTab& mes_valeurs = valeurs();
  const int nb_elem = domaine_VF.nb_elem(), nb_elem_tot = domaine_VF.nb_elem_tot(), nb_param = les_ch_param.size();
  const int nbcomp = mes_valeurs.dimension(1);
  const DoubleTab& centres_de_gravites = domaine_VF.xp();

  // ToDo Kokkos: factorize somewhere this array or rewrite valeur_aux_elems !
  IntTrav les_polys(nb_elem_tot);
  IntArrView les_polys_v = static_cast<IntVect&>(les_polys).view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem_tot, KOKKOS_LAMBDA(const int num_elem)
  {
    les_polys_v(num_elem) = num_elem;
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  if (nb_param==1 && nbcomp==1 && table.isfonction()==1)
    {
      // Ported on GPU. ToDo Kokkos, extend to more than one param or more than one nbcomp
      DoubleTrav val_param_aux_elems(nb_elem_tot, nbcomp);
      les_ch_param[0]->valeur_aux_elems(centres_de_gravites, les_polys, val_param_aux_elems);

      ParserView parser(table.parser(0).getString(), 1, nb_elem);
      parser.addVar("val");
      parser.parseString();
      CDoubleTabView val_params_aux_elems_v = val_param_aux_elems.view_ro();
      DoubleTabView mes_valeurs_v = mes_valeurs.view_wo();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(
                             const int num_elem)
      {
        for (int ncomp = 0; ncomp < nbcomp; ncomp++)
          {
            double val = val_params_aux_elems_v(num_elem, ncomp);
            parser.setVar(0, val, num_elem);
            mes_valeurs_v(num_elem, ncomp) = parser.eval(num_elem);
          }
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  else
    {
      ToDo_Kokkos("critical");
      DoubleTabs val_params_aux_elems;
      for (int i = 0; i < nb_param; i++)
        {
          DoubleTab vp(nb_elem_tot, mes_valeurs.dimension(1));
          val_params_aux_elems.add(vp);
        }
      for (int i = 0; i < nb_param; i++)
        les_ch_param[i]->valeur_aux_elems(centres_de_gravites, les_polys, val_params_aux_elems[i]);

      if (table.isfonction() != 2)
        {
          for (int num_elem = 0; num_elem < nb_elem; num_elem++)
            for (int ncomp = 0; ncomp < nbcomp; ncomp++)
              {
                std::vector<double> vals;
                for (int n = 0; n < nb_param; n++)
                  vals.push_back(val_params_aux_elems[n](num_elem, ncomp));
                mes_valeurs(num_elem, ncomp) = table.val(vals, ncomp);
              }
        }
      else
        {
          table.valeurs(val_params_aux_elems[0], centres_de_gravites, t, mes_valeurs);
        }
    }
  Champ_Fonc_base::mettre_a_jour(t);
}

int Champ_Fonc_Tabule_P0_VEF::initialiser(const double un_temps)
{
  Champ_Fonc_Tabule_P0_VEF::mettre_a_jour(un_temps);
  return 1;
}
