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

#include <Champ_Fonc_Fonction_txyz_Morceaux.h>
#include <Champ_Fonc_Tabule.h>

Implemente_instanciable(Champ_Fonc_Fonction_txyz_Morceaux,"Champ_Fonc_Fonction_txyz_Morceaux",TRUSTChamp_Morceaux_generique<Champ_Morceaux_Type::FONC_TXYZ>);
// XD champ_fonc_fonction_txyz_morceaux champ_don_base champ_fonc_fonction_txyz_morceaux 0 Field defined by analytical functions in each sub-domaine. It makes possible the definition of a field that depends on the time and the space.
// XD   attr problem_name ref_Pb_base problem_name 0 Name of the problem.
// XD   attr inco chaine inco 0 Name of the field (for example: temperature).
// XD   attr nb_comp entier nb_comp 0 Number of field components.
// XD   attr data bloc_lecture data 0 { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i } By default, the value val_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function, val_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator wishes to use a champ_fonc_fonction_txyz_morceaux type object.

Sortie& Champ_Fonc_Fonction_txyz_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

/*! @brief Lit les valeurs du champ uniforme par morceaux a partir d'un flot d'entree.
 *
 * On lit le nom du domaine (nom_domaine) le nombre de composantes du champ (nb_comp) la valeur par defaut
 *     du champ ainsi que les valeurs sur les sous domaines.
 *     Format:
 *      Champ_Fonc_Fonction_txyz_Morceaux pb champ nb_comp { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i }
 *
 */
Entree& Champ_Fonc_Fonction_txyz_Morceaux::readOn(Entree& is)
{
  int dim;
  Nom nom;
  is >> nom;
  interprete_get_domaine(nom);

  is >> nom_champ_parametre_;
  bool isNum = Champ_Fonc_Tabule::Check_if_int(nom_champ_parametre_);
  if (isNum)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Error in call to " << que_suis_je() << ":" << finl;
      Cerr << "The syntax has changed in version 1.8.2." << finl;
      Cerr << "You should now pass the dimension/number of components AFTER the field/parameter name." << finl;
      Cerr << "Please update your dataset or contact TRUST support team." << finl;
      Process::exit();
    }
  is >> dim;

  creer_tabs(dim);
  is >> nom;

  return complete_readOn(dim,que_suis_je(),is,nom);
}
