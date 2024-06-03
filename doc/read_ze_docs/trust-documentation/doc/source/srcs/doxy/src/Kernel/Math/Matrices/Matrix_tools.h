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

#ifndef Matrix_tools_included
#define Matrix_tools_included

#include <TRUSTTabs_forward.h>

#include <vector>

class Matrice;
class Matrice_Base;
class Matrice_Morse;
class Matrice_Morse_Sym;

class Matrix_tools
{
public :
  // conversion to morse format
  static void convert_to_morse_matrix(const Matrice_Base& in, Matrice_Morse& out);

  // conversion to Morse format keeping pointers to initial coefficients to later more efficiently update them
  static void convert_to_morse_matrix_with_ptrs( const Matrice_Base& in, Matrice_Morse&      out,
                                                 std::vector<const double *>& coeffs);

  // conversion to symetric morse format
  static void convert_to_symmetric_morse_matrix(const Matrice_Base& in, Matrice_Morse_Sym& out);

  // checking stencil
  static bool is_normalized_stencil(const IntTab& stencil);

  // checking symmetric stencil
  static bool is_normalized_symmetric_stencil(const IntTab& stencil);

  // building morse matrices by creating new matrix (from given stencil)
  // so we need to specify is the stencil is to attach or not to the matrix
  static void allocate_morse_matrix(const int nb_lines, const int nb_columns, const IntTab& stencil, Matrice_Morse& matrix, const bool& attach_stencil_to_matrix = false);

  static void build_morse_matrix(const int nb_lines, const int nb_columns, const IntTab& stencil, const ArrOfDouble& coefficients, Matrice_Morse& matrix);

// building symmetric morse matrices
  static void allocate_symmetric_morse_matrix(const int order, const IntTab& stencil, Matrice_Morse_Sym& matrix);

  static void build_symmetric_morse_matrix(const int order, const IntTab& stencil, const ArrOfDouble& coefficients, Matrice_Morse_Sym& matrix);

  // allocation for scaled addition
  static void allocate_for_scaled_addition(const Matrice& A, const Matrice& B, Matrice& C);

  static void allocate_for_symmetric_scaled_addition(const Matrice& A, const Matrice& B, Matrice& C);

  // scaled addition
  static void add_scaled_matrices(const Matrice& A, const double alpha, const Matrice& B, const double beta, Matrice& C);

  static void add_symmetric_scaled_matrices(const Matrice& A, const double alpha, const Matrice& B, const double beta, Matrice& C);

  // stencil analysis
  static bool is_null_stencil(const IntTab& stencil);

  static bool is_diagonal_stencil(const int nb_lines, const int nb_columns, const IntTab& stencil);

  // allocation from stencil specification
  static void allocate_from_stencil(const int nb_lines, const int nb_columns, const IntTab& stencil, Matrice& matrix, const bool& attach_stencil_to_matrix = false);

  // extending a matrix's stencil
  static void extend_matrix_stencil(const IntTab& stencil, Matrice& matrix, const bool& attach_stencil_to_matrix = false);

  // diagonal matrix times a square morse matrix 'mat'
  // if inverse then the 1/diag is used
  // this can be seen as a left multiplication
  static void matdiag_mult_matmorse(const DoubleTab& diag, Matrice_Morse& mat, const bool& inverse);

  //right multiplication version of matdiag_mult_matmorse
  static void matmorse_mult_matdiag(const DoubleTab& diag, Matrice_Morse& mat, const bool& inverse);

  // uniform diagonal matrix (i.e. same coefficient on the diag) times a square morse matrix 'mat'
  // if inverse then the 1/diag is used
  // this can be seen as a left multiplication
  static void uniform_matdiag_mult_matmorse(const double diag, Matrice_Morse& mat, const bool& inverse);

  //right multiplication version of uniform_matdiag_mult_matmorse
  static void matmorse_mult_uniform_matdiag(const double diag, Matrice_Morse& mat, const bool& inverse);

  //add empty rows/columns to matrix
  static void extend_matrix(Matrice_Morse& mat, int nl, int nc);
};

#endif /* Matrix_tools_included */
