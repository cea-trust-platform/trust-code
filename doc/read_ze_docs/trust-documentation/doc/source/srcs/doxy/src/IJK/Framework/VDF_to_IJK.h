/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef VDF_to_IJK_included
#define VDF_to_IJK_included
class Domaine_VF;
class IJK_Splitting;

#include <IJK_Field_forward.h>
#include <TRUSTTabs_forward.h>
#include <Static_Int_Lists.h>
#include <IJK_Splitting.h>

// This class provides methods to convert a vdf field to an ijk field and reversed
class VDF_to_IJK
{
public:
  void initialize(const Domaine_VF& domaine_vf, const IJK_Splitting& splitting,
                  IJK_Splitting::Localisation localisation,
                  int direction_for_x,
                  int direction_for_y,
                  int direction_for_z);
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void convert_from_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x, DoubleVect& x, bool add = false) const;
  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void convert_to_ijk(const DoubleVect& x, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x) const;

protected:
  // List of processors to which vdf data will be sent
  ArrOfInt pe_send_;
  // List of processors from which vdf data will be received
  ArrOfInt pe_recv_;
  // Size of received data
  ArrOfInt pe_recv_size_;
  // For each processor to which vdf data is sent, list of indices
  //  in the vdf field to send to the processor, ordered in
  //  ascending order of the local storage on the destination processor
  Static_Int_Lists pe_send_data_;
  Static_Int_Lists pe_recv_data_;



  // Local size of the ijk_field
  int ijk_ni_;
  int ijk_nj_;
};

#include <VDF_to_IJK.tpp>

#endif
