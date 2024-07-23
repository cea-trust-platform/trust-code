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
#include <Partitionneur_Fichier_Decoupage.h>
#include <EFichier.h>
#include <Domaine.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Fichier_Decoupage,"Partitionneur_Fichier_Decoupage",Partitionneur_base);
// XD partitionneur_fichier_decoupage partitionneur_deriv fichier_decoupage -1 This algorithm reads an array of integer values on the disc, one value for each mesh element. Each value is interpreted as the target part number n>=0 for this element. The number of parts created is the highest value in the array plus one. Empty parts can be created if some values are not present in the array. NL2 The file format is ASCII, and contains space, tab or carriage-return separated integer values. The first value is the number nb_elem of elements in the domain, followed by nb_elem integer values (positive or zero). NL2 This algorithm has been designed to work together with the \'ecrire_decoupage\' option. You can generate a partition with any other algorithm, write it to disc, modify it, and read it again to generate the .Zone files. NL2 Contrary to other partitioning algorithms, no correction is applied by default to the partition (eg. element 0 on processor 0 and corrections for periodic boundaries). If \'corriger_partition\' is specified, these corrections are applied.

Partitionneur_Fichier_Decoupage::Partitionneur_Fichier_Decoupage()
{
  filename_ = "";
  corriger_partition_ = 0;
}

/*! @brief Lecture des parametres du partitionneur sur disque.
 *
 * Fomat attendu:
 *     { fichier FILENAME }
 *   FILENAME est le nom d'un fichier existant au format ArrOfInt ascii.
 *
 */
Entree& Partitionneur_Fichier_Decoupage::readOn(Entree& is)
{
  Partitionneur_base::readOn(is);
  Cerr << " filename : " << filename_ << finl;
  if (corriger_partition_ == 1)
    Cerr << " corriger_partition => partition will be corrected (periodic + elem0_on_proc0)" << finl;
  return is;
}

Sortie& Partitionneur_Fichier_Decoupage::printOn(Sortie& os) const
{
  Cerr << "Partitionneur_Metis::printOn invalid\n" << finl;
  exit();
  return os;
}

void Partitionneur_Fichier_Decoupage::set_param(Param& param)
{
  param.ajouter("fichier",&filename_,Param::REQUIRED);            // XD attr fichier chaine fichier 0 File name
  param.ajouter_flag("corriger_partition",&corriger_partition_);  // XD attr corriger_partition rien corriger_partition 1 not_set
}

void Partitionneur_Fichier_Decoupage::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

void Partitionneur_Fichier_Decoupage::initialiser(const char *filename)
{
  filename_ = filename;
}

/*! @brief Lit le contenu du fichier "filename_" et stocke le resultat dans elem_part
 *
 */
void Partitionneur_Fichier_Decoupage::construire_partition(IntVect& elem_part, int& nb_parts_tot) const
{
  if (! ref_domaine_.non_nul())
    {
      Cerr << "Error in Partitionneur_Fichier_Decoupage::construire_partition\n";
      Cerr << " The domain has not been associated" << finl;
      exit();
    }
  if (filename_ == "")
    {
      Cerr << "Error in Partitionneur_Fichier_Decoupage::construire_partition\n";
      Cerr << " The file name has not been initialized" << finl;
      exit();
    }

  Cerr << "Reading of splitting file : " << filename_ << finl;
  EFichier file;
  file.ouvrir(filename_);
  if (!file.good())
    {
      Cerr << "Error in Partitionneur_Fichier_Decoupage::construire_partition\n";
      Cerr << " Failed to open file " << filename_ << finl;
      exit();
    }
  file >> elem_part;
  file >> nb_parts_tot;
  file.close();
  const int sz = elem_part.size_array();
  const int nelem = ref_domaine_->nb_elem();
  if (nelem != sz)
    {
      Cerr << "Error in Partitionneur_Fichier_Decoupage::construire_partition\n";
      Cerr << " The file contains an array of " << sz << " values.\n";
      Cerr << " The area contains " << nelem << " elements" << finl;
      exit();
    }
  if (corriger_partition_)
    {
      Cerr << " Correction of the partition." << finl;
      if (liste_bords_periodiques_.size() > 0)
        corriger_bords_avec_liste(ref_domaine_.valeur(),
                                  liste_bords_periodiques_,
                                  0,
                                  elem_part);
      corriger_elem0_sur_proc0(elem_part);
    }
}
