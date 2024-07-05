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

#ifndef ConstIJK_ptr_included
#define ConstIJK_ptr_included

#include <IJK_Field.h>

enum class DIRECTION { X=0, Y=1, Z=2 };

/*! @brief This class implements a accessor to IJK_Field values.
 *
 * It provides efficient access to center, left and right neighbours en i, j, and k directions
 *    and checks if i,j,k are within the bounds
 *
 */
template <typename _TYPE_, typename _TYPE_ARRAY_>
class ConstIJK_ptr
{
public:
  /*! @brief builds a pointer to field(i,j,k);
   *
   */
  ConstIJK_ptr(const IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>& field, int i, int j, int k)
  {
    ptr_ = &field(i, j, k);
    j_stride_ = field.j_stride();
    k_stride_ = field.k_stride();
#ifndef NDEBUG
    const int ghost = field.ghost();
    i_min_ = - ghost;
    i_max_ = field.ni() + ghost;
    j_min_ = - ghost;
    j_max_ = field.nj() + ghost;
    k_min_ = - ghost;
    k_max_ = field.nk() + ghost;
    i_ = i;
    j_ = j;
    k_ = k;
#endif
  }
  /*! @brief increments the pointer by j_stride (eg, j = j+1)
   *
   */
  void next_j()
  {
    ptr_ += j_stride_;
#ifndef NDEBUG
    j_++;
    assert(j_ < j_max_);
#endif
  }
  /*! @brief returns field(i+i_offset, j, k)
   *
   */
  void get_center(int i_offset, _TYPE_ & center) const
  {
    assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
    center = ptr_[i_offset];
  }

  /*! @brief returns left=field(i+i_offset-1, j, k) and center=field(i+i_offset, j, k)
   *
   */
  void get_left_center(DIRECTION _DIR_, int i_offset, _TYPE_ & left, _TYPE_ & center) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset > i_min_ && i_ + i_offset < i_max_);
        left = ptr_[i_offset - 1];
        center = ptr_[i_offset];
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ - 1 >= j_min_);
        center = ptr_[i_offset];
        left = ptr_[i_offset - j_stride_];
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ - 1 >= k_min_);
        center = ptr_[i_offset];
        left = ptr_[i_offset - k_stride_];
      }
  }

  void get_center_right(DIRECTION _DIR_, int i_offset, _TYPE_ & center, _TYPE_ & right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
        center = ptr_[i_offset];
        right = ptr_[i_offset + 1];
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ >= j_min_ && j_ + 1 <= j_max_);
        center = ptr_[i_offset];
        right = ptr_[i_offset + j_stride_];
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ + 1 < k_max_);
        center = ptr_[i_offset];
        right = ptr_[i_offset + k_stride_];
      }

  }

  void get_left_center_right(DIRECTION _DIR_, int i_offset, _TYPE_ & left, _TYPE_ & center, _TYPE_ & right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset > i_min_ && i_ + i_offset + 1 < i_max_);
        left = ptr_[i_offset - 1];
        center = ptr_[i_offset];
        right = ptr_[i_offset + 1];
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
        center = ptr_[i_offset];
        left = ptr_[i_offset - j_stride_];
        right = ptr_[i_offset + j_stride_];
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ - 1 >= k_min_ && k_ + 1 < k_max_);
        center = ptr_[i_offset];
        left = ptr_[i_offset - k_stride_];
        right = ptr_[i_offset + k_stride_];
      }

  }

  void get_leftleft_left_center_right(DIRECTION _DIR_, int i_offset, _TYPE_ & leftleft, _TYPE_ & left, _TYPE_ & center, _TYPE_ & right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
        leftleft = ptr_[i_offset-2];
        left     = ptr_[i_offset-1];
        center   = ptr_[i_offset];
        right    = ptr_[i_offset+1];
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
        assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
        leftleft = ptr_[i_offset-2*j_stride_];
        left     = ptr_[i_offset-j_stride_];
        center   = ptr_[i_offset];
        right    = ptr_[i_offset+j_stride_];
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
        assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
        leftleft = ptr_[i_offset-2*k_stride_];
        left     = ptr_[i_offset-k_stride_];
        center   = ptr_[i_offset];
        right    = ptr_[i_offset+k_stride_];
      }

  }


  void get_center(int i_offset, Simd_template<_TYPE_>& center) const
  {
    assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
    center = SimdGet(ptr_ + i_offset);
  }

  void get_left_center(DIRECTION _DIR_, int i_offset, Simd_template<_TYPE_>& left, Simd_template<_TYPE_>& center) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset < i_max_);
        SimdGetLeftCenter(ptr_ + i_offset, left, center);
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ - 1 >= j_min_);
        center = SimdGet(ptr_ + i_offset);
        left = SimdGet(ptr_ + i_offset - j_stride_);
      }

    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ - 1 >= k_min_);
        center = SimdGet(ptr_ + i_offset);
        left = SimdGet(ptr_ + i_offset - k_stride_);
      }
  }


  void get_center_right(DIRECTION _DIR_, int i_offset, Simd_template<_TYPE_>& center, Simd_template<_TYPE_>& right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset + 1 < i_max_);
        SimdGetCenterRight(ptr_ + i_offset, center, right);
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ + 1 <= j_max_);
        center = SimdGet(ptr_ + i_offset);
        right = SimdGet(ptr_ + i_offset + j_stride_);
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ + 1 <= k_max_);
        center = SimdGet(ptr_ + i_offset);
        right = SimdGet(ptr_ + i_offset + k_stride_);
      }

  }

  void get_left_center_right(DIRECTION _DIR_, int i_offset, Simd_template<_TYPE_>& left, Simd_template<_TYPE_>& center, Simd_template<_TYPE_>& right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset - 1 >= i_min_ && i_ + i_offset + 1 < i_max_);
        SimdGetLeftCenterRight(ptr_ + i_offset, left, center, right);
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(j_ - 1 >= j_min_ && j_ + 1 <= j_max_);
        center = SimdGet(ptr_ + i_offset);
        left = SimdGet(ptr_ + i_offset - j_stride_);
        right = SimdGet(ptr_ + i_offset + j_stride_);
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset < i_max_);
        assert(k_ - 1 >= k_min_ && k_ + 1 <= k_max_);
        center = SimdGet(ptr_ + i_offset);
        left = SimdGet(ptr_ + i_offset - k_stride_);
        right = SimdGet(ptr_ + i_offset + k_stride_);
      }

  }

  void get_leftleft_left_center_right(DIRECTION _DIR_, int i_offset, Simd_template<_TYPE_>& leftleft, Simd_template<_TYPE_>& left, Simd_template<_TYPE_>& center, Simd_template<_TYPE_>& right) const
  {
    if(_DIR_ == DIRECTION::X)
      {
        assert(i_ + i_offset - 2 >= i_min_ && i_ + i_offset + 1 < i_max_);
        SimdGetLeftleftLeftCenterRight(ptr_ + i_offset, leftleft, left, center, right);
      }
    if(_DIR_ == DIRECTION::Y)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
        assert(j_ - 2 >= j_min_ && j_ + 1 < j_max_);
        leftleft = SimdGet(ptr_ + i_offset - 2 * j_stride_);
        left     = SimdGet(ptr_ + i_offset - j_stride_);
        center   = SimdGet(ptr_ + i_offset);
        right    = SimdGet(ptr_ + i_offset + j_stride_);
      }
    if(_DIR_ == DIRECTION::Z)
      {
        assert(i_ + i_offset >= i_min_ && i_ + i_offset <= i_max_);
        assert(k_ - 2 >= k_min_ && k_ + 1 < k_max_);
        leftleft = SimdGet(ptr_ + i_offset - 2 * k_stride_);
        left     = SimdGet(ptr_ + i_offset - k_stride_);
        center   = SimdGet(ptr_ + i_offset);
        right    = SimdGet(ptr_ + i_offset + k_stride_);
      }
  }

  void get_left_center_c1c2(DIRECTION _COMPO1_, DIRECTION _COMPO2_, int i_offset, Simd_template<_TYPE_>& leftc1_leftc2, Simd_template<_TYPE_>& leftc1_centerc2, Simd_template<_TYPE_>& centerc1_leftc2, Simd_template<_TYPE_>& centerc1_centerc2) const
  {
    if(_COMPO1_ != DIRECTION::X && _COMPO2_ != DIRECTION::X)
      {
        int stride1 = _COMPO1_ == DIRECTION::Y ? j_stride_ : k_stride_;
        int stride2 = _COMPO2_ == DIRECTION::Y ? j_stride_ : k_stride_;
        leftc1_leftc2 = SimdGet(ptr_ + i_offset - j_stride_ - k_stride_);
        leftc1_centerc2 = SimdGet(ptr_ + i_offset - stride1);
        centerc1_leftc2 = SimdGet(ptr_ + i_offset - stride2);
        centerc1_centerc2 = SimdGet(ptr_ + i_offset);
      }
    else
      {
        if(_COMPO1_ == DIRECTION::X)
          {
            int stride = _COMPO2_ == DIRECTION::Y ? j_stride_ : k_stride_;
            SimdGetLeftCenter(ptr_ + i_offset - stride, leftc1_leftc2, centerc1_leftc2);
            SimdGetLeftCenter(ptr_ + i_offset, leftc1_centerc2, centerc1_centerc2);
          }
        else
          {
            int stride = _COMPO1_ == DIRECTION::Y ? j_stride_ : k_stride_;
            SimdGetLeftCenter(ptr_ + i_offset - stride, leftc1_leftc2, leftc1_centerc2);
            SimdGetLeftCenter(ptr_ + i_offset, centerc1_leftc2, centerc1_centerc2);
          }
      }
  }

protected:
  const _TYPE_ *ptr_; // pointer to the current location inside the IJK_Field
  int j_stride_; // local copy of field_.j_stride()
  int k_stride_; // idem
#ifndef NDEBUG
  // legal bounds
  int i_min_, i_max_, j_min_, j_max_, k_min_, k_max_;
  // current location of ptr_
  int i_, j_, k_;
#endif

private:
  int get_stride(DIRECTION _COMPO_)
  {
    switch(_COMPO_)
      {
      case DIRECTION::Y:
        return j_stride_;
      case DIRECTION::Z:
        return k_stride_;
      default:
        {
          Cerr << "ConstIJK_ptr::get_stride compo x not allowed" << finl;
          Process::exit();
          return 0;
        }
      }
  }
};

using ConstIJK_float_ptr = ConstIJK_ptr<float, ArrOfFloat>;
using ConstIJK_double_ptr = ConstIJK_ptr<double, ArrOfDouble>;

#endif
