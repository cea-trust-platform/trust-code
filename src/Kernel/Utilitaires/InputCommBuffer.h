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
// File:        InputCommBuffer.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#ifndef InputCommBuffer_included
#define InputCommBuffer_included
#include <Entree.h>
#include <sstream>
class OutputCommBuffer;
using std::istringstream;
// .DESCRIPTION        :
//  Classe outil utilisee exclusivement par Schema_Comm. C'est une classe
//  derivee de Entree dont le stream est de type istringstream (les donnees
//  lues par operator>> sont prises dans un buffer).
//  On utilise la classe comme suit:
//  (1) on reserve un buffer d'une certaine taille avec
//    char * buf = input_comm_buffer.reserve_buffer(taille);
//  (2) on remplit le buffer avec des donnees:
//    for (i=0; i<taille; i++)
//       buf[i] = .....;
//  (3) on cree un stream a partir du buffer:
//    input_comm_buffer.create_stream();
//  (4) on peut ensuite lire les donnees a travers l'operateur>>:
//    input_comm_buffer >> x >> y >> chaine >> ... ;
//  (5) quand on a fini de lire avec operateur>> on fait
//    input_comm_buffer.clear();
//  et on peut refaire (1)

class InputCommBuffer : public Entree
{
public:
  InputCommBuffer();
  ~InputCommBuffer();
  // Specifie la taille du buffer et renvoie son adresse.
  // L'utilisateur doit ensuite y mettre les donnees (tout le buffer
  // doit etre rempli).
  char * reserve_buffer(int bufsize);
  void   create_stream_from_output_stream(OutputCommBuffer&);
  // On cree le stream de lecture a partir du buffer.
  void create_stream();
  void clear();
private:
  istringstream * stream_;
  char * buffer_;
  int size_;
  int memorysize_; // memorysize_ >= size_
};
#endif
