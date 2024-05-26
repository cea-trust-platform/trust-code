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
#include <Partitionneur_Sous_Domaine.h>
#include <Interprete.h>
#include <EFichier.h>
#include <Domaine.h>
#include <Param.h>
#include <EChaine.h>

// XD partitionneur_sous_dom partitionneur_deriv sous_dom -1 Given a global partition of a global domain, 'sous-domaine' allows to produce a conform partition of a sub-domain generated from the bigger one using the keyword create_domain_from_sub_domain. The sub-domain will be partitionned in a conform fashion with the global domain.
Implemente_instanciable_sans_constructeur(Partitionneur_Sous_Domaine,"Partitionneur_Sous_Dom",Partitionneur_base);

Partitionneur_Sous_Domaine::Partitionneur_Sous_Domaine()
{
  filename_ = "";
  filename_ssz_ = "";
}

/*! @brief Lecture des parametres du partitionneur sur disque.
 *
 * Fomat attendu:
 *     {
 *         fichier FILENAME
 *         fichier_ssz FILENAME
 *     }
 *   FILENAME est le nom d'un fichier existant au format ArrOfInt ascii.
 *
 */
Entree& Partitionneur_Sous_Domaine::readOn(Entree& is)
{
  Partitionneur_base::readOn(is);
  if (filename_ssz_ == "" && name_ssz_ == "")
    Process::exit(que_suis_je() + " : at least one of filename_ssz or name_ssz are needed");
  Cerr << " filename : " << filename_ << finl;
  Cerr << " filename_ssz : " << filename_ssz_ << finl;
  Cerr << " name_ssz : " << name_ssz_ << finl;
  return is;
}

Sortie& Partitionneur_Sous_Domaine::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Sous_Domaine::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Sous_Domaine::set_param(Param& param)
{
  param.ajouter("fichier",&filename_,Param::REQUIRED); // XD_ADD_P chaine fichier
  param.ajouter("fichier_ssz",&filename_ssz_); // XD_ADD_P chaine fichier sous zonne
  param.ajouter("name_ssz",&name_ssz_); // XD_ADD_P chaine nom sous zonne
}

/*! @brief Lit le contenu du fichier "filename_" et stocke le resultat dans elem_part
 *
 */
void Partitionneur_Sous_Domaine::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
  if (filename_ == "")
    {
      Cerr << "Error in Partitionneur_Sous_Domaine::construire_partition\n";
      Cerr << " The file name has not been initialized" << finl;
      exit();
    }

  Cerr << "Reading of splitting file : " << filename_ << finl;
  EFichier file;
  file.ouvrir(filename_);
  if (!file.good())
    {
      Cerr << "Error in Partitionneur_Sous_Domaine::construire_partition\n";
      Cerr << " Failed to open file " << filename_ << finl;
      exit();
    }
  IntVect elem_part_glob;
  file >> elem_part_glob;
  file >> nb_parts_tot;
  file.close();

  ArrOfInt elem_ssz;
  if (filename_ssz_ != "")
    {
      Cerr << "Reading of subdomaine file : " << filename_ssz_ << finl;
      file.ouvrir(filename_ssz_);
      if (!file.good())
        {
          Cerr << "Error in Partitionneur_Sous_Domaine::construire_partition\n";
          Cerr << " Failed to open file " << filename_ssz_ << finl;
          exit();
        }
      file >> elem_ssz;
      file.close();
    }
  else
    elem_ssz = ref_cast(Sous_Domaine, Interprete::objet(name_ssz_)).les_elems();

  /* remplissage de elem_part */
  elem_part.resize(elem_ssz.size_array());
  for (int i = 0; i < elem_ssz.size_array(); i++)
    nb_parts_tot = std::max(nb_parts_tot, 1 + (elem_part[i] = elem_part_glob[elem_ssz[i]]));

}
