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

};


void TestTRUSTArray::test_ctor()
{
  // Int ctor
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
  }
  { // Same block re-used for Trav
    IntTrav a2(10);
    assert(ptr == a2.mem_->data());
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

}


/*! Not great, we just rely on 'assert' for now ... one day Google Test or something
 * similar ...
 */
int main()
{
  TestTRUSTArray tta;

  tta.test_ctor();
  tta.test_resize();
  tta.test_ref_arr();
  tta.test_ref_data();
  tta.test_trav();

  return 0;
}

