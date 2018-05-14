/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Entree_Brute.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////
#include <Entree_Brute.h>
#include <Process.h>
#include <EntreeSortie.h>
#include <sstream>

using std::istringstream;

Entree_Brute::Entree_Brute():
  Entree()
{
  set_bin(1);
  istrstream_ = new istringstream();
  set_istream(istrstream_);
}


Entree_Brute::~Entree_Brute()
{
  delete[] data_;
}

int Entree_Brute::set_bin(int bin)
{
  if (bin != 1)
    {
      Cerr << "Error in Entree_Brute::set_bin(int bin) : only binary format supported. Use EChaine otherwise." << finl;
      Process::exit();
    }
  Entree::set_bin(bin);
  return bin;
}

void Entree_Brute::set_data(char * data, unsigned sz)
{
  const std::istream& iss = get_istream();
  std::streambuf * sb = iss.rdbuf();
  if(!data_)
    {
      // For now forbid multiple calls ... could try deleting data_
      Cerr << "Entree_Brute::set_data(): Multiple calls forbidden!" << finl;
      Process::exit(-1);
    }
  data_ = new char[sz];
  std::copy(data, data+sz, data_);
  sb->pubsetbuf(data_, sz);
}
