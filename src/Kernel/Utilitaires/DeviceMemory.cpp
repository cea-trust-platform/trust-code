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

#include <DeviceMemory.h>
#include <iostream>
#include <Process.h>
#include <Nom.h>
#include <Device.h>
#ifndef LATATOOLS
#ifdef TRUST_USE_ROCM
#include <hip/hip_runtime.h>
#endif
#endif

map_t DeviceMemory::memory_map_;
size_t DeviceMemory::initial_free_ = 0;
// Typical size of internal_items (elem,face,som) used as threshold to detect excessive H2D/D2H copies or host array allocation
trustIdType DeviceMemory::internal_items_size_ = 1e9;
int DeviceMemory::nb_pas_dt_ = -1;

// Memory:
size_t DeviceMemory::deviceMemGetInfo(bool print_total) // free or total bytes of the device
{
  size_t free=0, total=0;
#ifndef LATATOOLS
#ifdef TRUST_USE_CUDA
  cudaMemGetInfo(&free, &total);
#endif
#ifdef TRUST_USE_ROCM
  auto err = hipMemGetInfo(&free, &total);
  if (err) Process::exit("Error during hipMemGetInfo");
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

/** Check if a chunk (even empty) is allocated on the device for the host ptr
 * Return true/false
*/
bool DeviceMemory::isAllocatedOnDevice(void * ptr)
{
  if (memory_map_.empty()) return false;
  auto it = memory_map_.find(ptr);
  return (it != memory_map_.end());
}

/** Return the device address of the host ptr
 * Return nullptr if ptr null or map empty
 * Fatal error if address not found on the device
*/
#ifndef LATATOOLS
void* DeviceMemory::addrOnDevice(void * ptr)
{
  if (ptr==nullptr || memory_map_.empty())
    return nullptr;
  int sz;
  void* device_ptr;
  void* host_ptr;
  for (auto it = memory_map_.begin(); it != memory_map_.end(); ++it)
    {
      host_ptr = it->first;
      std::tie(sz, device_ptr) = it->second;
      if (ptr==host_ptr) return device_ptr;
      else
        {
          // Cas de buffer_base (ptr n'est pas forcement l'adresse de debut du bloc memoire...)
          if (ptr >= host_ptr && ptr < static_cast<char *>(host_ptr) + sz)
            return static_cast<char *>(device_ptr) + (static_cast<char *>(ptr) - static_cast<char *>(host_ptr));
        }
    }
  // Reproduce OpenMP-target spec: FATAL ERROR: data in use_device clause was not found on device 1: host:0x3dc75600
  Process:: Journal() << "Error! Device address for host address " << ptrToString(ptr) << " not found:" << finl;
  DeviceMemory::printMemoryMap();
  Process::exit("Error! Device address for host address not found. See log.");
  return nullptr;
}

/** Add a new line to the memory_map_
 * ptr should be non-null, device_ptr and size may be null
 */
void DeviceMemory::add(void * ptr, void * device_ptr, int size)
{
  if (ptr==nullptr) return;
  DeviceMemory::getMemoryMap()[ptr] = {size, device_ptr};
  if (clock_on)
    {
      Process::Journal() << "Adding Host ptr: " << ptrToString(ptr) << " Device ptr: " << ptrToString(device_ptr)
                         << " size: " << size << finl;
      DeviceMemory::printMemoryMap();
    }
}

/** Delete a line into the memory_map_
 *
 */
void DeviceMemory::del(void * ptr)
{
  DeviceMemory::getMemoryMap().erase(ptr);
  if (clock_on)
    {
      Process::Journal() << "Deleting Host ptr: " << ptrToString(ptr) << finl;
      DeviceMemory::printMemoryMap();
    }
}

/** Print the memory map for debug
 *
 */
void DeviceMemory::printMemoryMap()
{
  int sz;
  void* device_ptr;
  Process::Journal() << "=== Memory blocks on the device ===" << finl;
  for (const auto& block : memory_map_)
    {
      void* ptr = block.first;
      std::tie(sz, device_ptr) = block.second;
      Process::Journal() << "Host ptr: " << ptrToString(ptr) << " Device ptr: " << ptrToString(device_ptr) << " size: " << sz << finl;
    }
  Process::Journal() << "===================================" << finl;
}

bool DeviceMemory::warning(trustIdType nb_items)
{
  return clock_on && nb_pas_dt_>1 && nb_items>=internal_items_size_;
}
#endif
