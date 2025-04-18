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

#include <TRUSTTrav.h>

#include <MD_Vector_seq.h>
#include <SChaine.h>
#include <EChaine.h>

#include <numeric>

#ifndef __NVCOMPILER
TEST(TRUSTArray, CantCopyRefData) {
  std::vector<int> v(3);
  ArrOfInt a1;
  // Make a1 be a ref_data:
  a1.ref_data(v.data(), 3);

  // It is forbidden to copy a ref_data:
  EXPECT_DEATH( { ArrOfInt a2(a1); }, ".*" );
}
#endif

//Constructors 
TEST(TRUSTArray, ConstructorDefault) {
    ArrOfInt a;
    EXPECT_EQ(a.get_mem_storage(), STORAGE::STANDARD); //Not a temp array
    EXPECT_EQ(a.get_mem(), nullptr); //Doesn't point to anything  
    EXPECT_TRUE(a.get_span().empty()); //Span is empty
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTArray, ConstructorSizeZero) {
    ArrOfInt a(0);
    EXPECT_EQ(a.get_mem(), nullptr); //Doesn't point to anything  
    EXPECT_TRUE(a.get_span().empty()); //Span is empty
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTArray, ConstructorSize) {
    ArrOfInt a(10);
    EXPECT_NE(a.get_mem(), nullptr); //Point to something
    EXPECT_EQ(a.get_span().data(), a.get_mem()->data()); //Mem and span describe the same memory
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTArray, ConstructorCopyWithEmptyArray) {
    ArrOfInt a0;
    ArrOfInt a(a0); 
    EXPECT_EQ(a.get_mem(), nullptr); //Doesn't point to anything  
    EXPECT_TRUE(a.get_span().empty()); //Span is empty
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTArray, ConstructorCopyData) {
    ArrOfInt a0(10);
    a0 = 42;
    ArrOfInt a(a0); 

    EXPECT_NE(a.get_mem(), nullptr); //Point to something
    EXPECT_EQ(a.get_span().data(), a.get_mem()->data()); //Mem and span describe the same memory
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
    EXPECT_NE(a.get_span().data(), a0.get_span().data()); //Deep copy: not the same adress in memory
    EXPECT_EQ(a[5], 42); //Value check
}

//Resize
TEST(TRUSTArray, ResizeDefaultToZero) {
  ArrOfInt a;
  a.resize(0); //Do nothing!
  EXPECT_EQ(a.get_mem(), nullptr); //Doesn't point to anything  
  EXPECT_TRUE(a.get_span().empty()); //Span is empty
  EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTArray, ResizeToAllocateEmptyArray) {
  ArrOfInt a;
  a.resize(10); //Should allocate
  EXPECT_NE(a.get_mem(), nullptr); //Point to something
  EXPECT_EQ(a.get_span().data(), a.get_mem()->data()); //Mem and span describe the same memory
  EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
  EXPECT_EQ(a.size_array(),10); //Size is now 10
}
TEST(TRUSTArray, ResizeDoesNotReallocate) {
  ArrOfInt a;
  a.resize(10); //Should allocate
  const int * ptr = a.addr(); //Get pointer
  // Resize down does not reallocate:
  a.resize(5);
  EXPECT_EQ(a.addr(), ptr); //pointer hasn't changed
}

//ref_arr
TEST(TRUSTArray, Ref_arr) {
  ArrOfInt a(10);
  {
    ArrOfInt b;
    b.ref_array(a);
    EXPECT_EQ(b.get_mem()->data(), a.get_mem()->data()); //Both point to the same data
    EXPECT_EQ(a.ref_count(), 2); //a knows two arrays are point towards it's data
    EXPECT_EQ(b.ref_count(), 2); //b knows two arrays are point towards it's data
    EXPECT_EQ(b.size_array(), 10); //Size of b is correct
  } // b is out of scope, destroyed
  EXPECT_EQ(a.ref_count(), 1);//a knows b was destroyed
}
TEST(TRUSTArray, Ref_arrSlice) {
  ArrOfInt a(10);
  {
    ArrOfInt b;
    int start = 2;
    int size = 4;
    b.ref_array(a, start, size);
    EXPECT_EQ(b.get_mem()->data(), a.get_mem()->data()); //Both point to the same data
    EXPECT_EQ(a.ref_count(), 2); //a knows two arrays are point towards it's data
    EXPECT_EQ(b.ref_count(), 2); //b knows two arrays are point towards it's data
    EXPECT_EQ(b.size_array(), 4); //Size of b, the slice, is correct
  } // b is out of scope, destroyed
  EXPECT_EQ(a.ref_count(), 1);//a knows b was destroyed
}
TEST(TRUSTArray, Ref_arrOfref_array) {
  ArrOfInt a(10);
  std::iota(a.addr(), a.addr()+a.size_array(),0);
  ArrOfInt ref1, ref2;
  ref1.ref_array(a, 2);
  ref2.ref_array(ref1, 2);
  EXPECT_EQ(ref2[0], 4);  
}

//ref_data
TEST(TRUSTArray, Ref_data) {
  int * pt_int = new int[10];
  std::fill(pt_int, pt_int + 10, 24);
  {
    ArrOfInt a;
    a.ref_data(pt_int, 10);
    EXPECT_EQ(a.get_mem(), nullptr);
    EXPECT_EQ(a.get_span().data(), pt_int);
  }
  {
    IntTab a;
    a.ref_data(pt_int, 10);
    EXPECT_EQ(a.get_mem(), nullptr);
    EXPECT_EQ(a.get_span().data(), pt_int);
    EXPECT_EQ(a(1), 24);
    a.resize(2,5);
    EXPECT_EQ(a(1, 1), 24);
  }
  delete [] pt_int;
}

//Conversions
TEST(TRUSTArray, RefAsSmall) {
  ArrOfFloat small;
  {
    BigArrOfFloat big(10);
    big = 24;
    big.ref_as_small(small);
  }
  // small should outlive big:
  EXPECT_EQ(small[2], 24.0f);
  EXPECT_EQ(small.size_array(), 10);
}
TEST(TRUSTArray, RefAsBig) {
  BigArrOfTID big;
  {
    ArrOfTID small(10);
    small = 12;
    small.ref_as_big(big);
  }
  // big should outlive small:
  EXPECT_EQ(big[2], 12);
  EXPECT_EQ(big.size_array(), 10);  
}
TEST(TRUSTArray, FromTidToInt) {
  ArrOfInt b;
    {
      BigArrOfTID a(10);
      a = 12;
      a.from_tid_to_int(b);
    }
    EXPECT_EQ(b[2], 12);
    EXPECT_EQ(b.size_array(), 10);
}
TEST(TRUSTArray, FromTidToIntPreservesLineSize) {
  IntTab b;
  { // Line size should be preserved in from_tid_to_int:
    BigTIDTab a(10, 2);
    a = 12;
    a.from_tid_to_int(b);
    EXPECT_EQ(b(2,1), 12);
    EXPECT_EQ(b.dimension(0), a.dimension(0));
    EXPECT_EQ(b.dimension(1), a.dimension(1));
    EXPECT_EQ(b.line_size(), a.line_size());
    }
}

//trav
TEST(TRUSTTrav, ConstructorDefault) {
    IntTrav a;
    EXPECT_EQ(a.get_mem_storage(), STORAGE::TEMP_STORAGE); //Is temp
    EXPECT_EQ(a.get_mem(), nullptr); //Doesn't point to anything  
    EXPECT_TRUE(a.get_span().empty()); //Span is empty
    EXPECT_EQ(a.get_data_location(), DataLocation::HostOnly);
}
TEST(TRUSTTrav, ConstructorCopyFromTrav){
  IntTrav a(10);
  a = 24;
  IntTrav b(a);  // copy only structure not data
  EXPECT_EQ(b.dimension(0),10);
  EXPECT_EQ(b(0), 0);
}
TEST(TRUSTTrav, ConstructorCopyFromTab){
    IntTab a(1,1);
    IntTrav b(a);  // from Tab
    EXPECT_EQ(a.nb_dim(), 2);
    EXPECT_EQ(b.nb_dim(), 2);
}

//TEST(TRUSTTrav, UnproperUse){
    // NOT A PROPER USAGE OF TRAV!! Should exit in Debug with strategy 2 - to be activated
    // once PolyMAC has been cleaned.
    //IntTrav a;
    //for (int i=0; i < 3000; i++){a.append_array(1);}
//}

TEST(TRUSTTrav, ReuseBlock) {
  int * ptr;
  {
    IntTrav a;
    EXPECT_EQ(a.get_mem_storage(), STORAGE::TEMP_STORAGE); //Is temp
    a.resize(10);
    ptr = a.get_mem()->data();
    EXPECT_EQ(a.size_array(), 10);// Correct size
  }

  { // Same block re-used for Trav
    IntTrav a(10);
    EXPECT_EQ(ptr, a.get_mem()->data());
  }
}
TEST(TRUSTTrav, append) {
  IntTrav a(10);
  a.append_array(5);
  EXPECT_EQ(a.size_array(),11);
}
TEST(TRUSTTrav, TabCanBecomeTravViaRef) {
  DoubleTrav a(10);
  DoubleVect b;
  EXPECT_EQ(b.get_mem_storage(), STORAGE::STANDARD);
  b.ref(a);
  EXPECT_EQ(b.get_mem_storage(), STORAGE::TEMP_STORAGE);
}

TEST(TRUSTTab, dim_int) {
  BigIntTab toto(2,3);
  trustIdType d0 = toto.dimension(0);
  // Below is the nice way to get an int for higher dimensions on BigTabs:
  // it is cleaner than doing a wild cast like
  //    int d1 = (int)toto.dimension(1);
  // and it will check for overflow with an assert.
  int d1 = toto.dimension_int(1);

  EXPECT_EQ(d0, 2);
  EXPECT_EQ(d1, 3);

  IntTab toto2(2,3);
  int d0_ = toto2.dimension(0);
  int d1_ = toto2.dimension(1);

  EXPECT_EQ(d0_, 2);
  EXPECT_EQ(d1_, 3);
}

TEST(MDVector, dump_restore) {
  MD_Vector md;
  MD_Vector_seq mdvs(6);
  md.copy(mdvs);

  SChaine out;
  DoubleVect arr(6);
  arr.set_md_vector(md);

  MD_Vector_tools::dump_vector_with_md(arr, out);

  EChaine in(out.get_str());
  DoubleVect arr2;
  MD_Vector_tools::restore_vector_with_md(arr2, in);

  EXPECT_EQ(arr2.size_totale() , 6);
  EXPECT_EQ(arr2.get_md_vector()->get_nb_items_tot(), 6);
}

