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
// File:        Matrice_Morse_Diag.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Morse_Diag.h>

Implemente_instanciable_sans_constructeur(Matrice_Morse_Diag,"Matrice_Morse_Diag",Matrice_Morse_Sym);

// Description:
//    Ecrit les trois tableaux de la structure de stockage
//    Morse sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie& s
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Matrice_Morse_Diag::printOn(Sortie& s) const
{
  return  Matrice_Morse::printOn(s);
}


// Description:
//    NON CODE
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree& s
//    Signification: le flot d'entree
//    Contraintes:
// Exception: NON CODE
// Effets de bord:
// Postcondition:
Entree& Matrice_Morse_Diag::readOn(Entree& s)
{
  return  Matrice_Morse::readOn(s) ;
}

// Description: provoquue une erreur le but de cette matrice est juste de calculer la diagonale
DoubleVect& Matrice_Morse_Diag::ajouter_multvect_(const DoubleVect& x, DoubleVect& y) const
{
  return Matrice_Morse::ajouter_multvect_(x,y);
}

// Description: provoquue une erreur le but de cette matrice est juste de calculer la diagonale
DoubleVect& Matrice_Morse_Diag::ajouter_multvectT_(const DoubleVect& x, DoubleVect& y) const
{
  Cerr<<" not implemented "<<finl;
  exit();
  return y;
}
// Description: provoquue une erreur le but de cette matrice est juste de calculer la diagonale
DoubleTab& Matrice_Morse_Diag::ajouter_multTab_(const DoubleTab& x, DoubleTab& y) const
{
  Cerr<<" not implemented "<<finl;
  exit();
  return y;
}

void Matrice_Morse_Diag::dimensionne_diag(int nb_case_tot)
{
  IntTab indice(nb_case_tot,2);
  for( int c=0; c<nb_case_tot; c++) indice(c,0)=indice(c,1)=c;
  dimensionner( nb_case_tot, 0);
  dimensionner(indice);
}
