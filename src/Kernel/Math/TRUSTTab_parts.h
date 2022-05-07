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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTTab_parts.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTTab_parts_included
#define TRUSTTab_parts_included

#include <MD_Vector_composite.h>
#include <TRUSTTabs.h>

// .DESCRIPTION
//  This class allows to acces to individual sub-parts of DoubleTab objects that have a MD_Vector_composite descriptor.
template <typename _TYPE_>
class TRUSTTab_parts
{
public:
  TRUSTTab_parts() { }
  TRUSTTab_parts(TRUSTVect<_TYPE_>&);
  void initialize(TRUSTVect<_TYPE_>&);
  void reset() { parts_.dimensionner_force(0); }
  int size() const { return parts_.size(); }

  // operator[] est non const sinon on pourrait contourner le l'attribut const d'une classe...
  TRUSTTab<_TYPE_>& operator[](int i) { return parts_[i]; }
  // En revanche, on a toujours le doit d'avoir la version const:
  const TRUSTTab<_TYPE_>& operator[](int i) const { return parts_[i]; }

private:
  // les operateurs suivants sont interdits !
  TRUSTTab_parts& operator=(const TRUSTTab_parts&) = delete;
  TRUSTTab_parts(const TRUSTTab_parts&) = delete;
  // parts_[i] is initialized by adopter() to point to a sub_part of the source array.
  TRUST_Vector<TRUSTTab,_TYPE_> parts_; // Et ouiiiiiiiiiiiiiiii
};

using DoubleTab_parts = TRUSTTab_parts<double>;
using IntTab_parts = TRUSTTab_parts<int>;

// .DESCRIPTION
//  This class allows to acces to individual sub-parts of const DoubleTab objects that have a MD_Vector_composite descriptor.
//  It is designed to preserve the const-ness of the object.
template <typename _TYPE_>
class ConstTRUSTTab_parts
{
public:
  ConstTRUSTTab_parts() { }
  ConstTRUSTTab_parts(const TRUSTVect<_TYPE_>&);
  void initialize(const TRUSTVect<_TYPE_>&);
  void reset() { parts_.dimensionner_force(0); }
  const TRUSTTab<_TYPE_>& operator[](int i) const { return parts_[i]; }
  int size() const { return parts_.size(); }

private:
  // les operateurs suivants sont interdits !
  ConstTRUSTTab_parts& operator=(const ConstTRUSTTab_parts&) = delete;
  ConstTRUSTTab_parts(const ConstTRUSTTab_parts&) = delete;
  // parts_[i] is initialized by adopter() to point to a sub_part of the source array.
  TRUST_Vector<TRUSTTab,_TYPE_> parts_; // Et ouiiiiiiiiiiiiiiii
};

using ConstDoubleTab_parts = ConstTRUSTTab_parts<double>;
using ConstIntTab_parts = ConstTRUSTTab_parts<int>;

#include <TRUSTTab_parts.tpp>

#endif /* TRUSTTab_parts_included */
