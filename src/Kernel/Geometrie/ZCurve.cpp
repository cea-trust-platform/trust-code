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

#include <ZCurve.h>

#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <bitset>
#include <fstream>

template<typename _SIZE_>
void ZCurve_32_64<_SIZE_>::Morton_sfc_pierre(std::vector<PointZC>& points, ArrOfInt_t& renum)
{
  throw ; // TODO
}

template<>
void ZCurve_32_64<int>::Morton_sfc_pierre(std::vector<PointZC>& points, ArrOfInt& renum)
{
  double xmin = std::numeric_limits<double>::max();
  double xmax = -xmin;
  double ymin = std::numeric_limits<double>::max();
  double ymax = -ymin;
  double zmin = std::numeric_limits<double>::max();
  double zmax = -zmin;
  for (auto &p: points)
    {
      xmin = std::min(p.x, xmin);
      xmax = std::max(p.x, xmax);
      ymin = std::min(p.y, ymin);
      ymax = std::max(p.y, ymax);
      zmin = std::min(p.z, zmin);
      zmax = std::max(p.z, zmax);
    }
  // Convert floating-point coordinates to integers:
  for (auto &p: points)
    {
      // Scale coordinates to fit within the 64-bit range
      uint64_t range = 1e12; //std::numeric_limits<uint64_t>::max();
      uint32_t scaledX = static_cast<uint32_t>((p.x - xmin) / (xmax - xmin) * (double)range);
      uint32_t scaledY = static_cast<uint32_t>((p.y - ymin) / (ymax - ymin) * (double)range);
      uint32_t scaledZ = Objet_U::dimension==3 ? static_cast<uint32_t>((p.z - zmin) / (zmax - zmin) * (double)range) : 0;
      uint64_t morton = 0;
      for (uint64_t i = 0; i < sizeof(uint32_t) * 8; ++i)
        {
          if (Objet_U::dimension==3)
            morton |= ((scaledX & (1ULL << i)) << (2 * i)) |
                      ((scaledY & (1ULL << i)) << (2 * i + 1)) |
                      ((scaledZ & (1ULL << i)) << (2 * i + 2));
          else
            morton |= (scaledX & (1ULL << i)) << i |
                      (scaledY & (1ULL << i)) << (i + 1);
        }
      p.morton = morton;
    }
  // Sort nodes, cells, and faces using their computed Morton codes.
  std::sort(points.begin(), points.end(), [](const PointZC &a, const PointZC &b) { return a.morton < b.morton; });
  // Build renum
  int i = 0;
  for (auto &p : points)
    renum((int)p.id) = i++;
}

template<typename _SIZE_>
void ZCurve_32_64<_SIZE_>::Morton_sfc_v2(std::vector<PointZC>& points, ArrOfInt_t& renum)
{
  throw ; // TODO
}

template<>
void ZCurve_32_64<int>::Morton_sfc_v2(std::vector<PointZC>& inputPoints, ArrOfInt& renum)
{
  // Compute Morton code
  auto mortonCode = [](uint32_t x, uint32_t y) -> uint64_t
  {
    uint64_t result = 0;
    uint64_t xx=x, yy=y;
    for (int i = 0; i < 32; i++)
      {
        result |= (xx & (1ULL << i)) << i;
        result |= (yy & (1ULL << i)) << (i+1);
      }
    return result;
  };

  // Find bounding box
  double minX = inputPoints[0].x, maxX = inputPoints[0].x;
  double minY = inputPoints[0].y, maxY = inputPoints[0].y;

  for (const auto& p : inputPoints)
    {
      if (p.x < minX) minX = p.x;
      if (p.x > maxX) maxX = p.x;
      if (p.y < minY) minY = p.y;
      if (p.y > maxY) maxY = p.y;
    }

  // Quantization function
  auto quantize = [&](double value, double minVal, double maxVal) -> uint32_t
  {
    constexpr uint32_t MAX_VAL = std::numeric_limits<uint32_t>::max();
    double normalized = (value - minVal) / (maxVal - minVal);
    normalized = std::clamp(normalized, 0.0, 1.0);
    return static_cast<uint32_t>(std::round(normalized * MAX_VAL));
  };

  // Create vector of original indices
  auto tmp_renum(renum);
  for (int i = 0; i < tmp_renum.size_array(); ++i)
    tmp_renum[i] = i;

  if (renum.size_array() != (int)inputPoints.size())
    Process::exit("pb with sizes!!");

  // Sort indices based on Morton code comparison
  std::sort(tmp_renum.begin(), tmp_renum.end(), [&](int a, int b)
  {
    uint32_t  ax = quantize(inputPoints[a].x, minX, maxX),
              ay = quantize(inputPoints[a].y, minY, maxY),
              bx = quantize(inputPoints[b].x, minX, maxX),
              by = quantize(inputPoints[b].y, minY, maxY);
    return mortonCode(ax, ay) < mortonCode(bx, by);
  });

  /*
    Cerr <<  "BEFORE SORT " << finl;
    for (int i = 0; i < (int)inputPoints.size(); i++)
      {
        Cerr <<  "  " << inputPoints[i].x << "  /   " << inputPoints[i].y << "\n";
      }
    Cerr <<  "AFTER SORT " << finl;
    for (int i = 0; i < (int)inputPoints.size(); i++)
      {
        int idx = tmp_renum[i];
        Cerr <<  "  " << inputPoints[idx].x << "  /   " << inputPoints[idx].y << "\n";
      }
    Cerr <<  "MORTON CODES " << finl;
    for (int i = 0; i <(int) inputPoints.size(); i++)
      {
        int idx = tmp_renum[i];
  //      std::cout << i << " " << idx << std::endl;
        auto ax = quantize(inputPoints[idx].x, minX, maxX),
             ay = quantize(inputPoints[idx].y, minY, maxY);
        std::bitset<64> bs(mortonCode(ax, ay));
  //      std::cout <<  "  " << bs << "\n";
      }
  */
  for (int i = 0; i < tmp_renum.size_array(); ++i)
    renum[tmp_renum[i]] = i;
}


template<typename _SIZE_>
void ZCurve_32_64<_SIZE_>::Dump_to_file(const DoubleTab_t& points, const std::string& filename)
{
  std::ofstream outFile(filename);
  if (!outFile.is_open())
    throw std::runtime_error("Failed to open file: " + filename);

  for (int i = 0; i < points.dimension(0); i++)
    outFile << points(i, 0) << " " << points(i, 1) << "\n";

  outFile.close();
}

template class ZCurve_32_64<int>;
#if INT_is_64_ == 2
template class ZCurve_32_64<trustIdType>;
#endif

