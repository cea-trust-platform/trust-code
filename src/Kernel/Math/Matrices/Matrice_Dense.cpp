/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Matrice_Dense.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/62
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Dense.h>
#include <fstream>
#include <iostream>
#include <Lapack.h>

Implemente_instanciable_sans_constructeur(Matrice_Dense,"Matrice_Dense",Matrice_Base);

Sortie& Matrice_Dense::printOn(Sortie& s) const
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  // s << nb_lines << " "<< nb_cols << "\n";
  for(int i=0; i<nb_lines; i++)
    {
      for( int j=0; j<nb_cols; j++)
        {
          s << Matrix_( i , j )<<" ";
        }
      s << "\n";
    }
  return s;
}

Entree& Matrice_Dense::readOn(Entree& s)
{
  return s;
}


Sortie& Matrice_Dense::imprimer_formatte( Sortie& s ) const
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  s << nb_lines << " ";
  s << nb_cols << "\n\n";
  for(int i=0; i<nb_lines; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          s << Matrix_( i , j )<<" ";
        }
      s <<"\n";
    }
  return s;
}


Matrice_Dense::Matrice_Dense( void )
{
  dimensionner( 0 , 0 );
}

Matrice_Dense::Matrice_Dense( const int& nb_lines , const int& nb_cols )
{
  dimensionner( nb_lines , nb_cols );
}


//This function allow to build a dense matrix from a file
//the format expected is the following :
// nb_lines  nb_columns
// first  line values ...
// second line values ...
//       ...
// last   line values ...
void Matrice_Dense::read_from_file( const Nom& filename )
{
  std::ifstream file(filename, ios::in);
  int nb_lines, nb_cols;

  if( file )
    {
      file >> nb_lines ;
      file >> nb_cols  ;
      dimensionner( nb_lines , nb_cols );
      for( int i=0; i<nb_lines; i++)
        {
          for(int j=0; j<nb_cols; j++)
            {
              file >> Matrix_( i , j );
            }
        }
    }
  else
    {
      Cerr<<"Error in Matrice_Dense::read_from_file"<<finl;
      Cerr<<"The file "<<filename<<" was not found or a problem occured while opening the file"<<finl;
      Cerr<<"Aborting..."<<finl;
      Process::abort( );
    }
}


//input : a vector of coefficients organized lines by lines
//for example, let's say that we have nbl lines and nbc columns
//then the first nbc coefficients are coefficients associated to the first line of the matrix
//then the next nbc coefficients are coefficients associated to the second line of the matrix, ect...
//Warning : the user need to resize correctly the matrix before to call this function
void Matrice_Dense::build_matrix_from_coefficients_line_by_line( const DoubleVect& coefficients )
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  assert( coefficients.size( ) == nb_lines * nb_cols );
  dimensionner( nb_lines , nb_cols );
  int counter=0;
  for( int i=0; i<nb_lines; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          Matrix_( i , j ) = coefficients( counter );
          counter++;
        }
    }
}

void Matrice_Dense::build_matrix_from_coefficients_column_by_column( const DoubleVect& coefficients )
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  assert( coefficients.size( ) == nb_lines * nb_cols );
  dimensionner( nb_lines , nb_cols );
  int counter=0;
  for(int j=0; j<nb_cols; j++)
    {
      for( int i=0; i<nb_lines; i++)
        {
          Matrix_( i , j ) = coefficients( counter );
          counter++;
        }
    }
}

//this function fills the matrix morse_matrix
void Matrice_Dense::convert_to_morse_matrix( Matrice_Morse& morse_matrix ) const
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( ) ;
  int nnz = nb_lines * nb_cols ;

  morse_matrix.dimensionner( nb_lines , nb_cols , nnz );

  int size_tab1 = nb_lines + 1 ;
  int size_tab2 = nnz ;
  morse_matrix.get_set_tab1().resize( size_tab1 );
  morse_matrix.get_set_tab2().resize( size_tab2 );
  morse_matrix.get_set_coeff().resize( nnz );

  for(int i=0; i<size_tab1; i++)
    {
      morse_matrix.get_set_tab1()( i ) =  /*already registred*/i*nb_cols + /*fortran index*/ 1 ;
    }
  int count = 0;
  for(int i=0 ; i<nb_lines ; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          morse_matrix.get_set_tab2()( count ) = j+1 ;
          morse_matrix.get_set_coeff()( count ) = Matrix_( i , j );
          count++;
        }
    }
}


void Matrice_Dense::dimensionner( const int& nb_lines , const int& nb_cols )
{
  Matrix_.resize( nb_lines , nb_cols );
}

int Matrice_Dense::nb_lignes( ) const
{
  return Matrix_.dimension( 0 );
}

int Matrice_Dense::nb_colonnes( ) const
{
  return Matrix_.dimension( 1 );
}

int Matrice_Dense::ordre( ) const
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  if( nb_lines == nb_cols )
    {
      return nb_lines ;
    }
  else
    {
      return 0 ;
    }
}

// Description:
//    Operation de multiplication-accumulation (saxpy) matrice vecteur.
//    Operation: resu = resu + Matrix_ * x
DoubleVect& Matrice_Dense::ajouter_multvect_( const DoubleVect& x , DoubleVect& resu ) const
{
  const int nb_lines = nb_lignes( ) ;
  const int nb_cols  = nb_colonnes( );
  assert( x.size_array( ) == nb_cols );
  //the test is in this order because size( ) maybe invalid
  assert( resu.size_array( ) == nb_lines || resu.size( ) == nb_lines );

  //for a given i such that 0 <= i <= nb_lines we have
  //resu( i ) = resu( i ) + Matrix_( i , j ) * x( j )
  //with 0 <= j <= nb_cols
  for(int i=0; i<nb_lines; i++)
    {
      double old_resu = resu( i );
      double prod_mat_vect = 0; // store the result of Matrix_( i , j ) * x( j ) with 0<= j <=nb_cols
      for(int j=0; j<nb_cols; j++)
        {
          prod_mat_vect += Matrix_( i , j ) * x( j );
        }
      resu( i ) = old_resu + prod_mat_vect ;
    }

  return resu;
}

//set the coefficient Matrix( i , j ) at value
void Matrice_Dense::set_coefficient( const int& i , const int& j , const double& value )
{
  assert( i < nb_lignes( ) );
  assert( j < nb_colonnes( ) );
  Matrix_( i , j ) = value;
}

void Matrice_Dense::build_the_transposed( Matrice_Dense& transposed ) const
{
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  transposed.dimensionner( nb_lines , nb_cols );
  for(int j=0; j<nb_cols; j++)
    {
      for(int i=0; i<nb_lines; i++)
        {
          double value = Matrix_( i , j );
          transposed.set_coefficient( j , i , value ) ;
        }
    }
}

// return a bool which indicates if the other matrix is the same or not
// return true  : Matrix_ and other_matrix are the same for a given tolerance tol
// return false :  Matrix_ and other_matrix are NOT the same for a given tolerance tol
bool Matrice_Dense::is_the_same( const Matrice_Dense& other_matrix , const double& tol ) const
{
  bool same = true;
  int nb_lines = nb_lignes( );
  int nb_cols  = nb_colonnes( );
  //check if the other has same number of lines/columns
  if( other_matrix.nb_lignes( ) != nb_lines )
    {
      same = false;
    }
  if( other_matrix.nb_colonnes( ) != nb_cols )
    {
      same = false;
    }
  //check each coefficient
  for(int i=0; i<nb_lines; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          if( dabs( other_matrix( i , j ) - Matrix_( i , j )) >= tol  )
            {
              same = false;
            }
        }
    }
  return same;
}

// Description:
//    Operation de multiplication-accumulation (saxpy) matrice vecteur,
//    par la matrice transposee.
//    Operation: resu = resu + A^{T}*x
DoubleVect& Matrice_Dense::ajouter_multvectT_(const DoubleVect& x,DoubleVect& resu) const
{

  const int nb_lines = nb_lignes( ) ;
  const int nb_cols  = nb_colonnes( );
  assert( x.size_array( ) == nb_lines );
  //the test is in this order because size( ) maybe invalid
  assert( resu.size_array( ) == nb_cols || resu.size( ) == nb_cols );

  //for a given i such that 0 <= i <= nb_cols we have
  //resu( i ) = resu( i ) + Matrix_( j , i ) * x( j )
  //with 0 <= j <= nb_lines
  for(int i=0; i<nb_cols; i++)
    {
      double old_resu = resu( i );
      double prod_mat_vect = 0;
      for(int j=0; j<nb_lines; j++)
        {
          prod_mat_vect += Matrix_( j , i ) * x( j );
        }
      resu( i ) = old_resu + prod_mat_vect ;
    }

  return resu;
}

void Matrice_Dense::scale( const double& x )
{
  const int nb_lines = nb_lignes( );
  const int nb_cols  = nb_colonnes( );
  for(int i=0; i<nb_lines; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          Matrix_( i , j ) *= x ;
        }
    }
}



void Matrice_Dense::get_stencil( IntTab& stencil ) const
{

  const int nb_lines = nb_lignes( );
  const int nb_cols  = nb_colonnes( );

  stencil.resize( 0, 2 );
  stencil.set_smart_resize( 1 );

  for(int i=0; i<nb_lines; i++)
    {
      for(int j=0; j<nb_cols; j++)
        {
          stencil.append_line( i , j );
        }
    }

  const int new_size = stencil.dimension( 0 );
  stencil.set_smart_resize( 0 );
  stencil.resize( new_size, 2 );

}


DoubleTab& Matrice_Dense::ajouter_multTab_( const DoubleTab& x , DoubleTab& resu ) const
{
  Cerr<<"Error in Matrice_Dense::ajouter_multTab_ ."<<finl;
  Cerr<<"This function is virtual pure in Matrice_Base but seems never used."<<finl;
  Cerr<<"One should get rid off it."<<finl;
  Cerr<<"Aborting..."<<finl;
  Process::abort( );
  return resu;
}

void Matrice_Dense::inverse()
{
  // This method compute the inverse of the matrix.
  // It uses the LAPACK library.
  const int nbLines = nb_lignes();
  const int nbCols = nb_colonnes();
  int info = 0;
  ArrOfInt ipiv(nbLines);
  ArrOfDouble work(nbLines);

  if (nbLines != nbCols)
    {
      Cerr << "Error in Matrice_Dense::inverse" << finl;
      Cerr << "The matrix is not a square matrix !" << finl;
      Process::abort( );
    }

  F77NAME(DGETRF)(&nbLines, &nbLines, Matrix_.addr(), &nbLines, ipiv.addr(), &info);
  assert(info == 0);
  F77NAME(DGETRI)(&nbLines, Matrix_.addr(), &nbLines, ipiv.addr(), work.addr(), &nbLines, &info);
  assert(info == 0);

  return;
}

void Matrice_Dense::multiplyToRight(const Matrice_Dense& B, Matrice_Dense& RES) const
{
  // This method allows to perform the multiplication of two matrix.
  // It is the "right" multiplication : (*this) * B = RES

  assert(nb_colonnes() == B.nb_lignes());
  assert(nb_lignes() == RES.nb_lignes());
  assert(B.nb_colonnes() == RES.nb_colonnes());

  for (int i = 0; i < nb_lignes(); ++i)
    {
      for (int j = 0; j < B.nb_colonnes(); ++j)
        {
          RES.set_coefficient(i, j, 0.0);
          for (int k = 0; k < nb_colonnes(); ++k)
            {
              RES.set_coefficient(i, j, RES(i, j) + Matrix_(i, k) * B(k, j));
            }
        }
    }
  return;
}
