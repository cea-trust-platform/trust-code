/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <OutputCommBuffer.h>

OutputCommBuffer::OutputCommBuffer()
{
  bin_ = 1;
  // Communication buffer should never try to convert int into long:
  avoid_conversion_ = true;

  ostream_ = std::make_unique<std::ostringstream>();
  // Typed view - memory is managed by Sortie:
  stream_ = static_cast<std::ostringstream *>(ostream_.get());

  // Initialisation avec les 2 lignes suivantes car sinon plantage sur PAR_Cx 7 procs sur AIX:
  // La STD est specifique sur cette machine
  *this << "";
  stream_->seekp(0);
}

OutputCommBuffer::~OutputCommBuffer()
{
  clear();
}

const char * OutputCommBuffer::get_buffer()
{
  // Need a copy:
  string_ = stream_->str();
  return string_.c_str();
}

int OutputCommBuffer::get_buffer_size()
{
  return (int)stream_->tellp();
}

void OutputCommBuffer::clear()
{
  if (stream_)
    stream_->seekp(0);
}
