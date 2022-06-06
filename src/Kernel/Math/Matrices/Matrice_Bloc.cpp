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

#include <Matrice_Morse_Sym.h>
#include <Matrice_Morse.h>
#include <Matrice_Bloc.h>
#include <Matrix_tools.h>
#include <TRUSTArrays.h>
#include <TRUSTTabs.h>

Implemente_instanciable_sans_constructeur(Matrice_Bloc,"Matrice_Bloc",Matrice_Base);

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Matrice_Bloc::printOn( Sortie& os ) const
{
  os << nb_blocs_ << finl;
  os << N_ << finl;
  os << M_ << finl;
  for ( int i=0; i<nb_blocs_; ++i )
    {
      os << blocs_[i] << finl;
    }
  return os;
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Matrice_Bloc::readOn( Entree& is )
{
  is >> nb_blocs_;
  is >> N_;
  is >> M_;
  blocs_.dimensionner( nb_blocs_ );
  for ( int i=0; i<nb_blocs_; ++i )
    {
      is >> blocs_[i];
    }
  return is;
}

int Matrice_Bloc::ordre( void ) const
{
  if ( M_ == N_)
    {
      return N_;
    }
  return 0;
}

int Matrice_Bloc::nb_lignes( void ) const
{
  int sum=0;
  for( int i=0; i<N_; ++i )
    {
      // Codage pour fonctionner avec le stockage de Mat_Bloc_Sym (j>=i)
      sum += get_bloc(i,M_-1).valeur( ).nb_lignes();
    }
  return sum;
}

int Matrice_Bloc::nb_colonnes( void ) const
{
  int sum=0;
  for( int i=0; i<M_; ++i )
    {
      sum += get_bloc(0,i).valeur( ).nb_colonnes();
    }
  return sum;
}

DoubleVect& Matrice_Bloc::ajouter_multvect_( const DoubleVect& x,
                                             DoubleVect&       r ) const
{
  const double * const_x_addr = x.addr( );
  double * x_addr = ( double * ) const_x_addr;
  double * r_addr = r.addr( );

  DoubleVect x_bloc; // Une sous-partie de x
  DoubleVect r_bloc; // Une sous-partie de y

  int r_bloc_debut = 0; // Premier element de r du bloc courant
  // Boucle sur les lignes de blocs
  for ( int i_ligne = 0; i_ligne < N_; ++i_ligne )
    {
      const int r_bloc_size = get_bloc( i_ligne, 0 ).valeur( ).nb_lignes( );
      r_bloc.ref_data( r_addr + r_bloc_debut, r_bloc_size );
      assert( r_bloc_debut+r_bloc_size <= r.size_array( ) );
      int x_bloc_debut = 0; // Premier element de x du bloc courant
      // Boucle sur les colonnes de blocs
      for ( int i_colonne = 0; i_colonne < M_; ++i_colonne )
        {
          const Matrice& sub_bloc = get_bloc( i_ligne, i_colonne );
          const int x_bloc_size = sub_bloc.valeur( ).nb_colonnes( );
          x_bloc.ref_data( x_addr + x_bloc_debut, x_bloc_size );
          assert( x_bloc_debut+x_bloc_size <= x.size_array( ) );
          sub_bloc.valeur( ).ajouter_multvect_( x_bloc, r_bloc );
          x_bloc_debut += x_bloc_size;
        }
      r_bloc_debut += r_bloc_size;
    }
  return r;
}

DoubleVect& Matrice_Bloc::ajouter_multvectT_(const DoubleVect& x,
                                             DoubleVect&       r) const
{
  const double * const_x_addr = x.addr( );
  double * x_addr = ( double * ) const_x_addr;
  double * r_addr = r.addr( );

  DoubleVect x_bloc; // Une sous-partie de x
  DoubleVect r_bloc; // Une sous-partie de r

  int r_bloc_debut = 0; // Premier element de r du bloc courant
  // Boucle sur les lignes de blocs
  for ( int i_ligne = 0; i_ligne < M_; ++i_ligne )
    {
      const int r_bloc_size = get_bloc( 0, i_ligne ).valeur( ).nb_colonnes( );
      r_bloc.ref_data( r_addr + r_bloc_debut, r_bloc_size );
      assert( r_bloc_debut+r_bloc_size <= r.size_array( ) );
      int x_bloc_debut = 0; // Premier element de x du bloc courant
      // Boucle sur les colonnes de blocs
      for ( int i_colonne = 0; i_colonne < N_; ++i_colonne )
        {
          const Matrice& sub_bloc = get_bloc( i_colonne, i_ligne );
          const int x_bloc_size = sub_bloc.valeur( ).nb_lignes( );
          x_bloc.ref_data( x_addr + x_bloc_debut, x_bloc_size );
          assert( x_bloc_debut+x_bloc_size <= x.size_array( ) );
          sub_bloc.valeur( ).ajouter_multvectT_( x_bloc, r_bloc );
          x_bloc_debut += x_bloc_size;
        }
      r_bloc_debut += r_bloc_size;
    }
  return r;
}

DoubleTab& Matrice_Bloc::ajouter_multTab_(const DoubleTab& x,
                                          DoubleTab&       r) const
{
  Cerr << "Error in 'Matrice_Bloc::ajouter_multTab_()':" << finl;
  Cerr << "  Not yet implemented" << finl;
  Process::exit( );

  return r;
}

void Matrice_Bloc::scale( const double x )
{
  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );

  for (int i=0; i<nb_line_blocks; i++)
    {
      for (int j=0; j<nb_column_blocks; j++)
        {
          Matrice_Base& matrix = get_bloc( i, j ).valeur( );
          matrix.scale( x );
        }
    }
}


void Matrice_Bloc::get_stencil( IntTab& stencil ) const
{
  assert_check_block_matrix_structure( );
  if(  is_stencil_up_to_date_ )
    {
      stencil = stencil_;
      return;
    }

  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );
  const int nb_stencils      =  nb_line_blocks * nb_column_blocks;

  VECT( IntTab ) local_stencils;
  local_stencils.dimensionner( nb_stencils );

  int imin = 0;
  int imax = 0;
  int jmin = 0;
  int jmax = 0;

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      for ( int j=0; j<nb_column_blocks; ++j )
        {
          const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

          imax = imin + local_matrix.nb_lignes( );
          jmax = jmin + local_matrix.nb_colonnes( );

          IntTab& local_stencil = local_stencils[ i * nb_column_blocks + j ];
          local_matrix.get_stencil( local_stencil );

          const int size = local_stencil.dimension( 0 );
          for ( int k=0; k<size; ++k )
            {
              local_stencil( k, 0 ) += imin;
              local_stencil( k, 1 ) += jmin;
            }
          jmin = jmax;
        }
      imin = imax;
      jmin = 0;
    }

  const int nb_lines = nb_lignes( );
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& local_stencil = local_stencils[ i ];
      const int size = local_stencil.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line = local_stencil( k, 0 );
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
      const IntTab& local_stencil = local_stencils[ i ];
      const int size = local_stencil.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line   = local_stencil( k, 0 );
          const int column = local_stencil( k, 1 );
          const int index  = offsets[ line ];

          assert( stencil( index, 0 ) < 0 );
          assert( stencil( index, 1 ) < 0 );
          assert( index < offsets[ line + 1 ] );

          stencil( index, 0 )  = line;
          stencil( index, 1 )  = column;
          offsets[ line ]     += 1;
        }
    }

}


void Matrice_Bloc::build_stencil( void )
{
  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );
  const int nb_stencils      = nb_line_blocks * nb_column_blocks;


  VECT( IntTab ) local_stencils;
  local_stencils.dimensionner( nb_stencils );

  int imin = 0;
  int imax = 0;
  int jmin = 0;
  int jmax = 0;

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      for ( int j=0; j<nb_column_blocks; ++j )
        {
          const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

          imax = imin + local_matrix.nb_lignes( );
          jmax = jmin + local_matrix.nb_colonnes( );

          IntTab& local_stencil = local_stencils[ i * nb_column_blocks + j ];
          local_matrix.get_stencil( local_stencil );

          const int size = local_stencil.dimension( 0 );
          for ( int k=0; k<size; ++k )
            {
              local_stencil( k, 0 ) += imin;
              local_stencil( k, 1 ) += jmin;
            }
          jmin = jmax;
        }
      imin = imax;
      jmin = 0;
    }

  const int nb_lines = nb_lignes( );
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& local_stencil = local_stencils[ i ];
      const int size = local_stencil.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line = local_stencil( k, 0 );
          offsets[ line + 1 ] += 1;
        }
    }

  for ( int i=0; i<nb_lines; ++i )
    {
      offsets[ i + 1 ] += offsets[ i ];
    }

  offsets_ = offsets ;

  const int stencil_size = offsets[ nb_lines ];
  stencil_.resize( stencil_size, 2 );

  stencil_ = -1;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& local_stencil = local_stencils[ i ];
      const int size = local_stencil.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line   = local_stencil( k, 0 );
          const int column = local_stencil( k, 1 );
          const int index  = offsets[ line ];

          assert( stencil_( index, 0 ) < 0 );
          assert( stencil_( index, 1 ) < 0 );
          assert( index < offsets[ line + 1 ] );

          stencil_( index, 0 )  = line;
          stencil_( index, 1 )  = column;
          offsets[ line ]     += 1;
        }
    }

  is_stencil_up_to_date_ = true ;
}

void Matrice_Bloc::get_stencil_and_coefficients( IntTab& stencil, ArrOfDouble& coefficients ) const
{
  assert_check_block_matrix_structure( );
  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );

  int imin = 0;
  int imax = 0;
  int jmin = 0;
  int jmax = 0;

  if( is_stencil_up_to_date_ )
    {
      stencil = stencil_;
      ArrOfInt offsets = offsets_;
      const int stencil_size = stencil_.dimension(0);
      coefficients.resize_array( stencil_size );

      IntTab      local_stencil;
      ArrOfDouble local_coeff;

      for ( int i=0; i<nb_line_blocks; ++i )
        {
          for ( int j=0; j<nb_column_blocks; ++j )
            {
              const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

              imax = imin + local_matrix.nb_lignes( );
              jmax = jmin + local_matrix.nb_colonnes( );

              local_matrix.get_stencil_and_coefficients( local_stencil, local_coeff );

              const int size = local_stencil.dimension( 0 );
              for ( int k=0; k<size; ++k )
                {
                  const int line           = local_stencil( k, 0 ) + imin;
                  const double coefficient = local_coeff[ k ];
                  const int index          = offsets[ line ];

                  coefficients[ index ] = coefficient;

                  offsets[ line ] += 1;
                }
              jmin = jmax;
            }
          imin = imax;
          jmin = 0;
        }
      return;
    }
  const int nb_stencils      =  nb_line_blocks * nb_column_blocks;

  VECT( IntTab ) vect_local_stencils;
  vect_local_stencils.dimensionner( nb_stencils );

  VECT( ArrOfDouble ) vect_local_coefficients;
  vect_local_coefficients.dimensionner( nb_stencils );

  for ( int i=0; i<nb_line_blocks; ++i )
    {
      for ( int j=0; j<nb_column_blocks; ++j )
        {
          const Matrice_Base& local_matrix = get_bloc( i, j ).valeur( );

          imax = imin + local_matrix.nb_lignes( );
          jmax = jmin + local_matrix.nb_colonnes( );

          IntTab&      local_stencil = vect_local_stencils[ i * nb_column_blocks + j ];
          ArrOfDouble& local_coefficients = vect_local_coefficients[i * nb_column_blocks + j ];
          local_matrix.get_stencil_and_coefficients( local_stencil, local_coefficients );

          const int size = local_stencil.dimension( 0 );
          for ( int k=0; k<size; ++k )
            {
              local_stencil( k, 0 ) += imin;
              local_stencil( k, 1 ) += jmin;
            }
          jmin = jmax;
        }
      imin = imax;
      jmin = 0;
    }

  const int nb_lines = nb_lignes( );
  ArrOfInt offsets( nb_lines + 1 );
  offsets[ 0 ] = 0;

  for ( int i=0; i<nb_stencils; ++i )
    {
      const IntTab& local_stencil = vect_local_stencils[ i ];
      const int size = local_stencil.dimension( 0 );

      for ( int k=0; k<size; ++k )
        {
          const int line = local_stencil( k, 0 );
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
      const IntTab&      local_stencil= vect_local_stencils[ i ];
      const ArrOfDouble& local_coefficients = vect_local_coefficients[ i ];

      const int size = local_stencil.dimension( 0 );
      assert( local_coefficients.size_array( ) == size );

      for ( int k=0; k<size; ++k )
        {
          const int line        = local_stencil( k, 0 );
          const int column      = local_stencil( k, 1 );
          const double coefficient = local_coefficients[ k ];
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



Sortie& Matrice_Bloc::imprimer( Sortie& os ) const
{
  for ( int i=0; i<N_; ++i )
    {
      for ( int j=0; j<M_; ++j )
        {
          os << "Bloc (" << i << "," << j << ")" << finl;
          get_bloc( i, j ).valeur( ).imprimer( os );
        }
    }
  return os;
}

Sortie& Matrice_Bloc::imprimer_formatte( Sortie& os ) const
{
  for ( int i=0; i<N_; ++i )
    {
      for ( int j=0; j<M_; ++j )
        {
          os << "Bloc (" << i << "," << j << ")" << finl;
          get_bloc( i, j ).valeur( ).imprimer_formatte( os );
        }
    }
  return os;
}

void Matrice_Bloc::dimensionner( int N, int M )
{
  N_ = N;
  M_ = M;
  nb_blocs_ = N*M;
  blocs_.dimensionner(nb_blocs_);
}

const Matrice& Matrice_Bloc::get_bloc( int i, int j ) const
{
  if ( ( i<0 ) || ( i>=N_ ) )
    {
      Cerr << "Error in 'Matrice_Bloc::get_bloc()': " << finl;
      Cerr << "  Line index is out of range" << finl;
      Process::exit( );
    }

  if ( ( j<0 ) || ( j>=M_ ) )
    {
      Cerr << "Error in 'Matrice_Bloc::get_bloc()': " << finl;
      Cerr << "  Column index is out of range" << finl;
      Process::exit( );
    }

  return blocs_[i * M_ + j];
}

Matrice& Matrice_Bloc::get_bloc( int i, int j )
{
  if ( ( i<0 ) || ( i>=N_ ) )
    {
      Cerr << "Error in 'Matrice_Bloc::get_bloc()': " << finl;
      Cerr << "  Line index is out of range" << finl;
      Process::exit( );
    }

  if ( ( j<0 ) || ( j>=M_ ) )
    {
      Cerr << "Error in 'Matrice_Bloc::get_bloc()': " << finl;
      Cerr << "  Column index is out of range" << finl;
      Process::exit( );
    }

  return blocs_[i * M_ + j];
}

Matrice_Bloc::Matrice_Bloc( int N, int M ) : Matrice_Base( )
{
  dimensionner(N,M);
  is_stencil_up_to_date_ = false ;
}

int Matrice_Bloc::dim( int d ) const
{
  if ( d==0 )
    {
      return N_;
    }
  else
    {
      return M_;
    }
}

int Matrice_Bloc::nb_bloc_lignes( void ) const
{
  return N_;
}

int Matrice_Bloc::nb_bloc_colonnes( void ) const
{
  return M_;
}

// // Description
// // Remplissage d'une Matrice_Morse par une Matrice_Bloc de Matrice_Morse quelconques
void Matrice_Bloc::BlocToMatMorse( Matrice_Morse& result ) const
{
  Matrix_tools::convert_to_morse_matrix( (*this), result );
}

//Produit matrice-vecteur

// Remplissage partiel (i premieres lignes) d'une matrice bloc par une matrice morse symetrique
// RR | RV
// ------
// VR | VV
void Matrice_Bloc::remplir(const IntLists& voisins, const DoubleLists& valeurs, const DoubleVect& terme_diag, const int i, const int n)
{
  dimensionner(2, 2) ;
  get_bloc(0,0).typer("Matrice_Morse_Sym");        // Bloc RR
  get_bloc(0,1).typer("Matrice_Morse");        // Bloc RV
  get_bloc(1,0).typer("Matrice_Morse");        // Bloc VR
  get_bloc(1,1).typer("Matrice_Morse");        // Bloc VV
  remplir(voisins, valeurs, terme_diag, i, n, i, n);
}

// Remplissage partiel (n premieres lignes, m premieres colonnes) d'une matrice bloc par une matrice morse non symetrique
// RR | RV
// ------
// VR | VV
void Matrice_Bloc::remplir(const IntLists& voisins, const DoubleLists& valeurs, const int i, const int n, const int j, const int m)
{
  dimensionner(2, 2) ;
  get_bloc(0,0).typer("Matrice_Morse");        // Bloc RR
  get_bloc(0,1).typer("Matrice_Morse");        // Bloc RV
  get_bloc(1,0).typer("Matrice_Morse");        // Bloc VR
  get_bloc(1,1).typer("Matrice_Morse");        // Bloc VV
  DoubleVect diagonale_vide;
  remplir(voisins, valeurs, diagonale_vide, i, n, j, m);
}

// Remplissage partiel (i premieres lignes, j premieres colonnes) d'une matrice bloc par une matrice morse symetrique ou non
// RR | RV
// ------
// VR | VV
void Matrice_Bloc::remplir(const IntLists& voisins, const DoubleLists& valeurs, const DoubleVect& terme_diag, const int i, const int n, const int j, const int m)
{
  Matrice_Morse& RR=ref_cast(Matrice_Morse,get_bloc(0,0).valeur());
  Matrice_Morse& RV=ref_cast(Matrice_Morse,get_bloc(0,1).valeur());
  Matrice_Morse& VR=ref_cast(Matrice_Morse,get_bloc(1,0).valeur());
  Matrice_Morse& VV=ref_cast(Matrice_Morse,get_bloc(1,1).valeur());

  // Premiere passe pour le dimensionnement
  int RR_compteur, RR_rang=0;
  int RV_compteur, RV_rang=0;
  int VR_compteur, VR_rang=0;
  int VV_compteur, VV_rang=0;

  int num_elem;
  for (num_elem=0; num_elem<n; num_elem++)
    {
      IntList_Curseur liste_vois(voisins[num_elem]);
      DoubleList_Curseur liste_val(valeurs[num_elem]);

      RR_compteur=0;
      RV_compteur=0;
      VR_compteur=0;
      VV_compteur=0;

      // Diagonale
      if (terme_diag.size_array()!=0)
        {
          if (num_elem<i)
            RR_compteur++;
          else
            VV_compteur++;
        }

      while (liste_vois)
        {
          int colonne = liste_vois.valeur();
          if (colonne<j)
            {
              if (num_elem<i)
                RR_compteur++;         // Sous Bloc RR
              else
                VR_compteur++;        // Sous Bloc VR
            }
          else
            {
              if (num_elem<i)
                RV_compteur++;        // Sous Bloc RV
              else
                VV_compteur++;         // Sous Bloc VV
            }
          ++liste_vois;
          ++liste_val;
        }
      RR_rang += RR_compteur;
      RV_rang += RV_compteur;
      VR_rang += VR_compteur;
      VV_rang += VV_compteur;
    }
  RR.dimensionner(i,        j,        RR_rang);        // Dimension RR
  RV.dimensionner(i,        m-j,        RV_rang);        // Dimension RV
  VR.dimensionner(n-i,        j,        VR_rang);        // Dimension VR
  VV.dimensionner(n-i,        m-j,        VV_rang);        // Dimension VV

  // Initialisations necessaires
  RR.get_set_tab1()=1;
  RV.get_set_tab1()=1;
  VR.get_set_tab1()=1;
  VV.get_set_tab1()=1;

  // Deuxieme passe pour le remplissage
  // Tableaux tab1, tab2 et coeff_ pour le bloc RR
  int* RR_tab1 = RR.get_set_tab1().addr();
  int* RR_tab2 = RR.get_set_tab2().addr();
  double* RR_coeff = RR.get_set_coeff().addr();
  int* RR_tab2_ptr = RR_tab2;

  // Tableaux tab1, tab2 et coeff_ pour le bloc RV
  int* RV_tab1 = RV.get_set_tab1().addr();
  int* RV_tab2 = RV.get_set_tab2().addr();
  double* RV_coeff = RV.get_set_coeff().addr();
  int* RV_tab2_ptr = RV_tab2;

  // Tableaux tab1, tab2 et coeff_ pour le bloc VR
  int* VR_tab1 = VR.get_set_tab1().addr();
  int* VR_tab2 = VR.get_set_tab2().addr();
  double* VR_coeff = VR.get_set_coeff().addr();
  int* VR_tab2_ptr = VR_tab2;

  // Tableaux tab1, tab2 et coeff_ pour le bloc VV
  int* VV_tab1 = VV.get_set_tab1().addr();
  int* VV_tab2 = VV.get_set_tab2().addr();
  double* VV_coeff = VV.get_set_coeff().addr();
  int* VV_tab2_ptr = VV_tab2;

  RR_rang=0;
  RV_rang=0;
  VR_rang=0;
  VV_rang=0;
  for (num_elem=0; num_elem<n; num_elem++)
    {
      IntList_Curseur liste_vois(voisins[num_elem]);
      DoubleList_Curseur liste_val(valeurs[num_elem]);

      RR_compteur=0;
      RV_compteur=0;
      VR_compteur=0;
      VV_compteur=0;
      if (num_elem<i)
        {
          *RR_tab1++ = RR_rang;
          *RV_tab1++ = RV_rang;
        }
      else
        {
          *VR_tab1++ = VR_rang;
          *VV_tab1++ = VV_rang;
        }
      // Diagonale eventuelle
      if (terme_diag.size_array()!=0)
        {
          if (num_elem<i)
            {
              *RR_tab2_ptr++ = num_elem;
              *RR_coeff++ = terme_diag[num_elem];
              RR_compteur++;
            }
          else
            {
              *VV_tab2_ptr++ = num_elem-i;
              *VV_coeff++ = terme_diag[num_elem];
              VV_compteur++;
            }
        }
      while (liste_vois)
        {
          int colonne = liste_vois.valeur();
          double coeff = liste_val.valeur();
          if (colonne<j)
            {
              if (num_elem<i) // Sous Bloc RR
                {
                  *RR_tab2_ptr++ = colonne;
                  *RR_coeff++ = coeff;
                  RR_compteur++;
                }
              else  // Sous Bloc VR
                {
                  *VR_tab2_ptr++ = colonne;
                  *VR_coeff++ = coeff;
                  VR_compteur++;
                }
            }
          else
            {
              if (num_elem<i) // Sous Bloc RV
                {
                  *RV_tab2_ptr++ = colonne-j;
                  *RV_coeff++ = coeff;
                  RV_compteur++;
                }
              else // Sous Bloc VV
                {
                  *VV_tab2_ptr++ = colonne-j;
                  *VV_coeff++ = coeff;
                  VV_compteur++;
                }
            }
          ++liste_vois;
          ++liste_val;
        }
      RR_rang += RR_compteur;
      RV_rang += RV_compteur;
      VR_rang += VR_compteur;
      VV_rang += VV_compteur;
    }
  RR.get_set_tab1()(i)=RR_rang;
  RV.get_set_tab1()(i)=RV_rang;
  VR.get_set_tab1()(n-i)=VR_rang;
  VV.get_set_tab1()(n-i)=VV_rang;
  // Passage a la numerotation Fortran
  RR.formeF();
  RV.formeF();
  VR.formeF();
  VV.formeF();

  // Compactage de la matrice
  RR.compacte();
  RV.compacte();
  VR.compacte();
  VV.compacte();
}

Matrice_Bloc& Matrice_Bloc::operator *=( double x )
{
  scale( x );
  return *this;
}

bool Matrice_Bloc::check_block_matrix_structure( void ) const
{
  const int nb_line_blocks   = nb_bloc_lignes( );
  const int nb_column_blocks = nb_bloc_colonnes( );


  for ( int i=0; i<nb_line_blocks; ++i )
    {
      int nb_lines = get_bloc( i, 0 ).valeur( ).nb_lignes( );

      for ( int j=0; j<nb_column_blocks; ++j )
        {
          if ( nb_lines != get_bloc( i, j ).valeur( ).nb_lignes( ) )
            {
              Cerr << "Invalid number of lines in bloc ( " << i << ", " << j << " )" << finl;
              return false;
            }
        }
    }

  for ( int j=0; j<nb_column_blocks; ++j )
    {
      int nb_columns = get_bloc( 0, j ).valeur( ).nb_colonnes( );

      for ( int i=0; i<nb_line_blocks; ++i )
        {
          if ( nb_columns != get_bloc( i, j ).valeur( ).nb_colonnes( ) )
            {
              Cerr << "Invalid number of columns in bloc ( " << i << ", " << j << " )" << finl;
              return false;
            }
        }
    }

  return true;
}

void Matrice_Bloc::assert_check_block_matrix_structure( void ) const
{
#ifndef NDEBUG
  if ( ! ( check_block_matrix_structure( ) ) )
    {
      Cerr << "Error in 'Matrice_Bloc::assert_check_block_matrix_structure( )':" << finl;
      Cerr << "  Exiting..." << finl;
      Process::exit( );
    }
#endif
}

