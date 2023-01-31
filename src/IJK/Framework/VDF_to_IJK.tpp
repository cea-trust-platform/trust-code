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
#ifndef VDF_to_IJK_H_TPP
#define VDF_to_IJK_H_TPP
#include <VDF_to_IJK.h>
#include <Schema_Comm_Vecteurs.h>
#include <Schema_Comm.h>
#include <communications.h>
#include <Statistiques.h>
//#include <Domaine_VF.h>
#include <IJK_Field.h>
#include <TRUSTTabs.h>
#include <Array_tools.h>
#include <Domaine.h>

template <typename _TYPE_, typename _TYPE_ARRAY_>
void VDF_to_IJK::convert_to_ijk(const DoubleVect& x, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x) const
{
  Schema_Comm_Vecteurs sch;
  int i;
  sch.begin_init();

  const int nsend = pe_send_.size_array();
  for (i = 0; i < nsend; i++)
    sch.add_send_area_template<double>(pe_send_[i], pe_send_data_.get_list_size(i));

  const int nrecv = pe_recv_.size_array();
  for (i = 0; i < nrecv; i++)
    sch.add_recv_area_template<double>(pe_recv_[i], pe_recv_data_.get_list_size(i));

  sch.end_init();

  sch.begin_comm();
  for (i = 0; i < nsend; i++)
    {
      const int ndata = pe_send_data_.get_list_size(i);
      ArrOfDouble& buf = sch.get_next_area_template<double>(pe_send_[i], ndata);
      for (int j = 0; j < ndata; j++)
        {
          const int index = pe_send_data_(i, j);
          buf[j] = x[index];
        }
    }
  sch.exchange();
  for (i = 0; i < nrecv; i++)
    {
      const int ndata = pe_recv_data_.get_list_size(i);
      const ArrOfDouble& buf = sch.get_next_area_template<double>(pe_recv_[i], ndata);
      for (int j = 0; j < ndata; j++)
        {
          const int index = pe_recv_data_(i, j);
          const int ii = index % ijk_ni_;
          const int jj = (index / ijk_ni_) % ijk_nj_;
          const int kk = (index / ijk_ni_) / ijk_nj_;
          ijk_x(ii,jj,kk) = (_TYPE_)buf[j];
        }
    }
  sch.end_comm();
}

template <typename _TYPE_, typename _TYPE_ARRAY_>
void VDF_to_IJK::convert_from_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& ijk_x, DoubleVect& x, bool add) const
{
  Schema_Comm_Vecteurs sch;
  sch.begin_init();
  int i;
  const int nsend = pe_send_.size_array();
  for (i = 0; i < nsend; i++)
    sch.add_recv_area_template<double>(pe_send_[i], pe_send_data_.get_list_size(i));

  const int nrecv = pe_recv_.size_array();
  for (i = 0; i < nrecv; i++)
    sch.add_send_area_template<double>(pe_recv_[i], pe_recv_data_.get_list_size(i));

  sch.end_init();

  sch.begin_comm();
  for (i = 0; i < nrecv; i++)
    {
      const int ndata = pe_recv_data_.get_list_size(i);
      ArrOfDouble& buf = sch.get_next_area_template<double>(pe_recv_[i], ndata);
      for (int j = 0; j < ndata; j++)
        {
          const int index = pe_recv_data_(i, j);
          const int ii = index % ijk_ni_;
          const int jj = (index / ijk_ni_) % ijk_nj_;
          const int kk = (index / ijk_ni_) / ijk_nj_;
          buf[j] = ijk_x(ii, jj, kk);
        }
    }
  sch.exchange();
  for (i = 0; i < nsend; i++)
    {
      const int ndata = pe_send_data_.get_list_size(i);
      const ArrOfDouble& buf = sch.get_next_area_template<double>(pe_send_[i], ndata);
      if (add)
        {
          for (int j = 0; j < ndata; j++)
            {
              const int index = pe_send_data_(i, j);
              x[index] += buf[j];
            }
        }
      else
        {
          for (int j = 0; j < ndata; j++)
            {
              const int index = pe_send_data_(i, j);
              x[index] = buf[j];
            }
        }

    }
  sch.end_comm();
}



#endif

