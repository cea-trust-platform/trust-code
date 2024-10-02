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

#include <TRUSTArray.h>
#include <TRUSTVect.h>
#include <TRUSTTab.h>
#include <TRUSTTrav.h>

#include <MD_Vector_std.h>

#include <assert.h>
#include <numeric>

#ifdef __NVCOMPILER
#pragma diag_suppress 177
#endif

void init_parallel_vect()
{
  int np = Process::nproc();
  int rk = Process::me();

  if (np != 2) return;

  ArrOfInt pe_vois(1);
  ArrsOfInt items_to_send(1);
  ArrsOfInt items_to_recv(1);
  ArrsOfInt bloc_to_recv(1);

  if (rk==0)
    {
      pe_vois[0] = 1;
      ArrOfInt& item = items_to_send[0];
      item.resize(2);
      item(0) = 2;
      item(1) = 3;

      //Receives one item as bloc
      ArrOfInt& block = bloc_to_recv[0];
      block.resize(2);
      block(0) = 4;  // block start
      block(1) = 5;  // block end - excluded

      //Receives one item as a single item
      ArrOfInt& item_r = items_to_recv[0];
      item_r.resize(1);
      item_r(0) = 5;

    }
  else // rk == 1
    {
      pe_vois[0] = 0;
      ArrOfInt& item = items_to_send[0];
      item.resize(2);
      item(0) = 0;
      item(1) = 1;

      //Receives no bloc
//      ArrOfInt& block = bloc_to_recv[0];
//      block.resize(2);
//      block(0) = 0;  // block start
//      block(1) = 2;  // block end - excluded

      //Receives two items as items
      ArrOfInt& item_r = items_to_recv[0];
      item_r.resize(2);
      item_r(0) = 4;
      item_r(1) = 5;
    }

  //Bloc and item reception should not overlap
  //Virtual items are always at the end of md_vector

    MD_Vector mdv;
    MD_Vector_std mds(6, 4, pe_vois, items_to_send, items_to_recv, bloc_to_recv);
    mdv.copy(mds);
    IntVect v(6);
    v.set_md_vector(mdv);

    // check - resize array?
    // Tester echange_espoace_virtuel()
    // Tester toutes les fonction block

  int res = mp_min_vect(v);

}

/* A simple 2 procs test */
TEST(MPI, Simple2procs) {

  declare_stat_counters();

  init_parallel_vect();

}

#ifdef __NVCOMPILER
#pragma diag_default 177
#endif


