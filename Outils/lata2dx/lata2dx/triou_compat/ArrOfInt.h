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

#ifndef ArrOfInt_H
#define ArrOfInt_H

#include <TRUSTTab.tpp>

using ArrOfInt = TRUSTArray<entier>;

using IntTab = TRUSTTab<entier>;

//class ArrOfInt : public TRUSTArray<entier>
//{
//public:
//  ArrOfInt() : TRUSTArray<entier>() { }
//  ArrOfInt(entier a,entier b) : TRUSTArray<entier>(a,b) { }
//
//  // see https://www.linuxtopia.org/online_books/programming_books/thinking_in_c++/Chapter14_018.html
//  inline ArrOfInt& operator=(entier x) { TRUSTArray<entier>::operator=(x); return *this; }
//};
//
//
//class IntTab : public TRUSTTab<entier>
//{
//public:
//  IntTab() : TRUSTTab<entier>() { }
//  IntTab(const entier i, const entier j) : TRUSTTab<entier>(i,j) { }
//};

#endif /* ArrOfInt_H */

