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
// File:        MD_Vector_composite.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MD_Vector_composite_included
#define MD_Vector_composite_included
#include <MD_Vector_std.h>
#include <VectMD_Vector.h>
#include <DoubleTabs.h>

// .DESCRIPTION
//  Metadata for a distributed composite vector.
//  This class describes a vector that contains one or more sub-vectors (see P1bubble arrays),
//  each sub-vector has the same shape as the base vector, plus one dimension if shapes_[i] > 0
//  See also class DoubleTab_Parts and class ConstDoubleTab_Parts
class MD_Vector_composite : public MD_Vector_base2
{
  Declare_instanciable(MD_Vector_composite);
public:
  void add_part(const MD_Vector& part, int shape = 0);

  int nb_parts() const
  {
    return data_.size();
  }
  const MD_Vector& get_desc_part(int i) const
  {
    return data_[i];
  }
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const;
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const;

  VECT(MD_Vector) data_;
  // Descriptor for the entire vector:
  MD_Vector_std global_md_;
  // Start index of the n-th part,
  // parts_offsets_[j] = SUM(i=0..j-1) of ( data_[i].valeure().nb_items_tot() * shapes[i] )
  ArrOfInt parts_offsets_;
  // Shape of each part of the array (see class DoubleTab_Parts)
  ArrOfInt shapes_;
};

#endif
