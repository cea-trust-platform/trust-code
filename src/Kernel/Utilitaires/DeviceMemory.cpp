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

#include <DeviceMemory.h>
#include <iostream>
#include <Process.h>

map_t DeviceMemory::memory_map_;
size_t DeviceMemory::initial_free_ = 0;

// Memory:
size_t DeviceMemory::deviceMemGetInfo(bool print_total) // free or total bytes of the device
{
  size_t free=0, total=0;
#ifndef LATATOOLS
#ifdef TRUST_USE_CUDA
  cudaMemGetInfo(&free, &total);
#endif
#ifdef TRUST_USE_HIP
  hipMemGetInfo(&free_bytes, &total_bytes);
#endif
#endif
  return print_total ? total : free;
}
size_t DeviceMemory::allocatedBytesOnDevice()
{
  size_t free = deviceMemGetInfo(0);
  initial_free_ = (initial_free_==0) ? free : initial_free_;
  return initial_free_ - free;
}

void DeviceMemory::printMemoryMap()
{
  int sz;
  DataLocation loc;
  std::cout << "=== Memory blocks on the device ===" << std::endl;
  for (const auto& block : memory_map_)
    {
      void* ptr = block.first;
      std::tie(sz, loc) = block.second;
      std::cout << "Host ptr: " << ptr << " size: " << sz << " loc: " << (int)loc << std::endl;
    }
  std::cout << "===================================" << std::endl;
}
