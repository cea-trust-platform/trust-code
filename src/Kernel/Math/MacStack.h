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
// File:        MacStack.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MacStack_H
#define MacStack_H

#define declare_stack(_TYPE_)                        \
  class STACK(_TYPE_) {                                \
  public :                                        \
    STACK(_TYPE_)(const int t=10);                \
    ~STACK(_TYPE_)();                                \
    int push(const _TYPE_&);                        \
    _TYPE_& pop(void);                                \
    _TYPE_& peek(void);                                \
    int isEmpty() const;                        \
    _TYPE_* getBase() { return base; }                \
  private:                                        \
    _TYPE_ *top ;                                \
    _TYPE_ *base ;                                \
    int taille ;                                \
  };

#define declare_pstack(_TYPE_)                        \
  class PSTACK(_TYPE_) {                        \
  public :                                        \
    PSTACK(_TYPE_)(const int t=10);                \
    ~PSTACK(_TYPE_)();                                \
    int push(_TYPE_*&);                                \
    _TYPE_*& pop(void);                                \
    _TYPE_*& peek(void);                        \
    int isEmpty() const;                        \
    _TYPE_** getBase() { return base; }                \
  private:                                        \
    _TYPE_**top ;                                \
    _TYPE_**base ;                                \
    int taille ;                                \
  };

#define implemente_stack(_TYPE_)                                        \
  STACK(_TYPE_)::STACK(_TYPE_)(const int t) : taille(t)                        \
  {                                                                        \
    base = new _TYPE_ [t];                                                \
    top = base;                                                                \
  }                                                                        \
  STACK(_TYPE_)::~STACK(_TYPE_)() {                                        \
    delete[] base ;                                                        \
  }                                                                        \
  _TYPE_& STACK(_TYPE_)::pop(void)                                        \
  {                                                                        \
    if (top!=base) return *(--top) ;                                        \
    return *base;                                                        \
  }                                                                        \
  _TYPE_& STACK(_TYPE_)::peek(void)                                        \
  {                                                                        \
    return *(top-1) ;                                                        \
  }                                                                        \
  int STACK(_TYPE_)::push(const _TYPE_& valeur)                                \
  {                                                                        \
    if ((top-base)==taille)                                                \
      {                                                                        \
        cout << "Stack too small : one double the size !!" << endl;        \
        _TYPE_* tmp = new _TYPE_[2*taille];                                \
        for (int i=0;i<taille;i++)                                        \
          tmp[i] = base[i];                                                \
        delete [] base;                                                        \
        base = tmp;                                                        \
        top = base+taille;                                                \
        taille=2*taille;                                                \
      }                                                                        \
    *top++=valeur ;                                                        \
    return 1;                                                                \
  }                                                                        \
  int STACK(_TYPE_)::isEmpty() const { return top==base ;}

#define implemente_pstack(_TYPE_)                                        \
  PSTACK(_TYPE_)::PSTACK(_TYPE_)(const int t) : taille(t)                \
  {                                                                        \
    base = new _TYPE_*[t];                                                \
    top = base;                                                                \
  }                                                                        \
  PSTACK(_TYPE_)::~PSTACK(_TYPE_)() {                                        \
    delete[] base ;                                                        \
  }                                                                        \
  _TYPE_*& PSTACK(_TYPE_)::pop(void)                                        \
  {                                                                        \
    if (top!=base) return *(--top) ;                                        \
    return *base;                                                        \
  }                                                                        \
  _TYPE_*& PSTACK(_TYPE_)::peek(void)                                        \
  {                                                                        \
    return *(top-1) ;                                                        \
  }                                                                        \
  int PSTACK(_TYPE_)::push(_TYPE_*& valeur)                                \
  {                                                                        \
    if ((top-base)==taille)                                                \
      {                                                                        \
        cout << "Stack too small: one double the size !!" << endl;        \
        _TYPE_** tmp = new _TYPE_*[2*taille];                                \
        for (int i=0;i<taille;i++)                                        \
          tmp[i] = base[i];                                                \
        delete [] base;                                                        \
        base = tmp;                                                        \
        top = base+taille;                                                \
        taille=2*taille;                                                \
      }                                                                        \
    *top++=valeur ;                                                        \
    return 1;                                                                \
  }                                                                        \
  int PSTACK(_TYPE_)::isEmpty() const { return top==base ;}
#endif
