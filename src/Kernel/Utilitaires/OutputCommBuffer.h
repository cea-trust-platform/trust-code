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
#ifndef OutputCommBuffer_included
#define OutputCommBuffer_included
#include <Sortie.h>
#include <sstream>
using  std::ostringstream;

// .DESCRIPTION        :
//  Classe outil utilisee exclusivement par Schema_Comm. C'est une classe
//  derivee de Entree dont le stream est de type ostrstream (les donnees
//  sont ecrites par operator<< dans un buffer en memoire).
//  On utilise la classe comme suit:
//  (1) on ecrit des donnees avec operator<< dans le buffer
//    ouput_comm_buffer << x << y << chaine << ... ;
//  (2) on recupere l'ensemble des donnees ecrites sous la forme d'un
//    bloc de memoire contigu de taille "get_buffer_size()" situe
//    a l'adresse "get_buffer()".
//  (3) on reinitialise le buffer avec "clear()"
//  et on peut refaire (1)

class OutputCommBuffer : public Sortie
{
public:
  OutputCommBuffer();
  ~OutputCommBuffer() override;
  void clear();
  const char * get_buffer();
  int get_buffer_size();
  ostringstream& get_stream()
  {
    return stream_;
  };
private:
  ostringstream stream_;
  std::string string_;
};
#endif
