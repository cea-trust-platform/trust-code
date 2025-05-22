/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef ZCurve_included
#define ZCurve_included

#include <TRUSTTab.h>

struct PointZC
{
  double x, y;
  double z = 0.0;
  trustIdType id;
  uint64_t morton;
};

template<typename _SIZE_>
struct ZCurve_32_64
{
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  static void Morton_sfc_pierre(std::vector<PointZC>& points, ArrOfInt_t& renum);

  static void Morton_sfc_v2(std::vector<PointZC>& points, ArrOfInt_t& renum);

  static void Dump_to_file(const DoubleTab_t& points, const std::string& filename);

private:
  static uint64_t MortonCode(uint32_t x, uint32_t y, uint32_t z);
  static uint64_t HilbertCode(uint32_t x, uint32_t y, uint32_t z);
};

using ZCurve = ZCurve_32_64<int>;

#endif
