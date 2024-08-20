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
std::string Champ_Parametrique::dirnameDefault="";
std::string Champ_Parametrique::dirnameCompute(int compute)
{
  return Champ_Parametrique::dirnameDefault.empty() ? "calcul"+std::to_string(compute) : Champ_Parametrique::dirnameDefault;
}

Entree& Champ_Parametrique::readOn(Entree& is)
{
  Champ_Parametrique::enabled=true;
  Nom fichier;
  Param param(que_suis_je());
  param.ajouter("fichier", &fichier, Param::REQUIRED);  // XD_ADD_P chaine Filename where fields are read
  param.lire_avec_accolades_depuis(is);
  // Lecture de tous les lignes du fichier parametrique:
  EFichier fic(fichier);
  Motcle motlu;
  fic >> motlu;
  if (motlu!="{") Process::exit("Waiting { !");
  while (motlu!="}")
    {
      Champ_Don ch;
      fic >> ch;
      champs_.add(ch);
      //Cerr << "[Parameter] Reading: " << ch->que_suis_je() << finl;
      fic >> motlu;
      // Pour eviter de surcharger plusieurs methodes de Champ_Don_base:
      fixer_nb_comp(ch->nb_comp());
    }
  // On fixe le premier parametre:
  Sortie_Fichier_base::set_root(newCompute());
  return is;
}

std::string Champ_Parametrique::newCompute() const
{
  if (champs_.size()==index_)
    return "";
  else
    {
      Nom previous_field("");
      if (index_)
        {
          previous_field = " from ";
          previous_field += champ()->que_suis_je();
        }
      index_++;
      Nom next_field = champ()->que_suis_je();
      Cerr << "[Parameter] Updating field" << previous_field << " to " << next_field << finl;
      return Champ_Parametrique::dirnameCompute(index_);
    }
}

