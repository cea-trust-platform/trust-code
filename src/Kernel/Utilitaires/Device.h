/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Device_included
#define Device_included

#include <Array_base.h>

static double clock_start;
static char* clock_on=NULL;
extern bool self_test();

extern void init_openmp();
extern void init_cuda();
extern void start_timer(int size=-1);
extern void end_timer(const std::string& str, int size=-1);

template <typename _TYPE_>
extern const _TYPE_* copyToDevice(const TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern _TYPE_* copyToDevice_(TRUSTArray<_TYPE_>& tab, DataLocation nextLocation, std::string arrayName);

template <typename _TYPE_>
extern _TYPE_* computeOnTheDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern void copyFromDevice(TRUSTArray<_TYPE_>& tab, std::string arrayName="??");

template <typename _TYPE_>
extern void copyPartialFromDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??");

template <typename _TYPE_>
extern void copyPartialToDevice(TRUSTArray<_TYPE_>& tab, int deb, int fin, std::string arrayName="??");

#endif
