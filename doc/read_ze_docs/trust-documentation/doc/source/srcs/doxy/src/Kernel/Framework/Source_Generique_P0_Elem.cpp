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

#include <Source_Generique_P0_Elem.h>
#include <Synonyme_info.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_VF.h>

Implemente_instanciable(Source_Generique_P0_Elem,"Source_Generique_VDF_P0_VDF",Source_Generique_base);
Add_synonym(Source_Generique_P0_Elem, "Source_Generique_Elem_PolyMAC_P0P1NC");
Add_synonym(Source_Generique_P0_Elem, "Source_Generique_Elem_PolyMAC_P0");

Sortie& Source_Generique_P0_Elem::printOn(Sortie& os) const
{
  return os << que_suis_je() ;
}

Entree& Source_Generique_P0_Elem::readOn(Entree& is)
{
  Source_Generique_base::readOn(is);
  return is;
}

DoubleTab& Source_Generique_P0_Elem::ajouter(DoubleTab& resu) const
{
  Champ espace_stockage;
  const Champ_base& champ_calc = ch_source_->get_champ(espace_stockage);
  const DoubleTab& valeurs_calc = champ_calc.valeurs();

  int nb_elem = le_dom->nb_elem();
  const DoubleVect& vol = le_dom->volumes();
  const DoubleVect& poro_vol = le_dom_cl->valeur().equation().milieu().porosite_elem();

  for (int elem = 0; elem<nb_elem; elem++)
    resu(elem) += valeurs_calc(elem)*vol(elem)*poro_vol(elem);

  resu.echange_espace_virtuel();
  return resu;
}

void Source_Generique_P0_Elem::associer_domaines(const Domaine_dis& domaine_dis,
                                                 const Domaine_Cl_dis& zcl_dis)
{
  le_dom = ref_cast(Domaine_VF,domaine_dis.valeur());
  le_dom_cl = zcl_dis;
}

Nom Source_Generique_P0_Elem::localisation_source()
{
  return "elem";
}
