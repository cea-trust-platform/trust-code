/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Echange_EV_Options.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Echange_EV_Options_included
#define Echange_EV_Options_included

class Echange_EV_Options
{
public:
  // SYNC is the default: the owner of the shared value sends the
  //  value to other processors
  // SUM: the owner of a shared value receives values from other processors
  //  that own the same value (either item_commun or in virtual space)
  //  and computes the sum. The user might want to SYNCHRONIZE afterwards
  // MAX: same as SUM, but the owner computes the max for all processors
  //  that share the data
  // MAXCOL1: only meaningful for arrays with line_size > 1. For each shared "line"
  //  of the multidimensionnal array, take the processor for which the first value on the line
  //  is the highest and copy the whole line to the processor that owns the item.
  enum Operation { SYNC, SUM, MAX, MINCOL1 };
  enum Items_Communs { ITEMS_COMMUNS_YES, ITEMS_COMMUNS_NO };
  enum Items_Virtuels { ITEMS_VIRTUELS_YES, ITEMS_VIRTUELS_NO };
  Echange_EV_Options(Operation op = SYNC,
                     Items_Communs items_communs = ITEMS_COMMUNS_YES,
                     Items_Virtuels items_virtuels = ITEMS_VIRTUELS_YES);
  Operation get_op() const
  {
    return op_;
  }
  Items_Communs get_items_communs() const
  {
    return items_communs_;
  }
  Items_Virtuels get_items_virtuels() const
  {
    return items_virtuels_;
  }
  int operator==(const Echange_EV_Options& x) const
  {
    return op_==x.op_ && items_communs_==x.items_communs_ && items_virtuels_==x.items_virtuels_;
  }
protected:
  Operation op_;
  // Flag: process items_communs or not ?
  Items_Communs items_communs_;
  Items_Virtuels items_virtuels_;
};

extern const Echange_EV_Options& echange_ev_opt_default;
#endif

