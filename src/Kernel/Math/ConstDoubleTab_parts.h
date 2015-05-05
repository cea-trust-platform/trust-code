/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        ConstDoubleTab_parts.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#ifndef ConstDoubleTab_parts_included
#define ConstDoubleTab_parts_included

#include <DoubleTabs.h>
#include <Vect_IntTab.h>

// .DESCRIPTION
//  This class allows to acces to individual sub-parts of const DoubleTab
//  objects that have a MD_Vector_composite descriptor. It is designed
//  to preserve the const-ness of the object.
class ConstDoubleTab_parts
{
public:
  ConstDoubleTab_parts() {};
  ConstDoubleTab_parts(const DoubleVect&);
  void initialize(const DoubleVect&);
  void reset();
  const DoubleTab& operator[](int i) const
  {
    return parts_[i];
  }
  int size() const
  {
    return parts_.size();
  }
private:
  // Interdit (voir commentaires dans .cpp)
  ConstDoubleTab_parts& operator=(const ConstDoubleTab_parts&);
  ConstDoubleTab_parts(const ConstDoubleTab_parts&);
  // parts_[i] is initialized by adopter() to point to a sub_part of
  //  the source array.
  VECT(DoubleTab) parts_;
};

class DoubleTab_parts
{
public:
  DoubleTab_parts() {};
  DoubleTab_parts(DoubleVect&);
  void initialize(DoubleVect&);
  void reset();
  // operator[] est non const sinon on pourrait contourner le
  // l'attribut const d'une classe...
  DoubleTab& operator[](int i)
  {
    return parts_[i];
  }
  // En revanche, on a toujours le doit d'avoir la version const:
  const DoubleTab& operator[](int i) const
  {
    return parts_[i];
  }
  int size() const
  {
    return parts_.size();
  }
private:
  // Interdit (voir commentaires dans .cpp)
  //  note: ca serait interdit meme si le parametre n'est pas const !!!
  DoubleTab_parts& operator=(const DoubleTab_parts&);
  DoubleTab_parts(const DoubleTab_parts&);
  // parts_[i] is initialized by adopter() to point to a sub_part of
  //  the source array.
  VECT(DoubleTab) parts_;
};

class ConstIntTab_parts
{
public:
  ConstIntTab_parts() {};
  ConstIntTab_parts(const IntVect&);
  void initialize(const IntVect&);
  void reset();
  const IntTab& operator[](int i) const
  {
    return parts_[i];
  }
  int size() const
  {
    return parts_.size();
  }
private:
  // Interdit (voir commentaires dans .cpp)
  ConstIntTab_parts& operator=(const ConstIntTab_parts&);
  ConstIntTab_parts(const ConstIntTab_parts&);
  // parts_[i] is initialized by adopter() to point to a sub_part of
  //  the source array.
  VECT(IntTab) parts_;
};

class IntTab_parts
{
public:
  IntTab_parts() {};
  IntTab_parts(IntVect&);
  void initialize(IntVect&);
  void reset();
  // operator[] est non const sinon on pourrait contourner le
  // l'attribut const d'une classe...
  IntTab& operator[](int i)
  {
    return parts_[i];
  }
  // En revanche, on a toujours le doit d'avoir la version const:
  const IntTab& operator[](int i) const
  {
    return parts_[i];
  }
  int size() const
  {
    return parts_.size();
  }
private:
  // Interdit (voir commentaires dans .cpp)
  //  note: ca serait interdit meme si le parametre n'est pas const !!!
  IntTab_parts& operator=(const IntTab_parts&);
  IntTab_parts(const IntTab_parts&);
  // parts_[i] is initialized by adopter() to point to a sub_part of
  //  the source array.
  VECT(IntTab) parts_;
};

#endif
