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
// XD Champ_Parametrique champ_don_base Champ_Parametrique 0 Parametric field

Sortie& Champ_Parametrique::printOn(Sortie& os) const { return Champ_Don_base::printOn(os); }

bool Champ_Parametrique::enabled=false;
Entree& Champ_Parametrique::readOn(Entree& is)
{
  Champ_Parametrique::enabled=true;
  Nom pb, fichier;
  Param param(que_suis_je());
  param.ajouter("fichier", &fichier, Param::REQUIRED);  // XD_ADD_P chaine Filename where fields are read
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
  // On fixe le premier parametre:
  int compute = newCompute();
  assert(compute>0);
  Sortie_Fichier_base::set_root("calcul"+std::to_string(compute));
  return is;
}

int Champ_Parametrique::newCompute() const
{
  if (champs_.size()==index_)
    return 0;
  else
    {
      index_++;
      // ToDo ameliorer message avec les caracteristiques du champ
      Cerr << "================================================" << finl;
      Cerr << "[Parameter] Updating to: " << champ().valeur() << (champ()->instationnaire() ? " (transient field) " : " (permanent field) ") << finl;
      Cerr << "================================================" << finl;
      return index_;
    }
}

