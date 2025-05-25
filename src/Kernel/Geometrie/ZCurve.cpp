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

namespace
{
constexpr unsigned int NB_BITS_2D = 32;
constexpr unsigned int NB_BITS_3D = 21;  // Nb of bits for the quantization of each coordinate - 21*3=63 < 64 -> will fit in uint64_t

constexpr uint32_t MAX_VAL_2D = std::numeric_limits<uint32_t>::max(); // binary: 11111...11 (32 times)
constexpr uint32_t MAX_VAL_3D = (1 << NB_BITS_3D) - 1; // binary: 00...0011...11 (eleven 0 and 21 ones)

enum CUBE_PERM {ID, U, U_INV, R, R_INV, F, F_INV};

using cube_pos_t = std::array<int8_t, 3>; // 3 signed integers representing the orientation of the unit x,y,z trihedron

using ar8_t = std::array<uint8_t, 8>;

const ar8_t unit_hilbert = { 0,7,3,4,1,6,2,5 };  // initial indexing of the unit Hilbert 3D curve

//// Cube permutations
//const ar8_t up =     { 1,3,0,2,5,7,4,6 };  // up permutation (Rubik's style)
//const ar8_t up_inv = { 2,0,3,1,6,4,7,5 };  // inverse up permutation (Rubik's style)
//const ar8_t rp =     { 2,3,6,7,0,1,4,5 };  // right perm
//const ar8_t rp_inv = { 4,5,0,1,6,7,2,3 };  // inverse right perm
//const ar8_t fp =     { 1,5,3,7,0,4,2,6 };  // front perm
//const ar8_t fp_inv = { 4,0,6,2,5,1,7,3 };  // inverse front perm

}

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
uint64_t ZCurve_32_64<_SIZE_>::MortonCode(uint32_t x, uint32_t y, uint32_t z)
{
  uint64_t result = 0;
  uint64_t xx=x, yy=y, zz=z;
  if (Objet_U::dimension == 2)
    for (unsigned i = 0; i < ::NB_BITS_2D; i++)
      {
        result |= (xx & (1ULL << i)) << i;
        result |= (yy & (1ULL << i)) << (i+1);
      }
  else // dim 3
    for (unsigned i = 0; i < ::NB_BITS_3D; i++)
      {
        result |= (xx & (1ULL << i)) << (2*i);
        result |= (yy & (1ULL << i)) << (2*i+1);
        result |= (zz & (1ULL << i)) << (2*i+2);
      }
  return result;
}

template<typename _SIZE_>
uint64_t ZCurve_32_64<_SIZE_>::HilbertCode(uint32_t x, uint32_t y, uint32_t z)
{
  uint64_t result = 0;
  uint64_t mort = ZCurve_32_64<_SIZE_>::MortonCode(x,y,z);
  if (Objet_U::dimension == 2)
    {
      const uint8_t ud[4] = {0, 3, 1, 2};  // up - down
      const uint8_t rl[4] = {3, 2, 0, 1};  // right - left
      unsigned int orient = 0; // 0: up, 1: right, 2: down, 3:left
      bool flip = false;
      for(int i = NB_BITS_2D-1; i >= 0; i--)
        {
          result <<= 2;
          uint64_t two_bits = (mort >> (2*i)) & 0b11;
          uint8_t new_two_bits = 0b00;
          switch(orient)
            {
            case 0:  // up
              new_two_bits = ud[two_bits];
              break;
            case 1:  // right
              new_two_bits = rl[two_bits];
              break;
            case 2:  // down
              new_two_bits = ud[3-two_bits];
              break;
            case 3:  // left
              new_two_bits = rl[3-two_bits];
              break;
            default:
              throw;
            }
          result |= flip ? (0b11 - new_two_bits) : new_two_bits;
          // Rotate:
          if (new_two_bits == 0b00) { orient = (orient+1) % 4; flip = !flip; }
          if (new_two_bits == 0b11) { orient = (orient+3) % 4; flip = !flip; }
        }
      return result;
    }
  else // dim 3
    {
      // Based on the nice images found there:
      //   https://pypi.org/project/numpy-hilbert-curve/

      // Compute p1 . p2 --- mathematical definition : (p1.p2)(x) = p1(p2(x)) = res(x)
      auto compose = [](const cube_pos_t& p1, const cube_pos_t& p2) -> cube_pos_t
      {
        cube_pos_t res;
        for (int i = 0; i < 3; i++)
          {
            uint8_t idx = std::abs(p1[i])-1;
            int8_t sig = p1[i] > 0 ? 1 : -1;
            res[i] = p2[idx]*sig;
          }
        return res;
      };

      auto apply_perm = [](const cube_pos_t& perm, const uint64_t& cod) -> uint8_t
      {
        std::array<uint8_t, 3> cod2;  // x,y,z
        cod2[0] = (uint8_t)(cod & 0b001);
        cod2[1] = (uint8_t)((cod & 0b010) >> 1);
        cod2[2] = (uint8_t)((cod & 0b100) >> 2);

        std::array<uint8_t, 3> res;

        for(int i=0; i < 3; i++)
          {
            int8_t v = perm[i];
            uint8_t idx = std::abs(v)-1;
            int8_t mask = (v < 0) ? 0b1 : 0b0;
            res[idx] = cod2[i] ^ mask; // XOR
          }
        return (res[0] | (res[1] << 1) | (res[2] << 2));
      };

      cube_pos_t curr_perm = {1,2,3};   // 1 represents X axis, 2 the Y axis, 3 the Z axis.
      // U R -> {2,-1,3} {1,3,-2} -> {2,3,1}

//      uint8_t tst = apply_perm({1,3,-2}, 0b010); // -> 0b000
//      std::cout << std::bitset<8>(tst) << "\n";
//      tst = apply_perm({1,3,-2}, 0b101);  // -> 0b111
//      std::cout << std::bitset<8>(tst) << "\n";
//      tst = apply_perm({1,3,-2}, 0b110); // -> 0b010
//      std::cout << std::bitset<8>(tst) << "\n";

////      curr_perm = compose(curr_perm, {2,-1,3});
//      std::cout << "YOOOOO\n";
//      std::cout << (int)curr_perm[0] << " "<< (int)curr_perm[1] << " "<< (int)curr_perm[2] << " \n";

      bool flip = false;
      for(int i = NB_BITS_3D-1; i >= 0; i--)
        {
          result <<= 3;
          uint64_t three_bits = (mort >> (3*i)) & 0b111;
          // Apply current permutation to pos
          uint64_t new_three_bits = apply_perm(curr_perm, three_bits);

          // Apply Hilbert ordering:
          new_three_bits = unit_hilbert[new_three_bits];

          uint64_t orig3b = new_three_bits;

          if(flip)
            new_three_bits = new_three_bits ^ 0b111;

          result |= new_three_bits;
          if (i==0) break;

          // Prepare cube position for next level at depth N+1:
          switch(orig3b)
            {
            case 0: // U R -> {2,-1,3} {1,3,-2} -> {2,3,1}
              curr_perm = compose(curr_perm, {2,3,1});
              break;
            case 7: // U' R -> {-2,1,3} {1,3,-2} -> {-2,3,-1}
              curr_perm = compose(curr_perm, {-2,3,-1});
              break;
            case 3: // F' flip
              curr_perm = compose(curr_perm, {-3,2,1});
              flip = !flip;
              break;
            case 4: // F flip
              curr_perm = compose(curr_perm, {3,2,-1});
              flip = !flip;
              break;
            case 1: // U flip
              curr_perm = compose(curr_perm, {2,-1,3});
              flip = !flip;
              break;
            case 6: // U' flip
              curr_perm = compose(curr_perm, {-2,1,3});
              flip = !flip;
              break;
            case 2: // nothing to do!
              break;
            case 5: // nothing to do!
              break;
            default:
              throw;
            }
        }
      return result;
    }
}


template<typename _SIZE_>
void ZCurve_32_64<_SIZE_>::Morton_sfc_v2(std::vector<PointZC>& points, ArrOfInt_t& renum)
{
  throw ; // TODO
}

template<>
void ZCurve_32_64<int>::Morton_sfc_v2(std::vector<PointZC>& inputPoints, ArrOfInt& renum)
{
  const int dim = Objet_U::dimension;

  // Find bounding box
  double minX = inputPoints[0].x, maxX = inputPoints[0].x;
  double minY = inputPoints[0].y, maxY = inputPoints[0].y;
  double minZ = inputPoints[0].z, maxZ = inputPoints[0].z;

  for (const auto& p : inputPoints)
    {
      if (p.x < minX) minX = p.x;
      if (p.x > maxX) maxX = p.x;
      if (p.y < minY) minY = p.y;
      if (p.y > maxY) maxY = p.y;
      if (Objet_U::dimension == 3)
        {
          if (p.z < minZ) minZ = p.z;
          if (p.z > maxZ) maxZ = p.z;
        }
    }

  // Quantization function
  auto quantize = [&](double value, double minVal, double maxVal) -> uint32_t
  {
    const uint32_t MAX_VAL = dim == 2 ? MAX_VAL_2D : MAX_VAL_3D;
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

  int ordering_type = atoi(getenv("TRUST_MESH_REORDERING")); // should never be null ptr if we're here

  if (ordering_type == 1) // Morton
    {
      Cerr << "  Using MORTON algorithm ..." << finl;

      // Sort indices based on Morton code comparison
      std::sort(tmp_renum.begin(), tmp_renum.end(), [&](int a, int b)
      {
        uint32_t  ax = quantize(inputPoints[a].x, minX, maxX),
                  ay = quantize(inputPoints[a].y, minY, maxY),
                  bx = quantize(inputPoints[b].x, minX, maxX),
                  by = quantize(inputPoints[b].y, minY, maxY),
                  az = 0.0, bz = 0.0;
        if (dim == 3)
          {
            az = quantize(inputPoints[a].z, minZ, maxZ);
            bz = quantize(inputPoints[b].z, minZ, maxZ);
          }
        return MortonCode(ax, ay, az) < MortonCode(bx, by, bz);
      });
    }
  else   // Hilbert
    {
      Cerr << "  Using HILBERT algorithm ..." << finl;

      // Sort indices based on Hilbert code comparison
      std::sort(tmp_renum.begin(), tmp_renum.end(), [&](int a, int b)
      {
        uint32_t  ax = quantize(inputPoints[a].x, minX, maxX),
                  ay = quantize(inputPoints[a].y, minY, maxY),
                  bx = quantize(inputPoints[b].x, minX, maxX),
                  by = quantize(inputPoints[b].y, minY, maxY),
                  az = 0.0, bz = 0.0;
        if (dim == 3)
          {
            az = quantize(inputPoints[a].z, minZ, maxZ);
            bz = quantize(inputPoints[b].z, minZ, maxZ);
          }
        return HilbertCode(ax, ay, az) < HilbertCode(bx, by, bz);
      });
    }

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
    {
      outFile << points(i, 0) << " " << points(i, 1);
      if (Objet_U::dimension == 3)
        outFile << " " << points(i, 2);
      outFile << "\n";
    }

  outFile.close();
}

template struct ZCurve_32_64<int>;
#if INT_is_64_ == 2
template struct ZCurve_32_64<trustIdType>;
#endif

