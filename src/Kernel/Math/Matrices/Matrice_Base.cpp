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
// File:        Matrice_Base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Base.h>
#include <Matrice_Morse.h>
#include <Matrice_Morse_Sym.h>
#include <Matrix_tools.h>

Implemente_base(Matrice_Base,"Matrice_Base",Objet_U);

Sortie& Matrice_Base::
printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}
Entree& Matrice_Base::
readOn(Entree& s )
{
  return s ;
}

// Description:
//    Fonction (hors classe) amie de la classe Matrice_Base.
//    Operateur de multiplication: renvoie (A*vect)
//    Appelle  Matrice_base::multv(const DoubleVect&) sur A.
// Precondition:
// Parametre: Matrice_Base& A
//    Signification: la matrice multiplicatrice
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& vect
//    Signification: le vecteur a multiplier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: DoubleVect
//    Signification: le vecteur resultat de l'operation
//    Contraintes: le vecteur est alloue par la methode
// Exception:
// Effets de bord:
// Postcondition:
DoubleVect operator * (const Matrice_Base& A, const DoubleVect& vect)
{
  DoubleVect r;
  return A.multvect(vect, r);
}

void Matrice_Base::get_stencil( IntTab& stencil ) const
{
  Cerr << "Error in 'Matrice_Base::get_stencil( )':" << finl;
  Cerr << "  This method should be implemented in derived class." << finl;
  Cerr << "  Exiting..." << finl;
  Process::exit( );
}

void Matrice_Base::get_symmetric_stencil( IntTab& stencil ) const
{
  Matrice_Morse tmp1;
  Matrix_tools::convert_to_morse_matrix( *this, tmp1 );

  Matrice_Morse_Sym tmp2( tmp1 );
  tmp2.get_symmetric_stencil( stencil );
}

void Matrice_Base::get_stencil_and_coefficients( IntTab&      stencil,
                                                 ArrOfDouble& coefficients ) const
{
  Cerr << "Error in 'Matrice_Base::get_stencill_and_coefficients( )':" << finl;
  Cerr << "  This method should be implemented in derived class" << finl;
  Cerr << "  Exiting..." << finl;
  Process::exit( );
}

void Matrice_Base::get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                           ArrOfDouble& coefficients ) const
{
  Matrice_Morse tmp1;
  Matrix_tools::convert_to_morse_matrix( *this, tmp1 );

  Matrice_Morse_Sym tmp2( tmp1 );
  tmp2.get_symmetric_stencil_and_coefficients( stencil, coefficients );
}

