/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Tabule_Morceaux.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Tabule_Morceaux.h>
#include <Sous_Zone.h>
#include <Domaine.h>
#include <Interprete.h>

Implemente_instanciable( Champ_Tabule_Morceaux, "Champ_Tabule_Morceaux", Champ_Don_base ) ;
// XD Champ_Tabule_Morceaux champ_don_base Champ_Tabule_Morceaux 0 set Tabulated field by sub-zone
// XD attr dom_name ref_domaine dom_name 0 Name of the domain
// XD attr nb_comp int nb_comp 0 Number of field components.
// XD attr data bloc_lecture data 0 subzone_1 nb_comp InputFieldName { table_dim InputFieldVal_1 InputFieldVal_2 .... OutputFieldVal_1 OutputFieldVal_2 ... } subzone_2 nb_comp InputFieldName { table_dim InputFieldVal_1 InputFieldVal_2 .... OutputFieldVal_1 OutputFieldVal_2 ... } ..... subzone_n nb_comp InputFieldName { table_dim InputFieldVal_1 InputFieldVal_2 .... OutputFieldVal_1 OutputFieldVal_2 ... }

Sortie& Champ_Tabule_Morceaux::printOn( Sortie& os ) const
{
  Champ_Don_base::printOn( os );
  return os;
}

Entree& Champ_Tabule_Morceaux::readOn( Entree& is )
{
  Motcle motlu;
  Nom nom;

  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& le_domaine=mon_domaine.valeur();

  int dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  valeurs_.resize(0, dim);

  le_domaine.creer_tableau_elements(valeurs_);
  le_domaine.creer_tableau_elements(num_zone_);

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
  while (motlu != Motcle("}") )
    {
      les_sous_zones.add(le_domaine.ss_zone(nom));
      Champ_Fonc_Tabule ch_tab;
      is >> ch_tab;
      champs_tabules.add(ch_tab);
      is >> nom;
      motlu=nom;
    }

  return is;
}

void Champ_Tabule_Morceaux::calculer_champ_tabule_morceaux()
{
  CONST_LIST_CURSEUR(REF(Sous_Zone)) curseur1(les_sous_zones);
  CONST_LIST_CURSEUR(Champ_Fonc_Tabule) curseur2(champs_tabules);

  if (nb_comp() == 1)
    {
      DoubleVect& mes_valeurs = valeurs();

      while(curseur1)
        {
          const Sous_Zone& ssz = (curseur1.valeur()).valeur();
          const DoubleTab& vals_champ = curseur2.valeur().valeurs();

          for( int poly=0; poly<ssz.nb_elem_tot(); poly++)
            mes_valeurs(ssz(poly)) = vals_champ(ssz(poly));

          ++curseur1;
          ++curseur2;
        }
    }
  else
    {
      DoubleTab& mes_valeurs = valeurs();
      int k,dim = nb_comp();

      while(curseur1)
        {
          const Sous_Zone& ssz = (curseur1.valeur()).valeur();
          const DoubleTab& vals_champ = curseur2.valeur().valeurs();

          for(int poly=0; poly<ssz.nb_elem_tot(); poly++)
            for( k=0; k< dim; k++)
              mes_valeurs(ssz(poly),k) = vals_champ(ssz(poly),k);

          ++curseur1;
          ++curseur2;
        }
    }

  valeurs().echange_espace_virtuel();
}

int Champ_Tabule_Morceaux::initialiser(const double& un_temps)
{
  num_zone_ = -1;

  CONST_LIST_CURSEUR(REF(Sous_Zone)) curseur1(les_sous_zones);
  int k=0;
  while(curseur1)
    {
      const Sous_Zone& ssz = (curseur1.valeur()).valeur();

      for( int poly=0; poly<ssz.nb_elem_tot(); poly++)
        {
          int elem= ssz(poly);
          num_zone_[elem] = k;
        }
      ++curseur1;
      ++k;
    }
  return Champ_Don_base::initialiser(un_temps);
}

void Champ_Tabule_Morceaux::mettre_a_jour(double un_temps)
{

  for (int i=0; i<nb_champs_tabules(); i++)
    champs_tabules[i].mettre_a_jour(un_temps);

  calculer_champ_tabule_morceaux();
}


DoubleTab& Champ_Tabule_Morceaux::valeur_aux_elems(const DoubleTab& positions,
                                                   const IntVect& les_polys,
                                                   DoubleTab& val) const
{

  DoubleTabs tabs(nb_champs_tabules());

  for (int i=0; i<champs_tabules.size(); i++)
    tabs[i] = champs_tabules[i].valeur_aux_elems(positions,les_polys,val);

  if (val.nb_dim()==1)
    {
      for( int i=0; i<les_polys.size(); i++)
        {
          int elem= les_polys[i];
          int izone = num_zone_[elem];
          val(i) = tabs[izone](i);
        }
    }
  else
    {
      int dim = val.dimension(1);
      for( int i=0; i<les_polys.size(); i++)
        {
          int elem= les_polys[i];
          int izone = num_zone_[elem];
          for (int j=0; j<dim; j++)
            val(i,j) = tabs[izone](i,j);
        }
    }
  return val;
}

double Champ_Tabule_Morceaux::valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const
{
  DoubleTab tab(nb_champs_tabules());

  for (int i=0; i<champs_tabules.size(); i++)
    tab[i] = champs_tabules[i].valeur_a_elem_compo(position,le_poly,ncomp);

  int izone = num_zone_[le_poly];

  return tab[izone];
}

