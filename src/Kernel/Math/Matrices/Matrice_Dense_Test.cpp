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
// File:        Matrice_Dense_Test.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <cppunit.h>
#ifdef cppunit_
#include <DoubleVect.h>
#include <TriouError.h>
#include <Matrice_Dense.h>
#include <SFichier.h>

class MatriceDenseTest: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( MatriceDenseTest );
  CPPUNIT_TEST( run_rectangle_matrix_read_from_file_test );
  CPPUNIT_TEST( run_square_matrix_built_from_coefficients );
  CPPUNIT_TEST_SUITE_END();


public:

  Matrice_Dense dense_matrix_;
  Matrice_Morse morse_matrix_;
  double threshold_ ;

  void setUp()
  {
    // init2();
  };
  MatriceDenseTest():CPPUNIT_NS::TestFixture::TestFixture()
  {
    threshold_ = 1e-13 ;
  };



  //here are tested :
  // - the reading of a dense matrix from a file
  // - access ( ) function
  // - the ajouter_multvect_ function
  // - the convert_to_morse_matrix function
  // - the scale function
  // there is 2 ways to check results, one with true result and on the other hand a
  // comparison between dense and morse version of the same matrix (results muste be the same)
  // there is a parameter threshold that can be adjust
  void run_rectangle_matrix_read_from_file_test( )
  {

    SFichier file ( "rectangle_dense_matrix.txt" );
    file << " 3 4 "<<finl;
    file << " 1    2  3.5   10   "<<finl;
    file << " 4.2  5  6     11.6 "<<finl;
    file << " 7.9  8  9.4  -12  "<<finl;

    dense_matrix_.read_from_file( "rectangle_dense_matrix.txt" );
    dense_matrix_.convert_to_morse_matrix( morse_matrix_ );

    int nb_cols = dense_matrix_.nb_colonnes( );
    int nb_lines = dense_matrix_.nb_lignes( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("function nb_colonnes is broken ",nb_cols, 4 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("function nb_lignes is broken ",nb_lines, 3 );

    //check access functions
    double coeff_35 = dense_matrix_( 0 , 2 );
    double diff = dabs( coeff_35 - 3.5 );
    int ok = ( diff < 1e-14 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("access function ( ) is broken ",ok, 1 );


    //check the ajouter_multvect_ function

    DoubleVect vector( nb_cols );
    for(int i=0; i<nb_cols; i++)
      {
        vector( i ) = i+1 ;
      }

    DoubleVect result_obtained_with_dense_matrix( nb_lines );
    DoubleVect result_obtained_with_morse_matrix( nb_lines );
    result_obtained_with_dense_matrix( 0 ) = 1;
    result_obtained_with_morse_matrix( 0 ) = 1;
    result_obtained_with_dense_matrix( 1 ) = 1;
    result_obtained_with_morse_matrix( 1 ) = 1;
    result_obtained_with_dense_matrix( 2 ) = 2;
    result_obtained_with_morse_matrix( 2 ) = 2;

    dense_matrix_.ajouter_multvect_( vector , result_obtained_with_dense_matrix );
    morse_matrix_.ajouter_multvect_( vector , result_obtained_with_morse_matrix );

    DoubleVect true_result( nb_lines );
    true_result( 0 ) = 56.5 ;
    true_result( 1 ) = 79.6 ;
    true_result( 2 ) = 6.1  ;

    DoubleVect error_dense_vs_morse( nb_lines );
    DoubleVect error( nb_lines );
    for(int i=0 ; i<nb_lines; i++)
      {
        error_dense_vs_morse( i ) = dabs( result_obtained_with_dense_matrix( i ) - result_obtained_with_morse_matrix( i )  );
        error( i ) = dabs( result_obtained_with_dense_matrix( i ) - true_result( i ) );
      }

    double norm_error_dense_vs_morse = error_dense_vs_morse.mp_norme_vect( );
    double norm_error = error.mp_norme_vect( );
    int error_is_ok = ( norm_error < threshold_ );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("The function ajouter_multvect_ is broken ",1,error_is_ok);
    error_is_ok = ( norm_error_dense_vs_morse < threshold_ );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There is a difference between results obtained using morse and dence matrices -- ajouter_multvect_ function -- ",1,error_is_ok);


    //now test the scale function
    dense_matrix_.scale( 0.5 );
    result_obtained_with_dense_matrix( 0 ) = 1;
    result_obtained_with_dense_matrix( 1 ) = 1;
    result_obtained_with_dense_matrix( 2 ) = 2;
    dense_matrix_.ajouter_multvect_( vector , result_obtained_with_dense_matrix );
    true_result( 0 ) =  28.7500 ;
    true_result( 1 ) =  40.3000 ;
    true_result( 2 ) =  4.0500  ;
    for(int i=0 ; i<nb_lines; i++)
      {
        error( i ) = dabs( result_obtained_with_dense_matrix( i ) - true_result( i ) );
      }

    norm_error = error.mp_norme_vect( );

    error_is_ok = ( norm_error < threshold_ );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("The function scale is broken ",1,error_is_ok);


  }


  // here we give the coefficients by hand
  // then we test :
  // - the build_matrix_from_coefficients_line_by_line function
  // - the ajouter_multvect_ function
  // - the matrix_from_coefficients_column_by_column function
  // - the build_the_transposed function
  // - the is_the_same function
  void run_square_matrix_built_from_coefficients( )
  {

    int nb_lines = 3;
    int nb_cols = 3;
    int nnz = 9;
    DoubleVect coeffs( nnz );
    //coeffs = 1 , 2 , ... , 9
    for(int i=0; i<nnz; i++)
      {
        coeffs( i ) = i+1;
      }

    //check build_matrix_from_coefficients_line_by_line function
    dense_matrix_.dimensionner( nb_lines , nb_cols );
    dense_matrix_.build_matrix_from_coefficients_line_by_line ( coeffs );

    DoubleVect vector( nb_cols );
    for(int i=0; i<nb_cols; i++)
      {
        vector( i ) = i+1 ;
      }

    DoubleVect result_obtained_with_dense_matrix( nb_lines );
    dense_matrix_.ajouter_multvect_( vector , result_obtained_with_dense_matrix );

    DoubleVect true_result( nb_lines );
    true_result( 0 ) = 14 ;
    true_result( 1 ) = 32 ;
    true_result( 2 ) = 50 ;
    DoubleVect error( nb_lines );
    for(int i=0 ; i<nb_lines; i++)
      {
        error( i ) = dabs( result_obtained_with_dense_matrix( i ) - true_result( i ) );
      }

    double norm_error = error.mp_norme_vect( );

    int error_is_ok = ( norm_error < threshold_   );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("The function build_matrix_from_coefficients_line_by_line is broken ",1,error_is_ok);

    //check is_the_same
    SFichier file1 ( "matrix1.txt" );
    file1 << " 3 3 "<<finl;
    file1 << " 1 2 3 "<<finl;
    file1 << " 0 5 9 "<<finl;
    file1 << " 8 9 2 "<<finl;
    SFichier file2 ( "matrix2.txt" );
    file2 << " 3 3 "<<finl;
    file2 << " 1 2 3.01 "<<finl;
    file2 << " 0 5 9 "<<finl;
    file2 << " 8 9 2 "<<finl;
    SFichier file3 ( "matrix3.txt" );
    file3 << " 3 3 "<<finl;
    file3 << " 1 2 3 "<<finl;
    file3 << " 0 5 9 "<<finl;
    file3 << " 8 9 2 "<<finl;
    Matrice_Dense matrix1,matrix2,matrix3;
    matrix1.read_from_file( "matrix1.txt" );
    matrix2.read_from_file( "matrix2.txt" );
    matrix3.read_from_file( "matrix3.txt" );
    int is_the_same = matrix1.is_the_same( matrix2 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("matrix1 is different from matrix2 ! so is_the_same function is broken ",0,is_the_same);
    is_the_same = matrix1.is_the_same( matrix3 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("matrix1 and matrix3 are the same ! so is_the_same function is broken ",1,is_the_same);

    //check build_matrix_from_coefficients_column_by_column and build_the_transposed
    Matrice_Dense transposed_matrix;
    dense_matrix_.build_the_transposed( transposed_matrix );
    dense_matrix_.build_matrix_from_coefficients_column_by_column ( coeffs );

    is_the_same = dense_matrix_.is_the_same( transposed_matrix );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("The function build_matrix_from_coefficients_column_by_column or build_the_transposed is broken ",1,is_the_same);

  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( MatriceDenseTest );

#endif


