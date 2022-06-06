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
#include <Partitionneur_Union.h>
#include <EFichier.h>
#include <Domaine.h>
#include <Param.h>
#include <EChaine.h>
#include <Sous_Zone.h>

// XD partitionneur_union partitionneur_deriv union 0 Let several local domains be generated from a bigger one using the keyword create_domain_from_sous_zone, and let their partitions be generated in the usual way. Provided the list of partition files for each small domain, the keyword 'union' will partition the global domain in a conform fashion with the smaller domains.
// XD attr liste bloc_lecture liste 0 List of the partition files with the following syntaxe: {sous_zone1 decoupage1  ... sous_zoneim decoupageim } where sous_zone1 ... sous_zomeim are small domains names and decoupage1 ... decoupageim are partition files.
Implemente_instanciable(Partitionneur_Union,"Partitionneur_Union",Partitionneur_base);

// Description:
//  Lecture des parametres du partitionneur sur disque.
//  Fomat attendu:
//    { sous_zone decoupage_sous_zone }
//  FILENAME est le nom d'un fichier existant au format ArrOfInt ascii.
Entree& Partitionneur_Union::readOn(Entree& is)
{
  Nom mot, fic;
  is >> mot;
  if (mot != "{")
    {
      Cerr << "Partitionneur_Union : list { sous_zone decoupage } expected." << finl;
      abort();
    }
  for (is >> mot; mot != "}"; is >> mot)
    {
      is >> fic;
      if (fic == "}")
        {
          Cerr << "Partitionneur_Union : partition file expected for sub-zone " << mot << finl;
          abort();
        }
      fic_ssz[mot.getString()] = fic.getString();
    }
  return is;
}

Sortie& Partitionneur_Union::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Union::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Union::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

// Description:
//  Lit le contenu du fichier "filename_" et stocke le resultat dans elem_part
// Precondition:
//  domaine associe et filename initialise
void Partitionneur_Union::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
  elem_part.resize(ref_domaine_->zone(0).nb_elem());
  elem_part = -1;
  for (auto &&kv : fic_ssz)
    {
      //on recupere la sous-zone par son nom et le decoupage en ouvrant le fichier...
      const Sous_Zone& ssz = ref_cast(Sous_Zone, Interprete::objet(kv.first));
      EFichier file;
      file.ouvrir(kv.second.c_str());
      IntVect dec_ssz;
      file >> dec_ssz;
      file.close();
      //... et on remplit un morceau de elem_part avec
      if (dec_ssz.size_array() != ssz.nb_elem_tot())
        {
          Cerr << "Partitionneur_Union : incoherent element number for sub-zone " << kv.first << finl;
          Process::exit();
        }
      for (int i = 0; i < ssz.nb_elem_tot(); i++)
        elem_part[ssz(i)] = dec_ssz(i);
    }
}
