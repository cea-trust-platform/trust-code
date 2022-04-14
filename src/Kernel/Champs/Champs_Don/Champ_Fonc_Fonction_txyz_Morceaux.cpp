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
// File:        Champ_Fonc_Fonction_txyz_Morceaux.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs/Champs_Don
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Fonction_txyz_Morceaux.h>
#include <Champ_Fonc_Tabule.h>

Implemente_instanciable(Champ_Fonc_Fonction_txyz_Morceaux,"Champ_Fonc_Fonction_txyz_Morceaux",TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TXYZ>);
// XD champ_fonc_fonction_txyz_morceaux champ_don_base champ_fonc_fonction_txyz_morceaux 0 Field defined by analytical functions in each sub-zone. It makes possible the definition of a field that depends on the time and the space.
// XD   attr problem_name ref_Pb_base problem_name 0 Name of the problem.
// XD   attr inco chaine inco 0 Name of the field (for example: temperature).
// XD   attr nb_comp int nb_comp 0 Number of field components.
// XD   attr data bloc_lecture data 0 { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i } By default, the value val_def is assigned to the field. It takes the sous_zone_i identifier Sous_Zone (sub_area) type object function, val_i. Sous_Zone (sub_area) type objects must have been previously defined if the operator wishes to use a champ_fonc_fonction_txyz_morceaux type object.

Sortie& Champ_Fonc_Fonction_txyz_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

// Description:
//    Lit les valeurs du champ uniforme par morceaux a partir d'un flot d'entree.
//    On lit le nom du domaine (nom_domaine) le nombre de composantes du champ (nb_comp) la valeur par defaut
//    du champ ainsi que les valeurs sur les sous zones.
//    Format:
//     Champ_Fonc_Fonction_txyz_Morceaux pb champ nb_comp { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i }
// Precondition: pour utiliser un objet de type Champ_Fonc_Fonction_txyz_Morceaux il faut avoir defini des objets de type Sous_Zone
Entree& Champ_Fonc_Fonction_txyz_Morceaux::readOn(Entree& is)
{
  int dim;
  Nom nom;
  is >> nom;

  interprete_get_domaine<Champ_Morceaux_Type::FONC_TXYZ>(nom);

  Nom val1, val2;
  is >> val1;
  is >> val2;
  Champ_Fonc_Tabule::Warn_old_chp_fonc_syntax("Champ_Fonc_Fonction_txyz_Morceaux", val1, val2, dim, nom_champ_parametre_);

  dim = lire_dimension(dim, que_suis_je());
  creer_tabs(dim);
  is >> nom;

  return complete_readOn(dim,que_suis_je(),is,nom);
}
