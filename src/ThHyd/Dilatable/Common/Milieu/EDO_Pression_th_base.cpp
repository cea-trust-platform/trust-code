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

#include <Sortie_libre_pression_imposee_QC.h>
#include <Fluide_Weakly_Compressible.h>
#include <EDO_Pression_th_base.h>
#include <Domaine_Cl_dis_base.h>

#include <Cond_lim.h>

Implemente_base(EDO_Pression_th_base, "EDO_Pression_th_base", Objet_U);

Sortie& EDO_Pression_th_base::printOn(Sortie& os) const { return os; }

Entree& EDO_Pression_th_base::readOn(Entree& is) { return is; }

/*! @brief Associe le fluide a l'EDO
 *
 * @param (Fluide_Dilatable_base& fl) le fluide associe
 */
void EDO_Pression_th_base::associer_fluide(const Fluide_Dilatable_base& fl)
{
  le_fluide_ = fl;
}

void EDO_Pression_th_base::associer_domaines(const Domaine_dis_base& dds, const Domaine_Cl_dis_base& domaine_cl)
{
  le_dom = ref_cast(Domaine_VF, dds);
  le_dom_Cl = domaine_cl;
}

void EDO_Pression_th_base::completer()
{
  const DoubleTab& tab_ICh = le_fluide_->inco_chaleur()->valeurs();
  const double Pth = le_fluide_->pression_th();

  if (Pth > -1.)
    M0 = masse_totale(Pth, tab_ICh);
  else
    {
      assert(sub_type(Fluide_Weakly_Compressible, le_dom.valeur()));
      M0 = masse_totale(ref_cast(Fluide_Weakly_Compressible, le_dom.valeur()).pression_th_tab(), tab_ICh);
    }

  le_fluide_->checkTraitementPth(le_dom_Cl.valeur());
}

void EDO_Pression_th_base::mettre_a_jour_CL(double P)
{
  if (sub_type(Fluide_Weakly_Compressible, le_dom.valeur())) return; /* bof */

  for (int n_bord = 0; n_bord < le_dom->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(n_bord);
      if (sub_type(Sortie_libre_pression_imposee_QC, la_cl.valeur()))
        {
          Sortie_libre_pression_imposee_QC& cl = ref_cast_non_const(Sortie_libre_pression_imposee_QC, la_cl.valeur());
          cl.set_Pth(P);
        }
    }
}
