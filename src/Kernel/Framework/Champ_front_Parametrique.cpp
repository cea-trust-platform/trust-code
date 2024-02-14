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

#include <Champ_front_Parametrique.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Schema_Implicite_base.h>
#include <Champ_front_uniforme.h>
#include <EFichier.h>
#include <EChaine.h>
#include <string>
#include <Champ_front.h>

Implemente_instanciable(Champ_front_Parametrique,"Champ_front_Parametrique",Champ_front_Tabule);

Sortie& Champ_front_Parametrique::printOn(Sortie& os) const
{
  return Champ_front_Tabule::printOn(os);
}

Entree& Champ_front_Parametrique::readOn(Entree& is)
{
  Nom pb, fichier;
  Param param(que_suis_je());
  param.ajouter("probleme", &pb, Param::REQUIRED);
  param.ajouter("fichier", &fichier, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  // Lecture de tous les lignes du fichier parametrique:
  std::string line;
  EFichier file(fichier);
  while (std::getline(file.get_ifstream(), line) && !line.empty()) parametres_.add(line);
  // Associer au schema temps:
  Probleme_base& pb_base = ref_cast(Probleme_base, Interprete::objet(pb));
  // Verifie que tous les champs parametriques ont le meme nombre
  LIST(REF(Champ_front_Parametrique))& Champs_front_Parametriques = pb_base.Champs_front_Parametriques();
  int nb_champs_parametriques = Champs_front_Parametriques.size();
  if (nb_champs_parametriques && Champs_front_Parametriques.dernier()->size()!=size())
    {
      Cerr << "Erreur, le champ parametrique lu dans le fichier " << fichier << " a " << size() << " elements." << finl;
      Cerr << "Les autres champs parametriques ont " << Champs_front_Parametriques.dernier()->size() << " elements." << finl;
      Process::exit("Cela doit etre identique.");
    }
  Champs_front_Parametriques.add(*this);
  // On fixe le premier parametre:
  int calcul = newParameter();
  assert(calcul>0);
  Sortie_Fichier_base::newDirectory(calcul);
  return is;
}

int Champ_front_Parametrique::newParameter()
{
  if (parametres_.size()==index_)
    {
      return 0;
    }
  else
    {
      Cerr << "================================================" << finl;
      Cerr << "Mise a jour d'un parametre:" << parametres_[index_] << finl;
      Cerr << "================================================" << finl;
      EChaine nouveau_champ_front(parametres_[index_]);
      Champ_front ch_front;
      nouveau_champ_front >> ch_front;
      int nb_comp = ch_front.nb_comp();
      fixer_nb_comp(nb_comp);
      if (sub_type(Champ_front_Tabule, ch_front.valeur()))
        {
          la_table = ref_cast(Champ_front_Tabule, ch_front.valeur()).getTable();
        }
      else if (sub_type(Champ_front_uniforme, ch_front.valeur()))
        {
          DoubleVect param(1);
          param(0)=0;
          DoubleTab tab_valeurs(1, nb_comp);
          tab_valeurs=ch_front->valeurs_au_temps(0);
          la_table.remplir(param,tab_valeurs);
        }
      else
        {
          Cerr << "Champ " << ch_front.que_suis_je() << " non supporte!" << finl;
          Process::exit();
        }
      index_++;
      return index_;
    }
}

