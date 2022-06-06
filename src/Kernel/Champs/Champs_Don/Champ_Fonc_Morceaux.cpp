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
  Nom nom;
  is >> nom;
  interprete_get_domaine(nom);

  int dim = lire_dimension(is, que_suis_je());
  creer_tabs(dim);
  is >> nom;

  if (nom != "{")
    {
      ref_pb = ref_cast(Probleme_base, Interprete::objet(nom));
      is >> nom_champ_parametre_;
      is >> nom;
    }

  return complete_readOn(dim,que_suis_je(),is,nom);
}
