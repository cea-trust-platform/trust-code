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

#include <Champs_compris.h>
#include <Champ_base.h>

Implemente_instanciable(Champs_compris, "Champs_compris", Objet_U);

Sortie& Champs_compris::printOn(Sortie& os) const { return os; }

Entree& Champs_compris::readOn(Entree& is) { return is; }

const Champ_base& Champs_compris::get_champ(const Motcle& motcle) const
{
  assert(motcle!="??");
  auto item = liste_champs_.find(motcle.getString());
  if (item != liste_champs_.end()) return item->second;
  throw Champs_compris_erreur();
}

bool Champs_compris::has_champ(const Motcle& motcle, REF(Champ_base)& ref_champ) const
{
  assert(motcle!="??");
  auto item = liste_champs_.find(motcle.getString());
  if (item != liste_champs_.end())
    {
      ref_champ = item->second;
      return true;
    }
  return false;
}

const Noms Champs_compris::liste_noms_compris() const
{
  Noms nom_compris;
  for (auto const& champ : liste_champs_)
    nom_compris.add(champ.first);
  return nom_compris;
}

void Champs_compris::ajoute_champ(const Champ_base& champ)
{
  // Adding a field name referring to champ inside liste_champs_ dictionnary
  auto add_key = [&](const Nom& n)
  {
    std::string nom_champ = n.getString();
    std::string upperCase = nom_champ, lowerCase = nom_champ;
    std::transform(nom_champ.begin(), nom_champ.end(), upperCase.begin(), ::toupper);
    std::transform(nom_champ.begin(), nom_champ.end(), lowerCase.begin(), ::tolower);
    liste_champs_[upperCase] = champ;
    liste_champs_[lowerCase] = champ;
  };

  // Adding field with its name...
  add_key(champ.le_nom());

  // ...its synonyms...
  const Noms& syno = champ.get_synonyms();
  int nb_syno = syno.size();
  for (int s = 0; s < nb_syno; s++)
    add_key(syno[s]);

  // ...and its components
  int nb_composantes = champ.nb_comp();
  for (int i = 0; i < nb_composantes; i++)
    if(champ.nom_compo(i) != "??")
      add_key(champ.nom_compo(i));

  Cerr<<"Champs_compris::ajoute_champ " << champ.le_nom() <<finl;
}

