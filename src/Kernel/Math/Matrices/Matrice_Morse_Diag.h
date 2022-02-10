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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Matrice_Morse_Diag.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Matrice_Morse_Diag_included
#define Matrice_Morse_Diag_included

#include <Matrice_Morse_Sym.h>
#include <Matrice_Diagonale.h>

class Matrice_Diagonale;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Matrice_Morse_Diag
//    Represente une matrice M (creuse) symetrique stockee au format Morse
//    Diagetrique: on ne stocke que la partie triangulaire superieure de la
//    matrice. Important: Tous les coefficients diagonaux doivent etre stockes
//    meme s'ils sont nuls (hypothese TRUST pour plusieurs methodes dont multvect).
//    Cette classe derive de Matrice_Morse_Sym
// .SECTION voir aussi
//    Matrice_Morse Matrice_Diag
//////////////////////////////////////////////////////////////////////////////
class Matrice_Morse_Diag : public Matrice_Morse_Sym
{

  Declare_instanciable_sans_constructeur(Matrice_Morse_Diag);

public :
  Matrice_Morse_Diag()
  {
    symetrique_=2;
  };
  void dimensionne_diag(int n);
  DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& y) const override ;
  DoubleVect& ajouter_multvectT_(const DoubleVect&, DoubleVect& ) const override;
  DoubleTab& ajouter_multTab_(const DoubleTab& ,DoubleTab& ) const override;
private:
};

#endif
