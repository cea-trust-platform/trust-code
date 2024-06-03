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

#include <Champ_Fonc_Fonction_txyz.h>

Implemente_instanciable(Champ_Fonc_Fonction_txyz,"Champ_Fonc_Fonction_txyz",Champ_Fonc_Fonction);
// XD champ_fonc_fonction_txyz champ_fonc_fonction champ_fonc_fonction_txyz -1 this refers to a field that is a function of another field and time and/or space coordinates

Sortie& Champ_Fonc_Fonction_txyz::printOn(Sortie& os) const { return os; }

// Voir Champ_Fonction_fonction
// Lecture du Champ a partir d'un flot d'entree (On ne sait traiter que les champs scalaires.)
Entree& Champ_Fonc_Fonction_txyz::readOn(Entree& is)
{
  int nbcomp, nbcomp_tmp_ = -1, old_table_syntax_ = 0;
  Nom val1, val2;
  is >> val1;
  is >> val2;
  // fix if user uses the old syntax ..
  Champ_Fonc_Tabule::Warn_old_chp_fonc_syntax_V_184("Champ_Fonc_Fonction_txyz", val2, nbcomp_tmp_, old_table_syntax_);
  if (old_table_syntax_)
    {
      noms_champs_parametre_.add(val1);
      nbcomp = nbcomp_tmp_;
    }
  else
    {
      noms_pbs_.add(val1);
      noms_champs_parametre_.add(val2);
      is >> nbcomp;
    }

  if(nbcomp==1)
    {
      fixer_nb_comp(nbcomp);
      la_table.lire_fxyzt(is,nbcomp);
    }
  else
    {
      Cerr << "Error reading from an object of type Champ_Fonc_Fonction_txyz" << finl;
      Cerr << "We know treating only the scalar fields " << finl;
      Process::exit();
    }
  return is;
}
