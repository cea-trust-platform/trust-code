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
// File:        Matrice_Dense.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Matrice_Dense_included
#define Matrice_Dense_included

#include <Matrice_Base.h>
#include <Matrice_Morse.h>
#include <TRUSTTab.h>

class Matrice_Dense : public Matrice_Base
{

  Declare_instanciable_sans_constructeur( Matrice_Dense );

public :
  Matrice_Dense( void );
  Matrice_Dense( const int& nb_lines , const int& nb_cols );
  void dimensionner( const int& nb_lines , const int& nb_cols );
  void read_from_file( const Nom& filename );
  void convert_to_morse_matrix( Matrice_Morse& morse_matrix ) const;
  inline const double& operator( )( const int& line , const int& col ) const;
  inline double& operator( )( const int& line , const int& col );
  void build_matrix_from_coefficients_line_by_line( const DoubleVect& coefficients );
  void build_matrix_from_coefficients_column_by_column( const DoubleVect& coefficients );
  bool is_the_same( const Matrice_Dense& other_matrix , const double& tol=1e-14 ) const;
  void build_the_transposed( Matrice_Dense& transposed ) const;
  void set_coefficient( const int& i , const int& j , const double& value );
  Sortie& imprimer_formatte( Sortie& s ) const override;

  int ordre() const override;
  int nb_lignes() const override;
  int nb_colonnes() const override;

  DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& r) const override;
  DoubleVect& ajouter_multvectT_(const DoubleVect& x, DoubleVect& r) const override;
  DoubleTab& ajouter_multTab_(const DoubleTab& x, DoubleTab& r) const override;

  void scale( const double& x ) override ;
  void get_stencil( IntTab& stencil ) const override;

  // Perform the matrix inversion
  void inverse();

  // Perform a matrix multipication : (*this) * B = RES
  void multiplyToRight(const Matrice_Dense& B, Matrice_Dense& RES) const;
  inline DoubleTab& coeffs()
  {
    return Matrix_;
  }
private :

  DoubleTab Matrix_ ;
};

// Access operators
inline const double& Matrice_Dense::operator( )(const int& line, const int& col) const
{
  assert( line < nb_lignes( ) );
  assert( col < nb_colonnes( ) );
  return Matrix_( line , col );
}

inline double& Matrice_Dense::operator( )(const int& line, const int& col)
{
  assert( line < nb_lignes( ) );
  assert( col < nb_colonnes( ) );
  return Matrix_( line , col );
}

#endif
