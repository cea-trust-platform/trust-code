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

#ifndef TRUSTTabs_forward_included
#define TRUSTTabs_forward_included

template <typename _TYPE_> class TRUSTArray;
using ArrOfInt = TRUSTArray<int>;
using ArrOfDouble = TRUSTArray<double>;

template<typename _TYPE_> class TRUSTVect;
using IntVect = TRUSTVect<int>;
using DoubleVect = TRUSTVect<double>;

template<typename _TYPE_> class TRUSTTab;
using IntTab = TRUSTTab<int>;
using DoubleTab = TRUSTTab<double>;

template<typename _TYPE_> class TRUSTTrav;
using IntTrav = TRUSTTrav<int>;
using DoubleTrav = TRUSTTrav<double>;

// previous VECT macros :-)
template<template<class> class _TRUST_TABL_,typename _TYPE_> class TRUST_Vector;
using Vect_ArrOfDouble = TRUST_Vector<TRUSTArray,double>; // remplace VECT(ArrOfDouble)
using Vect_ArrOfInt = TRUST_Vector<TRUSTArray,int>; // remplace VECT(ArrOfInt)
using ArrsOfDouble = Vect_ArrOfDouble;
using ArrsOfInt = Vect_ArrOfInt;
using Vect_DoubleVect = TRUST_Vector<TRUSTVect,double>; // remplace VECT(DoubleVect)
using Vect_IntVect = TRUST_Vector<TRUSTVect,int>; // remplace VECT(IntVect)
using DoubleVects = Vect_DoubleVect;
using IntVects = Vect_IntVect;
using Vect_DoubleTab = TRUST_Vector<TRUSTTab,double>; // remplace VECT(DoubleTab)
using Vect_IntTab = TRUST_Vector<TRUSTTab,int>; // remplace VECT(IntTab)
using DoubleTabs = Vect_DoubleTab;
using IntTabs = Vect_IntTab;

#endif /* TRUSTTabs_forward_included */
