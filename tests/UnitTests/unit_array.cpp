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

#include <TRUSTArray.h>
#include <TRUSTVect.h>
#include <TRUSTTab.h>
#include <TRUSTTrav.h>

#include <SChaine.h>
#include <EChaine.h>
#include <MD_Vector_seq.h>

#include <assert.h>
#include <numeric>

/*! Unit tests for arrays - To be run in debug mode!!
 */

class TestTRUSTArray
{
public:

  void test_ctor();
  void test_resize();
  void test_ref_arr();
  void test_ref_data();
  void test_trav();
  void test_conversion();
  void test_dim_64();
  void test_md_vect();
};


void TestTRUSTArray::test_ctor()
{
  ArrOfInt a1;
  assert(a1.storage_type_ == STORAGE::STANDARD);
  assert(a1.mem_ == nullptr);
  assert(a1.span_.empty());
  assert(a1.data_location_ == nullptr);

  ArrOfInt a2(0);
  assert(a2.mem_ == nullptr);
  assert(a2.span_.empty());
  assert(a2.data_location_ == nullptr);

  ArrOfInt a3(10);
  assert(a3.mem_ != nullptr);
  assert(a3.span_.data() == a3.mem_->data());
  assert(*a3.data_location_ == DataLocation::HostOnly);

  // Copy ctor
  ArrOfInt a4(a1);
  assert(a4.mem_ == nullptr);
  assert(a4.span_.empty());
  assert(a4.data_location_ == nullptr);

  a3 = 24;
  ArrOfInt a5(a3);
  assert(a5.mem_ != nullptr);
  assert(a5.span_.data() == a5.mem_->data());
  assert(*a5.data_location_ == DataLocation::HostOnly);
  assert(a5.span_.data() != a3.span_.data());
  assert(a5[5] == 24);
}

void TestTRUSTArray::test_resize()
{
  ArrOfInt a1;
  a1.resize(0);  // Do nothing!
  assert(a1.mem_ == nullptr);
  assert(a1.span_.empty());
  assert(a1.data_location_ == nullptr);

  ArrOfInt a3;
  a3.resize(10);
  assert(a3.mem_ != nullptr);
  assert(a3.span_.data() == a3.mem_->data());
  assert(*a3.data_location_ == DataLocation::HostOnly);
  assert(a3.size_array() == 10);
  const int * ptr = a3.addr();

  // Resize down does not reallocate:
  a3.resize(5);
  assert(a3.addr() == ptr);
}

void TestTRUSTArray::test_ref_arr()
{
  ArrOfInt a(10);
  {
    ArrOfInt b;
    b.ref_array(a);
    assert(b.mem_->data() == a.mem_->data());
    assert(a.ref_count() == 2);
    assert(b.ref_count() == 2);
    assert(b.size_array() == 10);
  }
  assert(a.ref_count() == 1);

  {
    ArrOfInt b;
    b.ref_array(a, 2 , 4);
    assert(a.ref_count() == 2);
    assert(b.ref_count() == 2);
    assert(b.size_array() == 4);
  }
  assert(a.ref_count() == 1);
  
  {
    // Non regression : ref_array of ref_array were improperly set up:
    ArrOfInt b(10);
    std::iota(b.addr(), b.addr()+b.size_array(),0);
    ArrOfInt ref1, ref2;
    ref1.ref_array(b, 2);
    ref2.ref_array(ref1, 2);
    assert(ref2[0] == 4);  // was 2!!
  }
}

void TestTRUSTArray::test_ref_data()
{
  int * pt_int = new int[10];
  std::fill(pt_int, pt_int + 10, 24);
  {
    ArrOfInt a;
    a.ref_data(pt_int, 10);
    assert(a.mem_ == nullptr);
    assert(a.span_.data() == pt_int);
  }
  {
    IntTab a;
    a.ref_data(pt_int, 10);
    assert(a.mem_ == nullptr);
    assert(a.span_.data() == pt_int);
    assert(a(1) == 24);
    a.resize(2,5);
    assert(a(1, 1) == 24);
  }
    
  delete [] pt_int;
}

void TestTRUSTArray::test_trav()
{
  IntTrav a1;
  assert(a1.storage_type_ == STORAGE::TEMP_STORAGE);
  assert(a1.mem_ == nullptr);
  assert(a1.span_.empty());
  assert(a1.data_location_ == nullptr);

  int * ptr;
  {
    IntTrav a2;
    assert(a2.storage_type_ == STORAGE::TEMP_STORAGE);
    a2.resize(10);
    ptr = a2.mem_->data();
    assert(a2.size_array() == 10);
  }
  { // Same block re-used for Trav
    IntTrav a2(10);
    assert(ptr == a2.mem_->data());
  }
  { // Same block re-used for Trav
    IntTrav a2(10);
    a2.append_array(5);
    assert(a2.size_array() == 11);
  }

  {
    IntTrav a2(10);
    a2 = 24;
    IntTrav b(a2);  // copy only structure not data
    assert(b.dimension(0) == 10);
    assert(b(0) == 0);
  }

  { // Copy ctor from other Trav
    IntTab a(1,1);
    IntTrav b(a);  // from Tab
    IntTrav c(b);  // from Trav

    assert(a.nb_dim() == 2);
    assert(b.nb_dim() == 2);
    assert(c.nb_dim() == 2);
  }

  {
    // NOT A PROPER USAGE OF TRAV!! Should exit in Debug with strategy 2 - to be activated
    // once PolyMAC has been cleaned.
//    IntTrav a2;
//    for (int i=0; i < 3000; i++)
//       a2.append_array(1);
  }
  {  // A normal tab might become a Trav via ref():
    DoubleTrav a(10);
    DoubleVect b;
    assert(b.get_mem_storage() == STORAGE::STANDARD);
    b.ref(a);
    assert(b.get_mem_storage() == STORAGE::TEMP_STORAGE);
  }
  
}


void TestTRUSTArray::test_conversion()
{
  // Test for big / small conversion
  {
    ArrOfFloat small;
    {
      BigArrOfFloat big(10);
      big = 24;
      big.ref_as_small(small);
    }
    // small should outlive big:
    assert(small[2] == 24);
    assert(small.size_array() == 10);
  }
  { // The other way around:
    BigArrOfTID big;
    {
      ArrOfTID small(10);
      small = 12;
      small.ref_as_big(big);
    }
    // big should outlive small:
    assert(big[2] == 12);
    assert(big.size_array() == 10);
  }
  {
    ArrOfInt b;
    {
      BigArrOfTID a(10);
      a = 12;
      a.from_tid_to_int(b);
    }
    assert(b[2] == 12);
    assert(b.size_array() == 10);
  }
  // Line size should be preserved in from_tid_to_int:
  {
    IntTab b;
    {
      BigTIDTab a(10, 2);
      a = 12;
      a.from_tid_to_int(b);
      assert(b(2,1) == 12);
      assert(b.dimension(0) == a.dimension(0));
      assert(b.dimension(1) == a.dimension(1));
      assert(b.line_size() == a.line_size());
    }
  }

}

void TestTRUSTArray::test_dim_64()
{
  BigIntTab toto(2,3);
  trustIdType d0 = toto.dimension(0);
  // Below is the nice way to get an int for higher dimensions on BigTabs:
  // it is cleaner than doing a wild cast like
  //    int d1 = (int)toto.dimension(1);
  // and it will check for overflow with an assert.
  int d1 = toto.dimension_int(1);

  assert(d0==2 && d1==3);

  IntTab toto2(2,3);
  int d0_ = toto2.dimension(0);
  int d1_ = toto2.dimension(1);
  assert(d0_==2 && d1_==3);
}

void TestTRUSTArray::test_md_vect()
{
  MD_Vector md;
  MD_Vector_seq mdvs(6);
  md.copy(mdvs);

  SChaine out;
  DoubleVect arr(6);
  arr.set_md_vector(md);

  MD_Vector_tools::dump_vector_with_md(arr, out);

  //  std::cout << out.get_str() << endl;

  EChaine in(out.get_str());
  DoubleVect arr2;
  MD_Vector_tools::restore_vector_with_md(arr2, in);
  assert(arr2.size_totale() == 6);
  assert(arr2.get_md_vector()->get_nb_items_tot() == 6);
}

/*! Not great, we just rely on 'assert' for now ... one day Google Test or something
 * similar ...
 */
int main(int argc, char ** argv)
{
#ifdef _OPENMP_TARGET
  Kokkos::initialize(argc, argv);
#endif

  TestTRUSTArray tta;

  tta.test_ctor();
  tta.test_resize();
  tta.test_ref_arr();
  tta.test_ref_data();
  tta.test_trav();
  tta.test_conversion();
  tta.test_dim_64();
  tta.test_md_vect();

#ifdef _OPENMP
  Kokkos::finalize();
#endif

  return 0;
}

