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
#include <FloatTab.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(FloatTab,"FloatTab",ArrOfFloat);

// Constructeurs

FloatTab::FloatTab() :
  ni_(0), nj_(0)
{
}

FloatTab::FloatTab(int ni, int nj) :
  ni_(0), nj_(0)
{
  resize(ni, nj);
}

FloatTab::FloatTab(const FloatTab& tab) :
  ArrOfFloat(tab), ni_(tab.ni_), nj_(tab.nj_)
{
}

// Operateur copie

const FloatTab& FloatTab::operator=(const FloatTab& tab)
{
  ArrOfFloat::operator=(tab);
  ni_ = tab.ni_;
  nj_ = tab.nj_;
  return *this;
}

// Destructeur

FloatTab::~FloatTab()
{
  ni_ = nj_ = 0;
}

// Entrees / sorties

Entree& FloatTab::readOn(Entree& is)
{
  // nbdim pour compatibilite avec IntTab
  int nbdim;
  is >> nbdim;
  assert(nbdim == 2);
  is >> ni_ >> nj_;
  assert(ni_ >= 0 && nj_ >= 0);
  int mysize;
  is >> mysize;
  assert(mysize == ni_ * nj_);
  // Resize en deux fois pour ne pas faire de memcpy inutile
  resize_array(0);
  resize_array(ni_ * nj_);
  if (size_ > 0)
    is.get(data_, size_);
  return is;
}

Sortie& FloatTab::printOn(Sortie& os) const
{
  os << (int)2 << finl;
  os << ni_ << nj_ << finl;
  os << size_ << finl;
  if (size_ > 0)
    os.put(data_, size_, nj_);
  return os;
}
