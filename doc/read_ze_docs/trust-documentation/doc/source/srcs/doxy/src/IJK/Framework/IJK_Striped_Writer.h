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

#ifndef IJK_Striped_Writer_included
#define IJK_Striped_Writer_included

#include <IJK_Field.h>

/*! @brief : class IJK_Striped_Writer
 *
 * Converts the input field to a linear lexicographic file written with optimized striping for lustre filesystem
 * (splits the data into chunks of size "stripesize", give the chunks to different nodes of the MPI job and each
 *  node writes some of the chunks to maximize bandwidth).
 * striping not yet implemented, all data is collected on the master node !
 */
class IJK_Striped_Writer
{
public:
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_parallele_plan_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_parallele_plan_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_parallel_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  long long write_data_parallel_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f);
  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  void write_data_parallel2_template(const char * filename,
                                     const int file_ni_tot, const int file_nj_tot, const int file_nk_tot,
                                     const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz);

  template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  void redistribute(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& input, TRUSTArray<_OUT_TYPE_>& output,
                    const int nitot, const int njtot, const int nktot, const int nbcompo, int component);
  template<typename _IN_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
  void redistribute_load(const TRUSTArray<_IN_TYPE_>& input, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& output,
                         const int nitot, const int njtot, const int nktot, const int nbcompo, const int component);

};

#include <IJK_Striped_Writer.tpp>
#endif
