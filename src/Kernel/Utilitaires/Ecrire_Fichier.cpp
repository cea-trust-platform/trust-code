/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Ecrire_Fichier.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Ecrire_Fichier.h>
#include <SFichier.h>
#include <Synonyme_info.h>

Implemente_instanciable(Ecrire_Fichier,"Ecrire_Fichier",Interprete);
Add_synonym(Ecrire_Fichier,"Ecrire_Fichier_Bin");

// Description:
//    Appel a la methode printOn de la classe Interprete
Sortie& Ecrire_Fichier::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

// Description:
//    Appel a la methode readOn de la classe Interprete
Entree& Ecrire_Fichier::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

void Ecrire_Fichier::interpreter_(Entree& is, int format_binaire)
{
  if (nproc() > 1)
    {
      Cerr << "Error: keyword Ecrire_Fichier_Bin cannot be used in parallel calculation. What do you mean ?" << finl;
      barrier();
      exit();
    }
  SFichier fic;
  Nom nom1, nom2;
  is >> nom1 >> nom2;
  Cerr << "Ecrire_Fichier_Bin: writing object " << nom1 << " in file " << nom2;
  if (format_binaire)
    Cerr << " in binary format." << finl;
  else
    Cerr << " in ascii format." << finl;
  Objet_U& obj = objet(nom1);
  fic.set_bin(format_binaire);
  if (!fic.ouvrir(nom2))
    {
      Cerr << " Error opening file: " << nom2 << finl;
      exit();
    }
  if (!format_binaire)
    {
      fic.get_ofstream().setf(ios::scientific);
      fic.get_ofstream().precision(8);
    }
  fic << obj;
  if (!fic.get_ostream().good())
    {
      Cerr << " An error occured while writing the file." << finl;
      exit();
    }
}

Entree& Ecrire_Fichier::interpreter(Entree& is)
{
  Ecrire_Fichier::interpreter_(is, 1 /* binaire */);
  return is;
}
