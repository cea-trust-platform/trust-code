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
// File:        FloatTab.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#ifndef FloatTab_included
#define FloatTab_included
#include <ArrOfFloat.h>

class DoubleTab;

class FloatTab : public ArrOfFloat
{
  Declare_instanciable_sans_constructeur_ni_destructeur(FloatTab);
public:
  FloatTab();
  FloatTab(int ni, int nj);
  FloatTab(const FloatTab& tab);
  const FloatTab& operator=(const FloatTab& tab);
  ~FloatTab();

  inline float   operator()(int i, int j) const;
  inline float& operator()(int i, int j);

  inline int dimension(int i) const;
  inline int nb_dim() const;
  inline void resize(int ni, int nj);

protected:
  int ni_, nj_;
};

// ***************************************************************

inline float   FloatTab::operator()(int i, int j) const
{
  assert(i >= 0 && i < ni_ && j >= 0 && j < nj_);
  return data_[i*nj_+j];
}

inline float& FloatTab::operator()(int i, int j)
{
  assert(i >= 0 && i < ni_ && j >= 0 && j < nj_);
  return data_[i*nj_+j];
}

inline int FloatTab::dimension(int i) const
{
  assert(i == 0 || i == 1);
  return (i == 0) ? ni_ : nj_;
}

inline int FloatTab::nb_dim() const
{
  return 2;
}

inline void FloatTab::resize(int ni, int nj)
{
  assert(ni >= 0 && nj >= 0);
  int new_size = ni * nj;
  resize_array(new_size);
  ni_ = ni;
  nj_ = nj;
}

#endif
