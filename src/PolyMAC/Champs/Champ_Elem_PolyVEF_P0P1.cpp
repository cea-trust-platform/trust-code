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

#include <Champ_Elem_PolyVEF_P0P1.h>
#include <Connectivite_som_elem.h>
#include <Domaine_PolyVEF.h>
#include <Domaine_Cl_PolyMAC.h>

Implemente_instanciable(Champ_Elem_PolyVEF_P0P1,"Champ_Elem_PolyVEF_P0P1",Champ_Elem_PolyMAC_P0);

Sortie& Champ_Elem_PolyVEF_P0P1::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_PolyVEF_P0P1::readOn(Entree& s) { return Champ_Elem_PolyMAC_P0::readOn(s) ; }

const Domaine_PolyVEF& Champ_Elem_PolyVEF_P0P1::domaine_PolyVEF() const
{
  return ref_cast(Domaine_PolyVEF, le_dom_VF.valeur());
}

void Champ_Elem_PolyVEF_P0P1::init_auxiliary_variables()
{
  const Domaine_PolyVEF_P0P1& dom = ref_cast(Domaine_PolyVEF_P0P1,le_dom_VF.valeur());
  const Static_Int_Lists& s_e = dom.som_elem();
  for (int n = 0; n < nb_valeurs_temporelles(); n++)
    if (futur(n).size_reelle_ok())
      {
        DoubleTab& vals = futur(n);
        vals.set_md_vector(MD_Vector()); //on enleve le MD_Vector...
        vals.resize_dim0(dom.mdv_elems_soms.valeur().get_nb_items_tot()); //...on dimensionne a la bonne taille...
        vals.set_md_vector(dom.mdv_elems_soms); //...et on remet le bon MD_Vector
        /* initialisation des variables aux sommets : moyennes de celles aux elems */
        for (int s = 0, ne_tot = dom.nb_elem_tot(); s < dom.nb_som(); s++)
          for (int i = 0, ns = s_e.get_list_size(s); i < ns; i++)
            for (int e = s_e(s, i), m = 0; m < vals.dimension(1); m++)
              vals(ne_tot + s, m) += vals(e, m) / ns;
        vals.echange_espace_virtuel();
      }
}

DoubleTab& Champ_Elem_PolyVEF_P0P1::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{
  if (&domain == &le_dom_VF->domaine() && valeurs().dimension_tot(0) > le_dom_VF->nb_elem_tot())
    {
      ConstDoubleTab_parts part(valeurs());
      result = part[1];
      return result;
    }
  else return Champ_Elem_PolyMAC_P0::valeur_aux_sommets(domain, result);
}

DoubleVect& Champ_Elem_PolyVEF_P0P1::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{
  if (domain == le_dom_VF->domaine() && valeurs().dimension_tot(0) > le_dom_VF->nb_elem_tot())
    {
      ConstDoubleTab_parts part(valeurs());
      for (int i = 0; i < std::min(result.size_totale(), part[1].dimension_tot(0)); i++)
        result(i) = part[1](i, ncomp);
      return result;
    }
  else return Champ_Elem_PolyMAC_P0::valeur_aux_sommets_compo(domain, result, ncomp);
}
