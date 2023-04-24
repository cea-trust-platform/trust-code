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

#include <Champ_Fonc_Tabule_Morceaux_Interp.h>
#include <TRUSTTab_parts.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Domaine.h>
#include <EChaine.h>

Implemente_instanciable(Champ_Fonc_Tabule_Morceaux_Interp, "Champ_Fonc_Tabule_Morceaux_Interp", Champ_Fonc_Tabule_Morceaux);
// XD Champ_Fonc_Tabule_Morceaux_Interp champ_don_base Champ_Tabule_Morceaux 0 Field defined by tabulated data in each sub-domaine. It makes possible the definition of a field which is a function of other fields.
// XD   attr domain_name ref_domaine domain_name 0 Name of the domain.
// XD   attr nb_comp int nb_comp 0 Number of field components.
// XD   attr data bloc_lecture data 0 { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i } By default, the value val_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function, val_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator wishes to use a champ_fonc_tabule_morceaux type object.

Sortie& Champ_Fonc_Tabule_Morceaux_Interp::printOn(Sortie& os) const { return Champ_Fonc_Tabule_Morceaux::printOn(os); }
Entree& Champ_Fonc_Tabule_Morceaux_Interp::readOn(Entree& is)
{
  read_pb_instead_of_domain = true;
  return Champ_Fonc_Tabule_Morceaux::readOn(is);
}

int Champ_Fonc_Tabule_Morceaux_Interp::initialiser(const double tps)
{
  /* remplissage de ch_param_interp (champ_fonc_interp) et des i_ch (champs utilises par chaque morceau) */
  std::vector<std::array<std::string, 2>> v_pb_ch(s_pb_ch.begin(), s_pb_ch.end()); //set -> vector

  ch_param_interp.resize(v_pb_ch.size());
//  for (auto &&pb_ch : v_pb_ch) /* (probleme, champ) -> champs_interp */
  for (int i = 0; i < (int)v_pb_ch.size(); i++)
    {
      // XXX : j'ai mis nb_comp = 1 en dur
      Nom ch_int = Nom("{ pb_loc ") + ref_pb->le_nom() + " pb_dist " + Nom(v_pb_ch[i][0]) + " nom_champ " + Nom(v_pb_ch[i][1]) + " nature IntensiveMaximum }";
      EChaine ech(ch_int);
      ech >> ch_param_interp[i];
    }

  for (auto &&pch : ch_param_interp)
    pch.initialiser(tps);

  for (int i = 0; i < (int) m_pb_ch.size(); i++)
    for (auto && pb_ch : m_pb_ch[i]) /* indices */
      morceaux[i].i_ch.push_back((int)(std::lower_bound(v_pb_ch.begin(), v_pb_ch.end(), pb_ch) - v_pb_ch.begin()));

  m_pb_ch.clear();
  s_pb_ch.clear();

  return TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TABULE>::initialiser(tps);
}

void Champ_Fonc_Tabule_Morceaux_Interp::mettre_a_jour(double time)
{

  DoubleTab& tab = valeurs(), vide;
  std::vector<const DoubleTab* > pval; /* valeurs des parametres */
  std::vector<DoubleTrav> tval; /* tableaux temporaires */
  std::vector<bool> is_multi; /* true si le champ correspondant est multi_composantes */
  pval.reserve(ch_param_interp.size()), tval.reserve(ch_param_interp.size()), is_multi.reserve(ch_param_interp.size());
  for (auto &&pch : ch_param_interp)
    {
      pch.mettre_a_jour(time);
      pval.push_back(&pch.valeurs());
      is_multi.push_back(pval.back()->dimension(1) > 1);
    }
  std::vector<double> vals(ch_param_interp.size());
  for (int e  = 0; e < mon_domaine->nb_elem(); e++)
    {
      const CHTAB& mor = morceaux[i_mor(e)];
      int N = tab.dimension(1), M = (int) mor.i_ch.size();
      if (N == 1 && M == 1)
        tab(e, 0) = mor.la_table.val_simple((*pval[mor.i_ch[0]])(e, 0));
      else for (int n = 0, m; n < N; n++)
          {
            for (vals.clear(), m = 0; m < M; m++) vals.push_back((*pval[mor.i_ch[m]])(e, n * is_multi[mor.i_ch[m]])); /* si le champ parametre est multi-compo, on prend la meme que celle du champ */
            tab(e, n) = mor.la_table.val(vals, n);
          }
    }
  tab.echange_espace_virtuel();
}
