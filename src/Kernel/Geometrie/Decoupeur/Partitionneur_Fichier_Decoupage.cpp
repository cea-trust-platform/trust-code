/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Partitionneur_Fichier_Decoupage.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#include <Partitionneur_Fichier_Decoupage.h>
#include <EFichier.h>
#include <Domaine.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Partitionneur_Fichier_Decoupage,"Partitionneur_Fichier_Decoupage",Partitionneur_base);

Partitionneur_Fichier_Decoupage::Partitionneur_Fichier_Decoupage()
{
  filename_ = "";
  corriger_partition_ = 0;
}

// Description:
//  Lecture des parametres du partitionneur sur disque.
//  Fomat attendu:
//    { fichier FILENAME }
//  FILENAME est le nom d'un fichier existant au format ArrOfInt ascii.
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
  param.ajouter("fichier",&filename_,Param::REQUIRED);
  param.ajouter_flag("corriger_partition",&corriger_partition_);
}

void Partitionneur_Fichier_Decoupage::associer_domaine(const Domaine& domaine)
{
  ref_domaine_ = domaine;
}

void Partitionneur_Fichier_Decoupage::initialiser(const char *filename)
{
  filename_ = filename;
}

// Description:
//  Lit le contenu du fichier "filename_" et stocke le resultat dans elem_part
// Precondition:
//  domaine associe et filename initialise
void Partitionneur_Fichier_Decoupage::construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const
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
  const int nelem = ref_domaine_.valeur().zone(0).nb_elem();
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
                                  elem_part);
      corriger_elem0_sur_proc0(elem_part);
    }
}
