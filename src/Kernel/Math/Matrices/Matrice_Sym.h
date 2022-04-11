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
// File:        Matrice_Sym.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Matrice_Sym_included
#define Matrice_Sym_included

#define _DEGRE_POLY_ 5
#define _SEUIL_GCP_ 1e-12

#include <TRUSTTabs_forward.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Matrice_Sym
//    Cette classe est la classe de base pour la representantion des
//    matrices symetriques. Cette classe est en fait une classe "interface"
//    qui donne acces a des methodes de resolutions de systemes lineaires
//    dont la matrice est symetrique: Gradient conjugue (preconditionne),
//    solveur SSOR, precondtionneur polynomial.
//    Cette classe ne contient pas de membre de donnee (hors mis ceux herites de
//    Process) car elle sera utilisee par heritage multiple.
//    Elle "accede" a la matrice via la methode de multiplication matrice-vecteur
//    DoubleVect& multvect(const DoubleVect&, DoubleVect& resu) const
//    qui est une methode abstraite.
// .SECTION voir aussi
//    Matrice_Morse_Sym
//    Cette classe n'herite pas d'objet U car elle est utilisee par
//    heritage multiple avec d'autre classe heritant deja d'objet U.
//    Voir par exemple Mat_Morse_Sym.
//    Classe abstraite
//    Methodes abstraites
//      int ssor(const DoubleVect& secmem, DoubleVect& solution,
//                           double=_OMEGA_SSOR_ ) const;
//      DoubleVect& multvect(const DoubleVect&, DoubleVect& resu) const [protegee]
//////////////////////////////////////////////////////////////////////////////

class Matrice_Sym
{
public :
  virtual ~Matrice_Sym() {};
  Matrice_Sym():est_definie_(0) {}

  int get_est_definie() const;
  void set_est_definie(int);
  void unsymmetrize_stencil(const int nb_lines, const IntTab& symmetric_stencil, IntTab& stencil) const;
  void unsymmetrize_stencil_and_coefficients(const int nb_lines, const IntTab& symmetric_stencil, const ArrOfDouble& symmetric_coefficients, IntTab& stencil, ArrOfDouble& coefficients) const;

protected :
  virtual DoubleTab& ajouter_multTab_(const DoubleTab&, DoubleTab& ) const=0 ;
  virtual DoubleVect& ajouter_multvect_(const DoubleVect&, DoubleVect& ) const=0 ;
  virtual DoubleVect& ajouter_multvectT_(const DoubleVect&, DoubleVect& ) const=0 ;

private :
  int est_definie_;
};

#endif
