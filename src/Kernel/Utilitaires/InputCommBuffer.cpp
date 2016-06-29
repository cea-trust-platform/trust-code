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
// File:        InputCommBuffer.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <InputCommBuffer.h>
#include <OutputCommBuffer.h>
#include <assert.h>
InputCommBuffer::InputCommBuffer()
{
  set_error_action(ERROR_CONTINUE);
  bin_ = 1;
  memorysize_ = 16;
  size_ = 0;
  // On alloue toujours quelque chose (create_stream a besoin d'un octet au moins
  // pour creer un stream de taille nulle)
  buffer_ = new char[memorysize_];
  stream_ = 0;
}

InputCommBuffer::~InputCommBuffer()
{
  assert(stream_ == 0);
  delete[] buffer_;
  buffer_ = 0;
  size_ = 0;
  memorysize_ = 0;
}

char * InputCommBuffer::reserve_buffer(int bufsize)
{
  assert(stream_ == 0);
  size_ = bufsize;
  if (size_ > memorysize_)
    {
      delete[] buffer_;
      memorysize_ = size_ * 2;
      buffer_ = new char[memorysize_];
    }
  return buffer_;
}

void InputCommBuffer::create_stream()
{
  assert(stream_ == 0);
  assert(size_ >= 0);
  // (Voir documentation de istrstream) :
  // Si size_ > 0 istrstream(buf, size_) cree un buffer de taille size_.
  // Si size_ = 0, istrstream(buf, size_) suppose que buf est une chaine
  //  de caracteres terminee par zero. Pour creer un stream de taille nulle,
  //  il faut donc faire ceci:
  if (size_ == 0)
    buffer_[0] = 0;
  stream_ = new istringstream(std::string(buffer_, size_));
  set_istream(stream_);
}

void InputCommBuffer::create_stream_from_output_stream(OutputCommBuffer& output_buf)
{
  assert(stream_ == 0);
  const char nullchar = 0;
  const char * buffer = output_buf.get_buffer();
  const int size = output_buf.get_buffer_size();
  // (Voir documentation de istrstream) :
  // Si size_ > 0 istrstream(buf, size_) cree un buffer de taille size_.
  // Si size_ = 0, istrstream(buf, size_) suppose que buf est une chaine
  //  de caracteres terminee par zero. Pour creer un stream de taille nulle,
  //  il faut donc faire ceci:
  if (size == 0)
    buffer = &nullchar;
  stream_ = new istringstream(std::string(buffer, size));
  set_istream(stream_);
}

void InputCommBuffer::clear()
{
  assert(stream_);
  delete stream_;
  stream_ = 0;
  set_istream(stream_);
  size_ = -1;
}
