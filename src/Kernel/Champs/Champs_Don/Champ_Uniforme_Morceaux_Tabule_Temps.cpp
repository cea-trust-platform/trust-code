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

#include <Champ_Uniforme_Morceaux_Tabule_Temps.h>
#include <Domaine.h>
#include <Interprete.h>
#include <Motcle.h>
#include <Sous_Zone.h>
Implemente_liste(Table);
Implemente_instanciable(Champ_Uniforme_Morceaux_Tabule_Temps,"Champ_Uniforme_Morceaux_Tabule_Temps",Champ_Uniforme_Morceaux_inst);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Uniforme_Morceaux_Tabule_Temps::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    Lit un champ uniforme par morceaux tabule dans
//    le temps a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entre modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef "defaut" attendu
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
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
  for( poly=0; poly<le_domaine.zone(0).nb_elem(); poly++)
    for( k=0; k< dim; k++)
      valeurs_(poly,k)=val(k);
  is >> nom;
  motlu=nom;
  while (motlu != Motcle("}") )
    {
      REF(Sous_Zone) refssz=les_sous_zones.add(le_domaine.ss_zone(nom));
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




// Description:
//    Mise a jour en temps du champ (tabule en temps)
// Precondition:
// Parametre: double tps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Uniforme_Morceaux_Tabule_Temps::me_calculer(double tps)
{
  if (!est_egal(temps(),tps))
    {
      LIST_CURSEUR(REF(Sous_Zone)) curseur1(les_sous_zones);
      LIST_CURSEUR(Table) curseur2(les_tables);

      if (nb_comp() == 1)
        {
          DoubleVect& mes_valeurs = valeurs();
          double val_ch;
          while(curseur1)
            {
              Sous_Zone& ssz = (curseur1.valeur()).valeur();
              Table& la_table = curseur2.valeur();

              val_ch = la_table.val(tps);

              for( int poly=0; poly<ssz.nb_elem_tot(); poly++)
                mes_valeurs(ssz(poly)) = val_ch;

              ++curseur1;
              ++curseur2;
            }
        }
      else
        {
          DoubleTab& mes_valeurs = valeurs();
          int k,dim = nb_comp();
          DoubleVect val_loc(dim);

          while(curseur1)
            {
              Sous_Zone& ssz = (curseur1.valeur()).valeur();
              Table& la_table = curseur2.valeur();

              la_table.valeurs(val_loc,tps);

              for(int poly=0; poly<ssz.nb_elem_tot(); poly++)
                for( k=0; k< dim; k++)
                  mes_valeurs(ssz(poly),k) = val_loc(k);

              ++curseur1;
              ++curseur2;
            }
        }
    }
  valeurs().echange_espace_virtuel();
}

