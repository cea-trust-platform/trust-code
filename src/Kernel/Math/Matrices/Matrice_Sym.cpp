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
// File:        Matrice_Sym.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Sym.h>
#include <DoubleTrav.h>
#include <IntTab.h>

int Matrice_Sym::get_est_definie() const
{
  return est_definie_;
}

void Matrice_Sym::set_est_definie(int val)
{
  est_definie_ = val;
}

void Matrice_Sym::unsymmetrize_stencil( const int& nb_lines,
                                        const IntTab& symmetric_stencil,
                                        IntTab&       stencil ) const
{

  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  const int symmetric_stencil_size = symmetric_stencil.dimension( 0 );
  for ( int k=0; k<symmetric_stencil_size; ++k )
    {
      const int line   = symmetric_stencil( k, 0 );
      const int column = symmetric_stencil( k, 1 );
      offsets[ line   + 1 ] += 1;
      offsets[ column + 1 ] += ( line == column ) ? 0 : 1 ;
    }

  for ( int i=0; i<nb_lines; ++i )
    {
      offsets[ i + 1 ] += offsets[ i ];
    }

  const int stencil_size = offsets[ nb_lines ];
  stencil.resize( stencil_size, 2 );

  stencil = -1;

  int index;
  for ( int k=0; k<symmetric_stencil_size; ++k )
    {
      const int line   = symmetric_stencil( k, 0 );
      const int column = symmetric_stencil( k, 1 );


      index = offsets[ line ];

      assert( stencil( index, 0 ) < 0 );
      assert( stencil( index, 1 ) < 0 );
      assert( index < offsets[ line + 1 ] );

      stencil( index, 0 ) = line;
      stencil( index, 1 ) = column;

      offsets[ line ] += 1;

      if ( line != column )
        {
          index = offsets[ column ];

          assert( stencil( index, 0 ) < 0 );
          assert( stencil( index, 1 ) < 0 );
          assert( index < offsets[ column + 1 ] );

          stencil( index, 0 ) = column;
          stencil( index, 1 ) = line;

          offsets[ column ] += 1;
        }
    }
}

void Matrice_Sym::unsymmetrize_stencil_and_coefficients( const int&      nb_lines,
                                                         const IntTab&      symmetric_stencil,
                                                         const ArrOfDouble& symmetric_coefficients,
                                                         IntTab&            stencil,
                                                         ArrOfDouble&       coefficients ) const
{
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  const int symmetric_stencil_size = symmetric_stencil.dimension( 0 );
  assert( symmetric_stencil_size == symmetric_coefficients.size_array( ) );
  for ( int k=0; k<symmetric_stencil_size; ++k )
    {
      const int line   = symmetric_stencil( k, 0 );
      const int column = symmetric_stencil( k, 1 );
      offsets[ line   + 1 ] += 1;
      offsets[ column + 1 ] += ( line == column ) ? 0 : 1 ;
    }

  for ( int i=0; i<nb_lines; ++i )
    {
      offsets[ i + 1 ] += offsets[ i ];
    }

  const int stencil_size = offsets[ nb_lines ];
  stencil.resize( stencil_size, 2 );
  coefficients.resize_array( stencil_size );

  stencil = -1;

  int index;
  for ( int k=0; k<symmetric_stencil_size; ++k )
    {
      const int line        = symmetric_stencil( k, 0 );
      const int column      = symmetric_stencil( k, 1 );
      const double coefficient = symmetric_coefficients[ k ];

      index = offsets[ line ];

      assert( stencil( index, 0 ) < 0 );
      assert( stencil( index, 1 ) < 0 );
      assert( index < offsets[ line + 1 ] );

      stencil( index, 0 )   = line;
      stencil( index, 1 )   = column;
      coefficients[ index ] = coefficient;

      offsets[ line ] += 1;

      if ( line != column )
        {
          index = offsets[ column ];

          assert( stencil( index, 0 ) < 0 );
          assert( stencil( index, 1 ) < 0 );
          assert( index < offsets[ column + 1 ] );

          stencil( index, 0 )   = column;
          stencil( index, 1 )   = line;
          coefficients[ index ] = coefficient;

          offsets[ column ] += 1;
        }
    }
}
