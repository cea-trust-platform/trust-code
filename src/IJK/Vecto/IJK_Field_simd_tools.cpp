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
#include <IJK_Field_simd_tools.h>

void  ConstIJK_float_ptr::get_center(int i_offset, float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  center = ptr_[i_offset];
}
/*! @brief returns left=field(i+i_offset-1, j, k) and center=field(i+i_offset, j, k)
 *
 */
void ConstIJK_float_ptr::get_left_center_x(int i_offset, float& left, float& center) const
{
  assert(i_ + i_offset > i_min_ && i_ + i_offset < i_max_);
  left = ptr_[i_offset - 1];
  center = ptr_[i_offset];
}
void ConstIJK_float_ptr::get_center_right_x(int i_offset, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + 1];
}
void ConstIJK_float_ptr::get_left_center_right_x(int i_offset, float& left, float& center, float& right) const
{
  assert(i_ + i_offset > i_min_ && i_ + i_offset + 1 < i_max_);
  left = ptr_[i_offset - 1];
  center = ptr_[i_offset];
  right = ptr_[i_offset + 1];
}
void ConstIJK_float_ptr::get_left_center_y(int i_offset, float& left, float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - j_stride_];
}
void ConstIJK_float_ptr::get_center_right_y(int i_offset, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ >= j_min_ && j_ + 1 <= j_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + j_stride_];
}
void ConstIJK_float_ptr::get_left_center_right_y(int i_offset, float& left, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - j_stride_];
  right = ptr_[i_offset + j_stride_];
}
void ConstIJK_float_ptr::get_left_center_z(int i_offset, float& left, float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - k_stride_];
}
void ConstIJK_float_ptr::get_center_right_z(int i_offset, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ + 1 < k_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + k_stride_];
}
void ConstIJK_float_ptr::get_left_center_right_z(int i_offset, float& left, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_ && k_ + 1 < k_max_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - k_stride_];
  right = ptr_[i_offset + k_stride_];
}
void ConstIJK_float_ptr::get_center(int i_offset, Simd_float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  center = SimdGet(ptr_ + i_offset);
}
void ConstIJK_float_ptr::get_left_center_x(int i_offset, Simd_float& left, Simd_float& center) const
{
  assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset < i_max_);
  SimdGetLeftCenter(ptr_ + i_offset, left, center);
}
void ConstIJK_float_ptr::get_center_right_x(int i_offset, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetCenterRight(ptr_ + i_offset, center, right);
}
void ConstIJK_float_ptr::get_left_center_right_x(int i_offset, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetLeftCenterRight(ptr_ + i_offset, left, center, right);
}
void ConstIJK_float_ptr::get_left_center_y(int i_offset, Simd_float& left, Simd_float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - j_stride_);
}
void ConstIJK_float_ptr::get_center_right_y(int i_offset, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ + 1 <= j_max_);
  center = SimdGet(ptr_ + i_offset);
  right = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_float_ptr::get_left_center_right_y(int i_offset, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - j_stride_);
  right = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_float_ptr::get_left_center_z(int i_offset, Simd_float& left, Simd_float& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - k_stride_);
}
void ConstIJK_float_ptr::get_center_right_z(int i_offset, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ + 1 <= k_max_);
  center = SimdGet(ptr_ + i_offset);
  right = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_float_ptr::get_left_center_right_z(int i_offset, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_ && k_ + 1 <= k_max_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - k_stride_);
  right = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_float_ptr::get_leftleft_left_center_right_x(int i_offset, Simd_float& leftleft, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetLeftleftLeftCenterRight(ptr_ + i_offset, leftleft, left, center, right);
}
void ConstIJK_float_ptr::get_leftleft_left_center_right_y(int i_offset, Simd_float& leftleft, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
  leftleft = SimdGet(ptr_ + i_offset - 2 * j_stride_);
  left     = SimdGet(ptr_ + i_offset - j_stride_);
  center   = SimdGet(ptr_ + i_offset);
  right    = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_float_ptr::get_leftleft_left_center_right_z(int i_offset, Simd_float& leftleft, Simd_float& left, Simd_float& center, Simd_float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
  leftleft = SimdGet(ptr_ + i_offset - 2 * k_stride_);
  left     = SimdGet(ptr_ + i_offset - k_stride_);
  center   = SimdGet(ptr_ + i_offset);
  right    = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_float_ptr::get_leftleft_left_center_right_x(int i_offset, float& leftleft, float& left, float& center, float& right) const
{
  assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
  leftleft = ptr_[i_offset-2];
  left     = ptr_[i_offset-1];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+1];
}
void ConstIJK_float_ptr::get_leftleft_left_center_right_y(int i_offset, float& leftleft, float& left, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
  leftleft = ptr_[i_offset-2*j_stride_];
  left     = ptr_[i_offset-j_stride_];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+j_stride_];

}
void ConstIJK_float_ptr::get_leftleft_left_center_right_z(int i_offset, float& leftleft, float& left, float& center, float& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
  leftleft = ptr_[i_offset-2*k_stride_];
  left     = ptr_[i_offset-k_stride_];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+k_stride_];
}

void ConstIJK_float_ptr::get_left_center_xy(int i_offset, Simd_float& leftx_lefty, Simd_float& leftx_centery, Simd_float& centerx_lefty, Simd_float& centerx_centery)
{
  SimdGetLeftCenter(ptr_ + i_offset - j_stride_, leftx_lefty, centerx_lefty);
  SimdGetLeftCenter(ptr_ + i_offset, leftx_centery, centerx_centery);
}
void ConstIJK_float_ptr::get_left_center_xz(int i_offset, Simd_float& leftx_leftz, Simd_float& leftx_centerz, Simd_float& centerx_leftz, Simd_float& centerx_centerz)
{
  SimdGetLeftCenter(ptr_ + i_offset - k_stride_, leftx_leftz, centerx_leftz);
  SimdGetLeftCenter(ptr_ + i_offset, leftx_centerz, centerx_centerz);
}

void ConstIJK_float_ptr::get_left_center_yx(int i_offset, Simd_float& lefty_leftx, Simd_float& lefty_centerx, Simd_float& centery_leftx, Simd_float& centery_centerx)
{
  SimdGetLeftCenter(ptr_ + i_offset - j_stride_, lefty_leftx, lefty_centerx);
  SimdGetLeftCenter(ptr_ + i_offset, centery_leftx, centery_centerx);
}
void ConstIJK_float_ptr::get_left_center_yz(int i_offset, Simd_float& lefty_leftz, Simd_float& lefty_centerz, Simd_float& centery_leftz, Simd_float& centery_centerz)
{
  lefty_leftz = SimdGet(ptr_ + i_offset - j_stride_ - k_stride_);
  lefty_centerz = SimdGet(ptr_ + i_offset - j_stride_);
  centery_leftz = SimdGet(ptr_ + i_offset - k_stride_);
  centery_centerz = SimdGet(ptr_ + i_offset);
}
void ConstIJK_float_ptr::get_left_center_zx(int i_offset, Simd_float& leftz_leftx, Simd_float& leftz_centerx, Simd_float& centerz_leftx, Simd_float& centerz_centerx)
{
  SimdGetLeftCenter(ptr_ + i_offset - k_stride_, leftz_leftx, leftz_centerx);
  SimdGetLeftCenter(ptr_ + i_offset, centerz_leftx, centerz_centerx);
}
void ConstIJK_float_ptr::get_left_center_zy(int i_offset, Simd_float& leftz_lefty, Simd_float& leftz_centery, Simd_float& centerz_lefty, Simd_float& centerz_centery)
{
  leftz_lefty = SimdGet(ptr_ + i_offset - j_stride_ - k_stride_);
  leftz_centery = SimdGet(ptr_ + i_offset - k_stride_);
  centerz_lefty = SimdGet(ptr_ + i_offset - j_stride_);
  centerz_centery = SimdGet(ptr_ + i_offset);
}

/*! @brief returns field(i+i_offset, j, k)
 *
 */
void ConstIJK_double_ptr::get_center(int i_offset, double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  center = ptr_[i_offset];
}
/*! @brief returns left=field(i+i_offset-1, j, k) and center=field(i+i_offset, j, k)
 *
 */
void ConstIJK_double_ptr::get_left_center_x(int i_offset, double& left, double& center) const
{
  assert(i_ + i_offset > i_min_ && i_ + i_offset < i_max_);
  left = ptr_[i_offset - 1];
  center = ptr_[i_offset];
}
void ConstIJK_double_ptr::get_center_right_x(int i_offset, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + 1];
}
void ConstIJK_double_ptr::get_left_center_right_x(int i_offset, double& left, double& center, double& right) const
{
  assert(i_ + i_offset > i_min_ && i_ + i_offset + 1 < i_max_);
  left = ptr_[i_offset - 1];
  center = ptr_[i_offset];
  right = ptr_[i_offset + 1];
}
void ConstIJK_double_ptr::get_left_center_y(int i_offset, double& left, double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - j_stride_];
}
void ConstIJK_double_ptr::get_center_right_y(int i_offset, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ >= j_min_ && j_ + 1 <= j_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + j_stride_];
}
void ConstIJK_double_ptr::get_left_center_right_y(int i_offset, double& left, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - j_stride_];
  right = ptr_[i_offset + j_stride_];
}
void ConstIJK_double_ptr::get_left_center_z(int i_offset, double& left, double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - k_stride_];
}
void ConstIJK_double_ptr::get_center_right_z(int i_offset, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ + 1 < k_max_);
  center = ptr_[i_offset];
  right = ptr_[i_offset + k_stride_];
}
void ConstIJK_double_ptr::get_left_center_right_z(int i_offset, double& left, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_ && k_ + 1 < k_max_);
  center = ptr_[i_offset];
  left = ptr_[i_offset - k_stride_];
  right = ptr_[i_offset + k_stride_];
}
void ConstIJK_double_ptr::get_center(int i_offset, Simd_double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  center = SimdGet(ptr_ + i_offset);
}
void ConstIJK_double_ptr::get_left_center_x(int i_offset, Simd_double& left, Simd_double& center) const
{
  assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset < i_max_);
  SimdGetLeftCenter(ptr_ + i_offset, left, center);
}
void ConstIJK_double_ptr::get_center_right_x(int i_offset, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetCenterRight(ptr_ + i_offset, center, right);
}
void ConstIJK_double_ptr::get_left_center_right_x(int i_offset, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetLeftCenterRight(ptr_ + i_offset, left, center, right);
}
void ConstIJK_double_ptr::get_left_center_y(int i_offset, Simd_double& left, Simd_double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - j_stride_);
}
void ConstIJK_double_ptr::get_center_right_y(int i_offset, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ + 1 <= j_max_);
  center = SimdGet(ptr_ + i_offset);
  right = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_double_ptr::get_left_center_right_y(int i_offset, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - j_stride_);
  right = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_double_ptr::get_left_center_z(int i_offset, Simd_double& left, Simd_double& center) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - k_stride_);
}
void ConstIJK_double_ptr::get_center_right_z(int i_offset, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ + 1 <= k_max_);
  center = SimdGet(ptr_ + i_offset);
  right = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_double_ptr::get_left_center_right_z(int i_offset, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
  assert(k_ - 1 >= k_min_ && k_ + 1 <= k_max_);
  center = SimdGet(ptr_ + i_offset);
  left = SimdGet(ptr_ + i_offset - k_stride_);
  right = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_double_ptr::get_leftleft_left_center_right_x(int i_offset, Simd_double& leftleft, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
  SimdGetLeftleftLeftCenterRight(ptr_ + i_offset, leftleft, left, center, right);
}
void ConstIJK_double_ptr::get_leftleft_left_center_right_y(int i_offset, Simd_double& leftleft, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
  leftleft = SimdGet(ptr_ + i_offset - 2 * j_stride_);
  left     = SimdGet(ptr_ + i_offset - j_stride_);
  center   = SimdGet(ptr_ + i_offset);
  right    = SimdGet(ptr_ + i_offset + j_stride_);
}
void ConstIJK_double_ptr::get_leftleft_left_center_right_z(int i_offset, Simd_double& leftleft, Simd_double& left, Simd_double& center, Simd_double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
  leftleft = SimdGet(ptr_ + i_offset - 2 * k_stride_);
  left     = SimdGet(ptr_ + i_offset - k_stride_);
  center   = SimdGet(ptr_ + i_offset);
  right    = SimdGet(ptr_ + i_offset + k_stride_);
}
void ConstIJK_double_ptr::get_leftleft_left_center_right_x(int i_offset, double& leftleft, double& left, double& center, double& right) const
{
  assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
  leftleft = ptr_[i_offset-2];
  left     = ptr_[i_offset-1];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+1];
}
void ConstIJK_double_ptr::get_leftleft_left_center_right_y(int i_offset, double& leftleft, double& left, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
  leftleft = ptr_[i_offset-2*j_stride_];
  left     = ptr_[i_offset-j_stride_];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+j_stride_];

}
void ConstIJK_double_ptr::get_leftleft_left_center_right_z(int i_offset, double& leftleft, double& left, double& center, double& right) const
{
  assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
  assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
  leftleft = ptr_[i_offset-2*k_stride_];
  left     = ptr_[i_offset-k_stride_];
  center   = ptr_[i_offset];
  right    = ptr_[i_offset+k_stride_];
}

void ConstIJK_double_ptr::get_left_center_xy(int i_offset, Simd_double& leftx_lefty, Simd_double& leftx_centery, Simd_double& centerx_lefty, Simd_double& centerx_centery)
{
  SimdGetLeftCenter(ptr_ + i_offset - j_stride_, leftx_lefty, centerx_lefty);
  SimdGetLeftCenter(ptr_ + i_offset, leftx_centery, centerx_centery);
}
void ConstIJK_double_ptr::get_left_center_xz(int i_offset, Simd_double& leftx_leftz, Simd_double& leftx_centerz, Simd_double& centerx_leftz, Simd_double& centerx_centerz)
{
  SimdGetLeftCenter(ptr_ + i_offset - k_stride_, leftx_leftz, centerx_leftz);
  SimdGetLeftCenter(ptr_ + i_offset, leftx_centerz, centerx_centerz);
}

void ConstIJK_double_ptr::get_left_center_yx(int i_offset, Simd_double& lefty_leftx, Simd_double& lefty_centerx, Simd_double& centery_leftx, Simd_double& centery_centerx)
{
  SimdGetLeftCenter(ptr_ + i_offset - j_stride_, lefty_leftx, lefty_centerx);
  SimdGetLeftCenter(ptr_ + i_offset, centery_leftx, centery_centerx);
}
void ConstIJK_double_ptr::get_left_center_yz(int i_offset, Simd_double& lefty_leftz, Simd_double& lefty_centerz, Simd_double& centery_leftz, Simd_double& centery_centerz)
{
  lefty_leftz = SimdGet(ptr_ + i_offset - j_stride_ - k_stride_);
  lefty_centerz = SimdGet(ptr_ + i_offset - j_stride_);
  centery_leftz = SimdGet(ptr_ + i_offset - k_stride_);
  centery_centerz = SimdGet(ptr_ + i_offset);
}

void ConstIJK_double_ptr::get_left_center_zx(int i_offset, Simd_double& leftz_leftx, Simd_double& leftz_centerx, Simd_double& centerz_leftx, Simd_double& centerz_centerx)
{
  SimdGetLeftCenter(ptr_ + i_offset - k_stride_, leftz_leftx, leftz_centerx);
  SimdGetLeftCenter(ptr_ + i_offset, centerz_leftx, centerz_centerx);
}
void ConstIJK_double_ptr::get_left_center_zy(int i_offset, Simd_double& leftz_lefty, Simd_double& leftz_centery, Simd_double& centerz_lefty, Simd_double& centerz_centery)
{
  leftz_lefty = SimdGet(ptr_ + i_offset - j_stride_ - k_stride_);
  leftz_centery = SimdGet(ptr_ + i_offset - k_stride_);
  centerz_lefty = SimdGet(ptr_ + i_offset - j_stride_);
  centerz_centery = SimdGet(ptr_ + i_offset);
}
