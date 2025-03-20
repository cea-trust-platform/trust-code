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

#include <Matrice_Dense.h>

TEST(Matrice_Dense, solve) {
      Matrice_Dense A(2,2);
      A(0,0) = 1;
      A(0,1) = 2;
      A(1,0) = 3;
      A(1,1) = 4;
      ArrOfDouble b(2);
      b(0)=3;
      b(1)=7;
      ArrOfDouble x(2);
      A.solve(b,x);
      EXPECT_DOUBLE_EQ(x(0), 1);
      EXPECT_DOUBLE_EQ(x(1), 1);
}

TEST(Matrice_Dense, AddTwoMatrix) {
      Matrice_Dense A(2,2);
      A(0,0) = 1;
      A(0,1) = 2;
      A(1,0) = 3;
      A(1,1) = 4;
      Matrice_Dense B(2,2);
      B(0,0) = 1;
      B(0,1) = 2;
      B(1,0) = 3;
      B(1,1) = 4;
      Matrice_Dense C(2,2);
      C=A+B;
      EXPECT_DOUBLE_EQ(C(0,0),2);
      EXPECT_DOUBLE_EQ(C(0,1),4);
      EXPECT_DOUBLE_EQ(C(1,0),6);
      EXPECT_DOUBLE_EQ(C(1,1),8);
}

TEST(Matrice_Dense, MultiplyMatrixByDouble) {
      Matrice_Dense A(2,2);
      A(0,0) = 1;
      A(0,1) = 2;
      A(1,0) = 3;
      A(1,1) = 4;
      double my_double=3;
      Matrice_Dense C(2,2);
      C=my_double*A;
      EXPECT_DOUBLE_EQ(C(0,0),3);
      EXPECT_DOUBLE_EQ(C(0,1),6);
      EXPECT_DOUBLE_EQ(C(1,0),9);
      EXPECT_DOUBLE_EQ(C(1,1),12);
}

TEST(Matrice_Dense, ScalarProduct) {
      Matrice_Dense A(2,2);
      A(0,0) = 1;
      A(0,1) = 2;
      A(1,0) = 3;
      A(1,1) = 4;
      DoubleVect B(2);
      B(0)=1;
      B(1)=1;
      DoubleVect C(2);
      C=A*B;
      EXPECT_DOUBLE_EQ(C(0),3);
      EXPECT_DOUBLE_EQ(C(1),7);
}
