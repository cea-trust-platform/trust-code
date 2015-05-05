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
// File:        Matrice_Nulle.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Matrice_Nulle_included
#define Matrice_Nulle_included

#include <Matrice_Base.h>

/////////////////////////////////////////////////////////////////////////////
//
// .NAME        : Matrice_Nulle
// .HEADER      : linalg linalg/src/Kernel/Math/Matrices
// .LIBRARY     : liblinalg
// .DESCRIPTION : class Matrice_Nulle
//
// <Description of class Matrice_Nulle>
//
/////////////////////////////////////////////////////////////////////////////

class IntTab;
class ArrOfDouble;

class Matrice_Nulle : public Matrice_Base
{

  Declare_instanciable_sans_constructeur( Matrice_Nulle ) ;

public :
  virtual int ordre( ) const;
  virtual int nb_lignes( ) const;
  virtual int nb_colonnes( ) const;

  virtual DoubleVect& ajouter_multvect_(const DoubleVect& x,
                                        DoubleVect&       r) const;

  virtual DoubleVect& ajouter_multvectT_(const DoubleVect& x,
                                         DoubleVect&       r) const;

  virtual DoubleTab& ajouter_multTab_(const DoubleTab& x,
                                      DoubleTab&       r) const;

  virtual void scale( const double& x );

  virtual void get_stencil( IntTab& stencil ) const;

  virtual void get_symmetric_stencil( IntTab& stencil ) const;

  virtual void get_stencil_and_coefficients( IntTab&      stencil,
                                             ArrOfDouble& coefficients ) const;

  virtual void get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                       ArrOfDouble& coefficients ) const;

public:
  Matrice_Nulle( void );
  Matrice_Nulle( int order );
  Matrice_Nulle( int nb_lines,
                 int nb_columns );

  void dimensionner( int order );
  void dimensionner( int nb_lines,
                     int nb_columns );

  double coeff( int i,
                int j ) const;

protected :
  int nb_lines_;
  int nb_columns_;

};

#endif /* Matrice_Nulle_included */
