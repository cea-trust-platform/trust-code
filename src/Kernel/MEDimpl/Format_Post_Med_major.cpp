/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Format_Post_Med_major.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <Format_Post_Med_major.h>

Implemente_instanciable_sans_constructeur(Format_Post_Med_major,"Format_Post_Med_major",Format_Post_Medfile);

// Description: erreur => exit
Sortie& Format_Post_Med_major::printOn(Sortie& os) const
{
  Cerr << "Format_Post_Mesh_major::printOn : error" << finl;
  exit();
  return os;
}

// Description: Lecture des parametres du postraitement au format "jeu de donnees"
//  Le format attendu est le suivant:
//  {
//    nom_fichier filename_sans_extension
//  }
Entree& Format_Post_Med_major::readOn(Entree& is)
{
  Format_Post_Med::readOn(is);
  return is;
}

EcrMED Format_Post_Med_major::getEcrMED() const
{
  EcrMED e;
  e.setMajorMode(true);
  e.setMEDCoupling(false);
  return e;
}
