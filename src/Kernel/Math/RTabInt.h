/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        RTabInt.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RTabInt_included
#define RTabInt_included

#include <assert.h>
#include <ArrOfInt.h>


////////////////////////////////////////
//
// CLASS: RTabInt
//
// .DESCRIPTION class RTabInt
//
//        La classe RTabInt implemente la notion de vecteur
//        d'entier redimensionnable.
//
////////////////////////////////////////

class RTabInt : public Objet_U
{

  Declare_instanciable_sans_constructeur(RTabInt);


public:

  // Constructeurs et destructeur
  RTabInt(int n=0, int x=0);

  // Tailles
  inline int size() const;

  inline void resize(int);
  int search(int);
  void add(int);
  inline int& operator[](int i);
  inline const int& operator[](int i) const ;
  inline int& operator()(int i);
  inline const int& operator()(int i) const ;

  const ArrOfInt& donnees() const;

private:
  static int TB_;
  int size_r_;
  ArrOfInt data;
  int min_data;
  int max_data;

};
// Description:
// operateur []
inline int RTabInt::size() const
{
  return size_r_;
}

// Description:
// operateur []
inline void RTabInt::resize(int n)
{
  if(n>data.size_array())
    {
      int sz = data.size_array();
      data.resize_array(n);
      data.resize_array(sz+TB_);
      for(int j=sz; j< sz+TB_; j++)
        data[j] = -1;
    }
}


// Description:
// operateur []
inline int& RTabInt::operator[](int i)
{
  return data[i];
}
inline const int& RTabInt::operator[](int i) const
{
  return data[i];
}

// Description:
// idem operator[]
inline int& RTabInt::operator()(int i)
{
  return data[i];
}
inline const int& RTabInt::operator()(int i) const
{
  return data[i];
}

#endif//RTABINT

