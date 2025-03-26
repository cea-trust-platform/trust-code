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


#include <Champs_compris_IJK.h>


/** A vectorial field is considered present in the structure if all its three components are there
 */
bool Champs_compris_IJK::has_champ_vectoriel(const Motcle& nom) const
{
  assert(nom!="??");
  auto item = liste_champs_vecto_.find(nom.getString());
  return item != liste_champs_vecto_.end();
}

const IJK_Field_vector3_double& Champs_compris_IJK::get_champ_vectoriel(const Motcle& nom) const
{
  assert(nom != "??");
  auto item = liste_champs_vecto_.find(nom.getString());
  if (item != liste_champs_vecto_.end()) return item->second;
  throw std::runtime_error(std::string("Vectoriel field ") + nom.getString() + std::string(" not found !"));
}

void Champs_compris_IJK::ajoute_champ_vectoriel(const IJK_Field_vector3_double& champ)
{
  // Adding a field name referring to champ inside liste_champs_ dictionnary
  auto add_key = [&](const Nom& n)
  {
    std::string nom_champ = n.getString();
    std::string upperCase = nom_champ, lowerCase = nom_champ;
    std::transform(nom_champ.begin(), nom_champ.end(), upperCase.begin(), ::toupper);
    std::transform(nom_champ.begin(), nom_champ.end(), lowerCase.begin(), ::tolower);

    if (has_champ_vectoriel(upperCase) || has_champ_vectoriel(lowerCase))
      {
        //TODO(teo.boutin) maybe check pointers equality before giving an error.
        Cerr << "Champs_compris_IJK::ajoute_champ_vectoriel : trying to add a field twice : " << upperCase << finl;
        Process::exit();
      }
    liste_champs_vecto_[upperCase] = champ;
    liste_champs_vecto_[lowerCase] = champ;
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
    ajoute_champ(champ[i]);

  Cerr<<"Champs_compris_IJK::ajoute_champ_vectoriel " << champ.le_nom() <<finl;
}


const Noms Champs_compris_IJK::liste_noms_compris_vectoriel() const
{
  Noms nom_compris;
  for (auto const& champ : liste_champs_vecto_)
    nom_compris.add(champ.first);
  return nom_compris;
}

void Champs_compris_IJK::switch_ft_fields()
{

  Cerr << "Champs_compris_IJK : start switch old to next" <<finl;


  for (auto& old : liste_champs_vecto_)
    {

      Nom old_name = old.first;

      if (old_name.debute_par("OLD_"))
        {
          Nom next_name = old_name.getSuffix("OLD_");

          OBS_PTR(IJK_Field_vector3_double) ptr = old.second;

          auto next = liste_champs_vecto_.find(next_name.getString());

          old.second = next->second;
          next->second = ptr;

          Cerr << "Champs_compris_IJK : swapping IJK field vector " << old_name << " with " << next_name <<finl;

        }


      if (old_name.debute_par("old_"))
        {
          Nom next_name = old_name.getSuffix("old_");

          OBS_PTR(IJK_Field_vector3_double) ptr = old.second;

          auto next = liste_champs_vecto_.find(next_name.getString());

          old.second = next->second;
          next->second = ptr;

          Cerr << "Champs_compris_IJK : swapping IJK field vector " << old_name << " with " << next_name <<finl;

        }

    }

  for (auto& old : liste_champs_)
    {

      Nom old_name = old.first;

      if (old_name.debute_par("OLD_"))
        {
          Nom next_name = old_name.getSuffix("OLD_");

          auto& ptr = old.second;

          auto next = liste_champs_.find(next_name.getString());

          old.second = next->second;
          next->second = ptr;

          Cerr << "Champs_compris_IJK : swapping IJK field " << old_name << " with " << next_name <<finl;

        }


      if (old_name.debute_par("old_"))
        {
          Nom next_name = old_name.getSuffix("old_");

          auto& ptr = old.second;

          auto next = liste_champs_.find(next_name.getString());

          old.second = next->second;
          next->second = ptr;

          Cerr << "Champs_compris_IJK : swapping IJK field " << old_name << " with " << next_name <<finl;

        }

    }
  Cerr << "Champs_compris_IJK : end switch old to next" <<finl;


}
