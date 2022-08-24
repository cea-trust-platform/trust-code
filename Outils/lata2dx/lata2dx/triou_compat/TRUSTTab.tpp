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

#ifndef TRUSTTab_TPP_included
#define TRUSTTab_TPP_included

#include <TRUSTArray.tpp>

template<typename _TYPE_>
class TRUSTTab: public TRUSTArray<_TYPE_>
{
public:
  TRUSTTab()
  {
    dimensions_[0] = 0;
    dimensions_[1] = 0;
  }

  TRUSTTab(const TRUSTTab<_TYPE_> &tab) : TRUSTArray<_TYPE_>(tab)
  {
    dimensions_[0] = tab.dimensions_[0];
    dimensions_[1] = tab.dimensions_[1];
  }

  TRUSTTab(const entier i, const entier j) : TRUSTArray<_TYPE_>(i*j)
  {
    dimensions_[0] = i;
    dimensions_[1] = j;
  }

  TRUSTTab& operator=(const TRUSTTab<_TYPE_>& tab)
  {
    TRUSTArray<_TYPE_>::operator=(tab);
    dimensions_[0] = tab.dimensions_[0];
    dimensions_[1] = tab.dimensions_[1];
    return *this;
  }

  void reset()
  {
    TRUSTArray<_TYPE_>::reset();
    dimensions_[0] = 0;
    dimensions_[1] = 0;
  }

  inline _TYPE_& operator()(entier i, entier j)
  {
    assert(i >= 0 && i < dimensions_[0] && j >= 0 && j < dimensions_[1]);
    const entier n = i * dimensions_[1] + j;
    _TYPE_ &x = TRUSTArray<_TYPE_>::operator[](n);
    return x;
  }

  inline _TYPE_ operator()(entier i, entier j) const
  {
    assert(i >= 0 && i < dimensions_[0] && j >= 0 && j < dimensions_[1]);
    const entier n = i * dimensions_[1] + j;
    _TYPE_ x = TRUSTArray<_TYPE_>::operator[](n);
    return x;
  }

  inline entier resize(entier i, entier j)
  {
    assert(i >= 0 && j >= 0);
    dimensions_[0] = i;
    dimensions_[1] = j;
    TRUSTArray<_TYPE_>::resize_array(i * j);
    return i * j;
  }

  inline entier dimension(entier i) const
  {
    assert(i >= 0 && i < 2);
    return dimensions_[i];
  }

  inline entier dimension_tot(entier i) const { return dimension(i); }

protected:
  _TYPE_& operator[](entier i) = delete;
  const _TYPE_& operator[](entier i) const = delete;

private:
  static constexpr entier MAXDIMTRUSTTab = 2;
  entier dimensions_[MAXDIMTRUSTTab];
};

#endif /* TRUSTTab_TPP_included */
