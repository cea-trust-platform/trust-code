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

#include <Champ_Parametrique.h>
#include <Param.h>
#include <Probleme_base.h>
#include <EFichier.h>
#include <EChaine.h>
#include <string>

Implemente_instanciable( Champ_Parametrique, "Champ_Parametrique", Champ_Don_base );
// ToDo XDATA

Sortie& Champ_Parametrique::printOn(Sortie& os) const { return Champ_Don_base::printOn(os); }

Entree& Champ_Parametrique::readOn(Entree& is)
{
  Nom pb, fichier;
  Param param(que_suis_je());
  // ToDo : ne plus lire le probleme ? Difficile car les termes sources n'ont pas les Champ_Don
  param.ajouter("probleme", &pb, Param::REQUIRED);
  param.ajouter("fichier", &fichier, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  // Lecture de tous les lignes du fichier parametrique:
  EFichier fic(fichier);
  Motcle motlu;
  Champ_Don ch;
  fic >> motlu;
  if (motlu!="{") Process::exit("Waiting { !");
  while (motlu!="}")
    {
      fic >> ch;
      champs_.add(ch);
      Cerr << "[Parameter] Reading: " << ch.valeur().que_suis_je() << finl;
      fic >> motlu;
    }
  // Associer au schema temps:
  Probleme_base& pb_base = ref_cast(Probleme_base, Interprete::objet(pb));
  // Verifie que tous les champs parametriques ont le meme nombre
  LIST(REF(Champ_Parametrique))& Champs_Parametriques = pb_base.Champs_Parametriques();
  int nb_champs_parametriques = Champs_Parametriques.size();
  if (nb_champs_parametriques && Champs_Parametriques.dernier()->size()!=size())
    {
      Cerr << "Erreur, le champ parametrique lu dans le fichier " << fichier << " a " << size() << " elements." << finl;
      Cerr << "Les autres champs parametriques ont " << Champs_Parametriques.dernier()->size() << " elements." << finl;
      Process::exit("Cela doit etre identique.");
    }
  Champs_Parametriques.add(*this);
  // On fixe le premier parametre:
  int calcul = newParameter();
  assert(calcul>0);
  Sortie_Fichier_base::set_root("calcul"+std::to_string(calcul));
  return is;
}

int Champ_Parametrique::newParameter()
{
  if (champs_.size()==index_)
    return 0;
  else
    {
      index_++;
      // ToDo ameliorer message avec les caracteristiques du champ
      Cerr << "================================================" << finl;
      Cerr << "[Parameter] Updating to: " << champ().valeur() << finl;
      Cerr << "================================================" << finl;
      return index_;
    }
}

