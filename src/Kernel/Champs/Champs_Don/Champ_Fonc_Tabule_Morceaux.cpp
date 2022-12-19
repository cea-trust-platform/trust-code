/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Champ_Fonc_Tabule_Morceaux.h>
#include <Zone.h>
#include <TRUSTTrav.h>
#include <Zone_VF.h>
#include <TRUSTTab_parts.h>

Implemente_instanciable(Champ_Fonc_Tabule_Morceaux, "Champ_Fonc_Tabule_Morceaux|Champ_Tabule_Morceaux", TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TABULE>);
// XD Champ_Fonc_Tabule_Morceaux champ_don_base Champ_Tabule_Morceaux 0 Field defined by tabulated data in each sub-zone. It makes possible the definition of a field which is a function of other fields.
// XD   attr domain_name ref_domaine domain_name 0 Name of the domain.
// XD   attr nb_comp int nb_comp 0 Number of field components.
// XD   attr data bloc_lecture data 0 { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object function, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator wishes to use a champ_fonc_tabule_morceaux type object.

Sortie& Champ_Fonc_Tabule_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

Entree& Champ_Fonc_Tabule_Morceaux::readOn(Entree& is)
{
  Nom nom, nom_ch;
  is >> nom;
  interprete_get_domaine(nom);
  mon_domaine->creer_tableau_elements(i_mor);
  i_mor = -1; // pour planter si on a oublie une sous-zone

  int nbcomp;
  is >> nbcomp;

  fixer_nb_comp(nbcomp);
  valeurs_.resize(0, nbcomp);
  mon_domaine->creer_tableau_elements(valeurs_);

  is >> nom;
  if(nom != "{")
    Process::exit(que_suis_je() + " : { expected instead of " + nom);

  for (is >> nom; nom != "}"; is >> nom)
    {
      CHTAB ch_lu;
      /* 1. lecture de la sous-zone */
      REF(Sous_Zone) refssz = les_sous_zones.add(mon_domaine->ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();

      /* 2. lecture des champs parametres */
      is >> nom, m_pb_ch.push_back({});
      if (nom != "{")
        {
          Cerr << "Errror reading old syntax: " << que_suis_je() << " " << domaine().le_nom() << " " << nbcomp << " { " << ssz.le_nom() << " " << nom << " 1 { ... } ... }" << finl;
          Cerr << "New syntax is like :       " << que_suis_je() << " " << domaine().le_nom() << " " << nbcomp << " { " << ssz.le_nom() << " { problem_name " << nom << " } { ... } ... }" << finl;
          Cerr << "Check the doc or release notes." << finl;
          Process::exit();
        }
      else for (is >> nom; nom != "}"; is >> nom)
          {
            is >> nom_ch;
            std::array<std::string, 2> pb_ch = { nom.getString(), nom_ch.getString() };
            s_pb_ch.insert(pb_ch), m_pb_ch.back().push_back(pb_ch); //ajout aux listes pour ce morceau et globale
          }

      const int nb_param = (int)m_pb_ch.back().size();

      is >> nom;
      if (nom != "{")
        Process::exit(que_suis_je() + " : { expected instead of " + nom);

      /* 1. lecture de la grille de parametres */
      DoubleVects params;
      for (int n = 0; n < nb_param; n++)
        {
          const int nb_val = lire_dimension(is, que_suis_je());
          DoubleVect param(nb_val);
          for (int i = 0; i < nb_val; i++) is >> param[i];
          params.add(param);
        }

      /* 2. lecture des valeurs des parametres */
      // taille totale du tableau de valeurs
      int size = nbcomp;
      for (int n = 0; n < nb_param; n++) size *= params[n].size();

      // lecture : tout dans un tableau 1D
      DoubleVect tab_valeurs(size);
      for (int i = 0; i < size; i++) is >> tab_valeurs[i];
      ch_lu.la_table.remplir(params, tab_valeurs);

      is >> nom;
      if (nom != "}")
        Process::exit(que_suis_je() + " : } expected instead of "+ nom);
      for (int i = 0; i < ssz.nb_elem_tot(); i++) i_mor(ssz(i)) = (int)morceaux.size();
      morceaux.push_back(ch_lu);
    }
  if (mp_min_vect(i_mor) == -1)
    Process::exit(que_suis_je() + " : some pieces of the field are missing!");

  return is;
}

int Champ_Fonc_Tabule_Morceaux::initialiser(const double tps)
{
  /* remplissage de ch_param (pointeurs vers les champs) et des i_ch (champs utilises par chaque morceau) */
  std::vector<std::array<std::string, 2>> v_pb_ch(s_pb_ch.begin(), s_pb_ch.end()); //set -> vector
  for (auto &&pb_ch : v_pb_ch) /* (probleme, champ) -> pointeurs */
    ch_param.push_back(&ref_cast(Probleme_base, Interprete::objet(Nom(pb_ch[0]))).get_champ(Nom(pb_ch[1])));
  for (int i = 0; i < (int) m_pb_ch.size(); i++)
    for (auto && pb_ch : m_pb_ch[i]) /* indices */
      morceaux[i].i_ch.push_back((int)(std::lower_bound(v_pb_ch.begin(), v_pb_ch.end(), pb_ch) - v_pb_ch.begin()));

  m_pb_ch.clear();
  s_pb_ch.clear();

  return TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TABULE>::initialiser(tps);
}

void Champ_Fonc_Tabule_Morceaux::mettre_a_jour(double time)
{

  DoubleTab& tab = valeurs(), vide;
  std::vector<const DoubleTab* > pval; /* valeurs des parametres */
  std::vector<DoubleTrav> tval; /* tableaux temporaires */
  std::vector<bool> is_multi; /* true si le champ correspondant est multi_composantes */
  pval.reserve(ch_param.size()), tval.reserve(ch_param.size()), is_multi.reserve(ch_param.size());
  for (auto &&pch : ch_param)
    {
      ConstDoubleTab_parts part(pch->valeurs()); //pour ignorer les variables aux de PolyMAC
      if (tab.get_md_vector() == part[0].get_md_vector()) /* on est bien aux elements -> utilisation directe */
        pval.push_back(&pch->valeurs());
      else /* sinon -> calcul des valeurs aux elems et stockage dans un tableau de tval */
        {
          IntVect polys(tab.dimension_tot(0));
          for (int i = 0; i < tab.dimension_tot(0); i++) polys[i] = i;
          tval.push_back(DoubleTab(tab.dimension_tot(0), pch->nb_comp()));
          pch->valeur_aux_elems(pch->a_une_zone_dis_base() ? ref_cast(Zone_VF, pch->zone_dis_base()).xp() : vide, polys, tval.back());
          pval.push_back(&tval.back());
        }
      is_multi.push_back(pval.back()->dimension(1) > 1);
    }
  std::vector<double> vals(ch_param.size());
  for (int e  = 0; e < mon_domaine->zone(0).nb_elem(); e++)
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
