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
// File:        Champ_Fonc_Morceaux.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Morceaux.h>

Implemente_instanciable(Champ_Fonc_Morceaux,"Champ_Fonc_Morceaux",TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC>);

Sortie& Champ_Fonc_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

// Description:
//    Lit les valeurs du champ uniforme par morceaux a partir d'un flot d'entree.
//    On lit le nom du domaine (nom_domaine) le nombre de composantes du champ (nb_comp) la valeur par defaut
//    du champ ainsi que les valeurs sur les sous zones.
//    Format:
//     Champ_Fonc_Morceaux nom_domaine nb_comp { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i }
// Precondition: pour utiliser un objet de type Champ_Fonc_Morceaux il faut avoir defini des objets de type Sous_Zone
Entree& Champ_Fonc_Morceaux::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  int k, poly;

  /* 1. nom du domaine */
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& le_domaine = mon_domaine.valeur();

  /* 2. dimension et creation des tableaux parser_idx (num de formule pour chaque maille) et valeurs_ */
  dim = lire_dimension(is, que_suis_je());
  fixer_nb_comp(dim);
  parser_idx.resize(0, dim), valeurs_.resize(0, dim);
  le_domaine.creer_tableau_elements(parser_idx);
  le_domaine.creer_tableau_elements(valeurs_);

  /* 3. probleme et champ variable (optionnel) */
  is >> nom;
  if (nom != "{")
    {
      ref_pb = ref_cast(Probleme_base, Interprete::objet(nom));
      is >> nom_champ_parametre_;
      is >> nom;
    }

  /* 4. morceaux : ligne "defaut" */
  if (nom != "{")
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      exit();
    }
  is >> motlu;
  if (motlu != Motcle("defaut"))
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected defaut instead of " << nom << finl;
      exit();
    }

  /* parsers par defaut */
  for (k = 0; k < dim; k++)
    {
      Parser_U psr;
      Nom tmp;
      is >> tmp;
      psr.setNbVar(5);
      psr.setString(tmp);
      psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
      if (ref_pb.non_nul())
        psr.addVar("val");
      psr.parseString();
      for (poly = 0; poly < le_domaine.zone(0).nb_elem_tot(); poly++)
        parser_idx(poly, k) = parser.size();
      parser.add(psr);
    }

  is >> nom;
  while (nom != Motcle("}"))
    {
      REF(Sous_Zone) refssz = les_sous_zones.add(le_domaine.ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();
      for (k = 0; k < dim; k++)
        {
          Parser_U psr;
          Nom tmp;
          is >> tmp;
          psr.setNbVar(5);
          psr.setString(tmp);
          psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
          if (ref_pb.non_nul())
            psr.addVar("val");
          psr.parseString();
          for (poly = 0; poly < ssz.nb_elem_tot(); poly++)
            parser_idx(ssz(poly), k) = parser.size();
          parser.add(psr);
        }
      is >> nom;
    }
  return is;
}
