/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Matrice_Bloc_included
#define Matrice_Bloc_included

#include <TRUSTTabs_forward.h>
#include <Matrice_Base.h>
#include <TRUSTLists.h>
#include <Matrice.h>

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*     classe Matrice_Bloc : stockage des blocs d`une matrice bloc A(N,M)     */
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
/*              [A21 A22 ... A2M]                                             */
/*          A = [... ... ... ...]  blocs_=[A11,A12,...,A1M,A21,A22,...,A2M,   */
/*              [AN1 AN2 ... ANM]                     ,...,AN1,AN2,...,ANM]   */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

class Matrice_Morse;

class Matrice_Bloc : public Matrice_Base
{
  Declare_instanciable_sans_constructeur(Matrice_Bloc);

public :
  int ordre( void ) const override;
  int nb_lignes( void ) const override;
  int nb_colonnes( void ) const override;

  // Methodes pour le calcul de r+=Ax codees dans les classes filles
  DoubleVect& ajouter_multvect_( const DoubleVect& x, DoubleVect& r ) const override;
  DoubleVect& ajouter_multvectT_( const DoubleVect& x, DoubleVect& r ) const override;
  DoubleTab& ajouter_multTab_( const DoubleTab& x, DoubleTab& r ) const override;

  // multiplication par un scalaire
  void scale( const double x ) override;

  void get_stencil( IntTab& stencil ) const override;
  void get_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;
  void get_stencil_and_coeff_ptrs(IntTab& stencil, std::vector<const double *>& coeff_ptr) const override;


  // Impression
  Sortie& imprimer( Sortie& s ) const override;
  Sortie& imprimer_formatte( Sortie& s ) const override;

  // Dimensionnement
  virtual void dimensionner( int N, int M );

  // Acces aux blocs
  virtual const Matrice& get_bloc( int i, int j ) const;
  virtual Matrice& get_bloc( int i, int j );

  void build_stencil( void ) override;

public :
  // Constructeurs :
  Matrice_Bloc( int N=0, int M=0 );

  // Acces aux caracteristiques du vecteur blocs_
  int dim( int d ) const;                // si d=0 => N_   si d=1 => M_
  int nb_bloc_lignes( void ) const;            // retourne N_
  int nb_bloc_colonnes(void ) const;           // retourne M_

  // Remplissage par une matrice morse symetrique
  void remplir(const IntLists& voisins, const DoubleLists& valeurs, const DoubleVect& terme_diag, const int i, const int n);

  // // Remplissage par une matrice morse
  void remplir(const IntLists& voisins, const DoubleLists& valeurs, const int i, const int n, const int j, const int m);

  // Remplissage par une matrice morse symetrique ou non
  void remplir(const IntLists& voisins, const DoubleLists& valeurs, const DoubleVect& terme_diag, const int i, const int n, const int j, const int m);

  // // Conversion vers une Matrice_Morse
  void block_to_morse( Matrice_Morse& matrix ) const;
  void block_to_morse_with_ptr( Matrice_Morse& result, std::vector<const double *>& coeffs) const;

  void BlocToMatMorse( Matrice_Morse& matrix ) const;


  Matrice_Bloc& operator *=( double x);

  bool check_block_matrix_structure( void ) const;

  void assert_check_block_matrix_structure( void ) const;

protected :
  VECT(Matrice) blocs_;           // les blocs de la matrices source A
  int N_;                       // 1ere dim de A
  int M_;                       // 2eme dim de A
  int nb_blocs_;                   // nb total des blocs de A (= N_ * M_)

  ArrOfInt offsets_;

  template<typename _TAB_T_, typename _VAL_T_>
  void get_stencil_coeff_templ( IntTab& stencil, _TAB_T_& coeff_sp) const;
};

#endif
