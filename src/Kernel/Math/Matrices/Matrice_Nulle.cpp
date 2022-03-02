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
// File:        Matrice_Nulle.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Nulle.h>
#include <IntTab.h>
#include <TRUSTArray.h>

Implemente_instanciable_sans_constructeur( Matrice_Nulle, "Matrice_Nulle", Matrice_Base ) ;

Sortie& Matrice_Nulle::printOn( Sortie& os ) const
{
  os << nb_lines_   << finl;
  os << nb_columns_ << finl;
  return os;
}

Entree& Matrice_Nulle::readOn( Entree& is )
{
  is >> nb_lines_;
  is >> nb_columns_;
  return is;
}

int Matrice_Nulle::ordre() const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );

  if ( nb_lines_ == nb_columns_ )
    {
      return nb_lines_;
    }
  return -1;
}

int Matrice_Nulle::nb_lignes() const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return nb_lines_;
}

int Matrice_Nulle::nb_colonnes() const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return nb_columns_;
}

DoubleVect& Matrice_Nulle::ajouter_multvect_(const DoubleVect& x,
                                             DoubleVect&       r) const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return r;
}

DoubleVect& Matrice_Nulle::ajouter_multvectT_(const DoubleVect& x,
                                              DoubleVect&       r) const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return r;
}

DoubleTab& Matrice_Nulle::ajouter_multTab_(const DoubleTab& x,
                                           DoubleTab&       r) const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return r;
}

void Matrice_Nulle::scale( const double& x )
{
  // nothing to do
}

void Matrice_Nulle::get_stencil( IntTab& stencil ) const
{
  stencil.resize( 0, 2 );
}

void Matrice_Nulle::get_symmetric_stencil( IntTab& stencil ) const
{
  if ( nb_lines_ != nb_columns_ )
    {
      Cerr << "Error in 'Matrice_Nulle::get_symmetric_stencil( )':" << finl;
      Cerr << "  Matrix is not square." << finl;
      Cerr << "  nb_lines   = " << nb_lines_ << finl;
      Cerr << "  nb_columns = " << nb_columns_ << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  get_stencil( stencil );
}

void Matrice_Nulle::get_stencil_and_coefficients( IntTab&      stencil,
                                                  ArrOfDouble& coefficients ) const
{
  stencil.resize( 0, 2 );
  coefficients.resize( 0 );
}

void Matrice_Nulle::get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                            ArrOfDouble& coefficients ) const
{
  if ( nb_lines_ != nb_columns_ )
    {
      Cerr << "Error in 'Matrice_Nulle::get_symmetric_stencil_and_coefficients( )':" << finl;
      Cerr << "  Matrix is not square." << finl;
      Cerr << "  nb_lines   = " << nb_lines_ << finl;
      Cerr << "  nb_columns = " << nb_columns_ << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
  get_stencil_and_coefficients( stencil,
                                coefficients );
}

Matrice_Nulle::Matrice_Nulle( void ) : Matrice_Base( )
{
  nb_lines_   = -1;
  nb_columns_ = -2;
}

Matrice_Nulle::Matrice_Nulle( int order ) : Matrice_Base( )
{
  assert( order > 0 );
  nb_lines_   = order;
  nb_columns_ = order;
}

Matrice_Nulle::Matrice_Nulle( int nb_lines, int nb_columns ) : Matrice_Base( )
{
  assert( nb_lines   > 0 );
  assert( nb_columns > 0 );
  nb_lines_ = nb_lines;
  nb_columns_ = nb_columns;
}

void Matrice_Nulle::dimensionner( int order )
{
  assert( order > 0 );
  nb_lines_   = order;
  nb_columns_ = order;
}

void Matrice_Nulle::dimensionner( int nb_lines, int nb_columns )
{
  assert( nb_lines   > 0 );
  assert( nb_columns > 0 );
  nb_lines_ = nb_lines;
  nb_columns_ = nb_columns;
}

double Matrice_Nulle::coeff( int i, int j ) const
{
  assert( nb_lines_   > 0 );
  assert( nb_columns_ > 0 );
  return 0.0;
}
