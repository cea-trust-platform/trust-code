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
// File:        IntListsListe.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <IntListsListe.h>
#include <Nom.h>

Sortie& IntListsListe::printOn(Sortie& s) const
{
  int i=sz;
  while(i--)
    data[i].printOn(s);
  return s<<finl;
}
Entree& IntListsListe::readOn(Entree& s)
{
  return s;
}
IntListsListe::IntListsListe(const IntListsListe& vect)
  : sz(vect.sz), data(new IntLists[vect.sz])
{
  if(!data)
    {
      Cerr << "Unable to carry out allocation " << finl;
      Process::exit();
    }
  int i=sz;
  if(i==0) data=0;
  else while(i--)
      data[i]=vect[i];
}
IntListsListe::IntListsListe() : sz(0), data(0) {}
IntListsListe::IntListsListe(int i): sz(i)
{
  if(i==0) data=0;
  else
    {
      data= new IntLists[i];
      if(!data)
        {
          Cerr << "Unable to carry out allocation" << finl;
          Process::exit();
        }
    }
}
IntListsListe::~IntListsListe()
{
  if(sz)
    {
      assert(data);
      delete[] data;
    }
  else
    assert (data==0) ;
}
IntListsListe& IntListsListe::operator=(const IntListsListe& vect)
{
  sz=vect.sz;
  if(data)
    delete[] data;
  if(sz)
    {
      data = new IntLists[sz];
      if(!data)
        {
          Cerr << "Unable to carry out allocation " << finl;
          Process::exit();
        }
      int i=sz;
      while(i--)
        data[i]=vect[i];
    }
  else data=0;
  return *this;
}
/* int IntListsListe::search(const IntLists& t) const
   {
   int i=sz;
   int retour=-1;
   while(i--)
   if( data[i] == t )
   { retour=i; i=0; }
   return retour;
   }*/
const IntLists& IntListsListe::operator[](int i) const
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}
IntLists& IntListsListe::operator[](int i)
{
  assert( (i>=0) && (i<sz) );
  return data[i];
}


void IntListsListe::dimensionner(int i)
{
  assert(sz==0);
  assert(data==0);
  sz =i;
  if(i==0)
    data=0;
  else
    {
      data = new IntLists [i];
      if(!data)
        {
          Cerr << "Unable to carry out allocation " << finl;
          Process::exit();
        }
    }
}

