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

#include <Champ_Elem_PolyVEF_P0.h>
#include <Connectivite_som_elem.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>

Implemente_instanciable(Champ_Elem_PolyVEF_P0,"Champ_Elem_PolyVEF_P0",Champ_Elem_PolyMAC_P0);

Sortie& Champ_Elem_PolyVEF_P0::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyVEF_P0::readOn(Entree& s) { return Champ_Elem_PolyMAC_P0::readOn(s) ; }

const Domaine_PolyVEF& Champ_Elem_PolyVEF_P0::domaine_PolyVEF() const
{
  return ref_cast(Domaine_PolyVEF, le_dom_VF.valeur());
}

void Champ_Elem_PolyVEF_P0::init_auxiliary_variables()
{
  const Domaine_PolyVEF_P0& dom = ref_cast(Domaine_PolyVEF_P0,le_dom_VF.valeur());
  const IntTab& f_e = dom.face_voisins();
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    if (futur(n).size_reelle_ok())
      {
        DoubleTab& vals = futur(n);
        vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
        vals.resize_dim0(dom.mdv_elems_fbord.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
        vals.set_md_vector(dom.mdv_elems_fbord); //...et on remet le bon MD_Vector
        /* initialisation des variables aux faces : par celle de l'elem amont */
        for (int f = 0, ne_tot = dom.nb_elem_tot(); f < dom.premiere_face_int(); f++)
          for (int m = 0, e = f_e(f, 0); m < vals.dimension(1); m++)
            vals(ne_tot + f, m) = vals(e, m);
        vals.echange_espace_virtuel();
      }
}

void Champ_Elem_PolyVEF_P0::init_grad(int full_stencil) const
{
  if (fgrad_d.size()) return;
  const IntTab& f_cl = fcl();
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, le_dom_VF.valeur());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites(); // CAL du champ à dériver
  domaine.fgrad(1, 0, 1, cls, f_cl, nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
}

void Champ_Elem_PolyVEF_P0::calc_grad(int full_stencil) const
{
  if (grad_a_jour) return;
  const IntTab& f_cl = fcl();
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, le_dom_VF.valeur());
  const Conds_lim& cls = domaine_Cl_dis().les_conditions_limites(); // CAL du champ à dériver
  domaine.fgrad(1, 0, 1, cls, f_cl, nullptr, nullptr, 1, full_stencil, fgrad_d, fgrad_e, fgrad_w);
  grad_a_jour = 1;
  tps_last_calc_grad_ = temps();
}
