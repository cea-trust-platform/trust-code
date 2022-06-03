/****************************************************************************
* Copyright (c) 2020, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Device.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Device_included
#define Device_included

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic push

inline void copyToDevice(const ArrOfInt& tab) {
    const int *tab_addr = tab.addr();
#pragma omp target enter data map(to:tab_addr[0:tab.size_array()])
}
inline void copyToDevice(ArrOfInt& tab) {
    int *tab_addr = tab.addr();
#pragma omp target enter data map(to:tab_addr[0:tab.size_array()])
}
inline void copyToDevice(const ArrOfDouble& tab) {
    const double *tab_addr = tab.addr();
#pragma omp target enter data map(to:tab_addr[0:tab.size_array()])
}
inline void copyToDevice(ArrOfDouble& tab) {
    double *tab_addr = tab.addr();
#pragma omp target enter data map(to:tab_addr[0:tab.size_array()])
}

#pragma GCC diagnostic pop
#endif
