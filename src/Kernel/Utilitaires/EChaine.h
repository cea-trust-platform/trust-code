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
// File:        EChaine.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EChaine_included
#define EChaine_included

#include <Nom.h>
#include <sstream>
using std::istringstream;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Une entree dont la source est une chaine de caracteres.
//   Le constructeur cree sa propre copie de la chaine, de sorte
//   que la chaine d'origine peut etre modifiee ensuite sans modifier
//   la EChaine.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class EChaine : public Entree
{

public:
  EChaine() : Entree(), istrstream_(0)
  {
    set_check_types(1);
  }
  EChaine(const char* str) : Entree(),  istrstream_(0)
  {
    set_check_types(1);
    init(str);
  }
  ~EChaine()  { }
  void init(const char *str)
  {
    if (istrstream_)
      delete istrstream_;
    // On cree une copie de la chaine:
    string_ = str;
    istrstream_ = new istringstream(string_);
    set_istream(istrstream_);
  }
  inline const char* get_str() const;

protected:
  istringstream* istrstream_;
  std::string string_;
private:

};

inline const char* EChaine::get_str() const
{
  return string_.c_str();
}
#endif
