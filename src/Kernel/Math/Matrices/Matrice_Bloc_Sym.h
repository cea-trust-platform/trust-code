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
// File:        Matrice_Bloc_Sym.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Matrice_Bloc_Sym_included
#define Matrice_Bloc_Sym_included



//
// .DESCRIPTION class Matrice_Bloc_Sym
//    Cette classe derive de Matrice_Bloc et Matrice_Sym.
//
// .SECTION voir aussi
// Matrice_Bloc Matrice_Sym

#include <Matrice_Bloc.h>
#include <Matrice_Sym.h>

////////////////////////////////////////////////////////////
//
// CLASS : Matrice_Bloc_Sym
//
////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*     classe Matrice_Bloc_Sym : stockage des blocs d`une matrice bloc A(N,M) */
/*                                                                            */
/*                           le stockage est effectue ligne par ligne         */
/*                           dans un vecteur de type VECT(Matrice).           */
/*     On utilise:                                                            */
/*              blocs_    = vecteur de matrices Aij                           */
/*              N_        = 1ere dim de blocs_                                */
/*              M_        = 2eme dim de blocs_                                */
/*              nb_blocs_ = nb total de blocs (= N_ * M_)                     */
/*                                                                            */
/*     Forme matricielle:                                                     */
/*                                                                            */
/*              [A11 A12 ... A1M]                                             */
/*              [... A22 ... A2M]                                             */
/*          A = [... ... ... ...]  blocs_=[A11,A12,...,A1M,A21,A22,...,A2M,   */
/*              [... ... ... ANM]                     ,...,AN1,AN2,...,ANM]   */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

class Matrice_Morse_Sym;

class Matrice_Bloc_Sym : public Matrice_Bloc, public Matrice_Sym
{
  Declare_instanciable_sans_constructeur(Matrice_Bloc_Sym);

public:

  // Constructeurs :
  Matrice_Bloc_Sym(int N=0, int M=0);

  // Impression
  Sortie& imprimer(Sortie& s) const;
  Sortie& imprimer_formatte(Sortie& s) const;

  //Multiplications
  virtual DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& y) const ;
  virtual DoubleVect& ajouter_multvectT_(const DoubleVect& x, DoubleVect& y) const ;
  virtual DoubleTab& ajouter_multTab_(const DoubleTab& x, DoubleTab& y) const ;

  //Conversions:
  void BlocSymToMatMorseSym(Matrice_Morse_Sym& mat) const;

  // Dimensionnement
  virtual void dimensionner(int N, int M); // dimensionnement de blocs_

  // Acces aux blocs: renvoie le bloc Aij avec A(N,M)
  virtual const Matrice& get_bloc(int i, int j) const ; // (0<=i<N , i<=j<M)
  virtual Matrice& get_bloc(int i, int j);

  virtual void get_stencil( IntTab& stencil ) const;

  virtual void get_symmetric_stencil( IntTab& stencil ) const;

  virtual void get_stencil_and_coefficients( IntTab&      stencil,
                                             ArrOfDouble& coefficients ) const;

  virtual void get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                       ArrOfDouble& coefficients ) const;

public :
  bool check_symmetric_block_matrix_structure( void ) const;
  void assert_check_symmetric_block_matrix_structure( void ) const;

protected:

};

#endif
