/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        SChaine.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////
#ifndef SChaine_included
#define SChaine_included
#include <Sortie.h>
#include <Process.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Cette classe derivee de Sortie empile ce qu'on lui envoie dans une
//   chaine de caracteres. On recupere le contenu de la chaine avec get_str().
// .SECTION voir aussi
//    EChaine
//////////////////////////////////////////////////////////////////////////////
class SChaine :  public Sortie
{
public:
  SChaine();
  ~SChaine();
  const char* get_str() const;
  unsigned get_size() const;
  void setf(IOS_FORMAT code);
//  void self_test();   // [ABN] to be put in unit tests ...
  int set_bin(int bin);

protected:
  mutable std::string string_;

private:

};
#endif
