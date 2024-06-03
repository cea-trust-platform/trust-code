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

#ifndef IJ_layout_included
#define IJ_layout_included


/*! @brief : This class describes an IJ plane of an IJK_Field_local.
 *
 * It just encapsulates the offset computation: j * j_stride_ + i
 *   with bound checking in debug... This class is usefull to safely optimize the code !
 *
 */
class IJ_layout
{
public:
  int ni() const { return ni_; }
  int nj() const { return nj_; }
  int ghost() const { return ghost_; }
  int j_stride() const { return j_stride_; }

  template <typename T>
  IJ_layout(const T& f) : ni_(f.ni()), nj_(f.nj()), ghost_(f.ghost()), j_stride_(f.j_stride()) { }

  // origin points to x(0,0,k) for an IJK_Field
  template <typename T>
  T& operator()(T *origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }

  // origin points to x(0,0,k) for an IJK_Field
  template <typename T>
  const T& operator()(const T *origin, int i, int j) const
  {
    assert(i >= -ghost_ && i < ni_ + ghost_);
    assert(j >= -ghost_ && j < nj_ + ghost_);
    return origin[j * j_stride_ + i];
  }

  // linear index of cell (i=0,j=0) is zero origin points to x(0,0,k) for an IJK_Field
  template <typename T>
  T& linear(T *origin, int linear_index) const
  {
    assert(linear_index >= -ghost_ * (j_stride_ + 1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }

  template <typename T>
  const T& linear(const T *origin, int linear_index) const
  {
    assert(linear_index >= -ghost_ * (j_stride_ + 1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    return origin[linear_index];
  }

  void linear_to_ij(int linear_index, int& i, int& j) const
  {
    assert(linear_index >= -ghost_ * (j_stride_ + 1));
    assert(linear_index < (ni_ + ghost_) + j_stride_ * (nj_ + ghost_ - 1));
    int l = linear_index + ghost_ * (j_stride_ + 1);
    j = (l / j_stride_) - ghost_;
    i = (l % j_stride_) - ghost_;
  }

  // ghost_ is the number of ghost cells allocated in memory in I and J directions
  int ni_, nj_, ghost_, j_stride_;
};


#endif /* IJ_layout_included */
