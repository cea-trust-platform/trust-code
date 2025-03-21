/****************************************************************************
* Copyright (c) 2024, CEA
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

#pragma GCC diagnostic push

#if __GNUC__ < 9
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif

#include <gtest/gtest.h>

#pragma GCC diagnostic pop

#include <Array_tools.h>


template <typename _TYPE_>
TRUSTArray<_TYPE_> get_array()
{
  int np = Process::nproc();
  if (np != 1) throw("np != 1");
  
  // Initialize TRUSTArray with some values
  TRUSTArray<_TYPE_> tableau(5);
  tableau[0] = (_TYPE_)-1.0;
  tableau[1] = (_TYPE_)3.0;
  tableau[2] = (_TYPE_)-1.0;
  tableau[3] = (_TYPE_)8.0;
  tableau[4] = (_TYPE_)0.0;

  return tableau;
}

template <typename _TYPE_, int dim>
TRUSTTab<_TYPE_, int> get_tab()
{
  int np = Process::nproc();
  if (np != 1) throw("np != 1");
  
  // Initialize TRUSTTab with some values
  TRUSTTab<_TYPE_, int> tableau(3,dim);
  tableau(0, 0) = (_TYPE_)-1.0;
  tableau(1, 0) = (_TYPE_)0.0;
  tableau(2, 0) = (_TYPE_)-1.0;
  if(dim>1)
    {
      tableau(0, 1) = (_TYPE_)3.0;
      tableau(1, 1) = (_TYPE_)-8.0;
      tableau(2, 1) = (_TYPE_)2.0;
    }
  if(dim>2)
    {
      tableau(0, 2) = (_TYPE_)-7.0;
      tableau(1, 2) = (_TYPE_)2.0;
      tableau(2, 2) = (_TYPE_)-3.0;
    }
  if(dim>3)
    {
      tableau(0, 3) = (_TYPE_)8.0;
      tableau(1, 3) = (_TYPE_)1.0;
      tableau(2, 3) = (_TYPE_)4.0;
    }
  return tableau;
}

template <typename _TYPE_, int dim>
void TestTriLexicographique()
{
  int np = Process::nproc();
  if (np != 1) return;

  if(dim < 0 || dim > 4) return;
    
  // Initialize TRUSTTab with some values
  TRUSTTab<_TYPE_, int> tableau = get_tab<_TYPE_,dim>();

  // Expected result
  TRUSTTab<_TYPE_, int> expected(3,dim);
  expected(0, 0) = (_TYPE_)-1.0;
  expected(1, 0) = (_TYPE_)-1.0;
  expected(2, 0) = (_TYPE_)0.0;
  if(dim>1)
    {
      expected(0, 1) = (_TYPE_)2.0;
      expected(1, 1) = (_TYPE_)3.0;
      expected(2, 1) = (_TYPE_)-8.0;
    }
  if(dim>2)
    {
      expected(0, 2) = (_TYPE_)-3.0;
      expected(1, 2) = (_TYPE_)-7.0;
      expected(2, 2) = (_TYPE_)2.0;
    }
  if(dim>3)
    {
      expected(0, 3) = (_TYPE_)4.0;
      expected(1, 3) = (_TYPE_)8.0;
      expected(2, 3) = (_TYPE_)1.0;
    }
  // Call the function
  tri_lexicographique_tableau(tableau);

  // Verify the output
  for(int i=0; i<3;i++)
    for (int j = 0; j < dim; j++)
      EXPECT_DOUBLE_EQ(tableau(i,j), expected(i,j)) << "Line " << i << " mismatch. Tab not sorted...";
}


template <typename _TYPE_, int dim>
void TestTriLexicographiqueIndex()
{
  int np = Process::nproc();
  if (np != 1) return;

  if(dim < 0 || dim > 4) return;
    
  // Initialize TRUSTTab with some values
  TRUSTTab<_TYPE_, int> tableau = get_tab<_TYPE_,dim>();

  // Expected result
  ArrOfInt expected(3);
  expected[0] = 2;
  expected[1] = 0;
  expected[2] = 1;
  
  // Call the function
  ArrOfInt index;
  tri_lexicographique_tableau_indirect(tableau, index);

  // Verify the output
  for(int i=0; i<3;i++)
    EXPECT_EQ(index[i], expected[i]) << "Line " << i << " mismatch. Tab not sorted...";
}


template <typename _TYPE_>
void TestArrayTrierRetirerDoublons()
{
  int np = Process::nproc();
  if (np != 1) return;
   
  // Initialize TRUSTArray with some values
  TRUSTArray<_TYPE_> array = get_array<_TYPE_>();

  // Expected result
  TRUSTArray<_TYPE_> expected(4);
  expected[0] = (_TYPE_)-1.0;
  expected[1] = (_TYPE_)0.0;
  expected[2] = (_TYPE_)3.0;
  expected[3] = (_TYPE_)8.0;

  // Call the function
  array_trier_retirer_doublons(array);

  // Verify the output  
  EXPECT_EQ(array.size_array(), 4) << "Array not resized properly";
  for(int i=0; i<4;i++)
    EXPECT_DOUBLE_EQ(array[i], expected[i]) << "Line " << i << " mismatch. Array not cleaned...";
}

template <typename _TYPE_>
void TestTableauTrierRetirerDoublons()
{
  int np = Process::nproc();
  if (np != 1) return;
   
  // Initialize TRUSTTab with some values
  TRUSTTab<_TYPE_, int> tableau = get_tab<_TYPE_,2>();
  tableau(0,1) = (_TYPE_)2.0;
  
  // Expected result
  TRUSTTab<_TYPE_, int> expected(2,2);
  expected(0, 0) = (_TYPE_)-1.0;
  expected(0, 1) = (_TYPE_)2.0;
  expected(1, 0) = (_TYPE_)0.0;
  expected(1, 1) = (_TYPE_)-8.0;

  // Call the function
  tableau_trier_retirer_doublons(tableau);

  // Verify the output
  EXPECT_EQ(tableau.dimension(0), 2) << "Tab not resized properly";
  for(int i=0; i<2; i++)
    for(int j=0; j<2; j++)
      EXPECT_DOUBLE_EQ(tableau(i,j), expected(i,j)) << "Line " << i << " mismatch. Tab not sorted...";
}


template <typename _TYPE_>
void TestArrayCalculerIntersection()
{
  int np = Process::nproc();
  if (np != 1) return;
   
  // Initialize arrays with some values
  TRUSTArray<_TYPE_> array1 = get_array<_TYPE_>();
  array1[2] = (_TYPE_)7.0;
  array1[4] = (_TYPE_)9.0;
  
  TRUSTArray<_TYPE_> array2 = get_array<_TYPE_>();
  array2[2] = (_TYPE_)4.0;
  array2[4] = (_TYPE_)10.0;
  
  // Expected result
  TRUSTArray<_TYPE_> expected(3);
  expected[0] = (_TYPE_)-1.0;
  expected[1] = (_TYPE_)3.0;
  expected[2] = (_TYPE_)8.0;

  // Call the function
  array_calculer_intersection(array1, array2);

  // Verify the output  
  EXPECT_EQ(array1.size_array(), 3) << "Array not resized properly";
  for(int i=0; i<3;i++)
    EXPECT_DOUBLE_EQ(array1[i], expected[i]) << "Line " << i << " mismatch. Array not correct...";
}

void TestArrayRetirerElements()
{
  int np = Process::nproc();
  if (np != 1) return;
      
  // Initialize arrays with some values
  ArrOfInt array1 = get_array<int>();
  array1[2] = 7;
  array1[4] = 9;
  
  ArrOfInt array2 = get_array<int>();
  array2[2] = 4;
  array2[4] = 10;
  
  // Expected result
  ArrOfInt expected(2);
  expected[0] = 7;
  expected[1] = 9;

  // Call the function
  array_retirer_elements(array1,array2);

  // Verify the output  
  EXPECT_EQ(array1.size_array(), 2) << "Array not resized properly";
  for(int i=0; i<2;i++)
    EXPECT_EQ(array1[i], expected[i]) << "Line " << i << " mismatch. Array not cleaned...";
}


void TestArrayBSearch()
{
  int np = Process::nproc();
  if (np != 1) return;
      
  // Initialize array with some values
  ArrOfInt array = get_array<int>();
  array[2] = 7;
  array[4] = 9;
  
  // Call the function
  int index = array_bsearch(array, 7);

  // Verify the output  
  EXPECT_EQ(index, 2) << "Error";
}

void TestCalculerRenumSansDoublons()
{
  int np = Process::nproc();
  if (np != 1) return;
   
  // Initialize TRUSTTab with some values
  IntTab tableau = get_tab<int,2>();
  tableau(0,1) = 2;
  
  // Expected results
  ArrOfInt expected_r(3);
  expected_r[0] = 0;
  expected_r[1] = 1;
  expected_r[2] = 0;

  ArrOfInt expected_ri(2);
  expected_ri[0] = 0;
  expected_ri[1] = 1;

  // Call the function
  ArrOfInt renum;
  ArrOfInt renum_inverse;
  calculer_renum_sans_doublons(tableau, renum, renum_inverse);

  // Verify the output
  EXPECT_EQ(renum.size_array(), 3) << "Array renum not resized properly";
  EXPECT_EQ(renum_inverse.size_array(), 2) << "Array renum_inverse not resized properly";
  for(int i=0; i<3; i++)
    EXPECT_EQ(renum[i], expected_r[i]) << "Line " << i << " mismatch.";
  for(int i=0; i<2; i++)
    EXPECT_EQ(renum_inverse[i], expected_ri[i]) << "Line " << i << " mismatch.";

}

// Wrapper tests for both double and int types
TEST(TRUSTArrayTools, TestTriLexicographique2DDouble) {
  TestTriLexicographique<double,2>();
}

TEST(TRUSTArrayTools, TestTriLexicographique4DDouble) {
  TestTriLexicographique<double,4>();
}

TEST(TRUSTArrayTools, TestTriLexicographique3DInt) {
  TestTriLexicographique<int,3>();
}

TEST(TRUSTArrayTools,TestTriLexicographiqueIndex3DInt) {
  TestTriLexicographiqueIndex<int,3>();
}

TEST(TRUSTArrayTools,TestTriLexicographiqueIndex4DDouble) {
  TestTriLexicographiqueIndex<double,4>();
}

TEST(TRUSTArrayTools,TestArrayTrierRetirerDoublons) {
  TestArrayTrierRetirerDoublons<double>();
}

TEST(TRUSTArrayTools,TestTableauTrierRetirerDoublons) {
  TestTableauTrierRetirerDoublons<int>();
}

TEST(TRUSTArrayTools,TestArrayCalculerIntersection) {
  TestArrayCalculerIntersection<int>();
}

TEST(TRUSTArrayTools,TestArrayRetirerElements) {
  TestArrayRetirerElements();
}

TEST(TRUSTArrayTools,TestArrayBSearch) {
  TestArrayBSearch();
}

TEST(TRUSTArrayTools,TestCalculerRenumSansDoublons) {
  TestCalculerRenumSansDoublons();
}
