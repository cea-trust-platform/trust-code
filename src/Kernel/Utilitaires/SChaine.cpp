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
// File:        SChaine.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////
#include <SChaine.h>
#include <EChaine.h>
#include <sstream>
using std::ostringstream;

void SChaine::self_test()
{
  char *toto =new char[100];
  strcpy(toto,"toto");
  (*this)<<toto<<endl;
  Nom titi(toto);
  (*this)<<titi;
  double t=0.1;
  (*this)<<t;
  // (*this)<<finl;

  //  int es=ostream_p->pcount();
  const char* s=get_str();
  cerr<<"there"<<s<<endl;
  (*this)<<"tutu"<<finl;
  (*this)<<"tralala"<<finl;
  const char* s2=get_str();
  cerr<<strlen(s2);

  cerr<<"HERE"<<s2<<endl;
  //cerr<<"FIN"<<endl;
  const char *ref="toto\ntoto0.1tutu\ntralala\n";
  EChaine test(s2);
  if ((strcmp(test.get_str(),ref))!=0)
    {
      Cerr<<"Problem with Echaine SChaine"<<finl;
      Cerr<<"ref: "<<ref<<"#"<<finl;
      Cerr<<"SChaine: "<< s2<<"#"<<(int)strcmp(s2,ref)<<finl;
      Cerr<<"EChaine: "<< test.get_str()<<"#"<<(int)strcmp(test.get_str(),ref)<<finl;
      Process::exit();
    }

  delete [] toto;
}

SChaine::SChaine()
{
  set_bin(0);
  ostringstream *os = new ostringstream();
  set_ostream(os);
}

void SChaine::setf(IOS_FORMAT code)
{
  Sortie::setf( code );
  //  Cerr << "Error in SChaine::setf(IOS_FORMAT code)" << finl;
  //  Process::exit();
}

SChaine::~SChaine()
{

}

const char* SChaine::get_str() const
{
  ostringstream& os= *((ostringstream*)(&((SChaine*)this)->get_ostream()));
  // Need a copy
  string_=string(os.str());
  return string_.c_str();
}

int SChaine::set_bin(int bin)
{
  if (bin != 0)
    {
      Cerr << "Error in SChaine::set_bin(int bin) : binary format not supported" << finl;
      Process::exit();
    }
  Sortie::set_bin(bin);
  return bin;
}
