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
// File:        RTabInt.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#include <RTabInt.h>
#include <math.h>
#ifdef SGI_
#include <bstring.h>
#endif

Implemente_instanciable_sans_constructeur(RTabInt,"RTabInt",Objet_U);

int RTabInt::TB_ = 1000;

//////////
// Description:
// printOn
//
Sortie& RTabInt::printOn(Sortie& os) const
{
  os << data;
  return os;
}

//////////
// Description:
// readOn
//
Entree& RTabInt::readOn(Entree& is)
{
  is >> data;
  size_r_ = data.size_array();
  return is;
}


//////////
// Description:
// constructeur
//
RTabInt::RTabInt(int n, int x)
{
  data.resize_array(n+TB_);
  int i;
  int init=-1;
  for(i=0; i<n; i++)
    data[i] = x;
  for(i=n; i<n+TB_; i++)
    data[i] = init;
  size_r_ = n;
  min_data=init;
  max_data=init;
}

//////////
// Description:
// Renvoi le tableau porte
//
const ArrOfInt& RTabInt::donnees() const
{
  return data;
}

// Description:
// Ajoute TB_ cases si i > size_r_
// sinon ajoute i en queue
void RTabInt::add(int i)
{
  int sz = data.size_array();
  if(size_r_ == sz)
    {
      data.resize_array(sz+TB_);
      data[size_r_] = i;
      size_r_++;
      for(int j=size_r_; j< sz+TB_; j++)
        data[j] = -1;
    }
  else
    {
      data[size_r_] = i;
      size_r_++;
    }
  if (i<min_data)
    min_data=i;
  else if (i>max_data)
    max_data=i;
}

// Description:
// Recherche un element egal a i dans le tableau
// Renvoi la valeur si elle existe, -1 sinon
int RTabInt::search(int i)
{
  if (i<min_data || i>max_data)
    return -1;
  int cpt=0;
  while((cpt<size_r_) && (data[cpt]!=i))
    cpt++;
  if(cpt==size_r_)
    return -1;
  else
    return data[cpt];
}
