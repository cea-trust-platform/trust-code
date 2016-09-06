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
// File:        Matrice_Diagonale.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Diagonale.h>
#include <IntTab.h>

Implemente_instanciable_sans_constructeur(Matrice_Diagonale,"Matrice_Diagonale",Matrice_Base);

Sortie& Matrice_Diagonale::printOn( Sortie& os ) const
{
  os << coefficients_ << finl;
  return os;
}

Entree& Matrice_Diagonale::readOn( Entree& is )
{
  is >> coefficients_;
  return is;
}

int Matrice_Diagonale::ordre() const
{
  return coefficients_.size();
}

int Matrice_Diagonale::nb_lignes( void ) const
{
  return coefficients_.size( );
}

int Matrice_Diagonale::nb_colonnes( void ) const
{
  return coefficients_.size( );
}

DoubleVect& Matrice_Diagonale::ajouter_multvect_( const DoubleVect& x, DoubleVect& r ) const
{
  int count = coefficients_.size( );

  assert( x.size( ) ==count );
  assert( r.size( ) ==count );

  register const double* c_ptr = coefficients_.addr( );
  register const double* x_ptr = x.addr( );
  register       double* r_ptr = r.addr( );

  for ( ; count; count-- )
    {
      const double x_ = *x_ptr;
      const double c_ = *c_ptr;
      double& r_ = *( r_ptr++ );
      r_ += c_ * x_;
      x_ptr++;
    }

  return r;
}

DoubleVect& Matrice_Diagonale::ajouter_multvectT_( const DoubleVect& x, DoubleVect& r ) const
{
  return ajouter_multvect_( x, r );
}

DoubleTab& Matrice_Diagonale::ajouter_multTab_( const DoubleTab& x, DoubleTab& r ) const
{
  if ( ( x.nb_dim() == 1 ) && ( r.nb_dim() == 1 ) )
    {
      ajouter_multvect_( x, r );
      return r;
    }

  assert( x.nb_dim( ) == 2 );
  assert( r.nb_dim( ) == 2 );

  const int size = coefficients_.size( );
  assert( x.dimension( 0 ) == size );
  assert( r.dimension( 0 ) == size );

  const int nb_components = x.dimension( 1 );
  assert( r.dimension( 1 ) == nb_components );

  for ( int i=0; i<size; ++i )
    {
      const double coefficient = coefficients_( i );
      for ( int j=0; j<nb_components; ++j )
        {
          r( i, j ) += x( i, j ) * coefficient;
        }
    }

  return r;
}

void Matrice_Diagonale::scale( const double& x )
{
  coefficients_ *= x;
}

void Matrice_Diagonale::get_stencil( IntTab& stencil ) const
{
  const int size = ordre( );

  stencil.resize( size, 2 );

  for ( int i=0; i<size; ++i )
    {
      stencil( i, 0 ) = i;
      stencil( i, 1 ) = i;
    }
}

void Matrice_Diagonale::get_symmetric_stencil( IntTab& stencil ) const
{
  get_stencil( stencil );
}

void Matrice_Diagonale::get_stencil_and_coefficients( IntTab&      stencil,
                                                      ArrOfDouble& coefficients ) const
{
  const int size = ordre( );

  stencil.resize( size, 2 );
  coefficients.resize_array( size );

  for ( int i=0; i<size; ++i )
    {
      stencil( i, 0 ) = i;
      stencil( i, 1 ) = i;
      coefficients[ i ] = coefficients_( i );
    }
}

void Matrice_Diagonale::get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                                ArrOfDouble& coefficients ) const
{
  get_stencil_and_coefficients( stencil,
                                coefficients );
}

Matrice_Diagonale::Matrice_Diagonale( void ) : Matrice_Base( ), coefficients_( )
{
  is_stencil_up_to_date_ = false ;
}

Matrice_Diagonale::Matrice_Diagonale( int size ) : Matrice_Base( ), coefficients_( size )
{
  is_stencil_up_to_date_ = false ;
}

Matrice_Diagonale::Matrice_Diagonale(const DoubleVect& coefficients) : Matrice_Base( ), coefficients_( coefficients )
{
  is_stencil_up_to_date_ = false ;
}

DoubleVect& Matrice_Diagonale::get_coefficients( void )
{
  return coefficients_;
}

const DoubleVect& Matrice_Diagonale::get_coefficients( void ) const
{
  return coefficients_;
}

void Matrice_Diagonale::dimensionner( int size )
{
  coefficients_.resize( size );
}

double  Matrice_Diagonale::coeff( int i, int j ) const
{
  if ( i==j )
    {
      return coefficients_[ i ];
    }
  else
    {
      return 0.0;
    }
}

double& Matrice_Diagonale::coeff( int i, int j )
{
  if ( i==j )
    {
      return coefficients_[ i ];
    }

  Cerr << "Error in 'Matrice_Diagonale::coeff()':" << finl;
  Cerr << "  non-const access is only allowed for diagonal coefficients" << finl;
  Process::exit( );

  return coefficients_[ 0];
}
