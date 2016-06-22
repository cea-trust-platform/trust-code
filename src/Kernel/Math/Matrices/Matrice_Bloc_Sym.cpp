/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Matrice_Bloc_Sym.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Matrice_Bloc_Sym.h>
#include <Matrice_Base.h>
#include <Matrix_tools.h>
#include <Vect_IntTab.h>
#include <Vect_ArrOfDouble.h>
#include <Matrice_Morse_Sym.h>

Implemente_instanciable_sans_constructeur(Matrice_Bloc_Sym,"Matrice_Bloc_Sym",Matrice_Bloc);

Sortie& Matrice_Bloc_Sym::printOn( Sortie& os ) const
{
  return Matrice_Bloc::printOn(os);
}

Entree& Matrice_Bloc_Sym::readOn( Entree& is )
{
  return Matrice_Bloc::readOn(is);
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*     Implementation des fonctions membres de la classe Matrice_Bloc_Sym       */
/*                                                                          */
/*--------------------------------------------------------------------------*/

Matrice_Bloc_Sym::Matrice_Bloc_Sym(int N, int M) : Matrice_Bloc( )
{
  dimensionner(N,M);
}

// **********************************
Sortie& Matrice_Bloc_Sym::imprimer(Sortie& os) const
{
  int i,j;
  for(i=0; i<N_; i++)
    for(j=i; j<M_; j++)
      (*this).get_bloc(i,j).valeur().imprimer(os);
  return os;
}

Sortie& Matrice_Bloc_Sym::imprimer_formatte(Sortie& os) const
{
  for(int i=0; i<N_; i++)
    {
      for(int j=i; j<M_; j++)
        {
          os<<"----------"<<finl;
          os<<"Bloc ("<<i<<","<<j<<"):"<<finl;
          os<<"----------"<<finl;
          (*this).get_bloc(i,j).valeur().imprimer_formatte(os);
        }
    }
  return os;
}

//Produit matrice-vecteur
DoubleVect& Matrice_Bloc_Sym::ajouter_multvect_(const DoubleVect& x, DoubleVect& y) const
{
  const double * const_x_addr = x.addr();
  double* x_addr = (double *) const_x_addr;
  double* y_addr = y.addr();
  // Si on met static la methode ne fonctionne pas pour une Matrice_Bloc_Sym de Matrice_Bloc_Sym
  // static DoubleVect xtmp;
  // static DoubleVect ytmp;

  DoubleVect xtmp;
  DoubleVect ytmp;

  int nbr_lignes;
  int nbr_colonnes;
  int decaly=0;
  for(int i=0; i<N_; i++)
    {
      int decalx=0;
      nbr_lignes=0;
      for(int j=0; j<i; j++)
        {
          const Matrice_Base& mij=get_bloc(j,i).valeur();
          nbr_colonnes=mij.nb_lignes();
          decalx+=nbr_colonnes;
        }
      for(int j=i; j<N_; j++)
        {
          const Matrice_Base& mij=get_bloc(i,j).valeur();
          if(!nbr_lignes) nbr_lignes=mij.nb_lignes();
          nbr_colonnes=mij.nb_colonnes();
          if (nbr_colonnes>0 && nbr_lignes>0)
            {
              xtmp.ref_data(x_addr+decalx,nbr_colonnes);
              assert((decalx+nbr_colonnes)<=x.size_array());
              ytmp.ref_data(y_addr+decaly,nbr_lignes);
              assert((decaly+nbr_lignes)<=y.size_array());
              mij.ajouter_multvect_(xtmp,ytmp);
              if(i!=j)
                {
                  xtmp.ref_data(x_addr+decaly,nbr_lignes);
                  assert((decaly+nbr_lignes)<=x.size_array());
                  ytmp.ref_data(y_addr+decalx,nbr_colonnes);
                  assert((decalx+nbr_colonnes)<=y.size_array());
                  mij.ajouter_multvectT_(xtmp,ytmp);
                }
              decalx+=nbr_colonnes;
            }
        }
      decaly+=nbr_lignes;
    }
  return y;
}

//Produit matrice transposee-vecteur
DoubleVect& Matrice_Bloc_Sym::ajouter_multvectT_(const DoubleVect& x, DoubleVect& y) const
{
  Cerr <<"Matrice_Bloc_Sym::ajouter_multvectT_ is not coded"<<finl<<flush;
  exit();
  return y;
}

//Produit matrice representee par un tableau-matrice
DoubleTab& Matrice_Bloc_Sym::ajouter_multTab_(const DoubleTab& x, DoubleTab& y) const
{
  Cerr <<"Matrice_Bloc_Sym::ajouter_multTab_ is not coded"<<finl<<flush;
  exit();
  return y;
}

void Matrice_Bloc_Sym::BlocSymToMatMorseSym(Matrice_Morse_Sym& mat) const
{
  if ( Process::je_suis_maitre( ) )
    {
      Cerr << "Conversion BlocSymToMatMorseSym..." << finl;
    }

  Matrix_tools::convert_to_symmetric_morse_matrix( (*this), mat );
  mat.set_est_definie( get_est_definie( ) );

  // int n=nb_lignes();

  // // Dimensionnement et initialisation d'une matrice Morse
  // Matrice_Morse mat_morse;
  // mat_morse.dimensionner(n,n,0);
  // mat_morse.tab1_=1;

  // // Remplissage de cette matrice en sommant les contributions de chaque bloc
  // Matrice_Morse contribution;
  // int ideb=0;

  // for (int ibloc=0; ibloc<nb_bloc_lignes(); ibloc++)
  //   {
  //     int nb_lignes=0;
  //     int jdeb=0;
  //     for (int jbloc=0; jbloc<ibloc; jbloc++)
  //       {
  //         if (!sub_type(Matrice_Bloc,get_bloc(jbloc,ibloc).valeur()))
  //           {
  //             Cerr << "Matrice_Bloc_Sym::BlocSymToMatMorseSym applies only to a Matrice_Bloc_Sym type matrix" << finl;
  //             Cerr << "constituted of Matrice_Bloc type matrixes." << finl;
  //             exit();
  //           }
  //         // Pour calculer le nombre de colonnes du bloc(j,i), on prend le nombre de lignes du bloc transpose bloc(i,j)
  //         const Matrice_Bloc& bloc = ref_cast(Matrice_Bloc,get_bloc(jbloc,ibloc).valeur());
  //         jdeb += bloc.nb_lignes();
  //       }
  //     for (int jbloc=ibloc; jbloc<nb_bloc_colonnes(); jbloc++)
  //       {
  //         if (!sub_type(Matrice_Bloc,get_bloc(ibloc,jbloc).valeur()))
  //           {
  //             Cerr << "Matrice_Bloc_Sym::BlocSymToMatMorseSym applies only to a Matrice_Bloc_Sym type matrix" << finl;
  //             Cerr << "constituted of Matrice_Bloc type matrixes." << finl;
  //             exit();
  //           }
  //         const Matrice_Bloc& bloc = ref_cast(Matrice_Bloc,get_bloc(ibloc,jbloc).valeur());
  //         // Transforme la Matrice_Bloc bloc en Matrice_Morse bloc_en_morse
  //         Matrice_Morse bloc_en_morse;
  //         bloc.BlocToMatMorse(bloc_en_morse);
  //         // Remplissage de la matrice contribution
  //         contribution.remplir(ideb, jdeb, n, n, bloc_en_morse);
  //         if (jbloc==ibloc) nb_lignes = bloc.nb_lignes();
  //         assert(nb_lignes==bloc_en_morse.nb_lignes());
  //         assert(nb_lignes==bloc.nb_lignes());
  //         jdeb += bloc.nb_colonnes();
  //         mat_morse += contribution;
  //       }
  //     ideb+=nb_lignes;
  //   }
  // // Remplissage de la Matrice_Morse_Symetrique
  // mat=mat_morse;
  // // La matrice est t'elle definie ? On regarde son bloc(0,0)
  // //  const Matrice_Bloc& bloc=ref_cast(Matrice_Bloc,get_bloc(0,0).valeur());
  // //  const Matrice_Morse_Sym& sous_bloc=ref_cast(Matrice_Morse_Sym,bloc.get_bloc(0,0).valeur());
  // //  mat.set_est_definie(sous_bloc.get_est_definie());
  // mat.set_est_definie( get_est_definie( ) );

  // Debut test
  int test=0;
  if(test)
    {
      int n = nb_lignes( );
      DoubleVect x(n);
      DoubleVect resu1(n);
      DoubleVect resu2(n);
      for (int i=0; i<x.size(); i++)
        {
          x=0;
          resu1=0;
          resu2=0;
          x(i)=1;
          mat.multvect(x,resu1);
          multvect(x,resu2);
          Journal() << "Test Conversion Natif/Morse " << i << "/" << x.size() << ":" << mp_norme_vect(resu1) << " " << mp_norme_vect(resu2) << finl;
          for (int j=0; j<x.size(); j++)
            {
              if (!est_egal(resu1(j),resu2(j)))
                {
                  Journal() << "Test Conversion Natif/Morse A(" << i << "," << j << ") : " << resu1(j) << " " << resu2(j) << finl;
                  Journal()<<"Format Matrice_Bloc_Sym:" <<finl;
                  imprimer_formatte(Journal());
                  Journal()<<"Format Morse:" <<finl;
                  mat.imprimer_formatte(Journal());
                  Cerr<<"Error in Matrice_Bloc_Sym::BlocSymToMatMorseSym" << finl;
                  Cerr<<"See the .log files of the calculation." << finl;
                  exit();
                }
            }
        }
    }

  // Matrice_Morse mat1;
  // Matrice_Morse mat2;
  // Matrix_tools::convert_to_morse_matrix( result, mat1 );
  // Matrix_tools::convert_to_morse_matrix( mat, mat2 );

  // mat2 *= -1;
  // mat1 += mat2;

  // Cerr << "***********************************" << finl;
  // Cerr << " bloc_sym to morse_sym:" << finl;
  // Cerr << "min  = " << mp_min_vect( mat1.coeff_,VECT_ALL_ITEMS ) << finl;
  // Cerr << "imin = " << local_imin_vect( mat1.coeff_,VECT_ALL_ITEMS ) << finl;
  // Cerr << "max  = " << mp_max_vect( mat1.coeff_,VECT_ALL_ITEMS ) << finl;
  // Cerr << "imax = " << local_imax_vect( mat1.coeff_,VECT_ALL_ITEMS ) << finl;
  // Cerr << "***********************************" << finl;
}

void Matrice_Bloc_Sym::dimensionner(int N, int M)
{
  if(N!=M)
    {
      Cerr << "Trying to size a Matrice_Bloc_Sym type matrix not squared !!"
           <<finl<<flush;
      exit();
    }
  N_ = N;
  M_ = M;
  nb_blocs_ = (N_*(N_+1))/2 ;
  blocs_.dimensionner(nb_blocs_);
}

const Matrice& Matrice_Bloc_Sym::get_bloc( int i, int j ) const
{
  if (j<i || i<0 || i>=N_ || j<0 || j>=M_)
    {
      Cerr << "Matrice_Bloc_Sym::bloc("<<i<<","<<j<<") does not exist." << finl;
      Process::exit();
    }
  return blocs_[i*(N_-1)-i*(i-1)/2+j];
}

Matrice& Matrice_Bloc_Sym::get_bloc( int i, int j )
{
  if (j<i || i<0 || i>=N_ || j<0 || j>=M_)
    {
      Cerr << "Matrice_Bloc_Sym::bloc(" << i << "," << j << ") out of range : N=" << N_ << " M_=" << M_ << finl;
      Process::exit();
    }
  return blocs_[i*(N_-1)-i*(i-1)/2+j];
}

void Matrice_Bloc_Sym::get_stencil( IntTab& stencil ) const
{
  assert_check_symmetric_block_matrix_structure( );

  IntTab symmetric_stencil;
  get_symmetric_stencil( symmetric_stencil );

  Matrice_Sym::unsymmetrize_stencil( nb_lignes( ),
                                     symmetric_stencil,
                                     stencil );
}

void Matrice_Bloc_Sym::get_symmetric_stencil( IntTab& stencil ) const
{
  assert_check_symmetric_block_matrix_structure( );

  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );
  const int nb_stencils      = nb_blocs_;

  VECT( IntTab ) local_stencils;
  local_stencils.dimensionner( nb_stencils );

  int imin  = 0;
  int imax  = 0;
  int jmin  = 0;
  int jmax  = 0;
  int jprev = 0;

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      for ( int j=i; j<nb_column_blocks; ++j )
        {
          const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

          imax  = imin + local_matrix.nb_lignes( );
          jmax  = jmin + local_matrix.nb_colonnes( );
          jprev = ( i==j ) ? jmax : jprev;

          int stencil_index = i *( nb_line_blocks - 1 ) - i * ( i - 1) / 2 + j;

          IntTab& stencil_ = local_stencils[ stencil_index ];

          if ( i == j )
            {
              local_matrix.get_symmetric_stencil( stencil_ );
            }
          else
            {
              local_matrix.get_stencil( stencil_ );
            }

          const int size = stencil_.dimension( 0 );
          for ( int k=0; k<size; ++k )
            {
              stencil_( k, 0 ) += imin;
              stencil_( k, 1 ) += jmin;
            }
          jmin = jmax;
        }
      imin = imax;
      jmin = jprev;
    }

  const int nb_lines = nb_lignes( );
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& stencil_ = local_stencils[ i ];
      const int size = stencil_.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line = stencil_( k, 0 );
          offsets[ line + 1 ] += 1;
        }
    }

  for ( int i=0; i<nb_lines; ++i )
    {
      offsets[ i + 1 ] += offsets[ i ];
    }

  const int stencil_size = offsets[ nb_lines ];
  stencil.resize( stencil_size, 2 );

  stencil = -1;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& stencil_ = local_stencils[ i ];
      const int size = stencil_.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line   = stencil_( k, 0 );
          const int column = stencil_( k, 1 );
          const int index  = offsets[ line ];

          assert( stencil( index, 0 ) < 0 );
          assert( stencil( index, 1 ) < 0 );
          assert( index < offsets[ line + 1 ] );

          stencil( index, 0 ) = line;
          stencil( index, 1 ) = column;

          offsets[ line ] += 1;
        }
    }
}

void Matrice_Bloc_Sym::get_stencil_and_coefficients( IntTab&      stencil,
                                                     ArrOfDouble& coefficients ) const
{
  assert_check_symmetric_block_matrix_structure( );

  IntTab symmetric_stencil;
  ArrOfDouble symmetric_coefficients;
  get_symmetric_stencil_and_coefficients( symmetric_stencil, symmetric_coefficients );

  Matrice_Sym::unsymmetrize_stencil_and_coefficients( nb_lignes( ),
                                                      symmetric_stencil,
                                                      symmetric_coefficients,
                                                      stencil,
                                                      coefficients );
}

void Matrice_Bloc_Sym::get_symmetric_stencil_and_coefficients( IntTab&      stencil,
                                                               ArrOfDouble& coefficients ) const
{
  assert_check_symmetric_block_matrix_structure( );

  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );
  const int nb_stencils      = nb_blocs_;

  VECT( IntTab ) local_stencils;
  local_stencils.dimensionner( nb_stencils );

  VECT( ArrOfDouble ) local_coefficients;
  local_coefficients.dimensionner( nb_stencils );

  int imin  = 0;
  int imax  = 0;
  int jmin  = 0;
  int jmax  = 0;
  int jprev = 0;

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      for ( int j=i; j<nb_column_blocks; ++j )
        {
          const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

          imax  = imin + local_matrix.nb_lignes( );
          jmax  = jmin + local_matrix.nb_colonnes( );
          jprev = ( i==j ) ? jmax : jprev;

          int stencil_index = i *( nb_line_blocks - 1 ) - i * ( i - 1) / 2 + j;

          IntTab&      stencil_      = local_stencils[ stencil_index ];
          ArrOfDouble& coefficients_ = local_coefficients[ stencil_index ];

          if ( i == j )
            {
              local_matrix.get_symmetric_stencil_and_coefficients( stencil_, coefficients_ );
            }
          else
            {
              local_matrix.get_stencil_and_coefficients( stencil_, coefficients_ );
            }

          const int size = stencil_.dimension( 0 );
          for ( int k=0; k<size; ++k )
            {
              stencil_( k, 0 ) += imin;
              stencil_( k, 1 ) += jmin;
            }
          jmin = jmax;
        }
      imin = imax;
      jmin = jprev;
    }

  const int nb_lines = nb_lignes( );
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& stencil_ = local_stencils[ i ];
      const int size = stencil_.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line = stencil_( k, 0 );
          offsets[ line + 1 ] += 1;
        }
    }

  for ( int i=0; i<nb_lines; ++i )
    {
      offsets[ i + 1 ] += offsets[ i ];
    }

  const int stencil_size = offsets[ nb_lines ];
  stencil.resize( stencil_size, 2 );
  coefficients.resize_array( stencil_size );

  stencil = -1;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab&      stencil_       = local_stencils[ i ];
      const ArrOfDouble& coefficients_ = local_coefficients[ i ];

      const int size = stencil_.dimension( 0 );
      assert( coefficients_.size_array( ) == size );

      for ( int k=0; k<size; ++k )
        {
          const int line        = stencil_( k, 0 );
          const int column      = stencil_( k, 1 );
          const double coefficient = coefficients_[ k ];
          const int index       = offsets[ line ];

          assert( stencil( index, 0 ) < 0 );
          assert( stencil( index, 1 ) < 0 );
          assert( index < offsets[ line + 1 ] );

          stencil( index, 0 )   = line;
          stencil( index, 1 )   = column;
          coefficients[ index ] = coefficient;

          offsets[ line ] += 1;
        }
    }
}

bool Matrice_Bloc_Sym::check_symmetric_block_matrix_structure( void ) const
{
  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      int nb_lines   = get_bloc( i, i ).valeur( ).nb_lignes( );
      int nb_columns = get_bloc( i, i ).valeur( ).nb_colonnes( );

      if  ( nb_lines != nb_columns )
        {
          Cerr << "bloc( " << i << ", " << i << " ) is not square" << finl;
          return false;
        }

      for ( int j=0; j<i; ++j )
        {
          if ( nb_columns != get_bloc( j, i ).valeur( ).nb_colonnes( ) )
            {
              Cerr << "Invalid number of columns in bloc ( " << j << ", " << i << " )" << finl;
              return false;
            }
        }

      for ( int j=i; j<nb_column_blocks; ++j )
        {
          if ( nb_lines != get_bloc( i, j ).valeur( ).nb_lignes( ) )
            {
              Cerr << "Invalid number of lines in bloc ( " << i << ", " << j << " )" << finl;
              return false;
            }
        }
    }

  return true;
}

void Matrice_Bloc_Sym::assert_check_symmetric_block_matrix_structure( void ) const
{
#ifndef NDEBUG
  if ( ! ( check_symmetric_block_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Bloc_Sym::assert_check_symmetric_block_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
#endif
}

