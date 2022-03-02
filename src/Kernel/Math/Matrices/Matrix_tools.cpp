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
// File:        Matrix_tools.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrix_tools.h>
#include <Matrice.h>
#include <Matrice_Base.h>
#include <Matrice_Nulle.h>
#include <Matrice_Diagonale.h>
#include <Matrice_Morse.h>
#include <Matrice_Morse_Sym.h>
#include <IntTab.h>
#include <Array_tools.h>

// conversion to morse format
void Matrix_tools::convert_to_morse_matrix( const Matrice_Base& in,
                                            Matrice_Morse&      out )
{
  IntTab stencil;
  ArrOfDouble coefficients;

  in.get_stencil_and_coefficients( stencil, coefficients );

  Matrix_tools::build_morse_matrix( in.nb_lignes( ),
                                    in.nb_colonnes( ),
                                    stencil,
                                    coefficients,
                                    out );
}



// conversion to symetric morse format
void Matrix_tools::convert_to_symmetric_morse_matrix( const Matrice_Base& in,
                                                      Matrice_Morse_Sym&  out )
{
  assert( in.nb_lignes( ) == in.nb_colonnes( ) );

  IntTab stencil;
  ArrOfDouble coefficients;

  in.get_symmetric_stencil_and_coefficients( stencil, coefficients );

  Matrix_tools::build_symmetric_morse_matrix( in.nb_lignes( ),
                                              stencil,
                                              coefficients,
                                              out );
}

// checking stencil
bool Matrix_tools::is_normalized_stencil( const IntTab& stencil )
{
  const int size = stencil.dimension( 0 );
  for ( int i=1; i<size; ++i )
    {
      int delta1 = stencil( i-1, 0 ) - stencil( i, 0 );
      int delta2 = stencil( i-1, 1 ) - stencil( i, 1 );
      int delta  = ( delta1 == 0 ) ? delta2 : delta1;

      if ( delta >= 0 )
        {
          return false;
        }
    }
  return true;
}

// checking symmetric stencil
bool Matrix_tools::is_normalized_symmetric_stencil( const IntTab& stencil )
{
  if ( ! ( is_normalized_stencil( stencil ) ) )
    {
      Cerr << "non-normalized stencil" << finl;
      return false;
    }

  const int size = stencil.dimension( 0 );
  for ( int i=0; i<size; ++i )
    {
      int delta = stencil( i, 0 ) - stencil( i, 1 );
      if ( delta > 0 )
        {
          Cerr << "( " << i << " ) line index > column index : ( " << stencil( i, 0 ) << ", " << stencil( i, 1 ) << " )" << finl;
          return false;
        }
    }

  return true;
}


// building morse matrices
void Matrix_tools::allocate_morse_matrix( const int&  nb_lines,
                                          const int&  nb_columns,
                                          const IntTab&  stencil,
                                          Matrice_Morse& matrix ,
                                          const bool& attach_stencil_to_matrix )
{
  assert( is_normalized_stencil( stencil ) );

  const int nb_coefficients = stencil.dimension( 0 );

  matrix.dimensionner( nb_lines,
                       nb_columns,
                       nb_coefficients );

  if ( nb_coefficients > 0 )
    {
      matrix.get_set_tab1( ) = 0 ;
      matrix.get_set_tab1( )( 0 ) = 1;
      for ( int i=0; i<nb_coefficients; ++i )
        {
          assert( stencil( i ,0 ) >= 0         );
          assert( stencil( i ,0 ) < nb_lines   );
          assert( stencil( i ,1 ) >= 0         );
          assert( stencil( i ,1 ) < nb_columns );

          matrix.get_set_tab1( )( stencil( i, 0 ) + 1 ) += 1;
          matrix.get_set_tab2( )( i ) = stencil( i, 1 ) + 1;
        }
      for ( int i=0; i<nb_lines; ++i )
        {
          matrix.get_set_tab1()( i + 1 ) += matrix.get_tab1()( i );
        }
    }

  if( attach_stencil_to_matrix )
    {
      matrix.set_stencil( stencil );
    }
}

void Matrix_tools::build_morse_matrix( const int&      nb_lines,
                                       const int&      nb_columns,
                                       const IntTab&      stencil,
                                       const ArrOfDouble& coefficients,
                                       Matrice_Morse&     matrix )
{
  assert( is_normalized_stencil( stencil ) );

  const int nb_coefficients = stencil.dimension( 0 );
  assert( nb_coefficients == coefficients.size_array( ) );

  matrix.dimensionner( nb_lines,
                       nb_columns,
                       nb_coefficients );

  if ( nb_coefficients > 0 )
    {
      matrix.get_set_tab1() =0  ;
      matrix.get_set_tab1()( 0 ) = 1;
      for ( int i=0; i<nb_coefficients; ++i )
        {
          assert( stencil( i ,0 ) >= 0         );
          assert( stencil( i ,0 ) < nb_lines   );
          assert( stencil( i ,1 ) >= 0         );
          assert( stencil( i ,1 ) < nb_columns );

          matrix.get_set_tab1()( stencil( i, 0 ) + 1 ) += 1;
          matrix.get_set_tab2()( i ) = stencil( i, 1 ) + 1;
          matrix.get_set_coeff()( i ) = coefficients[ i ];
        }
      for ( int i=0; i<nb_lines; ++i )
        {
          matrix.get_set_tab1()( i + 1 ) += matrix.get_tab1()( i );
        }
    }
}



// building symmetric morse matrices
void Matrix_tools::allocate_symmetric_morse_matrix( const int&      order,
                                                    const IntTab&      stencil,
                                                    Matrice_Morse_Sym& matrix )
{
  assert( is_normalized_symmetric_stencil( stencil ) );

  const int nb_coefficients = stencil.dimension( 0 );

  matrix.dimensionner( order,
                       order,
                       nb_coefficients );

  if ( nb_coefficients > 0 )
    {
      matrix.get_set_tab1()= 0 ;
      matrix.get_set_tab1()( 0 ) = 1;
      for ( int i=0; i<nb_coefficients; ++i )
        {
          assert( stencil( i ,0 ) >= 0               );
          assert( stencil( i ,0 ) < order            );
          assert( stencil( i ,1 ) >= 0               );
          assert( stencil( i ,1 ) < order            );
          assert( stencil( i, 0 ) <= stencil( i, 1 ) );

          matrix.get_set_tab1()( stencil( i, 0 ) + 1 ) += 1;
          matrix.get_set_tab2()( i ) = stencil( i, 1 ) + 1;
        }
      for ( int i=0; i<order; ++i )
        {
          matrix.get_set_tab1()( i + 1 ) += matrix.get_tab1()( i );
        }
    }
  matrix.set_symmetric( 1 );
}


void Matrix_tools::build_symmetric_morse_matrix( const int&      order,
                                                 const IntTab&      stencil,
                                                 const ArrOfDouble& coefficients,
                                                 Matrice_Morse_Sym& matrix )
{
  assert( is_normalized_symmetric_stencil( stencil ) );

  const int nb_coefficients = stencil.dimension( 0 );
  assert( nb_coefficients == coefficients.size_array( ) );

  matrix.dimensionner( order,
                       order,
                       nb_coefficients );

  if ( nb_coefficients > 0 )
    {
      matrix.get_set_tab1() = 0 ;
      matrix.get_set_tab1()( 0 ) = 1;
      for ( int i=0; i<nb_coefficients; ++i )
        {
          assert( stencil( i ,0 ) >= 0               );
          assert( stencil( i ,0 ) < order            );
          assert( stencil( i ,1 ) >= 0               );
          assert( stencil( i ,1 ) < order            );
          assert( stencil( i, 0 ) <= stencil( i, 1 ) );

          matrix.get_set_tab1()( stencil( i, 0 ) + 1 ) += 1;
          matrix.get_set_tab2()( i )  = stencil( i, 1 ) + 1;
          matrix.get_set_coeff()( i ) = coefficients[ i ];
        }
      for ( int i=0; i<order; ++i )
        {
          matrix.get_set_tab1()( i + 1 ) += matrix.get_tab1()( i );
        }
    }
  matrix.set_symmetric( 1 );
}


// allocation for scaled addition
void Matrix_tools::allocate_for_scaled_addition( const Matrice& A,
                                                 const Matrice& B,
                                                 Matrice&       C )
{
  assert( A.valeur( ).nb_lignes( )   == B.valeur( ).nb_lignes( )   );
  assert( A.valeur( ).nb_colonnes( ) == B.valeur( ).nb_colonnes( ) );

  IntTab A_stencil;
  A.valeur( ).get_stencil( A_stencil );
  const int A_size = A_stencil.dimension( 0 );

  IntTab B_stencil;
  B.valeur( ).get_stencil( B_stencil );
  const int B_size = B_stencil.dimension( 0 );

  int size = A_size + B_size;
  IntTab stencil;
  stencil.set_smart_resize( 1 );
  stencil.resize( size, 2 );

  for ( int i=0; i<A_size; ++i )
    {
      stencil( i , 0 ) = A_stencil( i, 0 );
      stencil( i , 1 ) = A_stencil( i, 1 ) ;
    }

  for ( int i=0; i<B_size; ++i )
    {
      stencil( i+A_size , 0 ) = B_stencil( i, 0 );
      stencil( i+A_size , 1 ) = B_stencil( i, 1 ) ;
    }

  tableau_trier_retirer_doublons( stencil );

  C.typer( "Matrice_Morse" );
  Matrice_Morse& C_ = ref_cast( Matrice_Morse, C.valeur( ) );

  allocate_morse_matrix( A.valeur( ).nb_lignes( ),
                         A.valeur( ).nb_colonnes( ),
                         stencil,
                         C_ );
}

void Matrix_tools::allocate_for_symmetric_scaled_addition( const Matrice& A,
                                                           const Matrice& B,
                                                           Matrice&       C )
{
  assert( A.valeur( ).nb_lignes( )   == A.valeur( ).nb_lignes( )   );
  assert( A.valeur( ).nb_lignes( )   == B.valeur( ).nb_lignes( )   );
  assert( A.valeur( ).nb_colonnes( ) == B.valeur( ).nb_colonnes( ) );

  IntTab A_stencil;
  A.valeur( ).get_symmetric_stencil( A_stencil );
  const int A_size = A_stencil.dimension( 0 );

  IntTab B_stencil;
  B.valeur( ).get_symmetric_stencil( B_stencil );
  const int B_size = B_stencil.dimension( 0 );

  int size = A_size + B_size;
  IntTab stencil;
  stencil.set_smart_resize( 1 );
  stencil.resize( size, 2 );

  for ( int i=0; i<A_size; ++i )
    {
      stencil( i , 0 ) = A_stencil( i, 0 );
      stencil( i , 1 ) = A_stencil( i, 1 ) ;
    }

  for ( int i=0; i<B_size; ++i )
    {
      stencil( i+A_size , 0 ) = B_stencil( i, 0 );
      stencil( i+A_size , 1 ) = B_stencil( i, 1 ) ;
    }

  tableau_trier_retirer_doublons( stencil );

  C.typer( "Matrice_Morse_Sym" );
  Matrice_Morse_Sym& C_ = ref_cast( Matrice_Morse_Sym, C.valeur( ) );

  allocate_symmetric_morse_matrix( A.valeur( ).nb_lignes( ),
                                   stencil,
                                   C_ );
}


// scaled addition
void Matrix_tools::add_scaled_matrices( const Matrice& A,
                                        const double&  alpha,
                                        const Matrice& B,
                                        const double&  beta,
                                        Matrice&       C )
{
  assert( sub_type( Matrice_Morse, C.valeur( ) ) );
  Matrice_Morse& C_ = ref_cast( Matrice_Morse, C.valeur( ) );

  assert( C_.check_sorted_morse_matrix_structure( ) );

  IntTab A_stencil;
  ArrOfDouble A_coefficients;
  A.valeur( ).get_stencil_and_coefficients( A_stencil,
                                            A_coefficients );

  IntTab B_stencil;
  ArrOfDouble B_coefficients;
  B.valeur( ).get_stencil_and_coefficients( B_stencil,
                                            B_coefficients );

  int A_k = 0;
  int B_k = 0;

  C_.get_set_coeff() = 0.0 ;

  const int nb_lines = C_.nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0   = C_.get_tab1()( i ) - 1;
      int k1   = C_.get_tab1()( i + 1 ) - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = C_.get_tab2()( k ) - 1;

          if ( ( A_k < A_stencil.dimension( 0 ) ) && ( A_stencil( A_k, 0 ) == i ) && ( A_stencil( A_k, 1 ) == j ) )
            {
              C_.get_set_coeff()( k ) += alpha * A_coefficients[ A_k ];
              ++A_k;
            }

          if ( ( B_k < B_stencil.dimension( 0 ) ) && ( B_stencil( B_k, 0 ) == i ) && ( B_stencil( B_k, 1 ) == j ) )
            {
              C_.get_set_coeff()( k ) += beta * B_coefficients[ B_k ];
              ++B_k;
            }
        }
    }

  assert( A_k == A_coefficients.size_array( ) );
  assert( B_k == B_coefficients.size_array( ) );
}

void Matrix_tools::add_symmetric_scaled_matrices( const Matrice& A,
                                                  const double&  alpha,
                                                  const Matrice& B,
                                                  const double&  beta,
                                                  Matrice&       C )
{
  assert( sub_type( Matrice_Morse_Sym, C.valeur( ) ) );
  Matrice_Morse_Sym& C_ = ref_cast( Matrice_Morse_Sym, C.valeur( ) );

  assert( C_.check_sorted_symmetric_morse_matrix_structure( ) );

  IntTab A_stencil;
  ArrOfDouble A_coefficients;
  A.valeur( ).get_symmetric_stencil_and_coefficients( A_stencil,
                                                      A_coefficients );

  IntTab B_stencil;
  ArrOfDouble B_coefficients;
  B.valeur( ).get_symmetric_stencil_and_coefficients( B_stencil,
                                                      B_coefficients );

  int A_k = 0;
  int B_k = 0;

  C_.get_set_coeff() = 0.0 ;

  const int nb_lines = C_.nb_lignes( );
  for ( int i=0; i<nb_lines; ++i )
    {
      int k0   = C_.get_tab1()( i ) - 1;
      int k1   = C_.get_tab1()( i + 1 ) - 1;

      for ( int k=k0; k<k1; ++k )
        {
          int j = C_.get_tab2()( k ) - 1;

          if ( ( A_k < A_stencil.dimension( 0 ) ) && ( A_stencil( A_k, 0 ) == i ) && ( A_stencil( A_k, 1 ) == j ) )
            {
              C_.get_set_coeff()( k ) += alpha * A_coefficients[ A_k ];
              ++A_k;
            }

          if ( ( B_k < B_stencil.dimension( 0 ) ) && ( B_stencil( B_k, 0 ) == i ) && ( B_stencil( B_k, 1 ) == j ) )
            {
              C_.get_set_coeff()( k ) += beta * B_coefficients[ B_k ];
              ++B_k;
            }
        }
    }

  assert( A_k == A_coefficients.size_array( ) );
  assert( B_k == B_coefficients.size_array( ) );
}

// stencil analysis
bool Matrix_tools::is_null_stencil( const IntTab& stencil )
{
  return ( stencil.dimension( 0 ) == 0 );
}

bool Matrix_tools::is_diagonal_stencil( const int& nb_lines,
                                        const int& nb_columns,
                                        const IntTab& stencil )
{
  if ( nb_lines != nb_columns )
    {
      return false;
    }

  const int size = stencil.dimension( 0 );

  if ( size == 0 )
    {
      return false;
    }

  for ( int i=0; i<size; ++i )
    {
      if ( stencil( i, 0 ) != stencil( i, 1 ) )
        {
          return false;
        }

      if ( stencil( i, 0 ) >= nb_lines )
        {
          return false;
        }

      if ( stencil( i, 1 ) >= nb_columns )
        {
          return false;
        }
    }

  return true;
}

void Matrix_tools::allocate_from_stencil( const int& nb_lines,
                                          const int& nb_columns,
                                          const IntTab& stencil,
                                          Matrice&      matrix ,
                                          const bool&   attach_stencil_to_matrix)
{
  if ( is_null_stencil( stencil ) )
    {
      matrix.typer( "Matrice_Nulle" );
      Matrice_Nulle& matrix_ = ref_cast( Matrice_Nulle, matrix.valeur( ) );
      matrix_.dimensionner( nb_lines, nb_columns );
    }
  else if ( is_diagonal_stencil( nb_lines,
                                 nb_columns,
                                 stencil ) )
    {
      matrix.typer( "Matrice_Diagonale" );
      Matrice_Diagonale& matrix_ = ref_cast( Matrice_Diagonale, matrix.valeur( ) );
      matrix_.dimensionner( nb_lines );
      if( attach_stencil_to_matrix )
        {
          matrix_.set_stencil( stencil );
        }
    }
  else
    {
      matrix.typer( "Matrice_Morse" );
      Matrice_Morse& matrix_ = ref_cast( Matrice_Morse, matrix.valeur( ) );
      allocate_morse_matrix( nb_lines,
                             nb_columns,
                             stencil,
                             matrix_,
                             attach_stencil_to_matrix );
    }
}

// extending a matrix's stencil
void Matrix_tools::extend_matrix_stencil( const IntTab& stencil,
                                          Matrice&      matrix ,
                                          const bool&   attach_stencil_to_matrix )
{
  if ( ! ( is_null_stencil( stencil ) ) )
    {
      const int nb_lines   = matrix.valeur( ).nb_lignes( );
      const int nb_columns = matrix.valeur( ).nb_colonnes( );

      IntTab full_stencil;
      matrix.valeur( ).get_stencil( full_stencil );
      full_stencil.set_smart_resize( 1 );
      const int size = stencil.dimension( 0 );

      const int old_size = full_stencil.size( ) / 2 ;

      full_stencil.resize( old_size + size, 2);

      for ( int i=0; i<size; ++i )
        {
          full_stencil( old_size + i , 0 ) = stencil( i, 0 );
          full_stencil( old_size + i , 1 ) = stencil( i, 1 );
        }

      tableau_trier_retirer_doublons( full_stencil );

      allocate_from_stencil( nb_lines,
                             nb_columns,
                             full_stencil,
                             matrix ,
                             attach_stencil_to_matrix );
    }
}



void Matrix_tools::matdiag_mult_matmorse( const DoubleTab& diag,
                                          Matrice_Morse& mat,
                                          const bool& inverse )
{
  const int nb_lignes = mat.nb_lignes( );
  const IntVect& tab1 = mat.get_tab1( );
  const IntVect& tab2 = mat.get_tab2( );
  int nnz_tot = 0;
  for( int i=0; i<nb_lignes; i++ )
    {
      const int nnz_i = tab1[ i+1 ] - tab1[ i ]; // nnz sur la ligne i
      for(int k=0; k<nnz_i; k++)
        {
          const int j = tab2[nnz_tot + k] - 1 ; // indice de la colonne
          double& coefficient = mat.coef( i, j );
          if( inverse )
            coefficient *= 1./diag[ i ] ;
          else
            coefficient *= diag[ i ] ;
        }
      nnz_tot += nnz_i;
    }
}

void Matrix_tools::matmorse_mult_matdiag( const DoubleTab& diag,
                                          Matrice_Morse& mat,
                                          const bool& inverse )
{
  const int nb_lignes = mat.nb_lignes( );
  const IntVect& tab1 = mat.get_tab1( );
  const IntVect& tab2 = mat.get_tab2( );
  int nnz_tot = 0;
  for( int i=0; i<nb_lignes; i++ )
    {
      const int nnz_i = tab1[ i+1 ] - tab1[ i ]; // nnz sur la ligne i
      for(int k=0; k<nnz_i; k++)
        {
          const int j = tab2[nnz_tot + k] - 1 ; // indice de la colonne
          double& coefficient = mat.coef( i, j );
          if( inverse )
            coefficient *= 1./diag[ j ] ;
          else
            coefficient *= diag[ j ] ;
        }
      nnz_tot += nnz_i;
    }
}


void Matrix_tools::uniform_matdiag_mult_matmorse( const double& diag,
                                                  Matrice_Morse& mat,
                                                  const bool& inverse )
{
  const int nb_lignes = mat.nb_lignes( );
  const IntVect& tab1 = mat.get_tab1( );
  const IntVect& tab2 = mat.get_tab2( );
  int nnz_tot = 0;
  for( int i=0; i<nb_lignes; i++ )
    {
      const int nnz_i = tab1[ i+1 ] - tab1[ i ]; // nnz sur la ligne i
      for(int k=0; k<nnz_i; k++)
        {
          const int j = tab2[nnz_tot + k] - 1 ; // indice de la colonne
          double& coefficient = mat.coef( i, j );
          if( inverse )
            coefficient *= 1./diag ;
          else
            coefficient *= diag ;
        }
      nnz_tot += nnz_i;
    }
}

void Matrix_tools::matmorse_mult_uniform_matdiag( const double& diag,
                                                  Matrice_Morse& mat,
                                                  const bool& inverse )
{
  uniform_matdiag_mult_matmorse( diag, mat, inverse );
}

void Matrix_tools::extend_matrix(Matrice_Morse& mat, int nl, int nc)
{

  IntVect& tab1 = mat.get_set_tab1(), old_tab1 = tab1;
  tab1.reset(), tab1.resize(nl + 1);
  for (int i = 0; i <= nl; i++) tab1(i) = old_tab1(std::min(i, old_tab1.size() - 1));
  mat.set_nb_columns(nc); //plus facile
}

