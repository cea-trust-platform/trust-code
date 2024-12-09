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

#include <Champ_Uniforme_Morceaux_Tabule_Temps.h>
#include <Interprete.h>
#include <Sous_Domaine.h>
#include <Domaine.h>
#include <Motcle.h>

Implemente_instanciable(Champ_Uniforme_Morceaux_Tabule_Temps, "Champ_Uniforme_Morceaux_Tabule_Temps", Champ_Uniforme_Morceaux_inst);
// XD champ_uniforme_morceaux_tabule_temps champ_uniforme_morceaux champ_uniforme_morceaux_tabule_temps -1 this type of field is constant in space on one or several sub_zones and tabulated as a function of time.


Sortie& Champ_Uniforme_Morceaux_Tabule_Temps::printOn(Sortie& os) const { return os; }

/*! @brief Lit un champ uniforme par morceaux tabule dans le temps a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entre modifie
 * @throws accolade ouvrante attendue
 * @throws mot clef "defaut" attendu
 * @throws accolade fermante attendue
 */
Entree& Champ_Uniforme_Morceaux_Tabule_Temps::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  int k, poly,nb_val;
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& le_domaine=mon_domaine.valeur();
  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  valeurs_.resize(0, dim);
  le_domaine.creer_tableau_elements(valeurs_);
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("{") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      exit();
    }
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("defaut") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected defaut instead of " << nom << finl;
      exit();
    }
  DoubleVect val(dim);
  for( k=0; k< dim; k++) is >> val(k);
  for( poly=0; poly<le_domaine.nb_elem(); poly++)
    for( k=0; k< dim; k++)
      valeurs_(poly,k)=val(k);
  is >> nom;
  motlu=nom;
  while (motlu != Motcle("}") )
    {
      les_sous_domaines.add(le_domaine.ss_domaine(nom));
      is >> motlu;
      if (motlu == Motcle("{") )
        {
          Table la_table;
          is >> nb_val;
          DoubleVect param(nb_val);
          DoubleTab tab_valeurs(nb_val,dim);
          for (int i = 0; i < nb_val; i++)
            is >> param[i];
          for (int i = 0; i < nb_val; i++)
            for (k = 0; k < dim; k++)
              is >> tab_valeurs(i, k);
          la_table.remplir(param,tab_valeurs);
          les_tables.add(la_table);
          is >> motlu;
          if (motlu !=  Motcle("}") )
            {
              Cerr << "Error while reading an object of type Champ_Uniforme_Morceaux_Tabule_Temps" << finl;
              Cerr << "We expected keyword } instead of " << motlu << finl;
              exit();
            }
        }
      else
        {
          Cerr << "Error while reading an object of type Champ_Uniforme_Morceaux_Tabule_Temps" << finl;
          Cerr << "We expected keyword { instead of " << motlu << finl;
          exit();
        }
      is >> nom;
      motlu=nom;
    }
  return is;
}

/*! @brief Mise a jour en temps du champ (tabule en temps)
 *
 * @param (double tps) le temps de mise a jour
 */
void Champ_Uniforme_Morceaux_Tabule_Temps::me_calculer(double tps)
{
  if (!est_egal(temps(),tps))
    {

      auto& list = les_tables.get_stl_list();
      auto itr2 = les_sous_domaines.get_stl_list().begin();

      if (nb_comp() == 1)
        {
          DoubleVect& mes_valeurs = valeurs();
          double val_ch;
          for (auto& itr : list)
            {
              Sous_Domaine& ssz = (*itr2).valeur();
              Table& la_table = itr;

              val_ch = la_table.val(tps);

              for( int poly=0; poly<ssz.nb_elem_tot(); poly++)
                mes_valeurs(ssz(poly)) = val_ch;

              ++itr2;
            }
        }
      else
        {
          DoubleTab& mes_valeurs = valeurs();
          int k,dim = nb_comp();
          DoubleVect val_loc(dim);

          for (auto& itr : list)
            {
              Sous_Domaine& ssz = (*itr2).valeur();
              Table& la_table = itr;

              la_table.valeurs(val_loc,tps);

              for(int poly=0; poly<ssz.nb_elem_tot(); poly++)
                for( k=0; k< dim; k++)
                  mes_valeurs(ssz(poly),k) = val_loc(k);

              ++itr2;
            }
        }
    }
  valeurs().echange_espace_virtuel();
}

