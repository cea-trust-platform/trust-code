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
// File:        CSR_Builder.h
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#ifndef CSR_Builder_included
#define CSR_Builder_included

// .DESCRIPTION
//  This class is a helper to build CSR stored matrices
class CSR_Builder
{
public:
  CSR_Builder();
  initialize(int nb_lines);
  inline void add_coeff(int line, int column, double coeff);
  build_matrix(Matrice_Bloc&);
protected:
  int nb_lines_;
  // List data contains groups of 2 integers like this:
  //   column, next_index, column, next_index, ...
  //  column is the parameter passed to add_coeff
  //  next_index is the group number of the next coefficient on the same line
  //   or -1 if this is the last coefficient.
  //   if not -1, the next group is
  //       column     = list_data_[2*next_index]
  //       next_index = list_data_[2*next_index+1]
  //   the coefficient is at coeff_data_[next_index]
  //  The first coefficient on line "l" is at list_head[l]
  ArrOfInt list_head_;
  // list_tail_[l] = -1 if the line "l" is empty, otherwise, gives the index
  //   of the last "next_index" on the line (we have list_data_[2*tail+1]=-1)
  ArrOfInt list_tail_;
  ArrOfInt list_data_;
  ArrOfDouble coeff_data_;
};

inline void CSR_Builder::add_coeff(int line, int column, double coeff)
{
  assert(line >= 0 && line < nb_lines_);

  // We put the new coefficient here:
  const int n = coeff_data_.size_array();
  assert(list_data_.size_array() == n*2); // mixing calls with and without "coeff" is forbidden
  list_data_.resize_array(n*2+2);
  list_data_[n*2] = column;
  list_data_[n*2+1] = -1; // new end of list
  coeff_data_.append_array(coeff);

  const int tail = list_tail_[line];
  if (tail < 0)
    {
      // this line is empty, initialize list_head_
      list_head_[line] = n;
    }
  else
    {
      // update the last next_index of the line:
      list_data_[2*tail+1] = n;
    }
  list_tail_[line] = n;
}

inline void CSR_Builder::add_coeff(int line, int column)
{
  assert(line >= 0 && line < nb_lines_);
  assert(coeff_data_.size_array() == 0); // mixing calls with and without "coeff" is forbidden
  // We put the new coefficient here:
  const int n = list_data_.size_array() / 2;
  list_data_.resize_array(n*2+2);
  list_data_[n*2] = column;
  list_data_[n*2+1] = -1; // new end of list

  const int tail = list_tail_[line];
  if (tail < 0)
    {
      // this line is empty, initialize list_head_
      list_head_[line] = n;
    }
  else
    {
      // update the last next_index of the line:
      list_data_[2*tail+1] = n;
    }
  list_tail_[line] = n;
}
#endif
