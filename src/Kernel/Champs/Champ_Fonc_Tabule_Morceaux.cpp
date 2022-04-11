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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Fonc_Tabule_Morceaux.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Tabule_Morceaux.h>
#include <Domaine.h>

Implemente_instanciable(Champ_Fonc_Tabule_Morceaux, "Champ_Fonc_Tabule_Morceaux|Champ_Tabule_Morceaux", Champ_Don_base);
// XD Champ_Fonc_Tabule_Morceaux champ_don_base Champ_Tabule_Morceaux 0 Field defined by tabulated data in each sub-zone. It makes possible the definition of a field which is a function of other fields.
// XD   attr domain_name ref_domaine domain_name 0 Name of the domain.
// XD   attr nb_comp int nb_comp 0 Number of field components.
// XD   attr data bloc_lecture data 0 { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object function, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator wishes to use a champ_fonc_tabule_morceaux type object.

Sortie& Champ_Fonc_Tabule_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

Entree& Champ_Fonc_Tabule_Morceaux::readOn(Entree& is)
{
  Nom nom;
  is >> nom;
  interprete_get_domaine<Champ_Morceaux_Type::FONC_TABULE>(nom);
  mon_domaine->creer_tableau_elements(table_idx);
  table_idx = -1; // pour planter si on a oublie une sous-zone

  int nbcomp;
  is >> nbcomp;

  fixer_nb_comp(nbcomp);
  valeurs_.resize(0, nbcomp);
  mon_domaine->creer_tableau_elements(valeurs_);

  is >> nom;
  if(Motcle(nom) != Motcle("{") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      Process::exit();
    }

  is >> nom;
  while (nom != Nom("}"))
    {
      CHTAB ch_lu;
      /* 1. lecture de la sous-zone */
      REF(Sous_Zone) refssz = les_sous_zones.add(mon_domaine->ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();

      /* 2. lecture des champs parametres */
      int old_table_syntax_ = 0;
      is >> nom;
      if (nom != "{")
        {
          ch_lu.noms_champs_parametre_.push_back(nom.getString());
          old_table_syntax_ = 1;
          Process::exit("old syntax nor supported!!");
        }
      else
        {
          assert (old_table_syntax_ == 0);
          while (true)
            {
              is >> nom;
              if (nom == "}") break;
              ch_lu.noms_pbs_.push_back(nom.getString());
              is >> nom;
              ch_lu.noms_champs_parametre_.push_back(nom.getString());
            }
        }
      const int nb_param = ch_lu.noms_champs_parametre_.size();

      if (old_table_syntax_ && ch_lu.noms_pbs_.size() != 0 && nb_param != 1)
        {
          Cerr << "What ??? Big problem in Champ_Fonc_Tabule::readOn !!!" << finl;
          throw;
        }

      is >> nom;
      if (nom == "{")
        {
          /* 1. lecture de la grille de parametres */
          VECT(DoubleVect) params;
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
            {
              Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
              Cerr << "We expected keyword } instead of " << nom << finl;
              Process::exit();
            }
        }
      else
        {
          Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
          Cerr << "We expected keyword { or fonction instead of " << nom << finl;
          Process::exit();
        }

      for (int e = 0; e < ssz.nb_elem_tot(); e++) table_idx(ssz(e)) = champs_lus.size();
      champs_lus.push_back(ch_lu);
      is >> nom;
    }
  return is;
}

void Champ_Fonc_Tabule_Morceaux::mettre_a_jour(double time)
{
  if (!init_)
    {
      initialiser(time);
      return;
    }
  DoubleTab& tab = valeurs();
  Domaine& le_domaine = mon_domaine.valeur();
  const IntTab& les_elems = le_domaine.zone(0).les_elems();
  const int nb_som_elem = le_domaine.zone(0).nb_som_elem(), nb_elem_tot = le_domaine.zone(0).nb_elem_tot();

  // check whether valeur_aux_elems is needed
  bool calc_centers = false;
  for (auto && ch : champs_lus) for (int i = 0; i < ch.champs_parametre_.size(); i++)
      {
        assert(ch.champs_parametre_[i]->valeurs().dimension(1) == 1 || ch.champs_parametre_[i]->valeurs().dimension(1) == tab.dimension(1));
        if (tab.get_md_vector() != ch.champs_parametre_[i]->valeurs().get_md_vector())
          ch.needs_projection.push_back(true), calc_centers = true;
        else ch.needs_projection.push_back(false);
      }

  // valeur aux elems : on les stocke dans un map {"pb_champ" : champ_aux_elems, ...}
  std::map<std::string, DoubleTab> val_params_aux_elems;
  if (calc_centers)
    {
      DoubleTab centres_de_gravites(0, dimension);
      le_domaine.creer_tableau_elements(centres_de_gravites);

      for (int i = 0; i < le_domaine.zone(0).nb_elem_tot(); i++)
        {
          int nb_som = 0, s, r;
          for (int j = 0; j < nb_som_elem && (s = les_elems(i, j)) >= 0; j++)
            for (r = 0, nb_som++; r < dimension; r++)
              centres_de_gravites(i, r) += le_domaine.coord(s, r);

          for (r = 0; r < dimension; r++) centres_de_gravites(i, r) /= nb_som;
        }

      IntVect les_polys(nb_elem_tot);
      for(int elem = 0; elem < nb_elem_tot; elem++) les_polys(elem) = elem;

      // Estimate the field parameter on cells:
      for (auto && ch : champs_lus) for (int i = 0; i < ch.champs_parametre_.size(); i++)
          {
            std::string nc = ch.noms_pbs_[i] + "_" + ch.noms_champs_parametre_[i];
            if (!val_params_aux_elems.count(nc))
              {
                if (ch.needs_projection[i])
                  {
                    DoubleTab vp(nb_elem_tot, ch.champs_parametre_[i]->valeurs().dimension(1));
                    ch.champs_parametre_[i]->valeur_aux_elems(centres_de_gravites, les_polys, vp);
                    val_params_aux_elems[nc] = vp;
                  }
                else
                  val_params_aux_elems[nc] = ch.champs_parametre_[i]->valeurs();
              }
          }
    }
  else
    {
      // Estimate the field parameter on cells:
      for (auto && ch : champs_lus) for (int i = 0; i < ch.champs_parametre_.size(); i++)
          {
            std::string nc = ch.noms_pbs_[i] + "_" + ch.noms_champs_parametre_[i];
            if (!val_params_aux_elems.count(nc)) val_params_aux_elems[nc] = ch.champs_parametre_[i]->valeurs();
          }
    }

  // Compute the field according to the parameter field
  const int nbcomp = tab.dimension(1);
  for (int e = 0; e < le_domaine.zone(0).nb_elem(); e++)
    if (nbcomp == 1 && champs_lus[table_idx(e)].noms_champs_parametre_.size() == 1)
      {
        std::string nc = champs_lus[table_idx(e)].noms_pbs_[0] + "_" + champs_lus[table_idx(e)].noms_champs_parametre_[0];
        const double v = val_params_aux_elems[nc](e, 0);
        tab(e, 0) = champs_lus[table_idx(e)].la_table.val_simple(v);
      }
    else for (int n = 0; n < nbcomp; n++)
        {
          std::vector<double> vals;
          for (unsigned int k = 0; k < champs_lus[table_idx(e)].noms_champs_parametre_.size(); k++)
            {
              std::string nc = champs_lus[table_idx(e)].noms_pbs_[k] + "_" + champs_lus[table_idx(e)].noms_champs_parametre_[k];
              vals.push_back(val_params_aux_elems[nc](e, champs_lus[table_idx(e)].champs_parametre_[k]->valeurs().dimension(1) == 1 ? 0 : n));
            }
          tab(e, n) = champs_lus[table_idx(e)].la_table.val(vals, n);
        }
  tab.echange_espace_virtuel();
}

int Champ_Fonc_Tabule_Morceaux::initialiser(const double time)
{
  for (auto &&ch : champs_lus)
    for (unsigned int i = 0; i < ch.noms_champs_parametre_.size(); i++)
      {
        REF(Champ_base) champ;
        const Probleme_base& pb_ch = ref_cast(Probleme_base, Interprete::objet(Nom(ch.noms_pbs_[i].c_str())));
        champ = pb_ch.get_champ(Nom(ch.noms_champs_parametre_[i].c_str()));
        ch.champs_parametre_.add(champ);
      }
  init_ = true;
  mettre_a_jour(time);
  return 1;
}
