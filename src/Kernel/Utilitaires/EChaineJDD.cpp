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

#include <EChaineJDD.h>

int EChaineJDD::file_cur_line_ = 1;

EChaineJDD::EChaineJDD() :
  Entree(), istrstream_(0), track_lines_(true)
{
  set_check_types(1);
}

EChaineJDD::EChaineJDD(const char* str) :
  Entree(),  istrstream_(0), track_lines_(true)
{
  set_check_types(1);
  init(str);
}

EChaineJDD::~EChaineJDD()  { }

void EChaineJDD::init(const char *str)
{
  if (istrstream_)
    delete istrstream_;
  istrstream_ = new istringstream(str);  // a copy of str is taken
  set_istream(istrstream_);
}

Entree& EChaineJDD::operator>>(True_int& ob) { return operator_template<True_int>(ob); }
Entree& EChaineJDD::operator>>(double& ob) { return operator_template<double>(ob); }

int EChaineJDD::get(char *ob, std::streamsize bufsize)
{
  if(track_lines_)
    {
      int jol = jumpOfLines();
      for(int jump=0; jump<jol; jump++)
        file_cur_line_++;
    }
  int ret = Entree::get(ob,bufsize);
  return ret;
}
