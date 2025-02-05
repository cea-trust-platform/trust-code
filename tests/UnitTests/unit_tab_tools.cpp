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

#if __GNUC__ < 9
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include <TRUSTTab_parts.h>

//=== This is testing src/Kernel/Math/TRUSTTab_tools.cpp
template <typename _TYPE_>
TRUSTTab<_TYPE_, int> init_md_vector_tab(const int rows, const int cols)
{
  int np = Process::nproc();
//int rk = Process::me();

  if (np != 1) throw("np != 1");

  ArrOfInt pe_vois(0);
  ArrsOfInt items_to_send(0);
  ArrsOfInt items_to_recv(0);
  ArrsOfInt bloc_to_recv(0);

  MD_Vector mdv;
  MD_Vector_std mds(rows, rows, pe_vois, items_to_send, items_to_recv, bloc_to_recv);
  mdv.copy(mds);

  TRUSTTab<_TYPE_, int> tab(rows, cols);

  tab.set_md_vector(mdv);

  return tab;
}

template <typename _TYPE_>
void TestLocalMaxAbsTab() {

      int np = Process::nproc();
      if (np != 1) return;

      // Define the size of the tab
      const int rows = 3;
      const int cols = 4;

      // Initialize TRUSTTab with some values
      TRUSTTab<_TYPE_, int> tableau = init_md_vector_tab<_TYPE_>(rows, cols);
      tableau(0, 0) = (_TYPE_)-1.0;
      tableau(0, 1) = (_TYPE_)3.0;
      tableau(0, 2) = (_TYPE_)-7.0;
      tableau(1, 0) = (_TYPE_)4.0;
      tableau(1, 1) = (_TYPE_)-8.0;
      tableau(1, 2) = (_TYPE_)2.0;
      tableau(2, 0) = (_TYPE_)-6.0;
      tableau(2, 1) = (_TYPE_)5.0;
      tableau(2, 2) = (_TYPE_)-3.0;

      // Expected result: max absolute values per column
      TRUSTArray<_TYPE_, int> expected_max(cols);
      expected_max[0] = 6.0;
      expected_max[1] = 8.0;
      expected_max[2] = 7.0;

      // Output array for max values
      TRUSTArray<_TYPE_, int> max_colonne(cols);

      // Call the function
      local_max_abs_tab(tableau, max_colonne);

      // Verify the output
      for (int j = 0; j < cols; j++) {
          EXPECT_DOUBLE_EQ(max_colonne[j], expected_max[j]) << "Column " << j << " max value mismatch.";
      }
}

// Wrapper tests for both double and float types
TEST(TRUSTTabTools, TestLocalMaxAbsTabFloat) {
  TestLocalMaxAbsTab<float>();
}

TEST(TRUSTTabTools, TestLocalMaxAbsTabDouble) {
  TestLocalMaxAbsTab<double>();
}

